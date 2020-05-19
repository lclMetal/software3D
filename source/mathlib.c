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
Vector3 project(short width, short height, Vector3 vertex, Matrix4x4 projectionMatrix, Vector3 *out);
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

const Matrix4x4 emptyMatrix;

int diffSign(float val1, float val2, float err)
{
    float difference = val1 - val2;

    if (difference < -err) return -1;
    if (difference >  err) return 1;
    return 0;
}

int largestOf3(float val1, float val2, float val3)
{
    float err = 0.001f;
    float largest = max(val1, max(val2, val3));

    if (!diffSign(largest, val1, err)) return 0;
    if (!diffSign(largest, val2, err)) return 1;
    return 2;
}

Point2D createPoint2D(int x, int y)
{
    Point2D p;

    p.x = x;
    p.y = y;

    return p;
}

Vector3 project(short width, short height, Vector3 vertex, Matrix4x4 projectionMatrix, Vector3 *out)
{
    Vector3 result;
    Vector3 transformed = transformVector3ByMatrix(vertex, projectionMatrix);

    result.x = transformed.x * width + width / 2.0f;
    result.y = -transformed.y * height + height / 2.0f;
    result.z = transformed.z;
    *out = transformed;

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
        // DEBUG_MSG_FROM("Failed: Vector magnitude was 0.", "normalizeVector3");
        return createVector3(0.0f, 0.0f, 0.0f);
    }

    return scaleVector3(vector, 1.0f / magnitude);
}

Vector3 subtractVector3(Vector3 a, Vector3 b)
{
    return createVector3(a.x - b.x, a.y - b.y, a.z - b.z);
}

Vector3 addVector3(Vector3 a, Vector3 b)
{
    return createVector3(a.x + b.x, a.y + b.y, a.z + b.z);
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

    quaternion.x = matrix.m11 * vector.x + matrix.m21 * vector.y +
                   matrix.m31 * vector.z + matrix.m41;
    quaternion.y = matrix.m12 * vector.x + matrix.m22 * vector.y +
                   matrix.m32 * vector.z + matrix.m42;
    quaternion.z = matrix.m13 * vector.x + matrix.m23 * vector.y +
                   matrix.m33 * vector.z + matrix.m43;

    divisor = matrix.m14 * vector.x + matrix.m24 * vector.y +
              matrix.m34 * vector.z + matrix.m44;


    if (abs(divisor) < 0.0001f)
    {
        // DEBUG_MSG_FROM("Failed: Can't divide by 0.", "transformVector3ByMatrix");
        return vector;
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

    result.m11 =  xAxis.x;
    result.m12 =  yAxis.x;
    result.m13 =  zAxis.x;
    result.m14 =  0.0f;

    result.m21 =  xAxis.y;
    result.m22 =  yAxis.y;
    result.m23 =  zAxis.y;
    result.m24 =  0.0f;

    result.m31 =  xAxis.z;
    result.m32 =  yAxis.z;
    result.m33 =  zAxis.z;
    result.m34 =  0.0f;

    result.m41 = -dotProductVector3(xAxis, cameraPosition);
    result.m42 = -dotProductVector3(yAxis, cameraPosition);
    result.m43 = -dotProductVector3(zAxis, cameraPosition);
    result.m44 =  1.0f;

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

    result.m11 =  ycos * zcos;
    result.m12 =  zcosxsin * ysin + xcos * zsin;
    result.m13 =  -xcoszcos * ysin + xsin * zsin;
    result.m14 =  0.0f;

    result.m21 =  -ycos * zsin;
    result.m22 =  xcoszcos - xsin * ysinzsin;
    result.m23 =  zcosxsin + xcos * ysinzsin;
    result.m24 =  0.0f;

    result.m31 =  ysin;
    result.m32 =  -ycos * xsin;
    result.m33 =  xcos * ycos;
    result.m34 =  0.0f;

    result.m41 =  0.0f;
    result.m42 =  0.0f;
    result.m43 =  0.0f;
    result.m44 =  1.0f;

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

    result.m11 = xScale;
    result.m12 = result.m13 = result.m14 = 0.0f;

    result.m22 = yScale;
    result.m21 = result.m23 = result.m24 = 0.0f;

    result.m33 = far / (near - far);
    result.m34 = -1.0f;
    result.m31 = result.m32 = 0.0f;

    result.m43 = near * far / (near - far);
    result.m41 = result.m42 = result.m44 = 0.0f;

    return result;
}

Matrix4x4 createTranslationMatrix(float x, float y, float z)
{
    Matrix4x4 result;

    result.m11 = 1.0f;
    result.m12 = 0.0f;
    result.m13 = 0.0f;
    result.m14 = 0.0f;

    result.m21 = 0.0f;
    result.m22 = 1.0f;
    result.m23 = 0.0f;
    result.m24 = 0.0f;

    result.m31 = 0.0f;
    result.m32 = 0.0f;
    result.m33 = 1.0f;
    result.m34 = 0.0f;

    result.m41 = x;
    result.m42 = y;
    result.m43 = z;
    result.m44 = 1.0f;

    return result;
}

Matrix4x4 multiplyMatrices(Matrix4x4 a, Matrix4x4 b)
{
    Matrix4x4 result;

    result.m11 = a.m11 * b.m11 + a.m12 * b.m21 + a.m13 * b.m31 + a.m14 * b.m41;
    result.m12 = a.m11 * b.m12 + a.m12 * b.m22 + a.m13 * b.m32 + a.m14 * b.m42;
    result.m13 = a.m11 * b.m13 + a.m12 * b.m23 + a.m13 * b.m33 + a.m14 * b.m43;
    result.m14 = a.m11 * b.m14 + a.m12 * b.m24 + a.m13 * b.m34 + a.m14 * b.m44;

    result.m21 = a.m21 * b.m11 + a.m22 * b.m21 + a.m23 * b.m31 + a.m24 * b.m41;
    result.m22 = a.m21 * b.m12 + a.m22 * b.m22 + a.m23 * b.m32 + a.m24 * b.m42;
    result.m23 = a.m21 * b.m13 + a.m22 * b.m23 + a.m23 * b.m33 + a.m24 * b.m43;
    result.m24 = a.m21 * b.m14 + a.m22 * b.m24 + a.m23 * b.m34 + a.m24 * b.m44;

    result.m31 = a.m31 * b.m11 + a.m32 * b.m21 + a.m33 * b.m31 + a.m34 * b.m41;
    result.m32 = a.m31 * b.m12 + a.m32 * b.m22 + a.m33 * b.m32 + a.m34 * b.m42;
    result.m33 = a.m31 * b.m13 + a.m32 * b.m23 + a.m33 * b.m33 + a.m34 * b.m43;
    result.m34 = a.m31 * b.m14 + a.m32 * b.m24 + a.m33 * b.m34 + a.m34 * b.m44;

    result.m41 = a.m41 * b.m11 + a.m42 * b.m21 + a.m43 * b.m31 + a.m44 * b.m41;
    result.m42 = a.m41 * b.m12 + a.m42 * b.m22 + a.m43 * b.m32 + a.m44 * b.m42;
    result.m43 = a.m41 * b.m13 + a.m42 * b.m23 + a.m43 * b.m33 + a.m44 * b.m43;
    result.m44 = a.m41 * b.m14 + a.m42 * b.m24 + a.m43 * b.m34 + a.m44 * b.m44;

    return result;
}

Matrix4x4 Invert(Matrix4x4 matrix)
{
    Matrix4x4 result;

    float a = matrix.m11, b = matrix.m12, c = matrix.m13, d = matrix.m14;
    float e = matrix.m21, f = matrix.m22, g = matrix.m23, h = matrix.m24;
    float i = matrix.m31, j = matrix.m32, k = matrix.m33, l = matrix.m34;
    float m = matrix.m41, n = matrix.m42, o = matrix.m43, p = matrix.m44;

    float kp_lo = k * p - l * o;
    float jp_ln = j * p - l * n;
    float jo_kn = j * o - k * n;
    float ip_lm = i * p - l * m;
    float io_km = i * o - k * m;
    float in_jm = i * n - j * m;

    float a11 = +(f * kp_lo - g * jp_ln + h * jo_kn);
    float a12 = -(e * kp_lo - g * ip_lm + h * io_km);
    float a13 = +(e * jp_ln - f * ip_lm + h * in_jm);
    float a14 = -(e * jo_kn - f * io_km + g * in_jm);

    float det = a * a11 + b * a12 + c * a13 + d * a14;

    float invDet, gp_ho, fp_hn, fo_gn, ep_hm, eo_gm, en_fm,
    gl_hk, fl_hj, fk_gj, el_hi, ek_gi, ej_fi;

    if (abs(det) < 0.0001f)
    {
        return emptyMatrix;
    }

    invDet = 1.0f / det;

    result.m11 = a11 * invDet;
    result.m21 = a12 * invDet;
    result.m31 = a13 * invDet;
    result.m41 = a14 * invDet;

    result.m12 = -(b * kp_lo - c * jp_ln + d * jo_kn) * invDet;
    result.m22 = +(a * kp_lo - c * ip_lm + d * io_km) * invDet;
    result.m32 = -(a * jp_ln - b * ip_lm + d * in_jm) * invDet;
    result.m42 = +(a * jo_kn - b * io_km + c * in_jm) * invDet;

    gp_ho = g * p - h * o;
    fp_hn = f * p - h * n;
    fo_gn = f * o - g * n;
    ep_hm = e * p - h * m;
    eo_gm = e * o - g * m;
    en_fm = e * n - f * m;

    result.m13 = +(b * gp_ho - c * fp_hn + d * fo_gn) * invDet;
    result.m23 = -(a * gp_ho - c * ep_hm + d * eo_gm) * invDet;
    result.m33 = +(a * fp_hn - b * ep_hm + d * en_fm) * invDet;
    result.m43 = -(a * fo_gn - b * eo_gm + c * en_fm) * invDet;

    gl_hk = g * l - h * k;
    fl_hj = f * l - h * j;
    fk_gj = f * k - g * j;
    el_hi = e * l - h * i;
    ek_gi = e * k - g * i;
    ej_fi = e * j - f * i;

    result.m14 = -(b * gl_hk - c * fl_hj + d * fk_gj) * invDet;
    result.m24 = +(a * gl_hk - c * el_hi + d * ek_gi) * invDet;
    result.m34 = -(a * fl_hj - b * el_hi + d * ej_fi) * invDet;
    result.m44 = +(a * fk_gj - b * ek_gi + c * ej_fi) * invDet;

    return result;
}