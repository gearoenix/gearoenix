import bpy

NEEDS_NORMAL = True
NEEDS_TANGENT = True
NEEDS_UV = True


def terminate(*msgs):
    msg = ''
    for m in msgs:
        msg += str(m) + ' '
    print('Fatal error: ' + msg)
    raise Exception(msg)


bobj = bpy.context.object
msh = bobj.data
msh.calc_normals_split()
msh.calc_tangents()
vertices = dict()
last_index = 0
occlusion_radius = 0.0
for p in msh.polygons:
    if len(p.vertices) > 3:
        terminate('Object', bobj.name, 'is not triangled!')
    for i, li in zip(p.vertices, p.loop_indices):
        vertex = []
        v = msh.vertices[i].co
        radius = v.length
        if radius > occlusion_radius:
            occlusion_radius = radius
        vertex.append(v[0])
        vertex.append(v[1])
        vertex.append(v[2])
        if NEEDS_NORMAL:
            normal = msh.loops[li].normal.normalized()
            vertex.append(normal[0])
            vertex.append(normal[1])
            vertex.append(normal[2])
        if NEEDS_TANGENT:
            tangent = msh.loops[li].tangent.normalized()
            vertex.append(tangent[0])
            vertex.append(tangent[1])
            vertex.append(tangent[2])
            vertex.append(msh.loops[li].bitangent_sign)
        if NEEDS_UV:
            uv_lyrs = msh.uv_layers
            if len(uv_lyrs) > 1 or len(uv_lyrs) < 1:
                terminate('Unexpected number of uv layers in', bobj.name)
            texco = uv_lyrs.active.data[li].uv
            vertex.append(texco[0])
            vertex.append(1.0 - texco[1])
        vertex = tuple(vertex)
        if vertex in vertices:
            vertices[vertex].append(last_index)
        else:
            vertices[vertex] = [last_index]
        last_index += 1
indices = [0 for _ in range(last_index)]
mesh_vertices = []
last_index = 0
for vertex, index_list in vertices.items():
    mesh_vertices.append(vertex)
    for i in index_list:
        indices[i] = last_index
    last_index += 1

s = 'static const std::vector<math::BasicVertex> vertices = {\n'
for v in mesh_vertices:
    s += '    math::BasicVertex { '
    s += 'math::Vec3(' + str(v[0]) + 'f, ' + \
        str(v[1]) + 'f, ' + str(v[2]) + 'f), '
    i = 3
    if NEEDS_NORMAL:
        s += 'math::Vec3(' + str(v[i]) + 'f, ' + \
            str(v[i + 1]) + 'f, ' + str(v[i + 2]) + 'f), '
        i += 3
    if NEEDS_TANGENT:
        s += 'math::Vec4(' + str(v[i]) + 'f, ' + str(v[i + 1]) + \
            'f, ' + str(v[i + 2]) + 'f, ' + str(v[i + 3]) + 'f), '
        i += 4
    if NEEDS_UV:
        s += 'math::Vec2(' + str(v[i]) + 'f, ' + str(v[i + 1]) + 'f), '
    s += '},\n'
s += '};\n\n'

s += 'static const std::vector<std::uint32> indices = {\n'
last_index = 0
while last_index < len(indices):
    i = 0
    s += '    '
    while last_index < len(indices) and i < 20:
        s += str(indices[last_index]) + ', '
        last_index += 1
        i += 1
    s += '\n'
s += '};\n\n'
s += 'const static core::Real occlusion_radius = ' + \
    str(occlusion_radius) + 'f;\n\n'

open('C:\\Users\\hossein\\Desktop\\1.txt', 'w').write(s)
