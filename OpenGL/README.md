# Escena en OpenGL

Ejecutable del proyecto de teoría: carga los modelos de todo el equipo (exportados a OBJ con sus texturas) y arma el panteón en OpenGL 3.3. Usa como base el código del laboratorio (`Shader.h`, `Camera.h`, `Mesh.h`, `Model.h`).

## Cómo compilar

1. Abre `OpenGL/ProyectoFinal.slnx` en Visual Studio 2026.
2. Elige **Release | Win32** (GLEW solo trae la versión de 32 bits).
3. Compila y ejecuta. El directorio de trabajo es la carpeta `OpenGL/`; el evento posterior a la compilación copia `glew32.dll` y `assimp-vc140-mt.dll` junto al ejecutable.

La escena tarda unos segundos en cargar al abrir.

## Estructura

```
OpenGL/
├── ProyectoFinal.slnx / ProyectoFinal.vcxproj
├── src/main.cpp          # Carga del layout, luces, skybox y controles
├── include/              # Código base del laboratorio, SOIL2 y stb_image
├── shaders/              # lighting (Phong + Fresnel + niebla), lamp, skybox
├── assets/
│   ├── models/           # Un folder por modelo: .obj, .mtl y texturas
│   ├── skybox/           # 6 caras del cielo nocturno
│   └── escena_layout.txt # Copia de Escena/escena_layout.txt
└── external/             # GLEW, GLFW, assimp, glm y SOIL2
```

Si cambian un modelo en Blender, vuelvan a exportarlo a `assets/models/<modelo>/` como OBJ (Forward -Z, Up Y, con materiales y Path Mode: Copy). Si cambian el acomodo, copien `Escena/escena_layout.txt` a `assets/`.

## Iluminación (entrega del 27 de octubre)

Dos técnicas:

- **Phong** con tres tipos de luz: la luna (direccional), las velas (puntuales, con parpadeo) y una linterna en la cámara (spotlight). Hay 186 velas en la escena; en cada cuadro se mandan al shader las 24 más cercanas a la cámara.
- **Fresnel** (aproximación de Schlick): brillo azulado de la luna en los bordes de los objetos.

Además, niebla densa exponencial que se concentra cerca del suelo y se funde con el horizonte del skybox. Las flamas de las velas se dibujan sin iluminación (shader `lamp`) y se estiran con el parpadeo. El papel picado usa la transparencia de su textura (`discard` en `lighting.frag`; `Model.h` carga las texturas en RGBA).

## Controles

| Tecla | Acción |
|---|---|
| W A S D / flechas | Mover la cámara |
| Ratón | Mirar alrededor |
| F | Prender o apagar la linterna |
| V | Prender o apagar las velas |
| R | Activar o quitar el brillo de Fresnel |
| N | Activar o quitar la niebla |
| + / - | Subir o bajar la luz de la luna |
| Esc | Salir |
