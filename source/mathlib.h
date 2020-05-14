
#ifndef TRI3D_MATH_H
#define TRI3D_MATH_H

typedef struct Point2DStruct
{
    int x;
    int y;
}Point2D;

typedef struct Vector3Struct
{
    float x;
    float y;
    float z;
}Vector3;

typedef struct QuaternionStruct
{
    float x;
    float y;
    float z;
    float w;
}Quaternion;

typedef struct Matrix4x4Struct
{
    float m11;
    float m12;
    float m13;
    float m14;

    float m21;
    float m22;
    float m23;
    float m24;

    float m31;
    float m32;
    float m33;
    float m34;

    float m41;
    float m42;
    float m43;
    float m44;
}Matrix4x4;

int diffSign(float val1, float val2, float err);
int largestOf3(float val1, float val2, float val3);
Point2D createPoint2D(int x, int y);
Point2D project(short width, short height, Vector3 vertex, Matrix4x4 projectionMatrix, Vector3 *out);
Vector3 createVector3(float x, float y, float z);
Vector3 scaleVector3(Vector3 vector, float scale);
Vector3 normalizeVector3(Vector3 vector);
Vector3 subtractVector3(Vector3 a, Vector3 b);
Vector3 crossProductVector3(Vector3 a, Vector3 b);
float dotProductVector3(Vector3 a, Vector3 b);
float magnitudeVector3(Vector3 vector);
Vector3 transformVector3ByMatrix(Vector3 vector, Matrix4x4 matrix);
Quaternion createQuaternion(float x, float y, float z, float w);
Quaternion vectorToQuaternion(Vector3 vector, float scalar);
Matrix4x4 createLookAtMatrix(Vector3 cameraPosition, Vector3 cameraTarget, Vector3 cameraUp);
Matrix4x4 createRotationXYZMatrix(float x, float y, float z);
Matrix4x4 createPerspectiveMatrix(float fov, float aspectRatio, float near, float far);
Matrix4x4 createTranslationMatrix(float x, float y, float z);
Matrix4x4 multiplyMatrices(Matrix4x4 a, Matrix4x4 b);

#endif
