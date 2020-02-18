#pragma once

#include "entities.h"

template <typename T>
class Vec2 {
  public:
    Vec2() : x(0), y(0) {}
    Vec2(T xx) : x(xx), y(xx) {}
    Vec2(T xx, T yy) : x(xx), y(yy) {}
    Vec2 operator+(const Vec2& v) const { return Vec2(x + v.x, y + v.y); }
    Vec2 operator/(const T& r) const { return Vec2(x / r, y / r); }
    Vec2 operator*(const T& r) const { return Vec2(x * r, y * r); }
    Vec2& operator/=(const T& r) {
        x /= r, y /= r;
        return *this;
    }
    Vec2& operator*=(const T& r) {
        x *= r, y *= r;
        return *this;
    }
    friend std::ostream& operator<<(std::ostream& s, const Vec2<T>& v) {
        return s << '[' << v.x << ' ' << v.y << ']';
    }
    friend Vec2 operator*(const T& r, const Vec2<T>& v) { return Vec2(v.x * r, v.y * r); }
    T x, y;
};

typedef Vec2<float> Vec2f;
typedef Vec2<int> Vec2i;

template <typename T>
class Vec3 {
  public:
    Vec3() : x(T(0)), y(T(0)), z(T(0)) {}
    Vec3(T xx) : x(xx), y(xx), z(xx) {}
    Vec3(T xx, T yy, T zz) : x(xx), y(yy), z(zz) {}
    Vec3 operator+(const Vec3& v) const { return Vec3(x + v.x, y + v.y, z + v.z); }
    Vec3 operator-(const Vec3& v) const { return Vec3(x - v.x, y - v.y, z - v.z); }
    Vec3 operator-() const { return Vec3(-x, -y, -z); }
    Vec3 operator*(const T& r) const { return Vec3(x * r, y * r, z * r); }
    Vec3 operator*(const Vec3& v) const { return Vec3(x * v.x, y * v.y, z * v.z); }
    T dotProduct(const Vec3<T>& v) const { return x * v.x + y * v.y + z * v.z; }
    Vec3& operator/=(const T& r) {
        x /= r, y /= r, z /= r;
        return *this;
    }
    Vec3& operator*=(const T& r) {
        x *= r, y *= r, z *= r;
        return *this;
    }
    Vec3 crossProduct(const Vec3<T>& v) const {
        return Vec3<T>(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
    }
    T norm() const { return x * x + y * y + z * z; }
    T length() const { return sqrt(norm()); }
    //[comment]
    // The next two operators are sometimes called access operators or
    // accessors. The Vec coordinates can be accessed that way v[0], v[1], v[2],
    // rather than using the more traditional form v.x, v.y, v.z. This useful
    // when vectors are used in loops: the coordinates can be accessed with the
    // loop index (e.g. v[i]).
    //[/comment]
    const T& operator[](uint8_t i) const { return (&x)[i]; }
    T& operator[](uint8_t i) { return (&x)[i]; }
    Vec3& normalize() {
        T n = norm();
        if (n > 0) {
            T factor = 1 / sqrt(n);
            x *= factor, y *= factor, z *= factor;
        }

        return *this;
    }

    friend Vec3 operator*(const T& r, const Vec3& v) { return Vec3<T>(v.x * r, v.y * r, v.z * r); }
    friend Vec3 operator/(const T& r, const Vec3& v) { return Vec3<T>(r / v.x, r / v.y, r / v.z); }

    friend std::ostream& operator<<(std::ostream& s, const Vec3<T>& v) {
        return s << '[' << v.x << ' ' << v.y << ' ' << v.z << ']';
    }

    T x, y, z;
};

typedef Vec3<float> Vec3f;
typedef Vec3<int> Vec3i;
static const float kInfinity = std::numeric_limits<float>::max();

bool rayTriangleIntersect(const Vec3f& orig, const Vec3f& dir, const Vec3f& v0, const Vec3f& v1, const Vec3f& v2, float& t, float& u, float& v) {
    Vec3f v0v1 = v1 - v0;
    Vec3f v0v2 = v2 - v0;
    Vec3f pvec = dir.crossProduct(v0v2);
    float det = v0v1.dotProduct(pvec);

    // ray and triangle are parallel if det is close to 0
    if (fabs(det) < 1e-8)
        return false;

    float invDet = 1 / det;

    Vec3f tvec = orig - v0;
    u = tvec.dotProduct(pvec) * invDet;
    if (u < 0 || u > 1)
        return false;

    Vec3f qvec = tvec.crossProduct(v0v1);
    v = dir.dotProduct(qvec) * invDet;
    if (v < 0 || u + v > 1)
        return false;

    t = v0v2.dotProduct(qvec) * invDet;

    return true;
}

class TriangleMesh : public Entity {
  public:
    // Build a triangle mesh from a face index array and a vertex index array
    TriangleMesh(const uint32_t nfaces,
                 const std::unique_ptr<uint32_t[]>& faceIndex,
                 const std::unique_ptr<uint32_t[]>& vertsIndex,
                 const std::unique_ptr<Vec3f[]>& verts,
                 std::unique_ptr<Vec3f[]>& normals,
                 std::unique_ptr<Vec2f[]>& st)
        : numTris(0), Entity() {
        pos = {-8, -10, -6};
        uint32_t k = 0, maxVertIndex = 0;
        // find out how many triangles we need to create for this mesh
        for (uint32_t i = 0; i < nfaces; ++i) {
            numTris += faceIndex[i] - 2;
            for (uint32_t j = 0; j < faceIndex[i]; ++j)
                if (vertsIndex[k + j] > maxVertIndex)
                    maxVertIndex = vertsIndex[k + j];
            k += faceIndex[i];
        }
        maxVertIndex += 1;

        // allocate memory to store the position of the mesh vertices
        P = std::unique_ptr<Vec3f[]>(new Vec3f[maxVertIndex]);
        for (uint32_t i = 0; i < maxVertIndex; ++i) { P[i] = verts[i]; }

        // allocate memory to store triangle indices
        trisIndex = std::unique_ptr<uint32_t[]>(new uint32_t[numTris * 3]);
        uint32_t l = 0;

        N = std::unique_ptr<Vec3f[]>(new Vec3f[numTris * 3]);
        texCoordinates = std::unique_ptr<Vec2f[]>(new Vec2f[numTris * 3]);
        for (uint32_t i = 0, k = 0; i < nfaces; ++i) {        // for each  face
            for (uint32_t j = 0; j < faceIndex[i] - 2; ++j) { // for each triangle in the face
                trisIndex[l] = vertsIndex[k];
                trisIndex[l + 1] = vertsIndex[k + j + 1];
                trisIndex[l + 2] = vertsIndex[k + j + 2];
                N[l] = normals[k];
                N[l + 1] = normals[k + j + 1];
                N[l + 2] = normals[k + j + 2];
                texCoordinates[l] = st[k];
                texCoordinates[l + 1] = st[k + j + 1];
                texCoordinates[l + 2] = st[k + j + 2];
                l += 3;
            }
            k += faceIndex[i];
        }
    }
    // Test if the ray interesests this triangle mesh
    bool intersect(const Vec3f& orig, const Vec3f& dir, float& tNear, uint32_t& triIndex, Vec2f& uv) const {
        uint32_t j = 0;
        bool isect = false;
        for (uint32_t i = 0; i < numTris; ++i) {
            const Vec3f& v0 = P[trisIndex[j]];
            const Vec3f& v1 = P[trisIndex[j + 1]];
            const Vec3f& v2 = P[trisIndex[j + 2]];
            float t = kInfinity, u, v;
            if (rayTriangleIntersect(orig, dir, v0, v1, v2, t, u, v) && t < tNear) {
                tNear = t;
                uv.x = u;
                uv.y = v;
                triIndex = i;
                isect = true;
            }
            j += 3;
        }

        return isect;
    }
    void getSurfaceProperties(const Vec3f& hitPoint, const Vec3f& viewDirection, const uint32_t& triIndex, const Vec2f& uv, Vec3f& hitNormal, Vec2f& hitTextureCoordinates) const {
        // face normal
        const Vec3f& v0 = P[trisIndex[triIndex * 3]];
        const Vec3f& v1 = P[trisIndex[triIndex * 3 + 1]];
        const Vec3f& v2 = P[trisIndex[triIndex * 3 + 2]];
        hitNormal = (v1 - v0).crossProduct(v2 - v0);
        hitNormal.normalize();

        // texture coordinates
        const Vec2f& st0 = texCoordinates[triIndex * 3];
        const Vec2f& st1 = texCoordinates[triIndex * 3 + 1];
        const Vec2f& st2 = texCoordinates[triIndex * 3 + 2];
        hitTextureCoordinates = (1 - uv.x - uv.y) * st0 + uv.x * st1 + uv.y * st2;
    }
    // member variables
    uint32_t numTris;                        // number of triangles
    std::unique_ptr<Vec3f[]> P;              // triangles vertex position
    std::unique_ptr<uint32_t[]> trisIndex;   // vertex index array
    std::unique_ptr<Vec3f[]> N;              // triangles vertex normals
    std::unique_ptr<Vec2f[]> texCoordinates; // triangles texture coordinates
};

TriangleMesh* generatePolyShphere(float rad, uint32_t divs) {
    // generate points
    float mpi = 3.14159265358979323846;
    float mpi_2 = 1.57079632679489661923;
    uint32_t numVertices = (divs - 1) * divs + 2;
    std::unique_ptr<Vec3f[]> P(new Vec3f[numVertices]);
    std::unique_ptr<Vec3f[]> N(new Vec3f[numVertices]);
    std::unique_ptr<Vec2f[]> st(new Vec2f[numVertices]);

    float u = -mpi_2;
    float v = -mpi;
    float du = mpi / divs;
    float dv = 2 * mpi/ divs;

    P[0] = N[0] = Vec3f(0, -rad, 0);
    uint32_t k = 1;
    for (uint32_t i = 0; i < divs - 1; i++) {
        u += du;
        v = -mpi;
        for (uint32_t j = 0; j < divs; j++) {
            float x = rad * cos(u) * cos(v);
            float y = rad * sin(u);
            float z = rad * cos(u) * sin(v);
            P[k] = N[k] = Vec3f(x, y, z);
            st[k].x = u / mpi + 0.5;
            st[k].y = v * 0.5 / mpi + 0.5;
            v += dv, k++;
        }
    }
    P[k] = N[k] = Vec3f(0, rad, 0);

    uint32_t npolys = divs * divs;
    std::unique_ptr<uint32_t[]> faceIndex(new uint32_t[npolys]);
    std::unique_ptr<uint32_t[]> vertsIndex(new uint32_t[(6 + (divs - 1) * 4) * divs]);

    // create the connectivity lists
    uint32_t vid = 1, numV = 0, l = 0;
    k = 0;
    for (uint32_t i = 0; i < divs; i++) {
        for (uint32_t j = 0; j < divs; j++) {
            if (i == 0) {
                faceIndex[k++] = 3;
                vertsIndex[l] = 0;
                vertsIndex[l + 1] = j + vid;
                vertsIndex[l + 2] = (j == (divs - 1)) ? vid : j + vid + 1;
                l += 3;
            } else if (i == (divs - 1)) {
                faceIndex[k++] = 3;
                vertsIndex[l] = j + vid + 1 - divs;
                vertsIndex[l + 1] = vid + 1;
                vertsIndex[l + 2] = (j == (divs - 1)) ? vid + 1 - divs : j + vid + 2 - divs;
                l += 3;
            } else {
                faceIndex[k++] = 4;
                vertsIndex[l] = j + vid + 1 - divs;
                vertsIndex[l + 1] = j + vid + 1;
                vertsIndex[l + 2] = (j == (divs - 1)) ? vid + 1 : j + vid + 2;
                vertsIndex[l + 3] = (j == (divs - 1)) ? vid + 1 - divs : j + vid + 2 - divs;
                l += 4;
            }
            numV++;
        }
        vid = numV;
    }

    return new TriangleMesh(npolys, faceIndex, vertsIndex, P, N, st);
}