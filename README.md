# 🕯️ Ofrenda de Día de Muertos en el cementerio

**Equipo "Los tilines"**
Proyecto final · Computación Gráfica e Interacción Humano-Computadora · Semestre 2027-1 · UNAM, Facultad de Ingeniería

---

## 🌼 Sobre el proyecto

Este proyecto representa una **ofrenda de Día de Muertos dentro de un cementerio**, una de las tradiciones más representativas de la cultura mexicana. Cada 1 y 2 de noviembre, las familias visitan a sus seres queridos en los panteones, limpian y decoran sus tumbas y colocan ofrendas con flores, velas, comida y fotografías para recibir sus almas.

La escena muestra una tumba sobre la que se levanta una ofrenda de varios niveles, rodeada de lápidas, árboles y papel picado, con un arco de cempasúchil, portarretratos, calaveritas, pan de muerto, copalero y veladoras.

Este repositorio contiene los **modelos 3D hechos en Blender**, junto con las texturas y los shaders que se usan en la escena final.

## 👥 Integrantes

| Rol | Nombre |
|---|---|
| Project Owner | Alejandra Melissa Saucedo Serralde |
| Scrum Master | Yasser Vladimir Cruz Miranda |
| Developer | Sebastian Andre Fuentes Poma |
| Developer | Cristian Fajardo Télles |

## 🗂️ Estructura del repositorio

```
Proyecto-BLENDER/
├── Modelos/     # Archivos .blend de cada modelo
├── Texturas/    # Imágenes usadas por los materiales
├── Shaders/     # Shaders de la escena
└── README.md
```

## 🎨 Modelos principales

Los 8 modelos principales fueron creados por el equipo en Blender:

| # | Modelo | Descripción | Estado |
|---|---|---|---|
| 1 | **Tumba** | Base de piedra con lápida en la cabecera. Es el soporte de la ofrenda y da el contexto del cementerio. | ⬜ Pendiente de subir |
| 2 | **Niveles de la ofrenda** | Escalones cubiertos con mantel que organizan los elementos; cada nivel tiene un significado simbólico. | ⬜ Pendiente de subir |
| 3 | **Veladora** | La luz que guía a las almas en su camino. En la escena final será la principal fuente de luz dinámica. | ⬜ Pendiente de subir |
| 4 | **Calaverita de azúcar** | Dulce tradicional que representa a los difuntos; suele llevar el nombre de la persona recordada. | ✅ `calaverita_base.blend` |
| 5 | **Pan de muerto** | Pan dulce con "huesitos" cruzados y una bolita al centro que representa el cráneo. | ⬜ Pendiente de subir |
| 6 | **Flores de cempasúchil** | La flor emblemática del Día de Muertos; su color y aroma marcan el camino de regreso de los difuntos. | ✅ `cempasuchil_base.blend` |
| 7 | **Portarretratos** | Marcos con las fotografías de los seres queridos a quienes se dedica la ofrenda. | ⬜ Pendiente de subir |
| 8 | **Copalero** | Sahumerio de barro donde se quema el copal; su humo purifica el espacio y guía a las almas. | ⬜ Pendiente de subir |

> Actualiza la columna *Estado* al subir cada modelo.

## 🛠️ Requisitos

- [Blender](https://www.blender.org/download/) 5.2 LTS (versión con la que se crearon los modelos).

## ▶️ Cómo usar los modelos

1. Clona el repositorio:
   ```bash
   git clone https://github.com/sebastianfuentesp-ship-it/Proyecto-BLENDER.git
   ```
2. Abre en Blender el archivo `.blend` que necesites desde la carpeta `Modelos/`.
3. Si un modelo usa texturas, deben estar en `Texturas/`. En Blender, revisa las rutas con `File > External Data > Make All Paths Relative`.

## 🤝 Cómo colaborar

Cada integrante sube sus propios modelos, para evitar conflictos, ya que los archivos `.blend` son binarios y Git no puede combinar cambios hechos al mismo archivo.

- Antes de trabajar y antes de subir, trae los cambios del equipo: `git pull origin main`.
- Usa nombres de archivo descriptivos en minúsculas y sin acentos: `calaverita_base.blend`, `pan_de_muerto.blend`.
- No subas los respaldos de Blender (`*.blend1`, `*.blend2`); ya están en `.gitignore`.
- Reduce la cantidad de polígonos con el modificador *Decimate* antes de exportar, para que la escena corra fluida en tiempo real.
- Haz commits claros, por ejemplo: `Agrega modelo de veladora`.

## 📌 Créditos

Todos los modelos fueron creados por el equipo "Los tilines" para el proyecto final de Computación Gráfica e IHC, semestre 2027-1.
