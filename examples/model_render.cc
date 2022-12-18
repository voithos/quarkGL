// clang-format off
// Must precede glfw/glad, to include OpenGL functions.
#include <qrk/quarkgl.h>
// clang-format on

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/flags/usage.h"
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

struct ModelRenderOptions {
  bool useVertexNormals = false;
  bool drawNormals = false;
  bool captureMouse = false;
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

// Called during game loop.
void renderImGuiUI(ModelRenderOptions& opts) {
  // ImGui::ShowDemoWindow();

  ImGui::Begin("Model Render");

  if (ImGui::CollapsingHeader("Rendering")) {
    ImGui::Checkbox("Vertex normals", &opts.useVertexNormals);
    ImGui::SameLine();
    helpMarker(
        "Whether to use vertex normals for rendering. If false, a normal map "
        "will be used if available.");
  }

  if (ImGui::CollapsingHeader("Interaction")) {
    ImGui::Checkbox("Capture mouse", &opts.captureMouse);
  }

  if (ImGui::CollapsingHeader("Debug")) {
    ImGui::Checkbox("Draw vertex normals", &opts.drawNormals);
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
                  /* samples */ 4);
  win.setClearColor(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
  win.setEscBehavior(qrk::EscBehavior::UNCAPTURE_MOUSE_OR_CLOSE);

  // Setup Dear ImGui.
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  ImGui_ImplGlfw_InitForOpenGL(win.getGlfwRef(), /*install_callbacks=*/true);
  ImGui_ImplOpenGL3_Init("#version 460 core");

  auto camera =
      std::make_shared<qrk::Camera>(/* position */ glm::vec3(0.0f, 0.0f, 3.0f));
  auto cameraControls = std::make_shared<qrk::OrbitCameraControls>(*camera);
  win.bindCamera(camera);
  win.bindCameraControls(cameraControls);

  qrk::Shader mainShader(qrk::ShaderPath("examples/shaders/model_render.vert"),
                         qrk::ShaderPath("examples/shaders/model_render.frag"));
  mainShader.addUniformSource(camera);

  // TODO: Pull this out into a material class.
  mainShader.setVec3("material.ambient", glm::vec3(0.1f));
  mainShader.setFloat("material.shininess", 32.0f);
  mainShader.setFloat("material.emissionAttenuation.constant", 1.0f);
  mainShader.setFloat("material.emissionAttenuation.linear", 0.09f);
  mainShader.setFloat("material.emissionAttenuation.quadratic", 0.032f);

  qrk::Shader normalShader(
      qrk::ShaderPath("examples/shaders/model.vert"),
      qrk::ShaderInline(normalShaderSource),
      qrk::ShaderPath("examples/shaders/model_normals.geom"));
  normalShader.addUniformSource(camera);

  qrk::SkyboxShader skyboxShader;
  skyboxShader.addUniformSource(camera);

  qrk::SkyboxMesh skybox({
      "examples/assets/skybox/right.jpg",
      "examples/assets/skybox/left.jpg",
      "examples/assets/skybox/top.jpg",
      "examples/assets/skybox/bottom.jpg",
      "examples/assets/skybox/front.jpg",
      "examples/assets/skybox/back.jpg",
  });

  // Create light registry and add lights.
  auto registry = std::make_shared<qrk::LightRegistry>();
  registry->setViewSource(camera);
  mainShader.addUniformSource(registry);

  auto directionalLight =
      std::make_shared<qrk::DirectionalLight>(glm::vec3(-0.2f, -1.0f, -0.3f));
  directionalLight->setSpecular(glm::vec3(0.5f, 0.5f, 0.5f));
  registry->addLight(directionalLight);

  auto pointLight =
      std::make_shared<qrk::PointLight>(glm::vec3(1.2f, 1.0f, 2.0f));
  pointLight->setSpecular(glm::vec3(0.5f, 0.5f, 0.5f));
  registry->addLight(pointLight);

  qrk::Shader lampShader(qrk::ShaderPath("examples/shaders/model.vert"),
                         qrk::ShaderInline(lampShaderSource));
  lampShader.addUniformSource(camera);

  // Create a mesh for the light.
  // TODO: Make this into a sphere.
  qrk::CubeMesh lightCube;
  lightCube.setModelTransform(glm::scale(
      glm::translate(glm::mat4(), pointLight->getPosition()), glm::vec3(0.2f)));

  // Load model.
  std::unique_ptr<qrk::Model> model = loadModelOrDefault();

  // Prepare opts for usage.
  ModelRenderOptions opts;

  win.enableFaceCull();
  win.loop([&](float deltaTime) {
    // ImGui logic.
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    win.setMouseInputPaused(io.WantCaptureMouse);
    win.setKeyInputPaused(io.WantCaptureKeyboard);

    renderImGuiUI(opts);

    // Post-process options. Some option values are used later during rendering.
    win.setMouseButtonBehavior(opts.captureMouse
                                   ? qrk::MouseButtonBehavior::CAPTURE_MOUSE
                                   : qrk::MouseButtonBehavior::NONE);

    // == Main render path ==
    // Draw main models.
    // TODO: Set up environment mapping with the skybox.
    mainShader.updateUniforms();
    mainShader.setBool("useVertexNormals", opts.useVertexNormals);
    model->draw(mainShader);

    if (opts.drawNormals) {
      // Draw the normals.
      normalShader.updateUniforms();
      model->draw(normalShader);
    }

    // Draw light source.
    lampShader.updateUniforms();
    lightCube.draw(lampShader);

    // Draw skybox.
    skyboxShader.updateUniforms();
    skybox.draw(skyboxShader);

    // == End render path ==

    // Finally, draw ImGui data.
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  });

  // Cleanup.
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  return 0;
}
