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
