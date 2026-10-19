#version 330 core
// Skybox con neblina: el cielo se funde con el color de la niebla cerca del horizonte.
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;
uniform vec3  fogColor;        // mismo color que en los modelos, p. ej. (0.29, 0.26, 0.35)
uniform float fogHorizon;      // que tan alto sube la niebla en el cielo (0.0 - 0.5), p. ej. 0.15

void main()
{
    vec3 dir = normalize(TexCoords);
    vec3 cielo = texture(skybox, dir).rgb;
    // dir.y = 0 en el horizonte, 1 arriba
    float f = 1.0 - smoothstep(-0.05, fogHorizon, dir.y);
    FragColor = vec4(mix(cielo, fogColor, f * 0.85), 1.0);
}
