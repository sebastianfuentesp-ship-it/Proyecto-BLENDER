# Ofrenda de Día de Muertos en el cementerio

**Equipo "Los tilines"**
Proyecto final · Computación Gráfica e Interacción Humano-Computadora · Semestre 2027-1 · UNAM, Facultad de Ingeniería

---

## Sobre el proyecto

Este proyecto representa una **ofrenda de Día de Muertos dentro de un cementerio**, una de las tradiciones más representativas de la cultura mexicana. Cada 1 y 2 de noviembre, las familias visitan a sus seres queridos en los panteones, limpian y decoran sus tumbas y colocan ofrendas con flores, velas, comida y fotografías para recibir sus almas.

La escena es un panteón de noche: una reja de entrada, un camino de cempasúchil que lleva a una capilla, tumbas con veladoras, calaveritas y portarretratos, catrinas, árboles y una ofrenda principal, todo bajo un cielo estrellado con luna y niebla.

Este repositorio contiene los **modelos 3D hechos en Blender**, sus texturas, los shaders y el acomodo de la escena que se arma en OpenGL.

## Integrantes

| Rol | Nombre |
|---|---|
| Project Owner | Alejandra Melissa Saucedo Serralde |
| Scrum Master | Yasser Vladimir Cruz Miranda |
| Developer | Sebastian Andre Fuentes Poma |
| Developer | Cristian Fajardo Télles |

## Avance

| Parte | Estado |
|---|---|
| Modelos de Yasser (texturizados) | Listo |
| Modelos de Melissa (altar, vela y copalero) y de Sebastian (tumba, pan de muerto y papel picado) | Listo, texturizados y en la escena |
| Modelos de Cristian | Pendiente de subir |
| Skybox nocturno (`Texturas/skybox/`) | Listo |
| Shaders de skybox, niebla y luz de luna (`Shaders/`) | Listo |
| Acomodo de la escena (`Escena/`) | Listo con los modelos de todo el equipo, terrazas y decoración variada |
| Carga de modelos y texturas en OpenGL (`OpenGL/`) | Listo |
| Iluminación: Phong y Fresnel, luces que se prenden y apagan con el teclado | Listo, entrega del 27 de octubre |
| Animaciones (por código, por keyframes y por input) | Pendiente |

## Estructura del repositorio

```
Proyecto-BLENDER/
├── Modelos/     # Archivos .blend, una carpeta por integrante
├── Texturas/    # Imágenes que usan los materiales, una carpeta por modelo (incluye skybox/)
├── Shaders/     # Shaders de la escena (skybox, niebla, modelo base)
├── Escena/      # Acomodo de la escena para OpenGL y vista previa en Blender
├── OpenGL/      # Ejecutable en OpenGL (Visual Studio, Win32) con la escena armada
└── README.md
```

## Modelos principales

| # | Modelo | Descripción | Estado |
|---|---|---|---|
| 1 | **Tumba** | Base de piedra con lápida en la cabecera. Es el soporte de la ofrenda y da el contexto del cementerio. | Subido y texturizado (Sebastian) |
| 2 | **Niveles de la ofrenda** | Escalones cubiertos con mantel que organizan los elementos; cada nivel tiene un significado simbólico. | Subido y texturizado (Melissa) |
| 3 | **Veladora** | La luz que guía a las almas en su camino. En la escena es la principal fuente de luz dinámica. | Subido y texturizado (Melissa) |
| 4 | **Calaverita de azúcar** | Dulce tradicional que representa a los difuntos; suele llevar el nombre de la persona recordada. | Subido y texturizado |
| 5 | **Pan de muerto** | Pan dulce con "huesitos" cruzados y una bolita al centro que representa el cráneo. | Subido y texturizado (Sebastian) |
| 6 | **Flores de cempasúchil** | La flor emblemática del Día de Muertos; su color y aroma marcan el camino de regreso de los difuntos. | Subido y texturizado |
| 7 | **Portarretratos** | Marcos con las fotografías de los seres queridos a quienes se dedica la ofrenda. | Subido y texturizado |
| 8 | **Copalero** | Sahumerio de barro donde se quema el copal; su humo purifica el espacio y guía a las almas. | Subido y texturizado (Melissa) |
| 9 | **Papel picado** | Banderitas de papel recortado con figuras tradicionales que adornan el panteón. | Subido con sus texturas (Sebastian) |

> Actualiza la columna *Estado* al subir cada modelo.

Archivos de los modelos del equipo:

| Archivo | Contenido |
|---|---|
| `Modelos/Melissa/altar_con_flamas.blend` | Altar de tres niveles, velas con flama y copalero, en un solo archivo |
| `Modelos/Sebastian/Modelos.blend` | Tumba y pan de muerto, en un solo archivo |
| `Modelos/Sebastian/Decoracion.blend` | Papel picado (texturas en `Texturas/Decoración/`) |

## Modelos de ambientación

Están en `Modelos/Yasser/` y `Modelos/Yasser/modelos_cargados/`, todos texturizados.

| Modelo | Archivo | Uso en la escena |
|---|---|---|
| Escenario con reja | `modelos_cargados/escenario_reja_cementerio.blend` | Suelo, barda, reja de entrada y camino |
| Capilla | `modelos_cargados/capilla.blend` | Al fondo del camino |
| Ángel del dolor | `modelos_cargados/angel_dolor.blend` | 5 tumbas, entre las tumbas de Sebastian |
| Árbol del panteón | `modelos_cargados/arbol_panteon.blend` | 84 árboles, casi todos alrededor de la barda |
| Catrina | `modelos_cargados/catrina.blend` | 2 catrinas |
| Platillos del altar | `modelos_cargados/platillos_altar.blend` | En el nivel de arriba del altar de muertos |
| Flor de cempasúchil | `flor_cempasuchil.blend` | Una sola flor, repetida 943 veces (en el camino con su versión ligera `flor_baja`) |
| Guirnalda de cempasúchil | `cempasuchil.blend` (cadenas de flores) | Sobre cada tumba y en el altar |
| Pétalos del camino | `petalos_camino.blend` | Pétalos sobre el camino |
| Terrazas de tierra | `terrazas.blend` | Dos escalones de tierra con muros de piedra a cada lado del camino, con escaleras |
| Tapete de flores | `extras.blend` | Tapete de pétalos con una calavera, frente al altar y en algunas tumbas (9) |
| Florero de talavera | `florero.blend` | Florero con un ramo de cempasúchil, en las tumbas (35) |
| Angelito | `angelito.blend` | Estatua chica de ángel sobre pedestal, en tumbas y sueltos (12) |
| Cruz de piedra | `cruz_tumba.blend` | Tumbas sencillas con cruz (6) |

## Texturas

Cada modelo usa materiales sencillos pensados para cargarse en OpenGL: un *Principled BSDF* con una sola imagen conectada a *Base Color* (sin nodos procedurales). Las imágenes están en `Texturas/<modelo>/` y los `.blend` las referencian con rutas relativas, así que hay que conservar la misma estructura de carpetas.

Las UV usan proyección de caja en metros reales, por lo que algunas texturas se repiten (tileables): en OpenGL configura las texturas con `GL_REPEAT`.

| Modelo | Carpeta de texturas |
|---|---|
| Escenario reja del cementerio | `Texturas/escenario_reja_cementerio/` |
| Capilla | `Texturas/capilla/` |
| Ángel del dolor | `Texturas/angel_dolor/` |
| Árbol del panteón | `Texturas/arbol_panteon/` |
| Catrina | `Texturas/catrina/` |
| Platillos del altar | `Texturas/platillos_altar/` |
| Portarretratos | `Texturas/portarretratos/` (`foto_retrato.png` es un retrato genérico; cámbialo por la foto que quieran) |
| Cempasúchil | `Texturas/cempasuchil/` |
| Calaverita | `Texturas/calaverita/` |
| Altar, velas y copalero | `Texturas/altar_con_flamas/` |
| Tumba | `Texturas/tumba/` |
| Pan de muerto | `Texturas/pan_de_muerto/` |
| Terrazas | `Texturas/terrazas/` |
| Tapete de flores, florero y cruz | `Texturas/extras/` |
| Papel picado | `Texturas/Decoración/` (originales) y `Texturas/papel_picado/` (recortadas para OpenGL, con transparencia) |
| Skybox | `Texturas/skybox/` (6 caras: right, left, top, bottom, front, back) |

## Shaders

| Archivo | Qué hace |
|---|---|
| `skybox.vs`, `skybox.fs` | Dibujan el cubemap y lo mezclan con el color de la niebla cerca del horizonte |
| `modelo.vs`, `modelo.fs` | Shader base de los modelos: textura, luz de luna (Phong) y niebla |
| `niebla.glsl` | Función `aplicaNiebla()` que oscurece con la distancia y se concentra cerca del suelo |

## Escena

La escena tiene unas 1,500 instancias. El terreno sube en dos terrazas de tierra a cada lado del camino, como en los panteones de México, con escaleras entre niveles. Hay 27 tumbas, 6 cruces de piedra y 5 ángeles en filas; las tumbas están pintadas de distintos colores y cada familia las arregla diferente: unas muy adornadas (guirnalda, florero, tapete de flores, angelito, arco de flores y muchas velas), otras más sencillas y algunas casi abandonadas.

El altar de muertos está al final del camino, frente a la capilla, con platillos, velas, copal, calaveritas, retratos y un tapete de flores al frente. El camino de cempasúchil es denso (943 flores), con 220 velas de colores por todo el panteón, 98 guirnaldas (en las tumbas, el altar, la capilla y los muros de las terrazas), 23 tiras de papel picado (sobre el camino, la entrada, el altar y los pasillos), una catrina gigante de unos 6 m junto a la entrada, otra junto al altar, 84 árboles alrededor de la barda y niebla densa.

Las capturas de la escena están en `Escena/preview_*.png`.

`Escena/escena_layout.txt` tiene una línea por objeto (`modelo x y z rotY escala`, en coordenadas de OpenGL, con un color opcional `r g b` al final para pintar el modelo). Cada modelo se carga una vez y se dibuja en cada posición cambiando su matriz `model`. `Escena/escena_preview.blend` muestra la escena armada en Blender, solo como referencia. El código para leer el archivo, montar el skybox y configurar la niebla está en [`Escena/README.md`](Escena/README.md). El proyecto de Visual Studio que arma la escena completa está en [`OpenGL/`](OpenGL/README.md).

## Requisitos

- [Blender](https://www.blender.org/download/) 5.2 LTS (versión con la que se crearon los modelos).

## Cómo usar los modelos

1. Clona el repositorio:
   ```bash
   git clone https://github.com/sebastianfuentesp-ship-it/Proyecto-BLENDER.git
   ```
2. Abre en Blender el archivo `.blend` que necesites desde la carpeta `Modelos/`.
3. Si un modelo usa texturas, deben estar en `Texturas/`. En Blender, revisa las rutas con `File > External Data > Make All Paths Relative`.

## Cómo colaborar

Cada integrante sube sus propios modelos para evitar conflictos, ya que los archivos `.blend` son binarios y Git no puede combinar cambios hechos al mismo archivo.

- Antes de trabajar y antes de subir, trae los cambios del equipo: `git pull origin main`.
- Usa nombres de archivo descriptivos en minúsculas y sin acentos: `calaverita_base.blend`, `pan_de_muerto.blend`.
- No subas los respaldos de Blender (`*.blend1`, `*.blend2`); ya están en `.gitignore`.
- Reduce la cantidad de polígonos con el modificador *Decimate* antes de exportar, para que la escena corra fluida en tiempo real.
- Haz commits claros, por ejemplo: `Agrego modelo de veladora`.

## Créditos

Todos los modelos fueron creados por el equipo "Los tilines" para el proyecto final de Computación Gráfica e IHC, semestre 2027-1.
