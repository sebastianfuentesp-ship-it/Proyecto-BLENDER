# Modelos

Un folder por modelo, con su `.obj`, su `.mtl` (si tiene) y sus texturas propias:

```
assets/models/calaverita/
├── calaverita.obj
├── calaverita.mtl
├── azucar_blanca.png
└── betun_rosa.png   (y el resto de sus texturas)
```

Las rutas de las texturas dentro del `.mtl` son solo el nombre del archivo, así que el `.png` tiene que estar en la misma carpeta que el `.obj`.

## Cómo exportar desde Blender

*File → Export → Wavefront (.obj)* con estas opciones:

- **Forward:** -Z · **Up:** Y (OpenGL usa Y hacia arriba; no lo vuelvan a rotar en el código).
- **Triangulated Mesh:** activado.
- **Apply Modifiers:** activado (si el modelo usa Geometry Nodes, así sale la malla real).
- **Selection Only:** activado, exportando una pieza por archivo.
- **Path Mode:** *Relative* o *Strip Path*, para que el `.mtl` no apunte a `C:\Users\...`.

## Convenciones

- Escala: 1 unidad de Blender = 1 unidad en OpenGL.
- Pivote (origen del modelo):
  - objetos que se apoyan (flor, calaverita, veladora…): **abajo al centro**;
  - objetos que cuelgan (cadenas, papel picado): **arriba al centro**.
- Pétalos, hojas y papel picado con grosor, o dibujarlos con `glDisable(GL_CULL_FACE)` y luz de dos caras en el shader (`if (!gl_FrontFacing) N = -N;`).
- Si un modelo se repite mucho (flores, veladoras), cargarlo **una vez** y dibujarlo varias veces cambiando solo la matriz `model`.

## Modelos de la escena

Estos son los modelos texturizados que carga `src/main.cpp`. Dónde va cada uno (posición, giro y escala) se define en `assets/escena_layout.txt`.

| Archivo | Triángulos | Pivote | Texturas |
|---|---|---|---|
| `escenario/escenario.obj` | 29,054 | origen de la escena | pasto, tierra del camino, piedra de la cerca, hierro forjado, hiedra |
| `capilla/capilla.obj` | 4,172 | abajo al centro | cantera, hierro |
| `angel_dolor/angel_dolor.obj` | 134,866 | abajo al centro | mármol blanco, piedra del monumento |
| `arbol_panteon/arbol_panteon.obj` | 9,483 | abajo al centro | corteza, follaje |
| `catrina/catrina.obj` | 192,287 | abajo al centro | hueso, vestido, sombrero, plumas, pulsera, base |
| `platillos_altar/platillos_altar.obj` | 126,852 | abajo | platillos y dulces del altar |
| `portarretratos/portarretratos.obj` | 172 | abajo al centro | marco y foto |
| `calaverita/calaverita.obj` | 16,396 | abajo al centro | azúcar blanca, betún de colores |
| `flor_cempasuchil/flor_cempasuchil.obj` | 4,838 | abajo al centro | pétalo, centro, tallo, hoja |
| `petalos_camino/petalos_camino.obj` | 79,712 | origen de la escena | pétalo de cempasúchil |

## Modelos anteriores (sin texturas)

| Archivo | Triángulos | Pivote |
|---|---|---|
| `cempasuchil/cempasuchil.obj` | 4,838 | abajo al centro |
| `flor_suelta/flor_suelta.obj` | 268 | abajo al centro |
| `cadena_cempasuchil/cadena_colgada_1.obj` | 4,824 | arriba al centro |
| `cadena_cempasuchil/cadena_colgada_2.obj` | 4,824 | arriba al centro |
| `cadena_cempasuchil/cadena_tira_1.obj` | 3,484 | arriba al centro |
| `cadena_cempasuchil/cadena_tira_2.obj` | 2,412 | arriba al centro |
| `cadena_cempasuchil/cadena_tira_3.obj` | 3,484 | arriba al centro |
