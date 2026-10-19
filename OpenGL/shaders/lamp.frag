#version 330 core
// Flama de las veladoras: color solido que cambia con el parpadeo de la luz
out vec4 FragColor;

uniform vec3 lampColor;

void main()
{
     FragColor = vec4(lampColor, 1.0f);
}
