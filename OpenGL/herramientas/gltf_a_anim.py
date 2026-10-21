# Convierte un .gltf exportado de Blender a un .anim de texto que Animacion.h lee sin assimp
# (el assimp viejo del proyecto no lee el glTF de Blender 5).
# Uso: python3 gltf_a_anim.py carpeta/n.gltf  -> carpeta/n.anim  (necesita numpy)
import sys, json, struct, os
import numpy as np

ruta = sys.argv[1]
g = json.load(open(ruta))
base = os.path.dirname(ruta)
bufs = [open(os.path.join(base, b['uri']), 'rb').read() for b in g['buffers']]
COMP = {5120: 'b', 5121: 'B', 5122: 'h', 5123: 'H', 5125: 'I', 5126: 'f'}
NC = {'SCALAR': 1, 'VEC2': 2, 'VEC3': 3, 'VEC4': 4, 'MAT4': 16}


def acc(i):
    a = g['accessors'][i]
    bv = g['bufferViews'][a['bufferView']]
    n, k = a['count'], NC[a['type']]
    dt = np.dtype(COMP[a['componentType']]).newbyteorder('<')
    paso = bv.get('byteStride', dt.itemsize * k)
    ini = bv.get('byteOffset', 0) + a.get('byteOffset', 0)
    crudo = np.frombuffer(bufs[bv['buffer']], dtype=np.uint8, count=paso * (n - 1) + dt.itemsize * k, offset=ini)
    out = np.empty((n, k), dtype=dt)
    for j in range(n):
        out[j] = np.frombuffer(crudo[j * paso: j * paso + dt.itemsize * k].tobytes(), dtype=dt)
    out = out.astype(np.float64)
    if a.get('normalized'):
        out /= {5121: 255.0, 5123: 65535.0, 5120: 127.0, 5122: 32767.0}[a['componentType']]
    return out


def local(n):
    if 'matrix' in n:
        return np.array(n['matrix'], dtype=np.float64)   # ya viene por columnas
    t = n.get('translation', [0, 0, 0]); x, y, z, w = n.get('rotation', [0, 0, 0, 1]); s = n.get('scale', [1, 1, 1])
    R = np.array([[1 - 2 * (y * y + z * z), 2 * (x * y - z * w), 2 * (x * z + y * w)],
                  [2 * (x * y + z * w), 1 - 2 * (x * x + z * z), 2 * (y * z - x * w)],
                  [2 * (x * z - y * w), 2 * (y * z + x * w), 1 - 2 * (x * x + y * y)]])
    M = np.eye(4); M[:3, :3] = R * np.array(s)[None, :]; M[:3, 3] = t
    return M.T.reshape(-1)   # por columnas, como glm


# nodos en orden: padres antes que hijos
orden, padre = [], {}
def recorre(i, p):
    padre[i] = p; orden.append(i)
    for c in g['nodes'][i].get('children', []):
        recorre(c, i)
for r in g['scenes'][g.get('scene', 0)]['nodes']:
    recorre(r, -1)
nuevo = {v: k for k, v in enumerate(orden)}

L = ['ANIM 1']
L.append('nodos %d' % len(orden))
for i in orden:
    n = g['nodes'][i]
    nombre = n.get('name', 'nodo%d' % i).replace(' ', '_')
    L.append('nodo %s %d %s' % (nombre, nuevo[padre[i]] if padre[i] >= 0 else -1, ' '.join('%.7g' % v for v in local(n))))

skin = g['skins'][0] if g.get('skins') else None
if skin:
    ibm = acc(skin['inverseBindMatrices'])
    L.append('huesos %d' % len(skin['joints']))
    for j, nd in enumerate(skin['joints']):
        L.append('hueso %d %s' % (nuevo[nd], ' '.join('%.7g' % v for v in ibm[j])))
else:
    L.append('huesos 0')

tex_de_mat = []
for m in g.get('materials', []):
    t = m.get('pbrMetallicRoughness', {}).get('baseColorTexture')
    tex_de_mat.append(g['images'][g['textures'][t['index']]['source']]['uri'] if t else '-')

mallas = []
for i in orden:
    n = g['nodes'][i]
    if 'mesh' not in n:
        continue
    piel = 1 if 'skin' in n else 0
    for pr in g['meshes'][n['mesh']]['primitives']:
        at = pr['attributes']
        P = acc(at['POSITION']); nv = len(P)
        N = acc(at['NORMAL']) if 'NORMAL' in at else np.tile([0, 1, 0], (nv, 1))
        T = acc(at['TEXCOORD_0']) if 'TEXCOORD_0' in at else np.zeros((nv, 2))
        J = acc(at['JOINTS_0']) if piel and 'JOINTS_0' in at else np.zeros((nv, 4))
        W = acc(at['WEIGHTS_0']) if piel and 'WEIGHTS_0' in at else np.zeros((nv, 4))
        I = acc(pr['indices']).reshape(-1).astype(int) if 'indices' in pr else np.arange(nv)
        tex = tex_de_mat[pr['material']] if 'material' in pr else '-'
        mallas.append((nuevo[i], piel, tex, P, N, T, J, W, I))
L.append('mallas %d' % len(mallas))
for nodo, piel, tex, P, N, T, J, W, I in mallas:
    L.append('malla %d %d %s %d %d' % (nodo, piel, tex, len(P), len(I)))
    for k in range(len(P)):
        L.append('%.6g %.6g %.6g %.5g %.5g %.5g %.6g %.6g %d %d %d %d %.5g %.5g %.5g %.5g' % (
            *P[k], *N[k], T[k][0], 1.0 - T[k][1], *J[k].astype(int), *W[k]))
    for k in range(0, len(I), 3):
        L.append('%d %d %d' % tuple(I[k:k + 3]))

# canales: uno por nodo, con sus llaves de posicion, rotacion (w x y z) y escala
canal = {}
dur = 0.0
for a in g.get('animations', []):
    for ch in a['channels']:
        s = a['samplers'][ch['sampler']]
        t = acc(s['input']).reshape(-1); v = acc(s['output'])
        if s.get('interpolation') == 'CUBICSPLINE':
            v = v.reshape(len(t), 3, -1)[:, 1, :]
        if s.get('interpolation') == 'STEP' and len(t) > 1:   # escalon: se repite cada llave justo antes de la siguiente
            t = np.repeat(t, 2)[1:]; t[1::2] -= 1e-4; v = np.repeat(v, 2, axis=0)[:-1]
        dur = max(dur, float(t[-1]))
        nd = ch['target'].get('node')
        if nd is None or nd not in nuevo:
            continue
        camino = ch['target']['path']
        if camino == 'rotation':
            v = v[:, [3, 0, 1, 2]]
        if camino in ('translation', 'rotation', 'scale'):
            canal.setdefault(nuevo[nd], {})[camino] = (t, v)
L.append('duracion %.6g' % dur)
L.append('canales %d' % len(canal))
for nd, cs in canal.items():
    L.append('canal %d' % nd)
    for camino, clave in (('translation', 'pos'), ('rotation', 'rot'), ('scale', 'esc')):
        if camino in cs:
            t, v = cs[camino]
            L.append('%s %d' % (clave, len(t)))
            for k in range(len(t)):
                L.append('%.6g %s' % (t[k], ' '.join('%.6g' % x for x in v[k])))
        else:
            L.append('%s 0' % clave)

salida = ruta[:-5] + '.anim'
open(salida, 'w').write('\n'.join(L) + '\n')
print(salida, len(orden), 'nodos', len(skin['joints']) if skin else 0, 'huesos', len(mallas), 'mallas', len(canal), 'canales', 'dur', dur)
