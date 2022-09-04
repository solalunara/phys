const char *g_pszFragShaderSource = "\
#version 330 \n\
in vec2 TexCoord; \
in vec3 TextColor; \
out vec4 FragColor; \
uniform sampler2D TextureToRender; \
void main() \
{ \
    FragColor = texture( TextureToRender, TexCoord ); \
    if ( FragColor.a < .1f ) discard; \
} \
";
