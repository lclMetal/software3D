// Written according to the following tutorial:
// https://www.davrous.com/2013/06/13/
// tutorial-series-learning-how-to-write-a-3d-soft-engine-from-scratch-in-c-
// typescript-or-javascript/

typedef struct CameraStruct
{
    Vector3 position;
    Vector3 target;
}Camera;

typedef struct FaceStruct
{
    short a;
    short b;
    short c;
}Face;

typedef struct MeshStruct
{
    char name[256];
    int vertexCount;
    Vector3* vertices;
    int faceCount;
    Face* faces;
    Vector3 position;
    Vector3 rotation;
    Matrix4x4 orientation;
}Mesh;

typedef struct ScreenStruct
{
    short width;
    short height;
}Screen;

enum flagsEnum
{
    ENABLE_MOUSE       = (1 << 0),
    ROTATE_X           = (1 << 1),
    ROTATE_Y           = (1 << 2),
    ROTATE_Z           = (1 << 3),
    ROTATE_SINGLE_ONLY = (1 << 4)
}flags = 0;

Mesh *cube;
Mesh *coords;
Camera camera;
Screen screen;

short mode = 0;

Screen createScreen(short width, short height);
Face createFace(short a, short b, short c);
void drawPointOnScreen(Screen *ptr, Point2D point);
Mesh *newMesh(char meshName[256], int vertexCount, int faceCount);
int setMeshVertex(Mesh *mesh, int vertexNum, Vector3 vertex);
int setMeshFace(Mesh *mesh, int faceNum, Face face);
void setMeshOrientation(Mesh *mesh, Vector3 orientation);
void renderMesh(Screen *screen, Camera *camera, Mesh *mesh);
void destroyMesh(Mesh *mesh);

Screen createScreen(short width, short height)
{
    Screen screen;

    screen.width = width;
    screen.height = height;

    return screen;
}

Face createFace(short a, short b, short c)
{
    Face face;

    face.a = a;
    face.b = b;
    face.c = c;

    return face;
}

void drawPointOnScreen(Screen *screen, Point2D point)
{
    if (!screen)
    {
        DEBUG_MSG_FROM("Failed: Screen pointer was NULL.", "drawPointOnScreen");
        return;
    }

    if (point.x >= 0 && point.y >= 0 &&
        point.x < screen->width && point.y < screen->height)
    {
        putpixel(point.x, point.y);
    }
}

Mesh *newMesh(char meshName[256], int vertexCount, int faceCount)
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

    ptr->faceCount = faceCount;
    ptr->faces = malloc(sizeof *(ptr->faces) * ptr->faceCount);

    if (!ptr->faces)
    {
        free(ptr->vertices);
        free(ptr);
        return NULL;
    }

    ptr->position = createVector3(0.0, 0.0, 0.0);
    ptr->rotation = createVector3(0.0, 0.0, 0.0);
    ptr->orientation = createTranslationMatrix(0.0, 0.0, 0.0);

    strcpy(ptr->name, meshName);

    return ptr;
}

int setMeshVertex(Mesh *mesh, int vertexNum, Vector3 vertex)
{
    if (!mesh) return -1;
    if (vertexNum < 0 || vertexNum >= mesh->vertexCount) return -2;

    mesh->vertices[vertexNum] = vertex;

    return 0;
}

int setMeshFace(Mesh *mesh, int faceNum, Face face)
{
    if (!mesh) return -1;
    if (faceNum < 0 || faceNum >= mesh->faceCount) return -2;

    mesh->faces[faceNum] = face;

    return 0;
}

void setMeshOrientation(Mesh *mesh, Vector3 orientation)
{
    mesh->orientation = createRotationXYZMatrix(orientation.x, orientation.y, orientation.z);
}

void renderMesh(Screen *screen, Camera *camera, Mesh *mesh)
{
    int i;
    Point2D point;
    Vector3 vertexA, vertexB, vertexC;
    Point2D pointA, pointB, pointC;
    Matrix4x4 viewMatrix = createLookAtMatrix(camera->position, camera->target, createVector3(0, 1, 0));
    Matrix4x4 projectionMatrix =
        createPerspectiveMatrix(PI/3.0, screen->width / (double)screen->height, 0.01, 1000.0);

    Matrix4x4 worldMatrix, tempMatrix, transformMatrix;

    // perform rotation one by one for each axis
    // https://gamedev.stackexchange.com/questions/67199/how-to-rotate-an-object-around-world-aligned-axes/67269#67269
    mesh->orientation = multiplyMatrices(mesh->orientation, createRotationXYZMatrix(mesh->rotation.x, 0.0, 0.0));
    mesh->orientation = multiplyMatrices(mesh->orientation, createRotationXYZMatrix(0.0, mesh->rotation.y, 0.0));
    mesh->orientation = multiplyMatrices(mesh->orientation, createRotationXYZMatrix(0.0, 0.0, mesh->rotation.z));
    mesh->rotation = createVector3(0.0, 0.0, 0.0);

    worldMatrix =
        multiplyMatrices(mesh->orientation,
            createTranslationMatrix(mesh->position.x, mesh->position.y, mesh->position.z));

    tempMatrix = multiplyMatrices(worldMatrix, viewMatrix);
    transformMatrix = multiplyMatrices(tempMatrix, projectionMatrix);

    for (i = 0; i < mesh->faceCount; i ++)
    {
        vertexA = mesh->vertices[mesh->faces[i].a];
        vertexB = mesh->vertices[mesh->faces[i].b];
        vertexC = mesh->vertices[mesh->faces[i].c];

        pointA = project(screen->width, screen->height, vertexA, transformMatrix);
        pointB = project(screen->width, screen->height, vertexB, transformMatrix);
        pointC = project(screen->width, screen->height, vertexC, transformMatrix);

        setpen(255, 0, 255, 0, 7);
        drawPointOnScreen(screen, pointA);
        drawPointOnScreen(screen, pointB);
        drawPointOnScreen(screen, pointC);

        setpen(255, 255, 255, 0, 1);
        moveto(pointA.x, pointA.y);
        lineto(pointB.x, pointB.y);
        lineto(pointC.x, pointC.y);
        lineto(pointA.x, pointA.y);
    }

    /*for (i = 0; i < mesh->vertexCount; i ++)
    {
        switch (mode)
        {
            case 0:
                point = project(screen->width, screen->height, mesh->vertices[i], transformMatrix);
                setpen(255, 255, 255, 0, 4);
                drawPointOnScreen(screen, point);
                break;

            case 1:
                point = project(screen->width, screen->height, mesh->vertices[i], transformMatrix);
                setpen(255, 255, 255, 0, 4);
                drawPointOnScreen(screen, point);
                setpen(255, 255, 255, 0, 1);
                moveto(point.x, point.y);
                point = project(screen->width, screen->height, coords->vertices[0], transformMatrix);
                lineto(point.x, point.y);
                break;

            case 2:
                point = project(screen->width, screen->height, mesh->vertices[i], transformMatrix);
                setpen(255, 255, 255, 0, 4);
                drawPointOnScreen(screen, point);
                setpen(255, 255, 255, 0, 1);

                if (i > 0)
                    lineto(point.x, point.y);

                moveto(point.x, point.y);
                break;
        }
    }*/
}

void destroyMesh(Mesh *mesh)
{
    if (!mesh) return;

    free(mesh->vertices);
    free(mesh->faces);
    free(mesh);
}
