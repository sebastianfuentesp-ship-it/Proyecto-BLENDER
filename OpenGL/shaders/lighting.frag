#version 330 core

// Basado en el lighting.frag del laboratorio (Phong con luz direccional, puntuales y spotlight).
// Se agrega: 12 veladoras como luces puntuales, termino de Fresnel (luz de borde de la luna),
// caras dobles para flores y petalos, y niebla de panteon.

#define NUMBER_OF_POINT_LIGHTS 12

struct Material
{
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct DirLight
{
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight
{
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight
{
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 color;

uniform vec3 viewPos;
uniform DirLight dirLight;
uniform PointLight pointLights[NUMBER_OF_POINT_LIGHTS];
uniform SpotLight spotLight;
uniform Material material;

// Fresnel: brillo azulado de la luna en los bordes de los objetos
uniform vec3  fresnelColor;     // color del borde
uniform float fresnelStrength;  // 0 = apagado
uniform float fresnelPower;     // que tan delgado es el borde (3 - 5)

// Niebla
uniform int   fogOn;
uniform vec3  fogColor;
uniform float fogDensity;
uniform float fogHeight;

// Function prototypes
vec3 CalcDirLight( DirLight light, vec3 normal, vec3 viewDir, vec3 albedo );
vec3 CalcPointLight( PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 albedo );
vec3 CalcSpotLight( SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 albedo );
vec3 aplicaNiebla( vec3 c, vec3 fragPos );

void main( )
{
    // Properties
    vec3 norm = normalize( Normal );
    if ( !gl_FrontFacing )
    {
        norm = -norm;   // flores y petalos se ven por los dos lados
    }
    vec3 viewDir = normalize( viewPos - FragPos );
    vec3 albedo = vec3( texture( material.diffuse, TexCoords ) );

    // Directional lighting (luna)
    vec3 result = CalcDirLight( dirLight, norm, viewDir, albedo );

    // Point lights (veladoras)
    for ( int i = 0; i < NUMBER_OF_POINT_LIGHTS; i++ )
    {
        result += CalcPointLight( pointLights[i], norm, FragPos, viewDir, albedo );
    }

    // Spot light (linterna)
    result += CalcSpotLight( spotLight, norm, FragPos, viewDir, albedo );

    // Fresnel (aproximacion de Schlick): mas brillo cuando la superficie se ve de canto
    float cosTheta = max( dot( norm, viewDir ), 0.0 );
    float fresnel = pow( 1.0 - cosTheta, fresnelPower );
    result += fresnelStrength * fresnel * fresnelColor * ( 0.4 + 0.6 * albedo );

    if ( fogOn == 1 )
    {
        result = aplicaNiebla( result, FragPos );
    }

    color = vec4( result, 1.0 );
}

// Calculates the color when using a directional light.
vec3 CalcDirLight( DirLight light, vec3 normal, vec3 viewDir, vec3 albedo )
{
    vec3 lightDir = normalize( -light.direction );

    // Diffuse shading
    float diff = max( dot( normal, lightDir ), 0.0 );

    // Specular shading
    vec3 reflectDir = reflect( -lightDir, normal );
    float spec = pow( max( dot( viewDir, reflectDir ), 0.0 ), material.shininess );

    // Combine results
    vec3 ambient = light.ambient * albedo;
    vec3 diffuse = light.diffuse * diff * albedo;
    vec3 specular = light.specular * spec * vec3( texture( material.specular, TexCoords ) );

    return ( ambient + diffuse + specular );
}

// Calculates the color when using a point light.
vec3 CalcPointLight( PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 albedo )
{
    vec3 lightDir = normalize( light.position - fragPos );

    // Diffuse shading
    float diff = max( dot( normal, lightDir ), 0.0 );

    // Specular shading
    vec3 reflectDir = reflect( -lightDir, normal );
    float spec = pow( max( dot( viewDir, reflectDir ), 0.0 ), material.shininess );

    // Attenuation
    float distance = length( light.position - fragPos );
    float attenuation = 1.0f / ( light.constant + light.linear * distance + light.quadratic * ( distance * distance ) );

    // Combine results
    vec3 ambient = light.ambient * albedo;
    vec3 diffuse = light.diffuse * diff * albedo;
    vec3 specular = light.specular * spec * vec3( texture( material.specular, TexCoords ) );

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return ( ambient + diffuse + specular );
}

// Calculates the color when using a spot light.
vec3 CalcSpotLight( SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 albedo )
{
    vec3 lightDir = normalize( light.position - fragPos );

    // Diffuse shading
    float diff = max( dot( normal, lightDir ), 0.0 );

    // Specular shading
    vec3 reflectDir = reflect( -lightDir, normal );
    float spec = pow( max( dot( viewDir, reflectDir ), 0.0 ), material.shininess );

    // Attenuation
    float distance = length( light.position - fragPos );
    float attenuation = 1.0f / ( light.constant + light.linear * distance + light.quadratic * ( distance * distance ) );

    // Spotlight intensity
    float theta = dot( lightDir, normalize( -light.direction ) );
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp( ( theta - light.outerCutOff ) / epsilon, 0.0, 1.0 );

    // Combine results
    vec3 ambient = light.ambient * albedo;
    vec3 diffuse = light.diffuse * diff * albedo;
    vec3 specular = light.specular * spec * vec3( texture( material.specular, TexCoords ) );

    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;

    return ( ambient + diffuse + specular );
}

// Niebla exponencial al cuadrado por distancia, mas espesa a ras de suelo
vec3 aplicaNiebla( vec3 c, vec3 fragPos )
{
    float dist = length( viewPos - fragPos );
    float fDist = 1.0 - exp( -pow( fogDensity * dist, 2.0 ) );
    float fAltura = exp( -max( fragPos.y, 0.0 ) / fogHeight );
    float f = clamp( fDist * ( 0.55 + 0.45 * fAltura ), 0.0, 1.0 );
    return mix( c, fogColor, f );
}
