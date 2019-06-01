typedef struct Point2DStruct
{
    float x;
    float y;
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
    float matrix[4][4];
}Matrix4x4;

const Matrix4x4 emptyMatrix;

Point2D createPoint2D(float x, float y);
Point2D project(short width, short height, Vector3 vertex, Matrix4x4 projectionMatrix);
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

Point2D createPoint2D(float x, float y)
{
    Point2D p;

    p.x = x;
    p.y = y;

    return p;
}

Point2D project(short width, short height, Vector3 vertex, Matrix4x4 projectionMatrix)
{
    Point2D result;
    Vector3 transformed = transformVector3ByMatrix(vertex, projectionMatrix);

    result.x = transformed.x * width + width / 2.0f;
    result.y = -transformed.y * height + height / 2.0f;

    return result;
}

Vector3 createVector3(float x, float y, float z)
{
    Vector3 vector;

    vector.x = x;
    vector.y = y;
    vector.z = z;

    return vector;
}

Vector3 scaleVector3(Vector3 vector, float scale)
{
    Vector3 result;

    result.x = vector.x * scale;
    result.y = vector.y * scale;
    result.z = vector.z * scale;

    return result;
}

Vector3 normalizeVector3(Vector3 vector)
{
    float magnitude = magnitudeVector3(vector);

    if (abs(magnitude) <= 0.0001f)
    {
        DEBUG_MSG_FROM("Failed: Vector magnitude was 0.", "normalizeVector3");
        return createVector3(0.0f, 0.0f, 0.0f);
    }

    return scaleVector3(vector, 1.0f / magnitude);
}

Vector3 subtractVector3(Vector3 a, Vector3 b)
{
    return createVector3(a.x - b.x, a.y - b.y, a.z - b.z);
}

Vector3 crossProductVector3(Vector3 a, Vector3 b)
{
    Vector3 vector;

    vector.x = a.y * b.z - a.z * b.y;
    vector.y = a.z * b.x - a.x * b.z;
    vector.z = a.x * b.y - a.y * b.x;

    return vector;
}

float dotProductVector3(Vector3 a, Vector3 b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

float magnitudeVector3(Vector3 vector)
{
    return sqrt(dotProductVector3(vector, vector));
}

Vector3 transformVector3ByMatrix(Vector3 vector, Matrix4x4 matrix)
{
    float divisor;
    Quaternion quaternion;
    Vector3 result;

    quaternion.x = matrix.matrix[0][0] * vector.x + matrix.matrix[1][0] * vector.y +
                   matrix.matrix[2][0] * vector.z + matrix.matrix[3][0];
    quaternion.y = matrix.matrix[0][1] * vector.x + matrix.matrix[1][1] * vector.y +
                   matrix.matrix[2][1] * vector.z + matrix.matrix[3][1];
    quaternion.z = matrix.matrix[0][2] * vector.x + matrix.matrix[1][2] * vector.y +
                   matrix.matrix[2][2] * vector.z + matrix.matrix[3][2];

    divisor = matrix.matrix[0][3] * vector.x + matrix.matrix[1][3] * vector.y +
              matrix.matrix[2][3] * vector.z + matrix.matrix[3][3];

    if (abs(divisor) < 0.0001f)
    {
        divisor = 0.0001f;
        DEBUG_MSG_FROM("Failed: Can't divide by 0. CameraPosition and Vertex are equal!",
                       "transformVector3ByMatrix");
    }

    quaternion.w = 1.0f / divisor;

    result = createVector3(quaternion.x * quaternion.w, quaternion.y * quaternion.w, quaternion.z * quaternion.w);

    return result;
}

Quaternion createQuaternion(float x, float y, float z, float w)
{
    Quaternion q;

    q.x = x;
    q.y = y;
    q.z = z;
    q.w = w;

    return q;
}

Quaternion vectorToQuaternion(Vector3 vector, float scalar)
{
    Quaternion q;

    q.x = vector.x;
    q.y = vector.y;
    q.z = vector.z;
    q.w = scalar;

    return q;
}

Matrix4x4 createLookAtMatrix(Vector3 cameraPosition, Vector3 cameraTarget, Vector3 cameraUp)
{
    Matrix4x4 result;

    Vector3 zAxis = normalizeVector3(subtractVector3(cameraPosition, cameraTarget));
    Vector3 xAxis = normalizeVector3(crossProductVector3(cameraUp, zAxis));
    Vector3 yAxis = crossProductVector3(zAxis, xAxis);

    result.matrix[0][0] =  xAxis.x;
    result.matrix[0][1] =  yAxis.x;
    result.matrix[0][2] =  zAxis.x;
    result.matrix[0][3] =  0.0f;

    result.matrix[1][0] =  xAxis.y;
    result.matrix[1][1] =  yAxis.y;
    result.matrix[1][2] =  zAxis.y;
    result.matrix[1][3] =  0.0f;

    result.matrix[2][0] =  xAxis.z;
    result.matrix[2][1] =  yAxis.z;
    result.matrix[2][2] =  zAxis.z;
    result.matrix[2][3] =  0.0f;

    result.matrix[3][0] = -dotProductVector3(xAxis, cameraPosition);
    result.matrix[3][1] = -dotProductVector3(yAxis, cameraPosition);
    result.matrix[3][2] = -dotProductVector3(zAxis, cameraPosition);
    result.matrix[3][3] =  1.0f;

    return result;
}

Matrix4x4 createRotationXYZMatrix(float x, float y, float z)
{
    Matrix4x4 result;

    float xcos, ycos, zcos;
    float xsin, ysin, zsin;
    float zcosxsin, xcoszcos, ysinzsin;

    xcos = cos(x);
    xsin = sin(x);
    ycos = cos(y);
    ysin = sin(y);
    zcos = cos(z);
    zsin = sin(z);

    zcosxsin = zcos * xsin;
    xcoszcos = xcos * zcos;
    ysinzsin = ysin * zsin;

    result.matrix[0][0] =  ycos * zcos;
    result.matrix[0][1] =  zcosxsin * ysin + xcos * zsin;
    result.matrix[0][2] =  -xcoszcos * ysin + xsin * zsin;
    result.matrix[0][3] =  0.0f;

    result.matrix[1][0] =  -ycos * zsin;
    result.matrix[1][1] =  xcoszcos - xsin * ysinzsin;
    result.matrix[1][2] =  zcosxsin + xcos * ysinzsin;
    result.matrix[1][3] =  0.0f;

    result.matrix[2][0] =  ysin;
    result.matrix[2][1] =  -ycos * xsin;
    result.matrix[2][2] =  xcos * ycos;
    result.matrix[2][3] =  0.0f;

    result.matrix[3][0] =  0.0f;
    result.matrix[3][1] =  0.0f;
    result.matrix[3][2] =  0.0f;
    result.matrix[3][3] =  1.0f;

    return result;
}

Matrix4x4 createPerspectiveMatrix(float fov, float aspectRatio, float near, float far)
{
    float yScale = 1.0f / tan(fov * 0.5f);
    float xScale = yScale / aspectRatio;

    Matrix4x4 result;

    if (fov <= 0.0f || fov >= PI)
    {
        char temp[32];
        sprintf(temp, "Failed: Invalid fov: %f.", fov);
        DEBUG_MSG_FROM(temp, "createPerspectiveMatrix");
        return emptyMatrix;
    }
    if (near <= 0.0f)
    {
        char temp[32];
        sprintf(temp, "Failed: Invalid near: %f.", near);
        DEBUG_MSG_FROM(temp, "createPerspectiveMatrix");
        return emptyMatrix;
    }
    if (far <= 0.0f)
    {
        char temp[32];
        sprintf(temp, "Failed: Invalid far: %f.", far);
        DEBUG_MSG_FROM(temp, "createPerspectiveMatrix");
        return emptyMatrix;
    }
    if (near >= far)
    {
        char temp[32];
        sprintf(temp, "Failed: Invalid near: %f.", near);
        DEBUG_MSG_FROM(temp, "createPerspectiveMatrix");
        return emptyMatrix;
    }

    result.matrix[0][0] = xScale;
    result.matrix[0][1] = result.matrix[0][2] = result.matrix[0][3] = 0.0f;

    result.matrix[1][1] = yScale;
    result.matrix[1][0] = result.matrix[1][2] = result.matrix[1][3] = 0.0f;

    result.matrix[2][2] = far / (near - far);
    result.matrix[2][3] = -1.0f;
    result.matrix[2][0] = result.matrix[2][1] = 0.0f;

    result.matrix[3][2] = near * far / (near - far);
    result.matrix[3][0] = result.matrix[3][1] = result.matrix[3][3] = 0.0f;

    return result;
}

Matrix4x4 createTranslationMatrix(float x, float y, float z)
{
    Matrix4x4 result;

    result.matrix[0][0] = 1.0f;
    result.matrix[0][1] = 0.0f;
    result.matrix[0][2] = 0.0f;
    result.matrix[0][3] = 0.0f;

    result.matrix[1][0] = 0.0f;
    result.matrix[1][1] = 1.0f;
    result.matrix[1][2] = 0.0f;
    result.matrix[1][3] = 0.0f;

    result.matrix[2][0] = 0.0f;
    result.matrix[2][1] = 0.0f;
    result.matrix[2][2] = 1.0f;
    result.matrix[2][3] = 0.0f;

    result.matrix[3][0] = x;
    result.matrix[3][1] = y;
    result.matrix[3][2] = z;
    result.matrix[3][3] = 1.0f;

    return result;
}

Matrix4x4 multiplyMatrices(Matrix4x4 a, Matrix4x4 b)
{
    Matrix4x4 result;

    result.matrix[0][0] = a.matrix[0][0] * b.matrix[0][0] + a.matrix[0][1] * b.matrix[1][0] + a.matrix[0][2] * b.matrix[2][0] + a.matrix[0][3] * b.matrix[3][0];
    result.matrix[0][1] = a.matrix[0][0] * b.matrix[0][1] + a.matrix[0][1] * b.matrix[1][1] + a.matrix[0][2] * b.matrix[2][1] + a.matrix[0][3] * b.matrix[3][1];
    result.matrix[0][2] = a.matrix[0][0] * b.matrix[0][2] + a.matrix[0][1] * b.matrix[1][2] + a.matrix[0][2] * b.matrix[2][2] + a.matrix[0][3] * b.matrix[3][2];
    result.matrix[0][3] = a.matrix[0][0] * b.matrix[0][3] + a.matrix[0][1] * b.matrix[1][3] + a.matrix[0][2] * b.matrix[2][3] + a.matrix[0][3] * b.matrix[3][3];

    result.matrix[1][0] = a.matrix[1][0] * b.matrix[0][0] + a.matrix[1][1] * b.matrix[1][0] + a.matrix[1][2] * b.matrix[2][0] + a.matrix[1][3] * b.matrix[3][0];
    result.matrix[1][1] = a.matrix[1][0] * b.matrix[0][1] + a.matrix[1][1] * b.matrix[1][1] + a.matrix[1][2] * b.matrix[2][1] + a.matrix[1][3] * b.matrix[3][1];
    result.matrix[1][2] = a.matrix[1][0] * b.matrix[0][2] + a.matrix[1][1] * b.matrix[1][2] + a.matrix[1][2] * b.matrix[2][2] + a.matrix[1][3] * b.matrix[3][2];
    result.matrix[1][3] = a.matrix[1][0] * b.matrix[0][3] + a.matrix[1][1] * b.matrix[1][3] + a.matrix[1][2] * b.matrix[2][3] + a.matrix[1][3] * b.matrix[3][3];

    result.matrix[2][0] = a.matrix[2][0] * b.matrix[0][0] + a.matrix[2][1] * b.matrix[1][0] + a.matrix[2][2] * b.matrix[2][0] + a.matrix[2][3] * b.matrix[3][0];
    result.matrix[2][1] = a.matrix[2][0] * b.matrix[0][1] + a.matrix[2][1] * b.matrix[1][1] + a.matrix[2][2] * b.matrix[2][1] + a.matrix[2][3] * b.matrix[3][1];
    result.matrix[2][2] = a.matrix[2][0] * b.matrix[0][2] + a.matrix[2][1] * b.matrix[1][2] + a.matrix[2][2] * b.matrix[2][2] + a.matrix[2][3] * b.matrix[3][2];
    result.matrix[2][3] = a.matrix[2][0] * b.matrix[0][3] + a.matrix[2][1] * b.matrix[1][3] + a.matrix[2][2] * b.matrix[2][3] + a.matrix[2][3] * b.matrix[3][3];

    result.matrix[3][0] = a.matrix[3][0] * b.matrix[0][0] + a.matrix[3][1] * b.matrix[1][0] + a.matrix[3][2] * b.matrix[2][0] + a.matrix[3][3] * b.matrix[3][0];
    result.matrix[3][1] = a.matrix[3][0] * b.matrix[0][1] + a.matrix[3][1] * b.matrix[1][1] + a.matrix[3][2] * b.matrix[2][1] + a.matrix[3][3] * b.matrix[3][1];
    result.matrix[3][2] = a.matrix[3][0] * b.matrix[0][2] + a.matrix[3][1] * b.matrix[1][2] + a.matrix[3][2] * b.matrix[2][2] + a.matrix[3][3] * b.matrix[3][2];
    result.matrix[3][3] = a.matrix[3][0] * b.matrix[0][3] + a.matrix[3][1] * b.matrix[1][3] + a.matrix[3][2] * b.matrix[2][3] + a.matrix[3][3] * b.matrix[3][3];

    return result;
}
