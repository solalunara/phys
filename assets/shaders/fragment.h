const char *g_pszFragShaderSource = "\
#version 330 \n\
in vec2 TexCoord; \
out vec4 FragColor; \
uniform sampler2D TextureToRender; \
void main() \
{ \
    FragColor = texture( TextureToRender, TexCoord ); \
} \
";

const char *g_pszTextFragShaderSource = "\
#version 330 \n\
in vec2 TexCoord; \
in vec3 TextColor; \
out vec4 FragColor; \
uniform sampler2D TextureToRender; \
void main() \
{ \
    vec4 sampled = vec4( 1.0, 1.0, 1.0, texture( TextureToRender, TexCoord ).r ); \
    FragColor = vec4( TextColor, 1.0 ) * sampled; \
    if ( FragColor.a < .1f ) discard; \
} \
";
