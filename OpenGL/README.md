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
├── include/              # Código base del laboratorio, Animacion.h (glTF con huesos), SOIL2 y stb_image
├── shaders/              # lighting (Phong + Fresnel + niebla), anim (huesos y viento), lamp, skybox
├── assets/
│   ├── models/           # Un folder por modelo: .obj, .mtl y texturas
│   ├── skybox/           # 6 caras del cielo nocturno
│   ├── escena_layout.txt # Copia de Escena/escena_layout.txt
│   └── animaciones_layout.txt # Donde van la señora que reza, el niño de la tumba, el niño que saluda y la banda
└── external/             # GLEW, GLFW, assimp, glm y SOIL2
```

Si cambian un modelo en Blender, vuelvan a exportarlo a `assets/models/<modelo>/` como OBJ (Forward -Z, Up Y, con materiales y Path Mode: Copy). Si cambian el acomodo, copien `Escena/escena_layout.txt` a `assets/`.

## Iluminación (entrega del 27 de octubre)

Dos técnicas:

- **Phong** con tres tipos de luz: la luna (direccional), las velas (puntuales, con parpadeo) y una linterna en la cámara (spotlight). Hay unas 350 velas en la escena; en cada cuadro se mandan al shader las 24 más cercanas a la cámara.
- **Fresnel** (aproximación de Schlick): brillo azulado de la luna en los bordes de los objetos.

Además, niebla densa exponencial que se concentra cerca del suelo y se funde con el horizonte del skybox. Las flamas de las velas se dibujan sin iluminación (shader `lamp`) y se estiran con el parpadeo. El papel picado usa la transparencia de su textura (`discard` en `lighting.frag`; `Model.h` carga las texturas en RGBA). Las tumbas se pintan con el uniform `tinte`, que multiplica el color de la textura con el color que trae cada línea del layout.

## Rendimiento

La escena tiene unas 1,600 instancias, así que el programa no dibuja todo en cada cuadro:

- Las matrices `model` de cada objeto se calculan una sola vez al cargar el layout.
- Cada modelo tiene una esfera que lo envuelve (sacada de los mínimos y máximos de sus mallas). Lo que queda fuera de la vista de la cámara no se dibuja.
- Con niebla, los objetos chicos (flores, velas, floreros) que están a más de 40 m no se dibujan porque la niebla ya los tapa (90 m sin niebla).
- De lejos (más de 11 m) se usa la versión ligera del modelo, `assets/models/<modelo>/<modelo>_lod.obj`, si existe. La tienen las personas, las flores, las guirnaldas, la calaverita, el florero, el árbol, el ángel, la catrina y otros.
- Los objetos se dibujan ordenados por modelo y `Mesh::Draw` guarda las ubicaciones de los uniforms en lugar de buscarlas en cada cuadro.
- El ángel del dolor y la catrina se aligeraron con *Decimate* (de 135 mil a 47 mil y de 192 mil a 96 mil triángulos).

## Personas por partes (para la animación)

Las cuatro personas (hombre, mujer, niño y niña) se dibujan completas en la escena (`persona_<tipo>_<a|b|c>`), y también están exportadas por partes en `assets/models/persona_<tipo>_partes/`: `torso`, `cabeza`, `brazoizq`, `brazoder`, `antebrazoizq`, `antebrazoder`, `musloizq`, `musloder`, `piernaizq` y `piernader` (la pierna desde la rodilla, con el pie). Cada parte tiene su origen en la articulación (cuello, hombro, cadera o rodilla), así que se puede girar con `glm::rotate` como en el ejemplo de animación por jerarquía del profe. El archivo `pivotes.txt` de cada carpeta dice dónde va cada articulación, quién es su padre y el desplazamiento desde el padre:

```cpp
model = T(posicion) * Ry(giro);                 // torso
cabeza = model * T(offset_cabeza) * Rx(angulo); // cabeza sobre el torso
pierna = muslo * T(offset_rodilla) * Rx(angulo_rodilla);
```

## Animaciones (entrega del 5 de noviembre)

Cada animación se prende y se apaga con una tecla (del 1 al 9).

| Tecla | Animación | Tipo | Cómo está hecha |
|---|---|---|---|
| 1 | Un hombre camina por el camino, de la entrada al altar y de regreso | Huesos (Blender) | Esqueleto de 11 huesos con keyframes; ciclo de pasos de 1 s. En el programa avanza por el camino y da media vuelta en cada punta |
| 2 | Una señora se hinca frente a una tumba y reza con las manos juntas | Huesos (Blender) | Se hinca, se queda rezando mientras la tecla esté activa y al apagarla se levanta |
| 3 | Un niño se hinca y abraza la lápida de una tumba | Huesos (Blender) | Igual que la anterior, solloza mientras está abrazado |
| 4 | La reja de la entrada se abre o se cierra | Objetos (Blender) | Keyframes en las bisagras de las dos hojas; para cerrarla la animación corre hacia atrás |
| 5 | Mariposas monarca vuelan alrededor del altar | Objetos (Blender) | Cada mariposa sigue una curva y aletea unas 6 veces por segundo |
| 6 | Sale humo de los copaleros del altar | Objetos (Blender) | Bocanadas que suben, crecen y se deshacen; se dibuja con transparencia |
| 7 | La banda toca | Código | Máquina de estados con jerarquía de partes, como el perro de la práctica: los músicos se mecen al ritmo, los de percusión golpean con los brazos y la vara del trombón entra y sale con la mano del músico |
| 8 | Un niño junto al altar saluda con la mano | Código | Máquina de estados: sube el brazo, mueve el antebrazo de lado a lado y baja el brazo al apagarla |
| 9 | El viento mueve el papel picado | Código | En el vertex shader (`anim.vs`) las tiras se mecen con senos; los postes no se mueven |

Las animaciones de Blender están en `assets/models/animaciones/` como glTF (`.gltf` + `.bin` + texturas) y se cargan con assimp en `include/Animacion.h`. Para las de huesos, `Animacion.h` calcula en cada cuadro la matriz de cada hueso a partir de los keyframes (interpola posición, rotación con slerp y escala) y `shaders/anim.vs` mueve cada vértice con la suma de sus huesos según su peso. `anim.vs` se usa con el mismo `lighting.frag`, así que las animaciones tienen la misma iluminación que el resto. La banda está en `assets/models/banda/`, un folder por músico con sus partes y su `pivotes.txt`.

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
| 1 a 9 | Animaciones (ver la tabla de arriba) |
| Esc | Salir |
