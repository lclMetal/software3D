// Written according to the following tutorial:
// https://www.davrous.com/2013/06/13/
// tutorial-series-learning-how-to-write-a-3d-soft-engine-from-scratch-in-c-
// typescript-or-javascript/

typedef struct Vector3Struct
{
    double x;
    double y;
    double z;
}Vector3;

typedef struct CameraStruct
{
    Vector3 position;
    Vector3 target;
}Camera;

typedef struct MeshStruct
{
    char name[256];
    int vertexCount;
    Vector3* vertices;
    Vector3 position;
    Vector3 rotation;
}Mesh;

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

void destroyMesh(Mesh *ptr)
{
    if (!ptr) return;

    free(ptr->vertices);
    free(ptr);
}
