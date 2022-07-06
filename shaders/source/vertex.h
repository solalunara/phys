const char *g_pszVertexShaderSource = "\
#version 330 \
layout (location = 0) in vec3 pos; \
layout (location = 1) in vec2 InTexCoord; \
out vec2 TexCoord; \
uniform mat4 Transform; \
uniform mat4 CameraTransform; \
uniform mat4 Perspective; \
void main() \
{ \
    gl_Position = Perspective * CameraTransform * transform * vec4( pos, 1.0 ); \
    TexCoord = InTexCoord; \
} \
";