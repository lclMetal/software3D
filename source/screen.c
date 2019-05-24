typedef struct Matrix4x4Struct
{
    double matrix[4][4];
}Matrix4x4;

typedef struct Point2DStruct
{
    double x;
    double y;
}Point2D;

typedef struct ScreenStruct
{
    short width;
    short height;
    unsigned char* buffer;
}Screen;

// Written according to the following tutorial:
// https://www.davrous.com/2013/06/13/
// tutorial-series-learning-how-to-write-a-3d-soft-engine-from-scratch-in-c-
// typescript-or-javascript/

typedef struct VertexStruct
{
    double x;
    double y;
    double z;
    double w;
}Vertex;

typedef struct CameraStruct
{
    Vertex position;
    Vertex target;
}Camera;

typedef struct MeshStruct
{
    char name[256];
    int vertexCount;
    Vertex* vertices;
    Vertex position;
    Vertex rotation;
}Mesh;

Vertex createVertex(double vx, double vy, double vz, double vw)
{
    Vertex v;

    v.x = vx;
    v.y = vy;
    v.z = vz;
    v.w = vw;

    return v;
}

double dotProduct(Vertex a, Vertex b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vertex subtractVertices(Vertex a, Vertex b)
{
    return createVertex(a.x - b.x, a.y - b.y, a.z - b.z, 0);
}

// source:
// https://stackoverflow.com/questions/8875909/model-lookat-matrix-c-opengl

Vertex crossProduct(Vertex vertex, Vertex rightVertex)
{
    double newX = vertex.y * rightVertex.z - vertex.z * rightVertex.y;
    double newY = vertex.z * rightVertex.x - vertex.x * rightVertex.z;
    double newZ = vertex.x * rightVertex.y - vertex.y * rightVertex.x;

    return createVertex(newX, newY, newZ, 0);
}

// source:
// https://stackoverflow.com/questions/8875909/model-lookat-matrix-c-opengl

Vertex normalizeVertex(Vertex vertex)
{
    double length = vertex.x * vertex.x + vertex.y * vertex.y + vertex.z * vertex.z;

    if (abs(length) < 0.001)
        return createVertex(0, 0, 0, 0);

    length = 1.0f / sqrt(length);
    return createVertex(vertex.x * length, vertex.y * length, vertex.z * length, 0);
}

Mesh *newMesh(char meshName[256], int vertexCount)
{
    Mesh *ptr = NULL;

    ptr = malloc(sizeof *ptr);

    if (!ptr) return NULL;

    ptr->vertexCount = vertexCount;
    ptr->vertices = malloc(sizeof *(ptr->vertices) * ptr->vertexCount);

    if (!ptr->vertices)
    {
        free(ptr);
        return NULL;
    }

    strcpy(ptr->name, meshName);

    return ptr;
}

int setMeshVertex(Mesh *targetMesh, int vertexNum, Vertex vertex)
{
    if (!targetMesh) return -1;
    if (vertexNum < 0 || vertexNum >= targetMesh->vertexCount) return -2;

    targetMesh->vertices[vertexNum] = vertex;

    return 0;
}

void destroyMesh(Mesh *ptr)
{
    if (!ptr) return;

    free(ptr->vertices);
    free(ptr);
}


Matrix4x4 lookAtLH(Vertex cameraPosition, Vertex cameraTarget, Vertex cameraUp);
Matrix4x4 perspectiveFovRH(double fov, double aspect, double near, double far);
Matrix4x4 rotationYawPitchRoll(double yaw, double pitch, double roll);
Matrix4x4 translation(double x, double y, double z);
Vertex multiplyVertexByMatrix4x4(Vertex vertex, Matrix4x4 matrix);
Matrix4x4 multiplyMatrix4x4ByMatrix4x4(Matrix4x4 ma, Matrix4x4 mb);

Screen *newScreen(short scrWidth, short scrHeight)
{
    Screen *ptr = NULL;

    ptr = malloc(sizeof *ptr);

    if (!ptr) return NULL;

    ptr->width = scrWidth;
    ptr->height = scrHeight;
    ptr->buffer = malloc(sizeof *(ptr->buffer) * (ptr->width * ptr->height * 3));

    if (!ptr->buffer)
    {
        free(ptr);
        return NULL;
    }

    return ptr;
}

void destroyScreen(Screen *ptr)
{
    if (!ptr) return;

    free(ptr->buffer);
    free(ptr);
}

void eraseScreen(Screen *ptr, unsigned char r, unsigned char g, unsigned char b)
{
    int i, j, k;

    if (!ptr) return;

    for (i = 0; i < ptr->height; i ++)
    {
        for (j = 0; j < ptr->width; j ++)
        {
            ptr->buffer[i * ptr->width * 3 + j * 3] = r;
            ptr->buffer[i * ptr->width * 3 + j * 3 + 1] = g;
            ptr->buffer[i * ptr->width * 3 + j * 3 + 2] = b;
        }
    }
}

void renderScreen(Screen *ptr)
{
    int i, j, k;

    if (!ptr) return;

    for (i = 0; i < ptr->height; i ++)
    {
        for (j = 0; j < ptr->width; j ++)
        {
            if (ptr->buffer[i * ptr->width * 3 + j * 3] > 0)
            {
            setpen(ptr->buffer[i * ptr->width * 3 + j * 3],
            ptr->buffer[i * ptr->width * 3 + j * 3 + 1],
            ptr->buffer[i * ptr->width * 3 + j * 3 + 2], 0, 1);
            putpixel(j, i);
            }
        }
    }
}

void putPixelOnScreen(Screen *ptr, short x, short y, unsigned char r, unsigned char g, unsigned char b)
{
    char temp[128];
    if (!ptr) return;

    sprintf(temp, "x: %i, y: %i, buffer: %i", x, y, y * ptr->width * 3 + x * 3);
    // DEBUG_MSG(temp);
    ptr->buffer[y * ptr->width * 3 + x * 3] = r;
    ptr->buffer[y * ptr->width * 3 + x * 3 + 1] = g;
    ptr->buffer[y * ptr->width * 3 + x * 3 + 2] = b;
    setpen(r, g, b, 0, 2);
    putpixel(x, y);
}

Point2D createPoint2D(double px, double py)
{
    Point2D p;

    p.x = px;
    p.y = py;

    return p;
}

Point2D project(Screen *screen, Vertex vertex, Matrix4x4 matrix)
{
    Point2D result;
    Vertex transformed = multiplyVertexByMatrix4x4(vertex, matrix);

    result.x = transformed.x + screen->width / 2.0f;
    result.y = -transformed.y + screen->height / 2.0f;

    return result;
}

void drawPointOnScreen(Screen *ptr, Point2D point)
{
    if (!ptr) return;

    if (point.x >= 0 && point.y >= 0 && point.x < ptr->width && point.y < ptr->height)
    {
        putPixelOnScreen(ptr, (short)point.x, (short)point.y, 255, 255, 255);
    }
}

void RenderMesh(Screen *screen, Camera *camera, Mesh *mesh)
{
    Matrix4x4 viewMatrix = lookAtLH(camera->position, camera->target, createVertex(0, 1, 0, 0));
    Matrix4x4 projectionMatrix = perspectiveFovRH(0.78f, screen->width / (double)screen->height, 0.01f, 1.0f);

    Matrix4x4 worldMatrix = multiplyMatrix4x4ByMatrix4x4(translation(mesh->position.x, mesh->position.y, mesh->position.z), rotationYawPitchRoll(mesh->rotation.y, mesh->rotation.x, mesh->rotation.z));

    /*Matrix4x4 transformMatrix = multiplyMatrix4x4ByMatrix4x4(multiplyMatrix4x4ByMatrix4x4(worldMatrix, viewMatrix), projectionMatrix);*/
    /*Matrix4x4 transformMatrix = multiplyMatrix4x4ByMatrix4x4(projectionMatrix, multiplyMatrix4x4ByMatrix4x4(worldMatrix, viewMatrix));*/
    Matrix4x4 transformMatrix = multiplyMatrix4x4ByMatrix4x4(multiplyMatrix4x4ByMatrix4x4(projectionMatrix, viewMatrix), worldMatrix);

    int i;

    for (i = 0; i < mesh->vertexCount; i ++)
    {
        char temp[128];
        Point2D point = project(screen, mesh->vertices[i], transformMatrix);
        drawPointOnScreen(screen, point);

        sprintf(temp, "vertex: %i x: %f, y: %f", i, point.x, point.y);
        DEBUG_MSG(temp);
    }
}

Matrix4x4 lookAtLH(Vertex cameraPosition, Vertex cameraTarget, Vertex cameraUp)
{
    Matrix4x4 result;

    Vertex zAxis = normalizeVertex(subtractVertices(cameraTarget, cameraPosition));
    Vertex xAxis = normalizeVertex(crossProduct(cameraUp, zAxis));
    Vertex yAxis = crossProduct(zAxis, xAxis);

    result.matrix[0][0] = xAxis.x;
    result.matrix[0][1] = yAxis.x;
    result.matrix[0][2] = zAxis.x;
    result.matrix[0][3] = 0;

    result.matrix[1][0] = xAxis.y;
    result.matrix[1][1] = yAxis.y;
    result.matrix[1][2] = zAxis.z;
    result.matrix[1][3] = 0;

    result.matrix[2][0] = xAxis.z;
    result.matrix[2][1] = yAxis.z;
    result.matrix[2][2] = zAxis.z;
    result.matrix[2][3] = 0;

    result.matrix[3][0] = -dotProduct(xAxis, cameraPosition);
    result.matrix[3][1] = -dotProduct(yAxis, cameraPosition);
    result.matrix[3][2] = -dotProduct(zAxis, cameraPosition);
    result.matrix[3][3] = 1;

    return result;
}

// source:
// https://stackoverflow.com/questions/6060169/
// is-this-a-correct-perspective-fov-matrix
Matrix4x4 perspectiveFovRH(double fov, double aspect, double near, double far)
{
    Matrix4x4 result;

    double D2R = PI / (double)180.0;
    double yScale = 1.0 / (double)tan(D2R * fov / (double)2.0);
    double xScale = yScale / (double)aspect;
    double nearMinusFar = near - far;

    result.matrix[0][0] = xScale;
    result.matrix[0][1] = 0;
    result.matrix[0][2] = 0;
    result.matrix[0][3] = 0;

    result.matrix[1][0] = 0;
    result.matrix[1][1] = yScale;
    result.matrix[1][2] = 0;
    result.matrix[1][3] = 0;

    result.matrix[2][0] = 0;
    result.matrix[2][1] = 0;
    result.matrix[2][2] = (far + near) / (double)nearMinusFar;
    result.matrix[2][3] = -1;

    result.matrix[3][0] = 0;
    result.matrix[3][1] = 0;
    result.matrix[3][2] = 2 * far * near / (double)nearMinusFar;
    result.matrix[3][3] = 1;

    return result;
}

Matrix4x4 rotationYawPitchRoll(double yaw, double pitch, double roll)
{
    Matrix4x4 result;

    // alpha = yaw
    // beta = pitch
    // gamma = roll

    result.matrix[0][0] = cos(yaw) * cos(pitch);
    result.matrix[0][1] = cos(yaw) * sin(pitch) * sin(roll) - sin(yaw) * cos(roll);
    result.matrix[0][2] = cos(yaw) * sin(pitch) * cos(roll) + sin(yaw) * sin(roll);
    result.matrix[0][3] = 1;

    result.matrix[1][0] = sin(yaw) * cos(pitch);
    result.matrix[1][1] = sin(yaw) * sin(pitch) * sin(roll) + cos(yaw) * cos(roll);
    result.matrix[1][2] = sin(yaw) * sin(pitch) * cos(roll) - cos(yaw) * sin(roll);
    result.matrix[1][3] = 1;

    result.matrix[2][0] = -sin(pitch);
    result.matrix[2][1] = cos(pitch) * sin(roll);
    result.matrix[2][2] = cos(pitch) * cos(roll);
    result.matrix[2][3] = 1;

    result.matrix[3][0] = 1;
    result.matrix[3][1] = 1;
    result.matrix[3][2] = 1;
    result.matrix[3][3] = 1;

    return result;
}

Matrix4x4 translation(double x, double y, double z)
{
    Matrix4x4 result;

    result.matrix[0][0] = 1;
    result.matrix[0][1] = 0;
    result.matrix[0][2] = 0;
    result.matrix[0][3] = x;

    result.matrix[1][0] = 0;
    result.matrix[1][1] = 1;
    result.matrix[1][2] = 0;
    result.matrix[1][3] = y;

    result.matrix[2][0] = 0;
    result.matrix[2][1] = 0;
    result.matrix[2][2] = 1;
    result.matrix[2][3] = z;

    result.matrix[3][0] = 0;
    result.matrix[3][1] = 0;
    result.matrix[3][2] = 0;
    result.matrix[3][3] = 1;

    return result;
}

Vertex multiplyVertexByMatrix4x4(Vertex vertex, Matrix4x4 matrix)
{
    Vertex result = createVertex(0, 0, 0, 0);

    result.x = matrix.matrix[0][0] * vertex.x + matrix.matrix[0][1] * vertex.y +
               matrix.matrix[0][2] * vertex.z + matrix.matrix[0][3] * vertex.w;
    result.y = matrix.matrix[1][0] * vertex.x + matrix.matrix[1][1] * vertex.y +
               matrix.matrix[1][2] * vertex.z + matrix.matrix[1][3] * vertex.w;
    result.z = matrix.matrix[2][0] * vertex.x + matrix.matrix[2][1] * vertex.y +
               matrix.matrix[2][2] * vertex.z + matrix.matrix[2][3] * vertex.w;
    result.w = matrix.matrix[3][0] * vertex.x + matrix.matrix[3][1] * vertex.y +
               matrix.matrix[3][2] * vertex.z + matrix.matrix[3][3] * vertex.w;

    return result;
}

Matrix4x4 multiplyMatrix4x4ByMatrix4x4(Matrix4x4 ma, Matrix4x4 mb)
{
    Matrix4x4 result;

    result.matrix[0][0] = ma.matrix[0][0] * mb.matrix[0][0] + ma.matrix[0][1] * mb.matrix[1][0] + ma.matrix[0][2] * mb.matrix[2][0] + ma.matrix[0][3] * mb.matrix[3][0];
    result.matrix[0][1] = ma.matrix[0][0] * mb.matrix[0][1] + ma.matrix[0][1] * mb.matrix[1][1] + ma.matrix[0][2] * mb.matrix[2][1] + ma.matrix[0][3] * mb.matrix[3][1];
    result.matrix[0][2] = ma.matrix[0][0] * mb.matrix[0][2] + ma.matrix[0][1] * mb.matrix[1][2] + ma.matrix[0][2] * mb.matrix[2][2] + ma.matrix[0][3] * mb.matrix[3][2];
    result.matrix[0][3] = ma.matrix[0][0] * mb.matrix[0][3] + ma.matrix[0][1] * mb.matrix[1][3] + ma.matrix[0][2] * mb.matrix[2][3] + ma.matrix[0][3] * mb.matrix[3][3];

    result.matrix[1][0] = ma.matrix[1][0] * mb.matrix[0][0] + ma.matrix[1][1] * mb.matrix[1][0] + ma.matrix[1][2] * mb.matrix[2][0] + ma.matrix[1][3] * mb.matrix[3][0];
    result.matrix[1][1] = ma.matrix[1][0] * mb.matrix[0][1] + ma.matrix[1][1] * mb.matrix[1][1] + ma.matrix[1][2] * mb.matrix[2][1] + ma.matrix[1][3] * mb.matrix[3][1];
    result.matrix[1][2] = ma.matrix[1][0] * mb.matrix[0][2] + ma.matrix[1][1] * mb.matrix[1][2] + ma.matrix[1][2] * mb.matrix[2][2] + ma.matrix[1][3] * mb.matrix[3][2];
    result.matrix[1][3] = ma.matrix[1][0] * mb.matrix[0][3] + ma.matrix[1][1] * mb.matrix[1][3] + ma.matrix[1][2] * mb.matrix[2][3] + ma.matrix[1][3] * mb.matrix[3][3];

    result.matrix[2][0] = ma.matrix[2][0] * mb.matrix[0][0] + ma.matrix[2][1] * mb.matrix[1][0] + ma.matrix[2][2] * mb.matrix[2][0] + ma.matrix[2][3] * mb.matrix[3][0];
    result.matrix[2][1] = ma.matrix[2][0] * mb.matrix[0][1] + ma.matrix[2][1] * mb.matrix[1][1] + ma.matrix[2][2] * mb.matrix[2][1] + ma.matrix[2][3] * mb.matrix[3][1];
    result.matrix[2][2] = ma.matrix[2][0] * mb.matrix[0][2] + ma.matrix[2][1] * mb.matrix[1][2] + ma.matrix[2][2] * mb.matrix[2][2] + ma.matrix[2][3] * mb.matrix[3][2];
    result.matrix[2][3] = ma.matrix[2][0] * mb.matrix[0][3] + ma.matrix[2][1] * mb.matrix[1][3] + ma.matrix[2][2] * mb.matrix[2][3] + ma.matrix[2][3] * mb.matrix[3][3];

    result.matrix[3][0] = ma.matrix[3][0] * mb.matrix[0][0] + ma.matrix[3][1] * mb.matrix[1][0] + ma.matrix[3][2] * mb.matrix[2][0] + ma.matrix[3][3] * mb.matrix[3][0];
    result.matrix[3][1] = ma.matrix[3][0] * mb.matrix[0][1] + ma.matrix[3][1] * mb.matrix[1][1] + ma.matrix[3][2] * mb.matrix[2][1] + ma.matrix[3][3] * mb.matrix[3][1];
    result.matrix[3][2] = ma.matrix[3][0] * mb.matrix[0][2] + ma.matrix[3][1] * mb.matrix[1][2] + ma.matrix[3][2] * mb.matrix[2][2] + ma.matrix[3][3] * mb.matrix[3][2];
    result.matrix[3][3] = ma.matrix[3][0] * mb.matrix[0][3] + ma.matrix[3][1] * mb.matrix[1][3] + ma.matrix[3][2] * mb.matrix[2][3] + ma.matrix[3][3] * mb.matrix[3][3];

    return result;
}

/*Matrix4x4 multiplyMatrix4x4ByMatrix4x4(Matrix4x4 ma, Matrix4x4 mb)
{
    Matrix4x4 result;

    result.matrix[0][0] = ma.matrix[0][0] * mb.matrix[0][0] + ma.matrix[0][1] * mb.matrix[1][0] + ma.matrix[0][2] * mb.matrix[2][0];
    result.matrix[0][1] = ma.matrix[0][0] * mb.matrix[0][1] + ma.matrix[0][1] * mb.matrix[1][1] + ma.matrix[0][2] * mb.matrix[2][1];
    result.matrix[0][2] = ma.matrix[0][0] * mb.matrix[0][2] + ma.matrix[0][1] * mb.matrix[1][2] + ma.matrix[0][2] * mb.matrix[2][2];
    result.matrix[0][3] = ma.matrix[0][0] * mb.matrix[0][3] + ma.matrix[0][1] * mb.matrix[1][3] + ma.matrix[0][2] * mb.matrix[2][3];

    result.matrix[1][0] = ma.matrix[1][0] * mb.matrix[0][0] + ma.matrix[1][1] * mb.matrix[1][0] + ma.matrix[1][2] * mb.matrix[2][0];
    result.matrix[1][1] = ma.matrix[1][0] * mb.matrix[0][1] + ma.matrix[1][1] * mb.matrix[1][1] + ma.matrix[1][2] * mb.matrix[2][1];
    result.matrix[1][2] = ma.matrix[1][0] * mb.matrix[0][2] + ma.matrix[1][1] * mb.matrix[1][2] + ma.matrix[1][2] * mb.matrix[2][2];
    result.matrix[1][3] = ma.matrix[1][0] * mb.matrix[0][3] + ma.matrix[1][1] * mb.matrix[1][3] + ma.matrix[1][2] * mb.matrix[2][3];

    result.matrix[2][0] = ma.matrix[2][0] * mb.matrix[0][0] + ma.matrix[2][1] * mb.matrix[1][0] + ma.matrix[2][2] * mb.matrix[2][0];
    result.matrix[2][1] = ma.matrix[2][0] * mb.matrix[0][1] + ma.matrix[2][1] * mb.matrix[1][1] + ma.matrix[2][2] * mb.matrix[2][1];
    result.matrix[2][2] = ma.matrix[2][0] * mb.matrix[0][2] + ma.matrix[2][1] * mb.matrix[1][2] + ma.matrix[2][2] * mb.matrix[2][2];
    result.matrix[2][3] = ma.matrix[2][0] * mb.matrix[0][3] + ma.matrix[2][1] * mb.matrix[1][3] + ma.matrix[2][2] * mb.matrix[2][3];

    result.matrix[3][0] = ma.matrix[3][0] * mb.matrix[0][0] + ma.matrix[3][1] * mb.matrix[1][0] + ma.matrix[3][2] * mb.matrix[2][0];
    result.matrix[3][1] = ma.matrix[3][0] * mb.matrix[0][1] + ma.matrix[3][1] * mb.matrix[1][1] + ma.matrix[3][2] * mb.matrix[2][1];
    result.matrix[3][2] = ma.matrix[3][0] * mb.matrix[0][2] + ma.matrix[3][1] * mb.matrix[1][2] + ma.matrix[3][2] * mb.matrix[2][2];
    result.matrix[3][3] = ma.matrix[3][0] * mb.matrix[0][3] + ma.matrix[3][1] * mb.matrix[1][3] + ma.matrix[3][2] * mb.matrix[2][3];

    return result;
}*/
