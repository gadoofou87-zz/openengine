R""(
#version 330 core
in vec2 frag_uv1;
in vec2 frag_uv2;
out vec4 color;
uniform sampler2D _MainTex;
uniform sampler2D _LightmapTex;
void main()
{
    color = texture(_MainTex, frag_uv1) + texture(_LightmapTex, frag_uv2);
}
)""
