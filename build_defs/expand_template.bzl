def expand_template_impl(ctx):
  ctx.actions.expand_template(
      template = ctx.file.template,
      output = ctx.outputs.out,
      substitutions = ctx.attr.substitutions,
  )

expand_template = rule(
    attrs = {
        "template": attr.label(
            mandatory = True,
            allow_single_file = True,
        ),
        "substitutions": attr.string_dict(mandatory = True),
        "out": attr.output(mandatory = True),
    },
    output_to_genfiles = True,
    implementation = expand_template_impl,
)
