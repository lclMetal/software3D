typedef double Matrix4x4[4][4];

Vertex multiplyVertexByMatrix4x4(Vertex vertex, Matrix4x4 matrix)
{
    Vertex result = createVertex(0, 0, 0, 0);

    result.x = matrix[0][0] * vertex.x + matrix[0][1] * vertex.y +
               matrix[0][2] * vertex.z + matrix[0][3] * vertex.w;
    result.y = matrix[1][0] * vertex.x + matrix[1][1] * vertex.y +
               matrix[1][2] * vertex.z + matrix[1][3] * vertex.w;
    result.z = matrix[2][0] * vertex.x + matrix[2][1] * vertex.y +
               matrix[2][2] * vertex.z + matrix[2][3] * vertex.w;
    result.w = matrix[3][0] * vertex.x + matrix[3][1] * vertex.y +
               matrix[3][2] * vertex.z + matrix[3][3] * vertex.w;

    return result;
}
