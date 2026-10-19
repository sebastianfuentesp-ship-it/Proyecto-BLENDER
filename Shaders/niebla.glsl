// Niebla para los fragment shaders de los modelos.
// Copia estas lineas en tu fragment shader (GLSL no tiene #include) y llama aplicaNiebla()
// al final, con la posicion del fragmento en espacio del mundo.
//
//   uniform vec3  fogColor;    // (0.29, 0.26, 0.35)  morado grisaceo de noche
//   uniform float fogDensity;  // 0.035 suave, 0.06 espesa
//   uniform float fogHeight;   // 2.0  la niebla se concentra cerca del piso
//   uniform vec3  viewPos;     // posicion de la camara

vec3 aplicaNiebla(vec3 color, vec3 fragPos)
{
    float dist = length(viewPos - fragPos);
    // niebla exponencial al cuadrado por distancia...
    float fDist = 1.0 - exp(-pow(fogDensity * dist, 2.0));
    // ...mas espesa a ras de suelo (niebla de panteon)
    float fAltura = exp(-max(fragPos.y, 0.0) / fogHeight);
    float f = clamp(fDist * (0.55 + 0.45 * fAltura), 0.0, 1.0);
    return mix(color, fogColor, f);
}
