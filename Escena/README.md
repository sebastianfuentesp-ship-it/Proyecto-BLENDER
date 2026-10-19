# Escena del panteón

Esta carpeta define cómo se acomodan los modelos en la escena final. Los modelos siguen en archivos separados: aquí solo están las posiciones.

| Archivo | Para qué sirve |
|---|---|
| `escena_layout.txt` | Lista de objetos para OpenGL: `modelo x y z rotY escala` (Y arriba, en metros). |
| `escena_preview.blend` | Vista previa en Blender: enlaza los `.blend` de `Modelos/` en las mismas posiciones, con cielo, niebla y veladoras. No se exporta. |
| `cielo_equirect.png` | El mismo cielo del skybox, en formato panorámico, para la vista previa en Blender. |
| `preview_*.png` | Renders de la vista previa. |

## Qué hay en la escena

- El escenario con la reja y la capilla al fondo del camino.
- Doce tumbas (el ángel del dolor repetido, con tamaño y giro variados), con calaveritas, portarretratos y veladoras al frente.
- La ofrenda principal (platillos, portarretratos y calaveritas) frente a la primera tumba de la derecha.
- Un camino de cempasúchil: pétalos sobre el camino (`petalos_camino`) y dos hileras de flores a las orillas, desde afuera de la reja hasta la capilla.
- Flores regadas por todo el panteón y al pie de cada tumba, dos catrinas y árboles adentro y afuera de la barda.

En total son unas 435 instancias, de las cuales 374 son flores. Cada modelo se carga **una sola vez** y se dibuja varias veces, cambiando solo la matriz `model`. Si hace falta más rendimiento, usa *instancing* (`glDrawElementsInstanced`) para las flores.

## Cargar el layout (C++)

```cpp
struct Instancia { std::string modelo; glm::vec3 pos; float rotY; float escala; };

std::vector<Instancia> cargaLayout(const std::string& ruta) {
    std::vector<Instancia> v; std::ifstream f(ruta); std::string linea;
    while (std::getline(f, linea)) {
        if (linea.empty() || linea[0] == '#') continue;
        std::istringstream ss(linea); Instancia i;
        ss >> i.modelo >> i.pos.x >> i.pos.y >> i.pos.z >> i.rotY >> i.escala;
        v.push_back(i);
    }
    return v;
}

// al dibujar:
for (auto& i : layout) {
    glm::mat4 model(1.0f);
    model = glm::translate(model, i.pos);
    model = glm::rotate(model, glm::radians(i.rotY), glm::vec3(0, 1, 0));
    model = glm::scale(model, glm::vec3(i.escala));
    shader.setMat4("model", model);
    modelos[i.modelo].Draw(shader);   // std::map<std::string, Model>
}
```

Los nombres de `modelo` corresponden a los archivos que exporten de Blender. Por ejemplo, `flor_cempasuchil` corresponde a `Modelos/Yasser/flor_cempasuchil.blend`, exportado como `flor_cempasuchil.obj`.

> Al exportar a OBJ/FBX desde Blender usa **Forward: -Z, Up: Y** (el valor por defecto). Así las coordenadas coinciden con las del layout.

## Skybox

Las 6 caras están en `Texturas/skybox/` en el orden que espera `GL_TEXTURE_CUBE_MAP_POSITIVE_X + i`:

```cpp
std::vector<std::string> caras = {
    "Texturas/skybox/right.png", "Texturas/skybox/left.png",
    "Texturas/skybox/top.png",   "Texturas/skybox/bottom.png",
    "Texturas/skybox/front.png", "Texturas/skybox/back.png"
};
// cargar cada una con glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, ...) sin voltearlas (stbi_set_flip_vertically_on_load(false))
// filtros: GL_LINEAR; wrap S/T/R: GL_CLAMP_TO_EDGE
```

Dibuja el skybox al final con `glDepthFunc(GL_LEQUAL)` y los shaders `Shaders/skybox.vs` / `skybox.fs`, pasando `view` sin traslación: `glm::mat4(glm::mat3(view))`.

## Niebla

La niebla está en `Shaders/niebla.glsl`: depende de la distancia y es más espesa cerca del piso. `Shaders/modelo.fs` ya la trae, junto con una luz de luna Phong. Usa los mismos valores en los modelos y en el skybox para que el horizonte se funda:

| Uniform | Valor sugerido |
|---|---|
| `fogColor` | `(0.29, 0.26, 0.35)` |
| `fogDensity` | `0.035` (suave) a `0.06` (espesa) |
| `fogHeight` | `2.0` |
| `fogHorizon` (skybox) | `0.15` |

Para regenerar el layout o el cielo se usaron scripts de Python con Blender 5.2 (`bpy`); si quieren mover cosas, también pueden editar `escena_layout.txt` a mano.
