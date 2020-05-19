#define PLANE_NEAR   0
#define PLANE_FAR    1
#define PLANE_BOTTOM 2
#define PLANE_TOP    3
#define PLANE_LEFT   4
#define PLANE_RIGHT  5

typedef struct PlaneStruct
{
    Vector3 normal;
    float d;
}Plane;

typedef struct CameraStruct
{
    Vector3 position;
    Vector3 target;
    Plane frustum[6];
}Camera;

typedef struct FaceStruct
{
    short indices[3];
    short normal;
    int poolIndex;
}Face;

typedef struct MeshStruct
{
    char name[256];

    int vertexCount;
    Vector3* vertices;
    Vector3* vertexProjections;

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

typedef struct TriangleStruct
{
    Vector3 p1;
    Vector3 p2;
    Vector3 p3;
}Triangle;

typedef struct ScreenStruct
{
    short width;
    short height;
}Screen;

typedef struct TriangleObjStruct
{
    Mesh *mesh;
    Face *face;
    short drawState;
    float shading;
    float faceDist;
}TriangleObj;

typedef struct TrianglePoolStruct
{
    int triCount;
    int maxTriCount;
    TriangleObj *triangles;
}TrianglePool;

void setCameraFrustum(Camera *camera, Matrix4x4 matrix);
void setCoefficients(Plane *pl, float a, float b, float c, float d);
int pointInCameraFrustum(Camera *camera, Vector3 vec);
Screen createScreen(short width, short height);
Face createFace(short v1, short v2, short v3);
Face createFaceWithNormal(short v1, short v2, short v3, short normal);
void drawPointOnScreen(Screen *ptr, Point2D point);
Mesh *newMesh(char meshName[256], int vertexCount, int faceCount, int normalCount);
MeshFile getMeshFileInfo(char fileName[256]);
Mesh *readMeshFromFile(char fileName[256]);
int setMeshVertex(Mesh *mesh, int vertexNum, Vector3 vertex);
int setMeshFace(Mesh *mesh, int faceNum, Face face);
int setMeshNormal(Mesh *mesh, int normalNum, Vector3 normal);
void setMeshOrientation(Mesh *mesh, Vector3 orientation);
void renderMesh(Screen *screen, Camera *camera, Mesh *mesh);
void fillTriangle(Triangle triangle, float rr, float gg, float bb);
void destroyMesh(Mesh *mesh);

void createPool(TrianglePool *this, int maxTriCount);
void addMeshFacesToPool(TrianglePool *tp, Mesh *mesh);
void addTriangleToPool(TrianglePool *tp, Mesh *mesh, Face *face);
void setTriangleInPool(TrianglePool *tp, int index, short drawState, float shading, float faceDist);
void resetTrianglePool(TrianglePool *tp);
void drawTrianglesFromPool(TrianglePool *tp);
void sortTrianglePoolInsertion(TrianglePool *tp);
void freeTrianglePool(TrianglePool *tp);

// Written according to the following tutorial:
// https://www.davrous.com/2013/06/13/
// tutorial-series-learning-how-to-write-a-3d-soft-engine-from-scratch-in-c-
// typescript-or-javascript/

const unsigned int ENABLE_MOUSE       = (1 << 0);
const unsigned int ROTATE_X           = (1 << 1);
const unsigned int ROTATE_Y           = (1 << 2);
const unsigned int ROTATE_Z           = (1 << 3);
const unsigned int ROTATE_SINGLE_ONLY = (1 << 4);
const unsigned int ENABLE_AXES        = (1 << 5);
const unsigned int BACKFACE_CULLING   = (1 << 6);
unsigned int flags = ENABLE_AXES | BACKFACE_CULLING;

Mesh *cube;
Mesh *cubbe;
Mesh *coords;
Camera camera;
Screen screen;

TrianglePool trianglePool;

short mode = 3;
int inspectFace = 0;

void setCameraFrustum(Camera *camera, Matrix4x4 matrix)
{
    setCoefficients(&camera->frustum[PLANE_NEAR], matrix.m13 + matrix.m14,
                                                  matrix.m23 + matrix.m24,
                                                  matrix.m33 + matrix.m34,
                                                  matrix.m43 + matrix.m44);

    setCoefficients(&camera->frustum[PLANE_FAR], -matrix.m13 + matrix.m14,
                                                 -matrix.m23 + matrix.m24,
                                                 -matrix.m33 + matrix.m34,
                                                 -matrix.m43 + matrix.m44);

    setCoefficients(&camera->frustum[PLANE_TOP], matrix.m12 + matrix.m14,
                                                    matrix.m22 + matrix.m24,
                                                    matrix.m32 + matrix.m34,
                                                    matrix.m42 + matrix.m44);

    setCoefficients(&camera->frustum[PLANE_BOTTOM], -matrix.m12 + matrix.m14,
                                                 -matrix.m22 + matrix.m24,
                                                 -matrix.m32 + matrix.m34,
                                                 -matrix.m42 + matrix.m44);

    setCoefficients(&camera->frustum[PLANE_RIGHT], matrix.m11 + matrix.m14,
                                                  matrix.m21 + matrix.m24,
                                                  matrix.m31 + matrix.m34,
                                                  matrix.m41 + matrix.m44);

    setCoefficients(&camera->frustum[PLANE_LEFT], -matrix.m11 + matrix.m14,
                                                   -matrix.m21 + matrix.m24,
                                                   -matrix.m31 + matrix.m34,
                                                   -matrix.m41 + matrix.m44);
    /*{
        char temp[256];
        sprintf(temp, "%f %f %f %f\n%f %f %f %f\n%f %f %f %f\n%f %f %f %f\n",
            matrix.m11, matrix.m12, matrix.m13, matrix.m14,
            matrix.m21, matrix.m22, matrix.m23, matrix.m24,
            matrix.m31, matrix.m32, matrix.m33, matrix.m34,
            matrix.m41, matrix.m42, matrix.m43, matrix.m44);
        DEBUG_MSG(temp);
    }*/
    /*{
        char temp[256];
        sprintf(temp, "[%f %f %f ] / %f\n[%f %f %f ] / %f\n[%f %f %f ] / %f\n[%f %f %f ] / %f\n[%f %f %f ] / %f\n[%f %f %f ] / %f\n",
            camera->frustum[PLANE_NEAR].normal.x, camera->frustum[PLANE_NEAR].normal.y, camera->frustum[PLANE_NEAR].normal.z, camera->frustum[PLANE_NEAR].d,
            camera->frustum[PLANE_FAR].normal.x, camera->frustum[PLANE_FAR].normal.y, camera->frustum[PLANE_FAR].normal.z, camera->frustum[PLANE_FAR].d,
            camera->frustum[PLANE_BOTTOM].normal.x, camera->frustum[PLANE_BOTTOM].normal.y, camera->frustum[PLANE_BOTTOM].normal.z, camera->frustum[PLANE_BOTTOM].d,
            camera->frustum[PLANE_TOP].normal.x, camera->frustum[PLANE_TOP].normal.y, camera->frustum[PLANE_TOP].normal.z, camera->frustum[PLANE_TOP].d,
            camera->frustum[PLANE_LEFT].normal.x, camera->frustum[PLANE_LEFT].normal.y, camera->frustum[PLANE_LEFT].normal.z, camera->frustum[PLANE_LEFT].d,
            camera->frustum[PLANE_RIGHT].normal.x, camera->frustum[PLANE_RIGHT].normal.y, camera->frustum[PLANE_RIGHT].normal.z, camera->frustum[PLANE_RIGHT].d);
        DEBUG_MSG(temp);
    }*/
}

void setCoefficients(Plane *pl, float a, float b, float c, float d)
{
    float l = max(0.0001f, magnitudeVector3(createVector3(a, b, c)));

    pl->normal = createVector3(a / l, b / l, c / l);
    pl->d = d / l;
}

int pointInCameraFrustum(Camera *camera, Vector3 vec)
{
    int i;
    float dist;

    for (i = 0; i < 6; i ++)
    {
        dist = dotProductVector3(camera->frustum[i].normal, vec) + camera->frustum[i].d;

        if (dist < 0.0f)
        {
            // char temp[256];
            // sprintf(temp, "%i, %f", i, dist);
            // DEBUG_MSG(temp);
            return 0;
        }
    }

    return 1;
}

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

MeshFile getMeshFileInfo(char fileName[256])
{
    MeshFile mf; // it's mf for MeshFile, you barbaric ogre >:(
    FILE *f = fopen(fileName, "r+");

    mf.vertexCount = mf.faceCount = mf.normalCount = 0;

    if (f)
    {
        char line[256];

        while (1)
        {
            if (fgets(line, sizeof line, f)) // read the file line by line
            {
                if (line[0] == 'v' && line[1] == ' ')      // this line is a vertex
                    mf.vertexCount++;
                else if (line[0] == 'v' && line[1] == 'n') // this line is a normal
                    mf.normalCount++;
                else if (line[0] == 'f' && line[1] == ' ') // this line is a face
                    mf.faceCount++;
            }
            else
                break;
        }

        fclose(f);
    }

    return mf;
}

Mesh *readMeshFromFile(char fileName[256])
{
    FILE *f;
    Mesh *mesh;
    MeshFile mf;
    int read; // as in: 'red', not: 'reed' :P
    char line[256] = "", errorMsg[256] = "";
    int vertexNum = 0, faceNum = 0, normalNum = 0, failed = 0;
    Vector3 vec;
    Face face;

    if (!(mf = getMeshFileInfo(fileName)).vertexCount) // mesh info was invalid
        return NULL;

    if (!(mesh = newMesh(fileName, mf.vertexCount, mf.faceCount, mf.normalCount))) // allocation failed
        return NULL;

    if (!(f = fopen(fileName, "r+"))) // file opening failed
    {
        free(mesh);
        return NULL;
    }

    while (1)
    {
        if (fgets(line, sizeof line, f)) // read the file line by line
        {
            if (line[0] == 'v' && line[1] == ' ') // this line is a vertex
            {
                if ((read = sscanf(line, "%*s %f %f %f", &vec.x, &vec.y, &vec.z)) == 4)
                {
                    setMeshVertex(mesh, vertexNum++, vec);
                }
                else
                {
                    failed = 1;
                    sprintf(errorMsg, "Failed: Parsing vertex %d from file %s failed. %d", vertexNum, fileName, ftell(f));
                }
            }
            else if (line[0] == 'v' && line[1] == 'n') // this line is a normal
            {
                if ((read = sscanf(line, "%*s %f %f %f", &vec.x, &vec.y, &vec.z)) == 4)
                {
                    setMeshNormal(mesh, normalNum++, vec);
                }
                else
                {
                    failed = 1;
                    sprintf(errorMsg, "Failed: Parsing normal %d from file %s failed.", normalNum, fileName);
                }
            }
            else if (line[0] == 'f' && line[1] == ' ') // this line is a face
            {
                if ((read = sscanf(line, "%*s %hd//%*hd %hd//%*hd %hd//%hd",
                                &face.indices[0], &face.indices[1], &face.indices[2], &face.normal)) == 7)
                {
                    face.indices[0]--; // the file uses indices starting from 1, but
                    face.indices[1]--; //     this program uses indices starting from 0
                    face.indices[2]--; //     so the indices read from the file have to
                    face.normal--;     //     be decremented by 1
                    setMeshFace(mesh, faceNum++, face);
                }
                else
                {
                    failed = 1;
                    sprintf(errorMsg, "Failed: Parsing face %d from file %s failed.", faceNum, fileName);
                }
            }

            if (failed) // something went wrong
            {
                free(mesh); // free the allocated memory
                fclose(f);  // close the file
                DEBUG_MSG_FROM(errorMsg, "readMeshFromFile"); // log the error message
                return NULL;
            }
        }
        else
            break;
    }

    fclose(f);

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
    Triangle tris;
    float shading;
    Matrix4x4 viewMatrix = createLookAtMatrix(camera->position, camera->target, createVector3(0.0f, 1.0f, 0.0f));
    Matrix4x4 projectionMatrix =
        createPerspectiveMatrix(PI/3.0f, screen->width / (float)screen->height, 0.1f, 100.0f);

    Matrix4x4 worldMatrix, tempMatrix, transformMatrix;
    Vector3 invertedCamera, projectedVertex;

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

    invertedCamera = transformVector3ByMatrix(camera->position, Invert(worldMatrix));

    setCameraFrustum(camera, transformMatrix);

    // reset the array of projections
    for (i = 0; i < mesh->vertexCount; i++)
    {
        mesh->vertexProjections[i] = project(screen->width, screen->height, mesh->vertices[i], transformMatrix, &projectedVertex);
    }

    for (i = 0; i < mesh->faceCount; i ++)
    {
        if (flags & BACKFACE_CULLING &&
                dotProductVector3(
                    subtractVector3(mesh->vertices[mesh->faces[i].indices[0]], invertedCamera), mesh->normals[mesh->faces[i].normal]) >= 0.0f)
        {
            /*setTriangleInPool(&trianglePool, mesh->faces[i].poolIndex, 0, trianglePool.triangles[mesh->faces[i].poolIndex].shading,
                trianglePool.triangles[mesh->faces[i].poolIndex].faceDist);*/
            trianglePool.triangles[mesh->faces[i].poolIndex].drawState = 0;
            continue;
        }

        // pre-optimized version called project() once for every vertex
        // of every face, amounting to total    2904 times
        // for the suzanne.obj model that has    507 vertices

        // optimized version calls project() only once for each vertex
        // of the mesh, amounting to total       507 times, logically

        // one call of project() amounts to       20 multiplications/divisions,
        // which means that every frame        58080 multiplications/divisions took place
        // instead of the minimum required     10140
        /*for (j = 0; j < 3; j ++)
        {
            if (mesh->vertexProjections[mesh->faces[i].indices[j]].x <= 0.0f)
            {*/
                //if (sign(projectedVertex.y) != sign(mesh->vertices[mesh->faces[i].indices[j]].y))
                    /*if (projectedVertex.x < -0.5 || projectedVertex.x > 0.5 ||
                        projectedVertex.y < -0.5 || projectedVertex.y > 0.5)*/
                //if (projectedVertex.z < 0.01f)
                //if (!pointInCameraFrustum(camera, projectedVertex)/* || projectedVertex.x < -0.5 || projectedVertex.x > 0.5 || projectedVertex.y < -0.5 || projectedVertex.y > 0.5*/)
                /*{
                    trianglePool.triangles[mesh->faces[i].poolIndex].drawState = 0;
                    goto SKIP;
                }
            }
        }*/

        {
            Vector3 vec1;
            Vector3 vec2 = createVector3(0.0f, 0.0f, 1.0f);

            vec1 = mesh->normals[mesh->faces[i].normal];
            // vec2 = transformVector3ByMatrix(createVector3(10*sin(frame/80.0), 10, 10*cos(frame/80.0)), Invert(worldMatrix));
            vec2 = transformVector3ByMatrix(createVector3(camera->position.x, camera->position.y, camera->position.z), Invert(worldMatrix));

            shading = max(0.0f, dotProductVector3(vec1, vec2) / (magnitudeVector3(vec1) * magnitudeVector3(vec2)));

            setTriangleInPool(&trianglePool, mesh->faces[i].poolIndex, 1, shading, dotProductVector3(
                transformVector3ByMatrix(mesh->vertices[mesh->faces[i].indices[0]], worldMatrix), camera->position));
        }
        /*tris.p1 = mesh->vertexProjections[mesh->faces[i].indices[0]];
        tris.p2 = mesh->vertexProjections[mesh->faces[i].indices[1]];
        tris.p3 = mesh->vertexProjections[mesh->faces[i].indices[2]];

        fillTriangle(tris, 0, 255 * shading, 0);

        // if mode is 1 or 2, draw the lines between the vertices
        if (mode && mode < 3)
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
        */


        /*setpen(255 * shading, 0, 255 * shading, 0, 5);
        moveto(vertices[0].x, vertices[0].y);
        lineto(vertices[1].x, vertices[1].y);
        lineto(vertices[2].x, vertices[2].y);
        lineto(vertices[0].x, vertices[0].y);*/

        /*if (i == inspectFace)
        {
            setpen(0, 255, 255, 0, 3);
            moveto(vertices[0].x, vertices[0].y);
            lineto(vertices[1].x, vertices[1].y);
            lineto(vertices[2].x, vertices[2].y);
            lineto(vertices[0].x, vertices[0].y);
        }*/

        //SKIP:
    }
}

void fillTriangle(Triangle triangle, float rr, float gg, float bb)
{
    float h, x, y, ang, size, sideLength;
    float a, b, c, a2, b2, c2, ang1, ang2, ang3;
    const float triangleSize = 150.0f;

    Triangle tri2;

    Vector3 vec = crossProductVector3(
        subtractVector3(createVector3(triangle.p2.x, triangle.p2.y, 0),
                        createVector3(triangle.p1.x, triangle.p1.y, 0)),
        subtractVector3(createVector3(triangle.p3.x, triangle.p3.y, 0),
                        createVector3(triangle.p1.x, triangle.p1.y, 0)));
    if (vec.z > 0.0f)
    {
        Triangle temp;
        temp = triangle;
        triangle.p2 = temp.p3;
        triangle.p3 = temp.p2;
    }

    a = distance(triangle.p1.x, triangle.p1.y, triangle.p2.x, triangle.p2.y);
    b = distance(triangle.p2.x, triangle.p2.y, triangle.p3.x, triangle.p3.y);
    c = distance(triangle.p3.x, triangle.p3.y, triangle.p1.x, triangle.p1.y);

    a2 = a * a;
    b2 = b * b;
    c2 = c * c;

    ang1 = acos((a2 + c2 - b2) / max(0.0001f, (float)(2.0f*a*c))); // illegal division by 0
    ang2 = acos((a2 + b2 - c2) / max(0.0001f, (float)(2.0f*a*b)));
    ang3 = acos((b2 + c2 - a2) / max(0.0001f, (float)(2.0f*b*c)));

    SET_TRIANGLE(0, 0, rr, gg, bb);
    SendActivationEvent("tri.0");

    switch (largestOf3(ang1, ang2, ang3))
    {
        case 0:
            tri2 = triangle;
            break;

        case 1:
            tri2.p1 = triangle.p2;
            tri2.p2 = triangle.p3;
            tri2.p3 = triangle.p1;
            break;

        case 2:
            tri2.p1 = triangle.p3;
            tri2.p2 = triangle.p1;
            tri2.p3 = triangle.p2;
            break;
    }

    a = distance(tri2.p1.x, tri2.p1.y, tri2.p2.x, tri2.p2.y);
    b = distance(tri2.p2.x, tri2.p2.y, tri2.p3.x, tri2.p3.y);
    c = distance(tri2.p3.x, tri2.p3.y, tri2.p1.x, tri2.p1.y);

    a2 = a * a;
    b2 = b * b;
    c2 = c * c;

    ang1 = acos((a2 + c2 - b2) / max(0.0001f, (float)(2.0f*a*c)));
    ang2 = acos((a2 + b2 - c2) / max(0.0001f, (float)(2.0f*a*b)));
    ang3 = acos((b2 + c2 - a2) / max(0.0001f, (float)(2.0f*b*c)));

    ang = degtorad(direction(tri2.p2.x, tri2.p2.y, tri2.p3.x, tri2.p3.y));
    h = distance(tri2.p1.x, tri2.p1.y, tri2.p2.x, tri2.p2.y);
    sideLength = cos(ang2) * h;

    x = cos(ang) * sideLength;
    y = sin(ang) * sideLength;

    size = (ang3 <= PI / 4.0f)
        ?distance(tri2.p2.x + x, tri2.p2.y - y, tri2.p3.x, tri2.p3.y)/triangleSize
        :distance(tri2.p2.x + x, tri2.p2.y - y, tri2.p1.x, tri2.p1.y)/triangleSize;

    SET_TRIANGLE((radtodeg(ang3) / 2.0f), (direction(tri2.p2.x + x, tri2.p2.y - y,
                                                   tri2.p3.x, tri2.p3.y)/2.0f),
                                                   rr, gg, bb);

    // Draw around the triangle to fix holes left between triangles
    /*setpen(rr, gg, bb, 0, 5);
    moveto(tri2.p2.x + x, tri2.p2.y - y);
    lineto(tri2.p1.x, tri2.p1.y);
    lineto(tri2.p2.x, tri2.p2.y);
    lineto(tri2.p3.x, tri2.p3.y);
    lineto(tri2.p1.x, tri2.p1.y);*/

    SendActivationEvent("tri.0");
    draw_from("tri", tri2.p2.x + x, tri2.p2.y - y, size);

    size = ((ang1 - (PI / 2.0f - ang3)) <= PI / 4.0f)
        ?distance(tri2.p2.x + x, tri2.p2.y - y, tri2.p1.x, tri2.p1.y)/triangleSize
        :distance(tri2.p2.x + x, tri2.p2.y - y, tri2.p2.x, tri2.p2.y)/triangleSize;

     SET_TRIANGLE(((radtodeg(ang1) - (90.0f - radtodeg(ang3))) / 2.0f),
        (direction(tri2.p2.x + x, tri2.p2.y - y,
                        tri2.p1.x, tri2.p1.y)/2.0f),
                        rr, gg, bb);
    SendActivationEvent("tri.0");
    draw_from("tri", tri2.p2.x + x, tri2.p2.y - y, size);

    /*setpen(rr, gg, bb, 0, 2);
    moveto(tri2.p2.x + x, tri2.p2.y - y);
    lineto(tri2.p1.x, tri2.p1.y);*/
    //setpen(0, 255, 0, 0, 1);
}

void destroyMesh(Mesh *mesh)
{
    if (!mesh) return;

    free(mesh->vertices);
    free(mesh->vertexProjections);
    free(mesh->faces);
    free(mesh);
}



void createPool(TrianglePool *this, int maxTriCount)
{
    if (this)
    {
        this->triangles = malloc(sizeof (TriangleObj) * maxTriCount);

        if (!this->triangles)
        {
            DEBUG_MSG("Couldn't allocate triangle pool.");
        }

        this->triCount = 0;
        this->maxTriCount = maxTriCount;
    }
}

void addMeshFacesToPool(TrianglePool *tp, Mesh *mesh)
{
    if (tp && tp->triangles && tp->triCount < tp->maxTriCount)
    {
        int i;

        for (i = 0; i < mesh->faceCount; i++)
        {
            addTriangleToPool(tp, mesh, &mesh->faces[i]);
        }
    }
}

void addTriangleToPool(TrianglePool *tp, Mesh *mesh, Face *face)
{
    if (tp && tp->triangles)
    {
        TriangleObj *temp = &tp->triangles[tp->triCount];
        temp->mesh = mesh;
        temp->drawState = 1;
        temp->face = face;
        temp->shading = 0.0f;
        temp->faceDist = 0.0f;
        temp->face->poolIndex = tp->triCount++;
    }
}

void setTriangleInPool(TrianglePool *tp, int index, short drawState, float shading, float faceDist)
{
    if (tp && tp->triangles && index < tp->triCount && index >= 0)
    {
        TriangleObj *temp = &tp->triangles[index];
        temp->drawState = drawState;
        temp->shading = shading;
        temp->faceDist = faceDist;
    }
}

void resetTrianglePool(TrianglePool *tp)
{
    if (tp)
    {
        tp->triCount = 0;
    }
}

void drawTrianglesFromPool(TrianglePool *tp)
{
    int i;
    Triangle tri;
    TriangleObj to;

    if (tp && tp->triangles)
    {
        for (i = 0; i < tp->triCount; i++)
        {
            to = tp->triangles[i];

            if (to.drawState)
            {
                tri.p1 = to.mesh->vertexProjections[to.face->indices[0]];
                tri.p2 = to.mesh->vertexProjections[to.face->indices[1]];
                tri.p3 = to.mesh->vertexProjections[to.face->indices[2]];

                fillTriangle(tri, 0.0f, floor(255.0f * to.shading), 0.0f);
            }
        }
    }

    // resetTrianglePool(tp);
}

void sortTrianglePoolInsertion(TrianglePool *tp)
{
    int i = 1;
    int j;
    TriangleObj temp;

    while (i < tp->triCount)
    {
        j = i;

        while (j > 0 && tp->triangles[j - 1].faceDist > tp->triangles[j].faceDist)
        {
            temp = tp->triangles[j - 1];
            tp->triangles[j - 1] = tp->triangles[j];
            tp->triangles[j - 1].face->poolIndex = j - 1;
            tp->triangles[j] = temp;
            tp->triangles[j].face->poolIndex = j;
            j--;
        }

        i++;
    }
}

void freeTrianglePool(TrianglePool *tp)
{
    if (tp && tp->triangles)
    {
        free(tp->triangles);
    }
}
