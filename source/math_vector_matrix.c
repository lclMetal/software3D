typedef struct Vector3Struct
{
    double x;
    double y;
    double z;
}Vector3;

typedef struct QuaternionStruct
{
    double x;
    double y;
    double z;
    double w;
}Quaternion;

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

Screen *screen;

const Matrix4x4 emptyMatrix;

Vector3 transformVector3ByMatrix4x4(Vector3 vector, Matrix4x4 matrix);

Vector3 createVector3(double x, double y, double z)
{
    Vector3 vector;

    vector.x = x;
    vector.y = y;
    vector.z = z;

    return vector;
}

Quaternion createQuaternion(double x, double y, double z, double w)
{
    Quaternion q;

    q.x = x;
    q.y = y;
    q.z = z;
    q.w = w;

    return q;
}

Quaternion vectorToQuaternion(Vector3 vector, double scalar)
{
    Quaternion q;

    q.x = vector.x;
    q.y = vector.y;
    q.z = vector.z;
    q.w = scalar;

    return q;
}

double dotProductVector3(Vector3 a, Vector3 b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
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

double magnitudeVector3(Vector3 vector)
{
    return sqrt(dotProductVector3(vector, vector));
}

Vector3 scaleVector3(Vector3 vector, double scale)
{
    Vector3 result;

    result.x = vector.x * scale;
    result.y = vector.y * scale;
    result.z = vector.z * scale;

    return result;
}

Vector3 normalizeVector3(Vector3 vector)
{
    double magnitude = magnitudeVector3(vector);

    if (abs(magnitude) <= 0.0001)
    {
        DEBUG_MSG_FROM("Failed: Vector magnitude was 0.", "normalizeVector3");
        return createVector3(0.0, 0.0, 0.0);
    }

    return scaleVector3(vector, 1.0 / magnitude);
}

Point2D createPoint2D(double px, double py)
{
    Point2D p;

    p.x = px;
    p.y = py;

    return p;
}

Point2D project(Screen *screen, Vector3 vertex, Matrix4x4 matrix)
{
    Point2D result;
    Vector3 transformed = transformVector3ByMatrix4x4(vertex, matrix);

    result.x = transformed.x * screen->width + screen->width / 2.0;
    result.y = -transformed.y * screen->height + screen->height / 2.0;

    return result;
}

Matrix4x4 lookAt(Vector3 cameraPosition, Vector3 cameraTarget, Vector3 cameraUp)
{
    Matrix4x4 result;

    Vector3 zAxis = normalizeVector3(subtractVector3(cameraPosition, cameraTarget));
    Vector3 xAxis = normalizeVector3(crossProductVector3(cameraUp, zAxis));
    Vector3 yAxis = crossProductVector3(zAxis, xAxis);

    result.matrix[0][0] =  xAxis.x;
    result.matrix[0][1] =  yAxis.x;
    result.matrix[0][2] =  zAxis.x;
    result.matrix[0][3] =  0.0;

    result.matrix[1][0] =  xAxis.y;
    result.matrix[1][1] =  yAxis.y;
    result.matrix[1][2] =  zAxis.y;
    result.matrix[1][3] =  0.0;

    result.matrix[2][0] =  xAxis.z;
    result.matrix[2][1] =  yAxis.z;
    result.matrix[2][2] =  zAxis.z;
    result.matrix[2][3] =  0.0;

    result.matrix[3][0] = -dotProductVector3(xAxis, cameraPosition);
    result.matrix[3][1] = -dotProductVector3(yAxis, cameraPosition);
    result.matrix[3][2] = -dotProductVector3(zAxis, cameraPosition);
    result.matrix[3][3] =  1.0;

    return result;
}

Matrix4x4 rotationXYZ(double x, double y, double z)
{
    Matrix4x4 result;

    double xcos, ycos, zcos;
    double xsin, ysin, zsin;
    double zcosxsin, xcoszcos, ysinzsin;

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
    result.matrix[0][3] =  0.0;

    result.matrix[1][0] =  -ycos * zsin;
    result.matrix[1][1] =  xcoszcos - xsin * ysinzsin;
    result.matrix[1][2] =  zcosxsin + xcos * ysinzsin;
    result.matrix[1][3] =  0.0;

    result.matrix[2][0] =  ysin;
    result.matrix[2][1] =  -ycos * xsin;
    result.matrix[2][2] =  xcos * ycos;
    result.matrix[2][3] =  0.0;

    result.matrix[3][0] =  0.0;
    result.matrix[3][1] =  0.0;
    result.matrix[3][2] =  0.0;
    result.matrix[3][3] =  1.0;

    return result;
}

Matrix4x4 perspectiveFov(double fov, double aspectRatio, double near, double far)
{
    double yScale = 1.0 / (double)tan(fov * 0.5);
    double xScale = yScale / aspectRatio;

    Matrix4x4 result;

    if (fov <= 0.0 || fov >= PI)
    {
        char temp[32];
        sprintf(temp, "invalid fov: %f", fov);
        DEBUG_MSG_FROM(temp, "perspectiveFov");
        return emptyMatrix;
    }
    if (near <= 0.0)
    {
        char temp[32];
        sprintf(temp, "invalid near: %f", near);
        DEBUG_MSG_FROM(temp, "perspectiveFov");
        return emptyMatrix;
    }
    if (far <= 0.0)
    {
        char temp[32];
        sprintf(temp, "invalid far: %f", far);
        DEBUG_MSG_FROM(temp, "perspectiveFov");
        return emptyMatrix;
    }
    if (near >= far)
    {
        char temp[32];
        sprintf(temp, "invalid near: %f", near);
        DEBUG_MSG_FROM(temp, "perspectiveFov");
        return emptyMatrix;
    }

    result.matrix[0][0] = xScale;
    result.matrix[0][1] = result.matrix[0][2] = result.matrix[0][3] = 0.0;

    result.matrix[1][1] = yScale;
    result.matrix[1][0] = result.matrix[1][2] = result.matrix[1][3] = 0.0;

    result.matrix[2][2] = far / (near - far);
    result.matrix[2][3] = -1.0;
    result.matrix[2][0] = result.matrix[2][1] = 0.0;

    result.matrix[3][2] = near * far / (near - far);
    result.matrix[3][0] = result.matrix[3][1] = result.matrix[3][3] = 0.0;

    return result;
}

Matrix4x4 translation(double x, double y, double z)
{
    Matrix4x4 result;

    result.matrix[0][0] = 1.0;
    result.matrix[0][1] = 0.0;
    result.matrix[0][2] = 0.0;
    result.matrix[0][3] = 0.0;

    result.matrix[1][0] = 0.0;
    result.matrix[1][1] = 1.0;
    result.matrix[1][2] = 0.0;
    result.matrix[1][3] = 0.0;

    result.matrix[2][0] = 0.0;
    result.matrix[2][1] = 0.0;
    result.matrix[2][2] = 1.0;
    result.matrix[2][3] = 0.0;

    result.matrix[3][0] = x;
    result.matrix[3][1] = y;
    result.matrix[3][2] = z;
    result.matrix[3][3] = 1.0;

    return result;
}

Vector3 transformVector3ByMatrix4x4(Vector3 vector, Matrix4x4 matrix)
{
    Quaternion quaternion;
    Vector3 result;

    quaternion.x = matrix.matrix[0][0] * vector.x + matrix.matrix[1][0] * vector.y +
                   matrix.matrix[2][0] * vector.z + matrix.matrix[3][0];
    quaternion.y = matrix.matrix[0][1] * vector.x + matrix.matrix[1][1] * vector.y +
                   matrix.matrix[2][1] * vector.z + matrix.matrix[3][1];
    quaternion.z = matrix.matrix[0][2] * vector.x + matrix.matrix[1][2] * vector.y +
                   matrix.matrix[2][2] * vector.z + matrix.matrix[3][2];
    quaternion.w = 1.0 / (matrix.matrix[0][3] * vector.x + matrix.matrix[1][3] * vector.y +
                          matrix.matrix[2][3] * vector.z + matrix.matrix[3][3]);
    result = createVector3(quaternion.x * quaternion.w, quaternion.y * quaternion.w, quaternion.z * quaternion.w);

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