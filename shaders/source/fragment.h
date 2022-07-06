const char *g_pszFragShaderSource = "\
#version 330 \
in vec2 TexCoord; \
out vec4 FragColor; \
uniform sampler2D TextureToRender; \
void main() \
{ \
    FragColor = texture( TextureToRender, TexCoord ); \
} \
";