// clang-format off
// Must precede glfw/glad, to include OpenGL functions.
#include <qrk/quarkgl.h>
// clang-format on

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/flags/usage.h"
#include "imGuIZMOquat.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

ABSL_FLAG(std::string, model, "", "Path to a model file");

const char* lampShaderSource = R"SHADER(
#version 460 core
out vec4 fragColor;

void main() { fragColor = vec4(1.0); }
)SHADER";

const char* normalShaderSource = R"SHADER(
#version 460 core
out vec4 fragColor;

void main() { fragColor = vec4(1.0, 1.0, 0.0, 1.0); }
)SHADER";

enum class CameraControlType {
  FLY = 0,
  ORBIT,
};

enum class LightingModel {
  BLINN_PHONG = 0,
  COOK_TORRANCE_GGX,
};

enum class GBufferVis {
  DISABLED = 0,
  POSITIONS,
  AO,
  NORMALS,
  ROUGHNESS,
  ALBEDO,
  METALLIC,
  EMISSION,
};

enum class ToneMapping {
  NONE = 0,
  REINHARD,
  REINHARD_LUMINANCE,
  ACES_APPROX,
  AMD,
};

// Options for the model render UI. The defaults here are used at startup.
struct ModelRenderOptions {
  // Model.
  glm::quat modelRotation = glm::identity<glm::quat>();

  // Rendering.
  LightingModel lightingModel = LightingModel::COOK_TORRANCE_GGX;

  glm::vec3 directionalDiffuse = glm::vec3(0.5f);
  glm::vec3 directionalSpecular = glm::vec3(0.5f);
  float directionalIntensity = 10.0f;
  glm::vec3 directionalDirection =
      glm::normalize(glm::vec3(-0.2f, -1.0f, -0.3f));

  bool shadowMapping = true;
  float shadowCameraCuboidExtents = 2.0f;
  float shadowCameraNear = 0.1f;
  float shadowCameraFar = 15.0f;
  float shadowCameraDistance = 5.0f;
  float shadowBiasMin = 0.0001;
  float shadowBiasMax = 0.001;

  bool useIrradianceMap = true;
  glm::vec3 ambientColor = glm::vec3(0.1f);
  bool ssao = true;
  float ssaoRadius = 0.5f;
  float ssaoBias = 0.025f;
  float shininess = 32.0f;
  float emissionIntensity = 5.0f;
  glm::vec3 emissionAttenuation = glm::vec3(0, 0, 1.0f);

  bool bloom = true;
  float bloomMix = 0.005;
  ToneMapping toneMapping = ToneMapping::ACES_APPROX;
  bool gammaCorrect = true;
  float gamma = 2.2f;

  bool fxaa = true;

  // Camera.
  CameraControlType cameraControlType = CameraControlType::ORBIT;
  float speed = 0;
  float sensitivity = 0;
  float fov = 0;
  float near = 0;
  float far = 0;
  bool captureMouse = false;

  // Debug.
  GBufferVis gBufferVis = GBufferVis::DISABLED;
  bool wireframe = false;
  bool drawNormals = false;

  // Performance.
  const float* frameDeltas = nullptr;
  int numFrameDeltas = 0;
  int frameDeltasOffset = 0;
  float avgFPS = 0;
  bool enableVsync = true;
};

// Helper to display a little (?) mark which shows a tooltip when hovered.
static void helpMarker(const char* desc) {
  ImGui::TextDisabled("(?)");
  if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort)) {
    ImGui::BeginTooltip();
    ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
    ImGui::TextUnformatted(desc);
    ImGui::PopTextWrapPos();
    ImGui::EndTooltip();
  }
}

enum class Scale {
  LINEAR = 0,
  LOG,
};

// Helper for a float slider value.
static bool floatSlider(const char* desc, float* value, float min, float max,
                        const char* fmt = nullptr,
                        Scale scale = Scale::LINEAR) {
  ImGuiSliderFlags flags = ImGuiSliderFlags_None;
  if (scale == Scale::LOG) {
    flags = ImGuiSliderFlags_Logarithmic;
  }
  return ImGui::SliderScalar(desc, ImGuiDataType_Float, value, &min, &max, fmt,
                             flags);
}

// Non-normative context for UI rendering. Used for accessing renderer info.
struct UIContext {
  qrk::Camera& camera;
  qrk::ShadowMap& shadowMap;
};

// Called during game loop.
void renderImGuiUI(ModelRenderOptions& opts, UIContext ctx) {
  // ImGui::ShowDemoWindow();

  ImGui::Begin("Model Render");

  if (ImGui::CollapsingHeader("Model", ImGuiTreeNodeFlags_DefaultOpen)) {
    // Perform some shenanigans so that the gizmo rotates along with the
    // camera while still representing the same model rotation.
    glm::quat rotViewSpace =
        glm::quat_cast(ctx.camera.getViewTransform()) * opts.modelRotation;
    ImGui::gizmo3D("Rotation", rotViewSpace, /*size=*/160);
    opts.modelRotation =
        glm::quat_cast(glm::inverse(ctx.camera.getViewTransform())) *
        glm::normalize(rotViewSpace);

    if (ImGui::Button("Reset")) {
      opts.modelRotation = glm::identity<glm::quat>();
    }
  }

  if (ImGui::CollapsingHeader("Rendering")) {
    ImGui::Combo("Lighting model", reinterpret_cast<int*>(&opts.lightingModel),
                 "Blinn-Phong\0Cook-Torrance GGX\0\0");
    ImGui::SameLine();
    helpMarker("Which lighting model to use for shading.");

    ImGui::Separator();
    ImGui::Text("Directional light");
    static bool lockSpecular = true;
    ImGui::ColorEdit3("Diffuse color",
                      reinterpret_cast<float*>(&opts.directionalDiffuse),
                      ImGuiColorEditFlags_Float | ImGuiColorEditFlags_HDR);
    ImGui::BeginDisabled(lockSpecular);
    ImGui::ColorEdit3("Specular color",
                      reinterpret_cast<float*>(&opts.directionalSpecular),
                      ImGuiColorEditFlags_Float | ImGuiColorEditFlags_HDR);
    ImGui::EndDisabled();
    ImGui::Checkbox("Lock specular", &lockSpecular);
    ImGui::SameLine();
    helpMarker(
        "Whether to lock the specular light color to the diffuse. Usually "
        "desired for PBR.");
    if (lockSpecular) {
      opts.directionalSpecular = opts.directionalDiffuse;
    }
    floatSlider("Intensity", &opts.directionalIntensity, 0.0f, 50.0f, nullptr,
                Scale::LINEAR);

    // Perform some shenanigans so that the gizmo rotates along with the
    // camera while still representing the same light dir..
    glm::vec3 dirViewSpace =
        glm::vec3(ctx.camera.getViewTransform() *
                  glm::vec4(opts.directionalDirection, 0.0f));
    ImGui::gizmo3D("Light dir", dirViewSpace, /*size=*/160);
    opts.directionalDirection =
        glm::vec3(glm::inverse(ctx.camera.getViewTransform()) *
                  glm::vec4(glm::normalize(dirViewSpace), 0.0f));
    ImGui::SliderFloat3("##directional_direction",
                        reinterpret_cast<float*>(&opts.directionalDirection),
                        -1.0f, 1.0f);

    ImGui::Separator();
    ImGui::Text("Shadows");

    ImGui::Checkbox("Shadow mapping", &opts.shadowMapping);
    ImGui::BeginDisabled(!opts.shadowMapping);
    ImTextureID shadowMapTexID =
        reinterpret_cast<void*>(ctx.shadowMap.getDepthTexture().getId());
    ImGui::Image(shadowMapTexID, glm::vec2(200, 200));
    floatSlider("Cuboid extents", &opts.shadowCameraCuboidExtents, 0.1f, 50.0f,
                nullptr, Scale::LOG);

    if (floatSlider("Near plane", &opts.shadowCameraNear, 0.01, 1000.0, nullptr,
                    Scale::LOG)) {
      if (opts.shadowCameraNear > opts.shadowCameraFar) {
        opts.shadowCameraFar = opts.shadowCameraNear;
      }
    }
    if (floatSlider("Far plane", &opts.shadowCameraFar, 0.01, 1000.0, nullptr,
                    Scale::LOG)) {
      if (opts.shadowCameraFar < opts.shadowCameraNear) {
        opts.shadowCameraNear = opts.shadowCameraFar;
      }
    }
    floatSlider("Distance from origin", &opts.shadowCameraDistance, 0.01,
                100.0f, nullptr, Scale::LOG);
    if (floatSlider("Bias min", &opts.shadowBiasMin, 0.0001, 1.0, "%.04f",
                    Scale::LOG)) {
      if (opts.shadowBiasMin > opts.shadowBiasMax) {
        opts.shadowBiasMax = opts.shadowBiasMin;
      }
    }
    if (floatSlider("Bias max", &opts.shadowBiasMax, 0.0001, 1.0, "%.04f",
                    Scale::LOG)) {
      if (opts.shadowBiasMax < opts.shadowBiasMin) {
        opts.shadowBiasMin = opts.shadowBiasMax;
      }
    }

    ImGui::EndDisabled();

    ImGui::Separator();
    ImGui::Text("Environment");

    ImGui::BeginDisabled(opts.lightingModel == LightingModel::BLINN_PHONG);
    ImGui::Checkbox("Use irradiance map", &opts.useIrradianceMap);
    ImGui::EndDisabled();

    ImGui::BeginDisabled(opts.lightingModel != LightingModel::BLINN_PHONG &&
                         opts.useIrradianceMap);
    ImGui::ColorEdit3("Ambient color",
                      reinterpret_cast<float*>(&opts.ambientColor),
                      ImGuiColorEditFlags_Float | ImGuiColorEditFlags_HDR);
    ImGui::SameLine();
    helpMarker("The color of the fixed ambient component.");
    ImGui::EndDisabled();

    ImGui::Checkbox("SSAO", &opts.ssao);
    ImGui::BeginDisabled(!opts.ssao);
    floatSlider("SSAO radius", &opts.ssaoRadius, 0.01, 5.0, "%.04f",
                Scale::LOG);
    floatSlider("SSAO bias", &opts.ssaoBias, 0.0001, 1.0, "%.04f", Scale::LOG);
    ImGui::EndDisabled();

    ImGui::BeginDisabled(opts.lightingModel != LightingModel::BLINN_PHONG);
    floatSlider("Shininess", &opts.shininess, 1.0f, 1000.0f, nullptr,
                Scale::LOG);
    ImGui::SameLine();
    helpMarker("Shininess of specular highlights. Only applies to Phong.");
    ImGui::EndDisabled();

    floatSlider("Emission intensity", &opts.emissionIntensity, 0.0f, 1000.0f,
                nullptr, Scale::LOG);
    ImGui::DragFloat3("Emission attenuation",
                      reinterpret_cast<float*>(&opts.emissionAttenuation),
                      /*v_speed=*/0.01f, 0.0f, 10.0f);
    ImGui::SameLine();
    helpMarker(
        "Constant, linear, and quadratic attenuation of emission lights.");

    ImGui::Separator();
    ImGui::Text("Post-processing");

    ImGui::Checkbox("Bloom", &opts.bloom);
    ImGui::BeginDisabled(!opts.bloom);
    floatSlider("Bloom mix", &opts.bloomMix, 0.001f, 1.0f, nullptr, Scale::LOG);
    ImGui::EndDisabled();

    ImGui::Combo("Tone mapping", reinterpret_cast<int*>(&opts.toneMapping),
                 "None\0Reinhard\0Reinhard luminance\0ACES (approx)\0AMD\0\0");
    ImGui::Checkbox("Gamma correct", &opts.gammaCorrect);
    ImGui::BeginDisabled(!opts.gammaCorrect);
    floatSlider("Gamma", &opts.gamma, 0.01f, 8.0f, nullptr, Scale::LOG);
    ImGui::EndDisabled();

    ImGui::Checkbox("FXAA", &opts.fxaa);
  }

  if (ImGui::CollapsingHeader("Camera")) {
    ImGui::RadioButton("Fly controls",
                       reinterpret_cast<int*>(&opts.cameraControlType),
                       static_cast<int>(CameraControlType::FLY));
    ImGui::SameLine();
    ImGui::RadioButton("Orbit controls",
                       reinterpret_cast<int*>(&opts.cameraControlType),
                       static_cast<int>(CameraControlType::ORBIT));

    floatSlider("Speed", &opts.speed, 0.1, 50.0);
    floatSlider("Sensitivity", &opts.sensitivity, 0.01, 1.0, nullptr,
                Scale::LOG);
    floatSlider("FoV", &opts.fov, qrk::MIN_FOV, qrk::MAX_FOV, "%.1f°");
    if (floatSlider("Near plane", &opts.near, 0.01, 1000.0, nullptr,
                    Scale::LOG)) {
      if (opts.near > opts.far) {
        opts.far = opts.near;
      }
    }
    if (floatSlider("Far plane", &opts.far, 0.01, 1000.0, nullptr,
                    Scale::LOG)) {
      if (opts.far < opts.near) {
        opts.near = opts.far;
      }
    }

    ImGui::Checkbox("Capture mouse", &opts.captureMouse);
  }

  if (ImGui::CollapsingHeader("Debug")) {
    ImGui::Combo(
        "G-Buffer vis", reinterpret_cast<int*>(&opts.gBufferVis),
        "Disabled\0Positions\0Ambient "
        "occlusion\0Normals\0Roughness\0Albedo\0Metallic\0Emission\0\0");
    ImGui::SameLine();
    helpMarker("What component of the G-Buffer to visualize.");

    ImGui::Checkbox("Wireframe", &opts.wireframe);
    ImGui::Checkbox("Draw vertex normals", &opts.drawNormals);
  }

  if (ImGui::CollapsingHeader("Performance")) {
    char overlay[32];
    sprintf(overlay, "Avg FPS %.02f", opts.avgFPS);
    ImGui::PlotLines("Frame time", opts.frameDeltas, opts.numFrameDeltas,
                     opts.frameDeltasOffset, overlay, 0.0f, 0.03f,
                     ImVec2(0, 80.0f));

    ImGui::Checkbox("Enable VSync", &opts.enableVsync);
  }

  ImGui::End();
  ImGui::Render();
}

/** Loads a model based on command line flag, or a default. */
std::unique_ptr<qrk::Model> loadModelOrDefault() {
  std::string modelPath = absl::GetFlag(FLAGS_model);

  if (!modelPath.empty()) {
    return std::make_unique<qrk::Model>(modelPath.c_str());
  }

  // Default to the gltf DamagedHelmet.
  auto helmet = std::make_unique<qrk::Model>(
      "examples/assets/DamagedHelmet/DamagedHelmet.gltf");
  return helmet;
}

int main(int argc, char** argv) {
  absl::SetProgramUsageMessage(
      "quarkGL model viewer. Usage:\n  model_render --model path/to/model.obj");
  absl::ParseCommandLine(argc, argv);

  qrk::Window win(1920, 1080, "Model Render", /* fullscreen */ false,
                  /* samples */ 0);
  win.setClearColor(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
  win.setEscBehavior(qrk::EscBehavior::UNCAPTURE_MOUSE_OR_CLOSE);

  // Setup Dear ImGui.
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  ImGui_ImplGlfw_InitForOpenGL(win.getGlfwRef(), /*install_callbacks=*/true);
  ImGui_ImplOpenGL3_Init("#version 460 core");

  // == Main setup ==

  // Prepare opts for usage.
  ModelRenderOptions opts;

  // Setup the camera.
  auto camera =
      std::make_shared<qrk::Camera>(/* position */ glm::vec3(0.0f, 0.0f, 3.0f));
  std::shared_ptr<qrk::CameraControls> cameraControls =
      std::make_shared<qrk::OrbitCameraControls>(*camera);
  win.bindCamera(camera);
  win.bindCameraControls(cameraControls);

  // Create light registry and add lights.
  auto lightRegistry = std::make_shared<qrk::LightRegistry>();
  lightRegistry->setViewSource(camera);

  auto directionalLight = std::make_shared<qrk::DirectionalLight>();
  lightRegistry->addLight(directionalLight);

  auto pointLight =
      std::make_shared<qrk::PointLight>(glm::vec3(1.2f, 1.0f, 2.0f));
  pointLight->setSpecular(glm::vec3(0.5f, 0.5f, 0.5f));
  lightRegistry->addLight(pointLight);

  // Create a mesh for the light.
  qrk::SphereMesh lightSphere;
  lightSphere.setModelTransform(
      glm::scale(glm::translate(glm::mat4(1.0f), pointLight->getPosition()),
                 glm::vec3(0.2f)));

  // Set up the main framebuffer that will store intermediate states.
  qrk::Framebuffer mainFb(win.getSize());
  auto mainColorAttachment =
      mainFb.attachTexture(qrk::BufferType::COLOR_HDR_ALPHA);
  mainFb.attachRenderbuffer(qrk::BufferType::DEPTH_AND_STENCIL);

  qrk::Framebuffer finalFb(win.getSize());
  auto finalColorAttachment =
      finalFb.attachTexture(qrk::BufferType::COLOR_ALPHA);

  // Build the G-Buffer and prepare deferred shading.
  qrk::DeferredGeometryPassShader geometryPassShader;
  geometryPassShader.addUniformSource(camera);

  auto gBuffer = std::make_shared<qrk::GBuffer>(win.getSize());
  auto lightingTextureRegistry = std::make_shared<qrk::TextureRegistry>();
  lightingTextureRegistry->addTextureSource(gBuffer);

  qrk::ScreenQuadMesh screenQuad;
  qrk::ScreenShader gBufferVisShader(
      qrk::ShaderPath("model_render/shaders/gbuffer_vis.frag"));

  qrk::ScreenShader lightingPassShader(
      qrk::ShaderPath("model_render/shaders/lighting_pass.frag"));
  lightingPassShader.addUniformSource(camera);
  lightingPassShader.addUniformSource(lightingTextureRegistry);
  lightingPassShader.addUniformSource(lightRegistry);

  // Setup shadow mapping.
  constexpr int SHADOW_MAP_SIZE = 2048;
  auto shadowMap =
      std::make_shared<qrk::ShadowMap>(SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);
  lightingTextureRegistry->addTextureSource(shadowMap);

  qrk::ShadowMapShader shadowShader;
  auto shadowCamera = std::make_shared<qrk::ShadowCamera>(directionalLight);
  shadowShader.addUniformSource(shadowCamera);
  lightingPassShader.addUniformSource(shadowCamera);

  // Setup SSAO.
  qrk::SsaoShader ssaoShader;
  ssaoShader.addUniformSource(camera);

  auto ssaoKernel = std::make_shared<qrk::SsaoKernel>();
  ssaoShader.addUniformSource(ssaoKernel);
  auto ssaoBuffer = std::make_shared<qrk::SsaoBuffer>(win.getSize());

  auto ssaoTextureRegistry = std::make_shared<qrk::TextureRegistry>();
  ssaoTextureRegistry->addTextureSource(gBuffer);
  ssaoTextureRegistry->addTextureSource(ssaoKernel);
  ssaoShader.addUniformSource(ssaoTextureRegistry);

  qrk::SsaoBlurShader ssaoBlurShader;
  auto ssaoBlurredBuffer = std::make_shared<qrk::SsaoBuffer>(win.getSize());
  lightingTextureRegistry->addTextureSource(ssaoBlurredBuffer);

  // Setup post processing.
  auto bloomPass = std::make_shared<qrk::BloomPass>(win.getSize());

  auto postprocessTextureRegistry = std::make_shared<qrk::TextureRegistry>();
  postprocessTextureRegistry->addTextureSource(bloomPass);
  qrk::ScreenShader postprocessShader(
      qrk::ShaderPath("model_render/shaders/post_processing.frag"));
  postprocessShader.addUniformSource(postprocessTextureRegistry);

  qrk::FXAAShader fxaaShader;

  // Setup skybox.
  qrk::SkyboxShader skyboxShader;
  skyboxShader.addUniformSource(camera);

  // Prepare some debug shaders.
  qrk::Shader normalShader(
      qrk::ShaderPath("model_render/shaders/model.vert"),
      qrk::ShaderInline(normalShaderSource),
      qrk::ShaderPath("model_render/shaders/model_normals.geom"));
  normalShader.addUniformSource(camera);

  qrk::Shader lampShader(qrk::ShaderPath("model_render/shaders/model.vert"),
                         qrk::ShaderInline(lampShaderSource));
  lampShader.addUniformSource(camera);

  // TODO: Make this more configurable.
  qrk::Texture hdr =
      qrk::Texture::loadHdr("examples/assets/ibl/Alexs_Apt_2k.hdr");
  constexpr int CUBEMAP_SIZE = 1024;
  qrk::EquirectCubemapConverter equirectCubemapConverter(
      CUBEMAP_SIZE, CUBEMAP_SIZE, /*generateMips=*/true);
  {
    qrk::DebugGroup debugGroup("HDR equirect to cubemap");
    equirectCubemapConverter.multipassDraw(hdr);
  }
  auto cubemap = equirectCubemapConverter.getCubemap();

  // Irradiance map averages radiance uniformly so it doesn't have a lot of high
  // frequency details and can thus be small.
  auto irradianceCalculator =
      std::make_shared<qrk::CubemapIrradianceCalculator>(32, 32);
  {
    qrk::DebugGroup debugGroup("Irradiance calculation");
    irradianceCalculator->multipassDraw(cubemap);
  }
  auto irradianceMap = irradianceCalculator->getIrradianceMap();
  lightingTextureRegistry->addTextureSource(irradianceCalculator);

  // Create prefiltered envmap for specular IBL. Similar to the irradiance map,
  // it doesn't have to be super large.
  auto prefilteredEnvMapCalculator =
      std::make_shared<qrk::GGXPrefilteredEnvMapCalculator>(512, 512);
  {
    qrk::DebugGroup debugGroup("Prefiltered env map calculation");
    prefilteredEnvMapCalculator->multipassDraw(cubemap);
  }
  auto prefilteredEnvMap = prefilteredEnvMapCalculator->getPrefilteredEnvMap();
  cubemap.setSamplerMipRange(0, 0);
  qrk::SkyboxMesh skybox(cubemap);

  // Load primary model.
  std::unique_ptr<qrk::Model> model = loadModelOrDefault();

  win.enableFaceCull();
  win.loop([&](float deltaTime) {
    // ImGui logic.
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    win.setMouseInputPaused(io.WantCaptureMouse);
    win.setKeyInputPaused(io.WantCaptureKeyboard);

    ModelRenderOptions prevOpts = opts;

    // Initialize with certain current options.
    opts.speed = cameraControls->getSpeed();
    opts.sensitivity = cameraControls->getSensitivity();
    opts.fov = camera->getFov();
    opts.near = camera->getNearPlane();
    opts.far = camera->getFarPlane();

    opts.frameDeltas = win.getFrameDeltas();
    opts.numFrameDeltas = win.getNumFrameDeltas();
    opts.frameDeltasOffset = win.getFrameDeltasOffset();
    opts.avgFPS = win.getAvgFPS();

    // Render UI.
    renderImGuiUI(opts, {.camera = *camera, .shadowMap = *shadowMap});

    // Post-process options. Some option values are used later during rendering.
    model->setModelTransform(glm::mat4_cast(opts.modelRotation));

    directionalLight->setDiffuse(opts.directionalDiffuse *
                                 opts.directionalIntensity);
    directionalLight->setSpecular(opts.directionalSpecular *
                                  opts.directionalIntensity);
    directionalLight->setDirection(opts.directionalDirection);

    cameraControls->setSpeed(opts.speed);
    cameraControls->setSensitivity(opts.sensitivity);
    camera->setFov(opts.fov);
    camera->setNearPlane(opts.near);
    camera->setFarPlane(opts.far);

    if (opts.cameraControlType != prevOpts.cameraControlType) {
      std::shared_ptr<qrk::CameraControls> newControls;
      switch (opts.cameraControlType) {
        case CameraControlType::FLY:
          newControls = std::make_shared<qrk::FlyCameraControls>();
          break;
        case CameraControlType::ORBIT:
          newControls = std::make_shared<qrk::OrbitCameraControls>(*camera);
          break;
      }
      newControls->setSpeed(cameraControls->getSpeed());
      newControls->setSensitivity(cameraControls->getSensitivity());
      cameraControls = newControls;
      win.bindCameraControls(cameraControls);
    }
    if (opts.enableVsync != prevOpts.enableVsync) {
      if (opts.enableVsync) {
        win.enableVsync();
      } else {
        win.disableVsync();
      }
    }

    win.setMouseButtonBehavior(opts.captureMouse
                                   ? qrk::MouseButtonBehavior::CAPTURE_MOUSE
                                   : qrk::MouseButtonBehavior::NONE);

    // == Main render path ==
    // Step 0: optional shadow pass.
    if (opts.shadowMapping) {
      qrk::DebugGroup debugGroup("Directional shadow map");
      shadowCamera->setCuboidExtents(opts.shadowCameraCuboidExtents);
      shadowCamera->setNearPlane(opts.shadowCameraNear);
      shadowCamera->setFarPlane(opts.shadowCameraFar);
      shadowCamera->setDistanceFromOrigin(opts.shadowCameraDistance);

      shadowMap->activate();
      shadowMap->clear();
      shadowShader.updateUniforms();
      model->draw(shadowShader);
      shadowMap->deactivate();
    }

    // Step 1: geometry pass. Build the G-Buffer.
    {
      qrk::DebugGroup debugGroup("Geometry pass");
      gBuffer->activate();
      gBuffer->clear();

      geometryPassShader.updateUniforms();

      // Draw model.
      if (opts.wireframe) {
        win.enableWireframe();
      }
      model->draw(geometryPassShader);
      if (opts.wireframe) {
        win.disableWireframe();
      }

      gBuffer->deactivate();
    }

    if (opts.gBufferVis != GBufferVis::DISABLED) {
      {
        qrk::DebugGroup debugGroup("G-Buffer vis");
        switch (opts.gBufferVis) {
          case GBufferVis::POSITIONS:
          case GBufferVis::AO:
            screenQuad.setTexture(gBuffer->getPositionAOTexture());
            break;
          case GBufferVis::NORMALS:
          case GBufferVis::ROUGHNESS:
            screenQuad.setTexture(gBuffer->getNormalRoughnessTexture());
            break;
          case GBufferVis::ALBEDO:
          case GBufferVis::METALLIC:
            screenQuad.setTexture(gBuffer->getAlbedoMetallicTexture());
            break;
          case GBufferVis::EMISSION:
            screenQuad.setTexture(gBuffer->getEmissionTexture());
            break;
          case GBufferVis::DISABLED:
            break;
        };
        gBufferVisShader.setInt("gBufferVis",
                                static_cast<int>(opts.gBufferVis));
        screenQuad.draw(gBufferVisShader);
      }

      // TODO: Refactor avoid needing to copy this.
      {
        qrk::DebugGroup debugGroup("Imgui pass");
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
      }
      return;
    }

    // Step 1.2: optional SSAO pass.
    // TODO: Extract into an "SSAO pass".
    if (opts.ssao) {
      qrk::DebugGroup debugGroup("SSAO pass");
      ssaoKernel->setRadius(opts.ssaoRadius);
      ssaoKernel->setBias(opts.ssaoBias);

      ssaoBuffer->activate();
      ssaoBuffer->clear();

      ssaoShader.updateUniforms();

      screenQuad.unsetTexture();
      screenQuad.draw(ssaoShader, ssaoTextureRegistry.get());

      ssaoBuffer->deactivate();

      // Step 1.2.1: SSAO blur.
      ssaoBlurredBuffer->activate();
      ssaoBlurredBuffer->clear();

      ssaoBlurShader.configureWith(*ssaoKernel, *ssaoBuffer);
      screenQuad.draw(ssaoBlurShader);

      ssaoBlurredBuffer->deactivate();
    }

    // Step 2: lighting pass. Draw to the main framebuffer.
    {
      qrk::DebugGroup debugGroup("Deferred lighting pass");
      mainFb.activate();
      mainFb.clear();

      // TODO: Set up environment mapping with the skybox.
      lightingPassShader.updateUniforms();
      lightingPassShader.setBool("shadowMapping", opts.shadowMapping);
      lightingPassShader.setFloat("shadowBiasMin", opts.shadowBiasMin);
      lightingPassShader.setFloat("shadowBiasMax", opts.shadowBiasMax);
      lightingPassShader.setBool("useIrradianceMap", opts.useIrradianceMap);
      lightingPassShader.setBool("ssao", opts.ssao);
      lightingPassShader.setInt("lightingModel",
                                static_cast<int>(opts.lightingModel));
      // TODO: Pull this out into a material class.
      lightingPassShader.setVec3("ambient", opts.ambientColor);
      lightingPassShader.setFloat("shininess", opts.shininess);
      lightingPassShader.setFloat("emissionIntensity", opts.emissionIntensity);
      lightingPassShader.setFloat("emissionAttenuation.constant",
                                  opts.emissionAttenuation.x);
      lightingPassShader.setFloat("emissionAttenuation.linear",
                                  opts.emissionAttenuation.y);
      lightingPassShader.setFloat("emissionAttenuation.quadratic",
                                  opts.emissionAttenuation.z);

      screenQuad.unsetTexture();
      screenQuad.draw(lightingPassShader, lightingTextureRegistry.get());

      mainFb.deactivate();
    }

    // Step 3: forward render anything else on top.
    {
      qrk::DebugGroup debugGroup("Forward pass");

      // Before we do so, we have to blit the depth buffer.
      gBuffer->blit(mainFb, GL_DEPTH_BUFFER_BIT);

      mainFb.activate();

      if (opts.drawNormals) {
        // Draw the normals.
        normalShader.updateUniforms();
        model->draw(normalShader);
      }

      // Draw light source.
      lampShader.updateUniforms();
      if (opts.wireframe) {
        win.enableWireframe();
      }
      lightSphere.draw(lampShader);
      if (opts.wireframe) {
        win.disableWireframe();
      }

      // Draw skybox.
      skyboxShader.updateUniforms();
      skybox.draw(skyboxShader);

      mainFb.deactivate();
    }

    // Step 4: post processing.
    if (opts.bloom) {
      qrk::DebugGroup debugGroup("Bloom pass");
      bloomPass->multipassDraw(/*sourceFb=*/mainFb);
    }

    {
      qrk::DebugGroup debugGroup("Tonemap & gamma");
      finalFb.activate();
      finalFb.clear();

      // Draw to the final FB using the post process shader.
      postprocessShader.updateUniforms();
      postprocessShader.setBool("bloom", opts.bloom);
      postprocessShader.setFloat("bloomMix", opts.bloomMix);
      postprocessShader.setInt("toneMapping",
                               static_cast<int>(opts.toneMapping));
      postprocessShader.setBool("gammaCorrect", opts.gammaCorrect);
      postprocessShader.setFloat("gamma", static_cast<int>(opts.gamma));
      screenQuad.setTexture(mainColorAttachment);
      screenQuad.draw(postprocessShader, postprocessTextureRegistry.get());

      finalFb.deactivate();
    }

    win.setViewport();

    // Finally draw to the screen via the FXAA shader.
    if (opts.fxaa) {
      qrk::DebugGroup debugGroup("FXAA");
      screenQuad.setTexture(finalColorAttachment);
      screenQuad.draw(fxaaShader);
    } else {
      finalFb.blitToDefault(GL_COLOR_BUFFER_BIT);
    }

    // == End render path ==

    // Finally, draw ImGui data.
    {
      qrk::DebugGroup debugGroup("Imgui pass");
      ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
  });

  // Cleanup.
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  return 0;
}
