R""(
#version 330 core
layout (location = 0) in vec3 vs_position;
layout (location = 1) in vec2 vs_uv1;
layout (location = 2) in vec2 vs_uv2;
out vec2 frag_uv1;
out vec2 frag_uv2;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
void main()
{
    gl_Position = projection * view * model * vec4(vs_position, 1.f);
    frag_uv1 = vs_uv1;
    frag_uv2 = vs_uv2;
}
)""
