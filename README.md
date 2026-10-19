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
| Modelos de Cristian, Melissa y Sebastian | Pendiente de subir |
| Skybox nocturno (`Texturas/skybox/`) | Listo |
| Shaders de skybox, niebla y luz de luna (`Shaders/`) | Listo |
| Acomodo de la escena (`Escena/`) | Listo, se ajustará al llegar los demás modelos |
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
| 1 | **Tumba** | Base de piedra con lápida en la cabecera. Es el soporte de la ofrenda y da el contexto del cementerio. | Pendiente de subir |
| 2 | **Niveles de la ofrenda** | Escalones cubiertos con mantel que organizan los elementos; cada nivel tiene un significado simbólico. | Pendiente de subir |
| 3 | **Veladora** | La luz que guía a las almas en su camino. En la escena final será la principal fuente de luz dinámica. | Pendiente de subir |
| 4 | **Calaverita de azúcar** | Dulce tradicional que representa a los difuntos; suele llevar el nombre de la persona recordada. | Subido y texturizado |
| 5 | **Pan de muerto** | Pan dulce con "huesitos" cruzados y una bolita al centro que representa el cráneo. | Pendiente de subir |
| 6 | **Flores de cempasúchil** | La flor emblemática del Día de Muertos; su color y aroma marcan el camino de regreso de los difuntos. | Subido y texturizado |
| 7 | **Portarretratos** | Marcos con las fotografías de los seres queridos a quienes se dedica la ofrenda. | Subido y texturizado |
| 8 | **Copalero** | Sahumerio de barro donde se quema el copal; su humo purifica el espacio y guía a las almas. | Pendiente de subir |

> Actualiza la columna *Estado* al subir cada modelo.

## Modelos de ambientación

Están en `Modelos/Yasser/` y `Modelos/Yasser/modelos_cargados/`, todos texturizados.

| Modelo | Archivo | Uso en la escena |
|---|---|---|
| Escenario con reja | `modelos_cargados/escenario_reja_cementerio.blend` | Suelo, barda, reja de entrada y camino |
| Capilla | `modelos_cargados/capilla.blend` | Al fondo del camino |
| Ángel del dolor | `modelos_cargados/angel_dolor.blend` | Tumbas, repetido 12 veces |
| Árbol del panteón | `modelos_cargados/arbol_panteon.blend` | 9 árboles, adentro y afuera de la barda |
| Catrina | `modelos_cargados/catrina.blend` | 2 catrinas |
| Platillos del altar | `modelos_cargados/platillos_altar.blend` | Ofrenda principal |
| Flor de cempasúchil | `flor_cempasuchil.blend` | Una sola flor, repetida 374 veces |
| Pétalos del camino | `petalos_camino.blend` | Pétalos sobre el camino |

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
| Skybox | `Texturas/skybox/` (6 caras: right, left, top, bottom, front, back) |

## Shaders

| Archivo | Qué hace |
|---|---|
| `skybox.vs`, `skybox.fs` | Dibujan el cubemap y lo mezclan con el color de la niebla cerca del horizonte |
| `modelo.vs`, `modelo.fs` | Shader base de los modelos: textura, luz de luna (Phong) y niebla |
| `niebla.glsl` | Función `aplicaNiebla()` que oscurece con la distancia y se concentra cerca del suelo |

## Escena

`Escena/escena_layout.txt` tiene una línea por objeto (`modelo x y z rotY escala`, en coordenadas de OpenGL). Cada modelo se carga una vez y se dibuja en cada posición cambiando su matriz `model`. `Escena/escena_preview.blend` muestra la escena armada en Blender, solo como referencia. El código para leer el archivo, montar el skybox y configurar la niebla está en [`Escena/README.md`](Escena/README.md). El proyecto de Visual Studio que arma la escena completa está en [`OpenGL/`](OpenGL/README.md).

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
