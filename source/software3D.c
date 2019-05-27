// Written according to the following tutorial:
// https://www.davrous.com/2013/06/13/
// tutorial-series-learning-how-to-write-a-3d-soft-engine-from-scratch-in-c-
// typescript-or-javascript/

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
    Matrix4x4 orientation;
}Mesh;

Mesh *cube;
Mesh *coords;
Camera camera;

enum flagsEnum
{
    ENABLE_MOUSE       = (1 << 0),
    ROTATE_X           = (1 << 1),
    ROTATE_Y           = (1 << 2),
    ROTATE_Z           = (1 << 3),
    ROTATE_SINGLE_ONLY = (1 << 4)
}flags = 0;

short mode = 0;

const Matrix4x4 emptyMatrix2;

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

    ptr->position = createVector3(0.0, 0.0, 0.0);
    ptr->rotation = createVector3(0.0, 0.0, 0.0);
    ptr->orientation = translation(0.0, 0.0, 0.0);

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

void destroyMesh(Mesh *mesh)
{
    if (!mesh) return;

    free(mesh->vertices);
    free(mesh);
}

Screen createScreen(short width, short height)
{
    Screen screen;

    screen.width = width;
    screen.height = height;

    return screen;
}

void putPixelOnScreen(Screen *ptr, short x, short y, unsigned char r, unsigned char g, unsigned char b)
{
    setpen(r, g, b, 0, 4);
    putpixel(x, y);
}

void drawPointOnScreen(Screen *ptr, Point2D point)
{
    if (!ptr) return;

    if (point.x >= 0 && point.y >= 0 && point.x < ptr->width && point.y < ptr->height)
    {
        putPixelOnScreen(ptr, (short)point.x, (short)point.y, 255, 255, 255);
    }
}

void setMeshOrientation(Mesh *mesh, Vector3 orientation)
{
    mesh->orientation = rotationXYZ(orientation.x, orientation.y, orientation.z);
}

void renderMesh(Screen *screen, Camera *camera, Mesh *mesh)
{
    int i;
    Matrix4x4 viewMatrix = lookAt(camera->position, camera->target, createVector3(0, 1, 0));
    Matrix4x4 projectionMatrix =
        perspectiveFov(PI/3.0, (double)screen->width / (double)screen->height, 0.01, 1000.0);

    Matrix4x4 worldMatrix, tempMatrix, transformMatrix;

    // perform rotation one by one for each axis
    // https://gamedev.stackexchange.com/questions/67199/how-to-rotate-an-object-around-world-aligned-axes/67269#67269
    mesh->orientation = multiplyMatrix4x4ByMatrix4x4(mesh->orientation, rotationXYZ(mesh->rotation.x, 0.0, 0.0));
    mesh->orientation = multiplyMatrix4x4ByMatrix4x4(mesh->orientation, rotationXYZ(0.0, mesh->rotation.y, 0.0));
    mesh->orientation = multiplyMatrix4x4ByMatrix4x4(mesh->orientation, rotationXYZ(0.0, 0.0, mesh->rotation.z));
    mesh->rotation = createVector3(0.0, 0.0, 0.0);

    worldMatrix =
        multiplyMatrix4x4ByMatrix4x4(mesh->orientation,
            translation(mesh->position.x, mesh->position.y, mesh->position.z));

    tempMatrix = multiplyMatrix4x4ByMatrix4x4(worldMatrix, viewMatrix);
    transformMatrix = multiplyMatrix4x4ByMatrix4x4(tempMatrix, projectionMatrix);

    for (i = 0; i < mesh->vertexCount; i ++)
    {
        char temp[128];
        Point2D point;

        switch (mode)
        {
            case 0:
                point = project(screen, mesh->vertices[i], transformMatrix);
                drawPointOnScreen(screen, point);
                break;

            case 1:
                point = project(screen, mesh->vertices[i], transformMatrix);
                drawPointOnScreen(screen, point);
                setpen(255, 255, 255, 0, 1);
                moveto(point.x, point.y);
                point = project(screen, coords->vertices[0], transformMatrix);
                lineto(point.x, point.y);
                break;

            case 2:
                point = project(screen, mesh->vertices[i], transformMatrix);
                drawPointOnScreen(screen, point);
                setpen(255, 255, 255, 0, 1);

                if (i > 0)
                    lineto(point.x, point.y);

                moveto(point.x, point.y);
                break;
        }

        sprintf(temp, "vertex: %i x: %f, y: %f", i, point.x, point.y);
        DEBUG_MSG(temp);
    }
}
