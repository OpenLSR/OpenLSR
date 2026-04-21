#pragma once
#include <d3d.h>
#include <math.h>

// D3DRM replacement code, only replaces the stuff we care about for LSR and nothing else

typedef D3DVALUE D3DRMMATRIX4D[4][4];

typedef struct _D3DRMQUATERNION
{
    D3DVALUE s;
    D3DVECTOR v;
} D3DRMQUATERNION, * LPD3DRMQUATERNION;

void WINAPI D3DRMMatrixFromQuaternion(D3DRMMATRIX4D m, LPD3DRMQUATERNION q) {
#pragma comment(linker, "/EXPORT:" __FUNCTION__"=" __FUNCDNAME__)
    float w, x, y, z;
    w = q->s;
    x = q->v.x;
    y = q->v.y;
    z = q->v.z;

    m[0][0] = 1.0f - 2.0f * (y * y + z * z);
    m[1][1] = 1.0f - 2.0f * (x * x + z * z);
    m[2][2] = 1.0f - 2.0f * (x * x + y * y);

    m[0][1] = 2.0f * (x * y - z * w);
    m[1][0] = 2.0f * (x * y + z * w);
    m[0][2] = 2.0f * (x * z + y * w);
    m[2][0] = 2.0f * (x * z - y * w);
    m[1][2] = 2.0f * (y * z - x * w);
    m[2][1] = 2.0f * (y * z + x * w);

    m[0][3] = 0.0f;
    m[1][3] = 0.0f;
    m[2][3] = 0.0f;
    m[3][0] = 0.0f;
    m[3][1] = 0.0f;
    m[3][2] = 0.0f;
    m[3][3] = 1.0f;
}


LPD3DRMQUATERNION WINAPI D3DRMQuaternionSlerp(LPD3DRMQUATERNION q, LPD3DRMQUATERNION a, LPD3DRMQUATERNION b, float t) {
#pragma comment(linker, "/EXPORT:" __FUNCTION__"=" __FUNCDNAME__)
    float dot, temp, theta, u;
    bool flip;

    dot = a->s * b->s + a->v.x * b->v.x + a->v.y * b->v.y + a->v.z * b->v.z;

    flip = dot < 0.0f;
    if (flip) dot = -dot;

    if (1.0f - dot > 0.0001f) {
        theta = acos(dot);
        temp = sin(theta * (1.0f - t)) / sin(theta);
        u = sin(theta * t) / sin(theta);
    }
    else {
        temp = 1.0f - t;
        u = t;
    }

    if (flip) u = -u;

    q->s = temp * a->s + u * b->s;
    q->v.x = temp * a->v.x + u * b->v.x;
    q->v.y = temp * a->v.y + u * b->v.y;
    q->v.z = temp * a->v.z + u * b->v.z;
    return q;
}