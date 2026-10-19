#version 330 core
// Skybox: cubo unitario centrado en la camara.
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;   // pasar mat4(mat3(view)) para quitar la traslacion

void main()
{
    TexCoords = aPos;
    vec4 pos = projection * view * vec4(aPos, 1.0);
    gl_Position = pos.xyww;  // profundidad = 1.0: siempre al fondo (usar glDepthFunc(GL_LEQUAL))
}
