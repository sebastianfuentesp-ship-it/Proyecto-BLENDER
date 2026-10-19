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

| Archivo | De quién | Triángulos | Pivote | Texturas |
|---|---|---|---|---|
| `escenario/escenario.obj` | Yasser | 29,054 | origen de la escena | pasto, tierra del camino, piedra de la cerca, hierro forjado, hiedra |
| `capilla/capilla.obj` | Yasser | 4,172 | abajo al centro | cantera, hierro |
| `angel_dolor/angel_dolor.obj` | Yasser | 134,866 | abajo al centro | mármol blanco, piedra del monumento |
| `arbol_panteon/arbol_panteon.obj` | Yasser | 9,483 | abajo al centro | corteza, follaje |
| `catrina/catrina.obj` | Yasser | 192,287 | abajo al centro | hueso, vestido, sombrero, plumas, pulsera, base |
| `platillos_altar/platillos_altar.obj` | Yasser | 126,852 | abajo | platillos y dulces del altar |
| `portarretratos/portarretratos.obj` | Yasser | 172 | abajo al centro | marco y foto |
| `calaverita/calaverita.obj` | Yasser | 16,396 | abajo al centro | azúcar blanca, betún de colores |
| `flor_cempasuchil/flor_cempasuchil.obj` | Yasser | 4,838 | abajo al centro | pétalo, centro, tallo, hoja |
| `petalos_camino/petalos_camino.obj` | Yasser | 79,712 | origen de la escena | pétalo de cempasúchil |
| `guirnalda/guirnalda.obj` | Yasser | 10,720 | arriba al centro | pétalo y centro de cempasúchil |
| `altar/altar.obj` | Melissa | 36 | abajo al centro | mantel morado |
| `vela/vela.obj` | Melissa | 328 | abajo al centro | cera, mecha |
| `flama/flama.obj` | Melissa | 1,248 | el mismo de la vela | flama (se dibuja con el shader `lamp`) |
| `copalero/copalero.obj` | Melissa | 12,344 | abajo al centro | barro, copal |
| `tumba/tumba.obj` | Sebastian | 5,652 | abajo al centro | cantera, mármol de la lápida, granito del nicho |
| `pan_de_muerto/pan_de_muerto.obj` | Sebastian | 6,460 | abajo al centro | pan, azúcar |
| `papel_picado/papel_picado.obj` | Sebastian | 382 | abajo al centro (entre los postes) | 6 diseños de papel picado con transparencia, postes, cordel |
| `terrazas/terrazas.obj` | Yasser | 336 | origen de la escena | muro de piedra, tierra |
| `flor_baja/flor_baja.obj` | Yasser | 1,209 | abajo al centro | la flor de cempasúchil con *Decimate* al 25 %, para el camino |
| `guirnalda_baja/guirnalda_baja.obj` | Yasser | 1,447 | arriba al centro | guirnalda con *Decimate* al 30 %, para los muros |
| `tapete_flores/tapete_flores.obj` | Yasser | 12 | abajo al centro | tapete de pétalos con calavera |
| `florero/florero.obj` | Yasser | 6,219 | abajo al centro | talavera, pétalo, centro, tallo, hoja |
| `angelito/angelito.obj` | Yasser | 12,120 | abajo al centro | mármol blanco, piedra del pedestal |
| `cruz_tumba/cruz_tumba.obj` | Yasser | 36 | abajo al centro | cantera |

El pan de muerto se exporta sin los cristales de azúcar de Geometry Nodes (eran 27 mil instancias, más de 5 millones de triángulos); el azúcar queda en la textura. El copalero se exporta con *Decimate* al 20 %.
