# Escena del panteón

Esta carpeta define cómo se acomodan los modelos en la escena final. Los modelos siguen en archivos separados: aquí solo están las posiciones.

| Archivo | Para qué sirve |
|---|---|
| `escena_layout.txt` | Lista de objetos para OpenGL: `modelo x y z rotY escala [r g b]` (Y arriba, en metros; el color es opcional y tiñe la textura). |
| `escena_preview.blend` | Vista previa en Blender: enlaza los `.blend` de `Modelos/` en las mismas posiciones, con cielo y niebla. No se exporta. |
| `piezas/papel_picado_tira.blend` | Tira de papel picado (dos postes, cordel y banderitas) armada con las texturas de `Decoracion.blend`. |
| `cielo_equirect.png` | El mismo cielo del skybox, en formato panorámico, para la vista previa en Blender. |
| `preview_*.png` | Capturas de la escena en OpenGL. |

## Qué hay en la escena

- El escenario con la reja y la capilla al fondo del camino.
- Dos terrazas de tierra a cada lado del camino (a 0.45 m y 0.9 m), con muros de piedra y escaleras, como los panteones en ladera de México.
- 40 tumbas de cinco tipos en filas sobre las terrazas, mirando al camino: 15 de cantera (modelo de Sebastian), 7 de azulejo, 2 de tierra con cruz de madera, 4 mausoleos familiares y 12 cruces de piedra, además de 4 ángeles del dolor. Las tumbas están pintadas de colores distintos (azul, rosa mexicano, verde, amarillo, cal blanca) y algunas se ven viejas.
- Cada tumba es de una "familia" que la arregla distinto: muy arreglada (guirnalda, florero de talavera, corona de flores en tripié, angelito, tapete de flores, arco de cempasúchil, muchas velas, comida y retratos), media, sencilla o casi abandonada (una vela y pocas flores).
- 126 personas: familias paradas o hincadas frente a sus tumbas, gente viendo el altar y caminando en grupitos por el camino y los pasillos (8 modelos distintos, repetidos con un tinte de ropa diferente).
- El altar de muertos al final del camino, frente a la capilla (`ofrenda`, al estilo de una ofrenda tradicional): tres niveles con manteles rosas y papel picado, cajas de madera a los lados, banca con los platillos, huacal con naranjas, macetas de cempasúchil, floreros con flores rosas, calabazas, jarritos, caña, pan de muerto, calaveritas, retratos, velas y copal, con pétalos regados y un camino de flores naranjas y rosas en el piso.
- Un camino de cempasúchil denso: pétalos sobre el camino (`petalos_camino`), tres hileras de flores a cada orilla y velas. Tres tapetes de flores con diseños distintos sobre el camino (sol, corazón y franjas).
- 21 tiras de papel picado: sobre el camino cada 1.2 m, en la entrada, detrás del altar y en los pasillos entre tumbas.
- Guirnaldas en las tumbas, el altar, la fachada de la capilla y a lo largo de los muros de las terrazas.
- 459 velas en todo el panteón (también en las orillas de las terrazas), 12 angelitos, una catrina gigante (unos 6 m) junto a la entrada, otra junto al altar y 84 árboles alrededor de la barda.

En total son unas 2,000 instancias, de las cuales unas 1,050 son flores y 459 velas. Para que el camino no pese más, las flores de las orillas usan `flor_baja` (la flor con *Decimate*, 1,209 triángulos) y las guirnaldas de los muros usan `guirnalda_baja`. Cada modelo se carga **una sola vez** y se dibuja varias veces, cambiando solo la matriz `model`. Si hace falta más rendimiento, usa *instancing* (`glDrawElementsInstanced`) para las flores.

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
