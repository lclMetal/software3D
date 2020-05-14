
#include "source/mathlib.h"

#ifndef TRI3D_MAIN_H
#define TRI3D_MAIN_H

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

typedef struct TriangleStruct
{
    Point2D p1;
    Point2D p2;
    Point2D p3;
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
void fillTriangle(Triangle triangle, unsigned char r, unsigned char g, unsigned char b);
void destroyMesh(Mesh *mesh);

void createPool(TrianglePool *this, int maxTriCount);
void addMeshFacesToPool(TrianglePool *tp, Mesh *mesh);
void addTriangleToPool(TrianglePool *tp, Mesh *mesh, Face *face);
void setTriangleInPool(TrianglePool *tp, int index, short drawState, float shading, float faceDist);
void resetTrianglePool(TrianglePool *tp);
void drawTrianglesFromPool(TrianglePool *tp);
void sortTrianglePoolInsertion(TrianglePool *tp);
void freeTrianglePool(TrianglePool *tp);

#endif
