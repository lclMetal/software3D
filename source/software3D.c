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
    short indices[3];
    short normal;
}Face;

typedef struct MeshStruct
{
    char name[256];

    int vertexCount;
    Vector3* vertices;
    Point2D* vertexProjections;

    int faceCount;
    Face* faces;

    int normalCount;
    Vector3* normals;

    Vector3 position;
    Vector3 rotation;
    Matrix4x4 orientation;
}Mesh;

typedef struct MeshFileStruct
{
    int vertexCount;
    int faceCount;
    int normalCount;
}MeshFile;

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
    ROTATE_SINGLE_ONLY = (1 << 4),
    ENABLE_AXES        = (1 << 5)
}flags = ENABLE_AXES;

Mesh *cube;
Mesh *coords;
Camera camera;
Screen screen;

short mode = 0;

Screen createScreen(short width, short height);
Face createFace(short v1, short v2, short v3);
Face createFaceWithNormal(short v1, short v2, short v3, short normal);
void drawPointOnScreen(Screen *ptr, Point2D point);
Mesh *newMesh(char meshName[256], int vertexCount, int faceCount, int normalCount);
void fseekEndOfLine(FILE *f);
MeshFile getMeshFileInfo(char fileName[256]);
Mesh *readMeshFromFile(char fileName[256]);
int setMeshVertex(Mesh *mesh, int vertexNum, Vector3 vertex);
int setMeshFace(Mesh *mesh, int faceNum, Face face);
int setMeshNormal(Mesh *mesh, int normalNum, Vector3 normal);
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

Face createFace(short v1, short v2, short v3)
{
    Face face;

    face.indices[0] = v1;
    face.indices[1] = v2;
    face.indices[2] = v3;
    face.normal = 0;

    return face;
}

Face createFaceWithNormal(short v1, short v2, short v3, short normal)
{
    Face face;

    face.indices[0] = v1;
    face.indices[1] = v2;
    face.indices[2] = v3;
    face.normal = normal;

    return face;
}

void drawPointOnScreen(Screen *screen, Point2D point)
{
    if (!screen)
    {
        DEBUG_MSG_FROM("Failed: Screen pointer was NULL.", "drawPointOnScreen");
        return;
    }

    if (point.x >= 0.0f && point.y >= 0.0f &&
        point.x < screen->width && point.y < screen->height)
    {
        putpixel(point.x, point.y);
    }
}

Mesh *newMesh(char meshName[256], int vertexCount, int faceCount, int normalCount)
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

    ptr->vertexProjections = malloc(sizeof *(ptr->vertexProjections) * ptr->vertexCount);

    if (!ptr->vertexProjections)
    {
        free(ptr->vertices);
        free(ptr);
        return NULL;
    }

    ptr->faceCount = faceCount;
    ptr->faces = malloc(sizeof *(ptr->faces) * ptr->faceCount);

    if (!ptr->faces)
    {
        free(ptr->vertices);
        free(ptr->vertexProjections);
        free(ptr);
        return NULL;
    }

    ptr->normalCount = normalCount;
    ptr->normals = malloc(sizeof *(ptr->normals) * ptr->normalCount);

    if (!ptr->normals)
    {
        free(ptr->vertices);
        free(ptr->vertexProjections);
        free(ptr->faces);
        free(ptr);
        return NULL;
    }

    ptr->position = createVector3(0.0f, 0.0f, 0.0f);
    ptr->rotation = createVector3(0.0f, 0.0f, 0.0f);
    ptr->orientation = createTranslationMatrix(0.0f, 0.0f, 0.0f);

    strcpy(ptr->name, meshName);

    return ptr;
}

void fseekEndOfLine(FILE *f)
{
    if (f)
    {
        char c;

        while ((c = fgetc(f)) != '\n')
        {
            if (feof(f))
                break;
        }
    }
}

MeshFile getMeshFileInfo(char fileName[256])
{
    MeshFile mf; // it's mf for MeshFile, you barbaric ogre >:(
    FILE *f = fopen(fileName, "r+");

    mf.vertexCount =
    mf.faceCount   =
    mf.normalCount = 0;

    if (f)
    {
        while (1)
        {
            char c = fgetc(f);

            if (feof(f)) break;

            switch (c)
            {
                case 'v': // this line is either a vertex or a normal
                    c = fgetc(f);

                    switch (c)
                    {
                        // only 'v' --> this is a vertex
                        case ' ': mf.vertexCount ++;
                            break;

                        // 'vn' --> this is a normal
                        case 'n': mf.normalCount ++;
                            break;
                    }
                    break;

                // 'f' --> this is a face
                case 'f': mf.faceCount ++;
                    break;

                // end-of-line, nothing to see here
                case '\n':
                    break;

                // any other character --> ignore, search for next line
                default:
                    fseekEndOfLine(f);
                    break;
            }
        }

        fclose(f);
    }

    return mf;
}

Mesh *readMeshFromFile(char fileName[256])
{
    Mesh *mesh = NULL;
    FILE *f = NULL;
    MeshFile mf;

    mf = getMeshFileInfo(fileName);

    if (!mf.vertexCount)
        return NULL;

    f = fopen(fileName, "r+");

    if (!f)
        return NULL;

    mesh = newMesh(fileName, mf.vertexCount, mf.faceCount , mf.normalCount);

    if (!mesh)
        return NULL;

    if (f)
    {
        Vector3 vec = createVector3(0.0f, 0.0f, 0.0f);
        Face face;
        int vertexNum = 0, faceNum = 0, normalNum = 0;

        while (1)
        {
            char c = fgetc(f), d;

            if (feof(f))
                break;

            switch (c)
            {
                case 'v': // this line is either a vertex or a normal
                    d = fgetc(f);

                    switch (d)
                    {
                        // only 'v' --> this is a vertex
                        case ' ':
                            fscanf(f, "%f %f %f",
                                    &vec.x, &vec.y, &vec.z);
                            setMeshVertex(mesh, vertexNum++, vec);
                            break;

                        // 'vn' --> this is a normal
                        case 'n':
                            fscanf(f, " %f %f %f",
                                    &vec.x, &vec.y, &vec.z);
                            setMeshNormal(mesh, normalNum++, vec);
                            break;

                        default:
                            break;
                    }
                    break;

                // 'f' --> this is a face
                case 'f':
                    fscanf(f, "%hd//%hd %hd//%hd %hd//%hd",
                            &face.indices[0], &face.normal,
                            &face.indices[1], &face.normal,
                            &face.indices[2], &face.normal);
                    setMeshFace(mesh, faceNum++,
                        createFaceWithNormal(face.indices[0] - 1,
                                             face.indices[1] - 1,
                                             face.indices[2] - 1, face.normal - 1));
                    break;

                // end-of-line, nothing to see here
                case '\n':
                    break;

                // any other character --> ignore, search for next line
                default:
                    fseekEndOfLine(f);
                    break;
            }
        }

        fclose(f);
    }

    return mesh;
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

int setMeshNormal(Mesh *mesh, int normalNum, Vector3 normal)
{
    if (!mesh) return -1;
    if (normalNum < 0 || normalNum >= mesh->normalCount) return -2;

    mesh->normals[normalNum] = normal;

    return 0;
}

void setMeshOrientation(Mesh *mesh, Vector3 orientation)
{
    mesh->orientation = createRotationXYZMatrix(orientation.x, orientation.y, orientation.z);
}

void renderMesh(Screen *screen, Camera *camera, Mesh *mesh)
{
    int i, j;
    Point2D vertices[3];
    Matrix4x4 viewMatrix = createLookAtMatrix(camera->position, camera->target, createVector3(0.0f, 1.0f, 0.0f));
    Matrix4x4 projectionMatrix =
        createPerspectiveMatrix(PI/3.0f, screen->width / (float)screen->height, 0.01f, 1000.0f);

    Matrix4x4 worldMatrix, tempMatrix, transformMatrix;

    // perform rotation one by one for each axis
    // https://gamedev.stackexchange.com/questions/67199/how-to-rotate-an-object-around-world-aligned-axes/67269#67269
    mesh->orientation = multiplyMatrices(mesh->orientation, createRotationXYZMatrix(mesh->rotation.x, 0.0f, 0.0f));
    mesh->orientation = multiplyMatrices(mesh->orientation, createRotationXYZMatrix(0.0f, mesh->rotation.y, 0.0f));
    mesh->orientation = multiplyMatrices(mesh->orientation, createRotationXYZMatrix(0.0f, 0.0f, mesh->rotation.z));
    mesh->rotation = createVector3(0.0f, 0.0f, 0.0f);

    worldMatrix =
        multiplyMatrices(mesh->orientation,
            createTranslationMatrix(mesh->position.x, mesh->position.y, mesh->position.z));

    tempMatrix = multiplyMatrices(worldMatrix, viewMatrix);
    transformMatrix = multiplyMatrices(tempMatrix, projectionMatrix);

    // reset the array of projections
    for (i = 0; i < mesh->vertexCount; i++)
    {
        mesh->vertexProjections[i].x = mesh->vertexProjections[i].y = 0.0f;
    }

    for (i = 0; i < mesh->faceCount; i ++)
    {
        // pre-optimized version called project() once for every vertex
        // of every face, amounting to total    2904 times
        // for the suzanne.obj model that has    507 vertices

        // optimized version calls project() only once for each vertex
        // of the mesh, amounting to total       507 times, logically

        // one call of project() amounts to       20 multiplications/divisions,
        // which means that every frame        58080 multiplications/divisions took place
        // instead of the minimum required     10140
        for (j = 0; j < 3; j ++)
        {
            if (mesh->vertexProjections[mesh->faces[i].indices[j]].x <= 0)
            {
                vertices[j] = project(screen->width, screen->height, mesh->vertices[mesh->faces[i].indices[j]], transformMatrix);
                mesh->vertexProjections[mesh->faces[i].indices[j]] = vertices[j];
            }
            else
                vertices[j] = mesh->vertexProjections[mesh->faces[i].indices[j]];
        }

        // if mode is 1 or 2, draw the lines between the vertices
        if (mode)
        {
            setpen(255, 255, 255, 0, 1);
            moveto(vertices[0].x, vertices[0].y);
            lineto(vertices[1].x, vertices[1].y);
            lineto(vertices[2].x, vertices[2].y);
            lineto(vertices[0].x, vertices[0].y);
        }

        // if mode is 0 or 1, draw the vertices as pink dots
        if (mode <= 1)
        {
            setpen(255, 0, 255, 0, 3);
            drawPointOnScreen(screen, vertices[0]);
            drawPointOnScreen(screen, vertices[1]);
            drawPointOnScreen(screen, vertices[2]);
        }
    }
}

void destroyMesh(Mesh *mesh)
{
    if (!mesh) return;

    free(mesh->vertices);
    free(mesh->vertexProjections);
    free(mesh->faces);
    free(mesh);
}
