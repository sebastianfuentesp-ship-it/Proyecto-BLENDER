#version 330 core
// Fragment shader de ejemplo: textura + Phong con luz de luna + niebla.
// Sirve de base para la entrega de iluminacion (agregar veladoras, Fresnel, etc.).
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform sampler2D texture_diffuse1;
uniform vec3 viewPos;
uniform vec3 lightDir;     // direccion de la luna, p. ej. (-0.3, -0.5, -0.8)
uniform vec3 lightColor;   // (0.55, 0.6, 0.8)
uniform vec3 ambient;      // (0.12, 0.1, 0.16)

uniform vec3  fogColor;
uniform float fogDensity;
uniform float fogHeight;

vec3 aplicaNiebla(vec3 color, vec3 fragPos)
{
    float dist = length(viewPos - fragPos);
    float fDist = 1.0 - exp(-pow(fogDensity * dist, 2.0));
    float fAltura = exp(-max(fragPos.y, 0.0) / fogHeight);
    float f = clamp(fDist * (0.55 + 0.45 * fAltura), 0.0, 1.0);
    return mix(color, fogColor, f);
}

void main()
{
    vec3 albedo = texture(texture_diffuse1, TexCoords).rgb;
    vec3 N = normalize(Normal);
    vec3 L = normalize(-lightDir);
    vec3 V = normalize(viewPos - FragPos);
    vec3 Hh = normalize(L + V);
    float diff = max(dot(N, L), 0.0);
    float spec = pow(max(dot(N, Hh), 0.0), 32.0) * 0.2;
    vec3 color = albedo * (ambient + diff * lightColor) + spec * lightColor;
    FragColor = vec4(aplicaNiebla(color, FragPos), 1.0);
}
