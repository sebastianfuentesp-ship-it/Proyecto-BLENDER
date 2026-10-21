#version 330 core
// Igual que lighting.vs, pero con huesos: cada vertice se mueve con la suma de hasta 4 huesos por su peso.
// piel = 0: malla sin esqueleto (animacion por objetos), solo se usa la matriz model.
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;
layout (location = 3) in ivec4 huesoIds;
layout (location = 4) in vec4 pesos;

const int MAX_HUESOS = 40;

out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 huesos[MAX_HUESOS];
uniform int piel;
uniform float viento;   // 0 = sin viento; papel picado: las tiras se mecen bajo el cordel
uniform float tiempo;
uniform float fase;

void main()
{
    vec3 p = position;
    if (viento > 0.0)
    {
        // entre mas abajo del cordel y mas lejos de los postes, mas se mueve la tira
        float bajo = clamp(3.1 - p.y, 0.0, 0.8);
        float centro = clamp(1.0 - (p.x / 2.45) * (p.x / 2.45), 0.0, 1.0);
        float ola = sin(tiempo * 3.2 + p.x * 1.8 + fase) + 0.4 * sin(tiempo * 7.1 + p.x * 4.0 + fase);
        p.z += viento * ola * (0.5 * bajo + 0.06) * centro;
        p.y += viento * 0.04 * abs(ola) * bajo * centro;
    }
    mat4 m = model;
    if (piel == 1)
    {
        mat4 s = huesos[huesoIds.x] * pesos.x + huesos[huesoIds.y] * pesos.y + huesos[huesoIds.z] * pesos.z + huesos[huesoIds.w] * pesos.w;
        m = model * s;
    }
    gl_Position = projection * view * m * vec4(p, 1.0f);
    FragPos = vec3(m * vec4(p, 1.0f));
    Normal = mat3(transpose(inverse(m))) * normal;
    TexCoords = texCoords;
}
