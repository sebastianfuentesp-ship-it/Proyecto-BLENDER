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
| Animaciones: 3 de huesos y 3 por objetos hechas en Blender, 3 por código, las 9 con teclado | Listo, entrega del 5 de noviembre |

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
| 4 | **Calaverita de azúcar** | Dulce tradicional que representa a los difuntos; suele llevar el nombre de la persona recordada. | Subido, texturizado y con topología en quads |
| 5 | **Pan de muerto** | Pan dulce con "huesitos" cruzados y una bolita al centro que representa el cráneo. | Subido y texturizado (Sebastian) |
| 6 | **Flores de cempasúchil** | La flor emblemática del Día de Muertos; su color y aroma marcan el camino de regreso de los difuntos. | Subido, texturizado y con topología en quads |
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
| Tapete de flores | `extras.blend` | Tapete de pétalos con una calavera |
| Florero blanco | `florero.blend` | Florero de porcelana blanca con un ramo de cempasúchil, en las tumbas (42) |
| Angelito | `angelito.blend` | Estatua chica de ángel sobre pedestal, en tumbas y sueltos (12) |
| Cruz de piedra | `cruz_tumba.blend` | Tumbas sencillas con cruz (12) |
| Ofrenda tradicional | `ofrenda.blend` | El altar de muertos de la escena: tres niveles con manteles rosas y papel picado, cajas de madera, banca, huacal con naranjas, macetas, calabazas, jarritos, caña y pétalos en el piso |
| Coronas de flores | `corona_flores.blend`, `corona_corazon.blend`, `corona_cruz.blend`, `corona_blanca.blend`, `corona_lagrima.blend`, `corona_gladiolas.blend`, `corona_banda.blend`, `corona_amarilla.blend` | Ocho coronas en tripié como las de funeraria, junto a las tumbas: redonda, de corazón, en cruz, blanca con listón, de lágrima con listón morado "Descanse en paz", de gladiolas, roja y blanca con banda y amarilla con listón "Con cariño" |
| Tapetes de flores | `tapete_cruz.blend`, `tapete_corazon.blend`, `tapete_sol.blend`, `tapete_franjas.blend`, `tapete_calavera.blend`, `tapete_mariposa.blend`, `tapete_vela.blend` | Siete diseños más de tapete (cruz, corazón, sol, franjas, calavera, mariposa monarca y vela), en el camino y en las tumbas |
| Tumbas nuevas | `mausoleo.blend`, `tumba_tierra.blend`, `tumba_azulejo.blend` | Mausoleo familiar con techo de teja, tumba de tierra con cruz de madera y tumba de azulejo con lápida |
| Personas | `persona_hombre.blend`, `persona_mujer.blend`, `persona_nino.blend`, `persona_nina.blend` | Hombre, mujer, niño y niña modelados en quads y separados por partes para animarlos (cabeza, torso, brazos, muslos y piernas desde la rodilla, cada una con su origen en la articulación). Cada uno tiene tres variantes de ropa, piel y cabello, repetidas en toda la escena (unas 110 personas). La cara tiene topología de cara en quads: loops alrededor de ojos y boca, nariz y orejas extruidas y globos de los ojos |

| Banda de viento | `banda.blend` | Siete músicos con uniforme (camisa vino, chaleco negro bordado, moño y sombrero): tuba (sousáfono), tambora con platillos, tarola, dos trompetas, clarinete y trombón de vara. Todo en quads y separado por partes para animarlo |
| Animaciones de huesos | `anim_caminar.blend`, `anim_rezar.blend`, `anim_abrazar.blend` | Una persona con esqueleto (cadera, torso, cabeza, brazos, antebrazos, muslos y piernas) en cada archivo: un hombre que camina, una señora que se hinca a rezar y un niño que se hinca a abrazar una lápida |
| Animaciones por objetos | `reja_animada.blend`, `mariposas.blend`, `humo_copal.blend` | Keyframes de objetos: las hojas de la reja que se abren, tres mariposas monarca que vuelan alrededor del altar y el humo de los dos copaleros |

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
| Tapetes de flores, florero y cruz | `Texturas/extras/` |
| Ofrenda y coronas | `Texturas/ofrenda/` (el papel picado usa `Texturas/papel_picado/`) |
| Mausoleo, tumba de tierra y de azulejo | `Texturas/tumbas_extra/` |
| Personas | `Texturas/personas/` |
| Papel picado | `Texturas/Decoración/` (originales) y `Texturas/papel_picado/` (recortadas para OpenGL, con transparencia) |
| Skybox | `Texturas/skybox/` (6 caras: right, left, top, bottom, front, back) |

## Shaders

| Archivo | Qué hace |
|---|---|
| `skybox.vs`, `skybox.fs` | Dibujan el cubemap y lo mezclan con el color de la niebla cerca del horizonte |
| `modelo.vs`, `modelo.fs` | Shader base de los modelos: textura, luz de luna (Phong) y niebla |
| `niebla.glsl` | Función `aplicaNiebla()` que oscurece con la distancia y se concentra cerca del suelo |

## Escena

La escena tiene unas 2,000 instancias. El terreno sube en dos terrazas de tierra a cada lado del camino, como en los panteones de México, con escaleras entre niveles. Hay 40 tumbas de cinco tipos (tumba de cantera, tumba de azulejo, tumba de tierra, mausoleo y cruz de piedra) y 4 ángeles en filas; las tumbas están pintadas de distintos colores y cada familia las arregla diferente: unas muy adornadas (guirnalda, florero, tapete de flores, corona, angelito, arco de flores y muchas velas), otras más sencillas y algunas casi abandonadas. 125 personas visitan el panteón (hombres, mujeres, niños y niñas): familias frente a sus tumbas, gente viendo el altar y caminando por el camino.

El altar de muertos está al final del camino, frente a la capilla, al estilo de una ofrenda tradicional: tres niveles con manteles rosas y papel picado, cajas de madera a los lados, una banca con los platillos, un huacal con naranjas, macetas de cempasúchil, floreros con flores rosas, calabazas, jarritos, caña, pan de muerto, calaveritas, retratos, velas y copal, con pétalos regados y un camino de flores naranjas y rosas en el piso. El camino de cempasúchil es denso (más de 1,000 flores), con 464 velas por todo el panteón (en las tumbas, el altar, el camino y las orillas de las terrazas), cuatro tapetes de flores sobre el camino, 100 guirnaldas, 21 tiras de papel picado (sobre el camino, la entrada, detrás del altar y en los pasillos), una catrina gigante de unos 6 m junto a la entrada, otra junto al altar, 84 árboles alrededor de la barda y niebla densa.

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
