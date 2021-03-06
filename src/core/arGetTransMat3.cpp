/*
 Copyright (C) 2010  ARToolkitPlus Authors

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.

 Authors:
 Daniel Wagner
 Pavel Rojtberg
 */

#include <ARToolKitPlus/Tracker.h>
#include <ARToolKitPlus/matrix.h>
#include <cmath>

#define CHECK_CALC 0

namespace ARToolKitPlus {

static int check_rotation(ARFloat rot[2][3]);
static int check_dir(ARFloat dir[3], ARFloat st[2], ARFloat ed[2], ARFloat cpara[3][4]);

int Tracker::arGetAngle(ARFloat rot[3][3], ARFloat *wa, ARFloat *wb, ARFloat *wc) {
    ARFloat a, b, c;
    ARFloat sina, cosa, sinb, cosb, sinc, cosc;
#if CHECK_CALC
    ARFloat w[3];
    int i;
    for(i=0;i<3;i++) w[i] = rot[i][0];
    for(i=0;i<3;i++) rot[i][0] = rot[i][1];
    for(i=0;i<3;i++) rot[i][1] = rot[i][2];
    for(i=0;i<3;i++) rot[i][2] = w[i];
#endif

    if (rot[2][2] > 1.0) {
        /* printf("cos(beta) = %f\n", rot[2][2]); */
        rot[2][2] = 1.0;
    } else if (rot[2][2] < -1.0) {
        /* printf("cos(beta) = %f\n", rot[2][2]); */
        rot[2][2] = -1.0;
    }
    cosb = rot[2][2];
    b = (ARFloat) acos(cosb);
    sinb = (ARFloat) sin(b);
    if (b >= 0.000001 || b <= -0.000001) {
        cosa = rot[0][2] / sinb;
        sina = rot[1][2] / sinb;
        if (cosa > 1.0) {
            /* printf("cos(alph) = %f\n", cosa); */
            cosa = 1.0;
            sina = 0.0;
        }
        if (cosa < -1.0) {
            /* printf("cos(alph) = %f\n", cosa); */
            cosa = -1.0;
            sina = 0.0;
        }
        if (sina > 1.0) {
            /* printf("sin(alph) = %f\n", sina); */
            sina = 1.0;
            cosa = 0.0;
        }
        if (sina < -1.0) {
            /* printf("sin(alph) = %f\n", sina); */
            sina = -1.0;
            cosa = 0.0;
        }
        a = (ARFloat) acos(cosa);
        if (sina < 0)
            a = -a;

        sinc = (rot[2][1] * rot[0][2] - rot[2][0] * rot[1][2]) / (rot[0][2] * rot[0][2] + rot[1][2] * rot[1][2]);
        cosc = -(rot[0][2] * rot[2][0] + rot[1][2] * rot[2][1]) / (rot[0][2] * rot[0][2] + rot[1][2] * rot[1][2]);
        if (cosc > 1.0) {
            /* printf("cos(r) = %f\n", cosc); */
            cosc = 1.0;
            sinc = 0.0;
        }
        if (cosc < -1.0) {
            /* printf("cos(r) = %f\n", cosc); */
            cosc = -1.0;
            sinc = 0.0;
        }
        if (sinc > 1.0) {
            /* printf("sin(r) = %f\n", sinc); */
            sinc = 1.0;
            cosc = 0.0;
        }
        if (sinc < -1.0) {
            /* printf("sin(r) = %f\n", sinc); */
            sinc = -1.0;
            cosc = 0.0;
        }
        c = (ARFloat) acos(cosc);
        if (sinc < 0)
            c = -c;
    } else {
        a = b = 0.0;
        cosa = cosb = 1.0;
        sina = sinb = 0.0;
        cosc = rot[0][0];
        sinc = rot[1][0];
        if (cosc > 1.0) {
            /* printf("cos(r) = %f\n", cosc); */
            cosc = 1.0;
            sinc = 0.0;
        }
        if (cosc < -1.0) {
            /* printf("cos(r) = %f\n", cosc); */
            cosc = -1.0;
            sinc = 0.0;
        }
        if (sinc > 1.0) {
            /* printf("sin(r) = %f\n", sinc); */
            sinc = 1.0;
            cosc = 0.0;
        }
        if (sinc < -1.0) {
            /* printf("sin(r) = %f\n", sinc); */
            sinc = -1.0;
            cosc = 0.0;
        }
        c = (ARFloat) acos(cosc);
        if (sinc < 0)
            c = -c;
    }

    *wa = a;
    *wb = b;
    *wc = c;

    return 0;
}

// Non-FixedPoint version of arGetRot
//
int Tracker::arGetRot(ARFloat a, ARFloat b, ARFloat c, ARFloat rot[3][3]) {
    ARFloat sina, sinb, sinc;
    ARFloat cosa, cosb, cosc;
#if CHECK_CALC
    ARFloat w[3];
    int i;
#endif

    sina = (ARFloat) sin(a);
    cosa = (ARFloat) cos(a);
    sinb = (ARFloat) sin(b);
    cosb = (ARFloat) cos(b);
    sinc = (ARFloat) sin(c);
    cosc = (ARFloat) cos(c);
    rot[0][0] = cosa * cosa * cosb * cosc + sina * sina * cosc + sina * cosa * cosb * sinc - sina * cosa * sinc;
    rot[0][1] = -cosa * cosa * cosb * sinc - sina * sina * sinc + sina * cosa * cosb * cosc - sina * cosa * cosc;
    rot[0][2] = cosa * sinb;
    rot[1][0] = sina * cosa * cosb * cosc - sina * cosa * cosc + sina * sina * cosb * sinc + cosa * cosa * sinc;
    rot[1][1] = -sina * cosa * cosb * sinc + sina * cosa * sinc + sina * sina * cosb * cosc + cosa * cosa * cosc;
    rot[1][2] = sina * sinb;
    rot[2][0] = -cosa * sinb * cosc - sina * sinb * sinc;
    rot[2][1] = cosa * sinb * sinc - sina * sinb * cosc;
    rot[2][2] = cosb;

#if CHECK_CALC
    for(i=0;i<3;i++) w[i] = rot[i][2];
    for(i=0;i<3;i++) rot[i][2] = rot[i][1];
    for(i=0;i<3;i++) rot[i][1] = rot[i][0];
    for(i=0;i<3;i++) rot[i][0] = w[i];
#endif

    return 0;
}

int Tracker::arGetNewMatrix(ARFloat a, ARFloat b, ARFloat c, ARFloat trans[3], ARFloat trans2[3][4],
        ARFloat cpara[3][4], ARFloat ret[3][4]) {
    ARFloat cpara2[3][4];
    ARFloat rot[3][3];
    int i, j;

    arGetRot(a, b, c, rot);

    if (trans2 != NULL) {
        for (j = 0; j < 3; j++) {
            for (i = 0; i < 4; i++) {
                cpara2[j][i] = cpara[j][0] * trans2[0][i] + cpara[j][1] * trans2[1][i] + cpara[j][2] * trans2[2][i];
            }
        }
    } else {
        for (j = 0; j < 3; j++) {
            for (i = 0; i < 4; i++) {
                cpara2[j][i] = cpara[j][i];
            }
        }
    }

    for (j = 0; j < 3; j++) {
        for (i = 0; i < 3; i++) {
            ret[j][i] = cpara2[j][0] * rot[0][i] + cpara2[j][1] * rot[1][i] + cpara2[j][2] * rot[2][i];
        }
        ret[j][3] = cpara2[j][0] * trans[0] + cpara2[j][1] * trans[1] + cpara2[j][2] * trans[2] + cpara2[j][3];
    }

    return (0);
}

int Tracker::arGetInitRot(ARMarkerInfo *marker_info, ARFloat cpara[3][4], ARFloat rot[3][3]) {
    ARFloat wdir[3][3];
    ARFloat w, w1, w2, w3;
    int dir;
    int j;

    dir = marker_info->dir;

    for (j = 0; j < 2; j++) {
        w1 = marker_info->line[(4 - dir + j) % 4][0] * marker_info->line[(6 - dir + j) % 4][1] - marker_info->line[(6
                - dir + j) % 4][0] * marker_info->line[(4 - dir + j) % 4][1];
        w2 = marker_info->line[(4 - dir + j) % 4][1] * marker_info->line[(6 - dir + j) % 4][2] - marker_info->line[(6
                - dir + j) % 4][1] * marker_info->line[(4 - dir + j) % 4][2];
        w3 = marker_info->line[(4 - dir + j) % 4][2] * marker_info->line[(6 - dir + j) % 4][0] - marker_info->line[(6
                - dir + j) % 4][2] * marker_info->line[(4 - dir + j) % 4][0];

        wdir[j][0] = w1 * (cpara[0][1] * cpara[1][2] - cpara[0][2] * cpara[1][1]) + w2 * cpara[1][1] - w3 * cpara[0][1];
        wdir[j][1] = -w1 * cpara[0][0] * cpara[1][2] + w3 * cpara[0][0];
        wdir[j][2] = w1 * cpara[0][0] * cpara[1][1];
        w = (ARFloat) sqrt(wdir[j][0] * wdir[j][0] + wdir[j][1] * wdir[j][1] + wdir[j][2] * wdir[j][2]);
        wdir[j][0] /= w;
        wdir[j][1] /= w;
        wdir[j][2] /= w;
    }

    if (check_dir(wdir[0], marker_info->vertex[(4 - dir) % 4], marker_info->vertex[(5 - dir) % 4], cpara) < 0) {
        return -1;
    }

    if (check_dir(wdir[1], marker_info->vertex[(7 - dir) % 4], marker_info->vertex[(4 - dir) % 4], cpara) < 0) {
        return -1;
    }

    if (check_rotation(wdir) < 0) {
        return -1;
    }

    wdir[2][0] = wdir[0][1] * wdir[1][2] - wdir[0][2] * wdir[1][1];
    wdir[2][1] = wdir[0][2] * wdir[1][0] - wdir[0][0] * wdir[1][2];
    wdir[2][2] = wdir[0][0] * wdir[1][1] - wdir[0][1] * wdir[1][0];
    w = (ARFloat) sqrt(wdir[2][0] * wdir[2][0] + wdir[2][1] * wdir[2][1] + wdir[2][2] * wdir[2][2]);
    wdir[2][0] /= w;
    wdir[2][1] /= w;
    wdir[2][2] /= w;
    /*
     if( wdir[2][2] < 0 ) {
     wdir[2][0] /= -w;
     wdir[2][1] /= -w;
     wdir[2][2] /= -w;
     }
     else {
     wdir[2][0] /= w;
     wdir[2][1] /= w;
     wdir[2][2] /= w;
     }
     */

    rot[0][0] = wdir[0][0];
    rot[1][0] = wdir[0][1];
    rot[2][0] = wdir[0][2];
    rot[0][1] = wdir[1][0];
    rot[1][1] = wdir[1][1];
    rot[2][1] = wdir[1][2];
    rot[0][2] = wdir[2][0];
    rot[1][2] = wdir[2][1];
    rot[2][2] = wdir[2][2];

    return 0;
}

static int check_dir(ARFloat dir[3], ARFloat st[2], ARFloat ed[2], ARFloat cpara[3][4]) {
    ARMat *mat_a;
    ARFloat world[2][3];
    ARFloat camera[2][2];
    ARFloat v[2][2];
    ARFloat h;
    int i, j;

    mat_a = Matrix::alloc(3, 3);
    for (j = 0; j < 3; j++)
        for (i = 0; i < 3; i++)
            mat_a->m[j * 3 + i] = cpara[j][i];
    Matrix::selfInv(mat_a);
    world[0][0] = mat_a->m[0] * st[0] * (ARFloat) 10.0 + mat_a->m[1] * st[1] * (ARFloat) 10.0 + mat_a->m[2]
            * (ARFloat) 10.0;
    world[0][1] = mat_a->m[3] * st[0] * (ARFloat) 10.0 + mat_a->m[4] * st[1] * (ARFloat) 10.0 + mat_a->m[5]
            * (ARFloat) 10.0;
    world[0][2] = mat_a->m[6] * st[0] * (ARFloat) 10.0 + mat_a->m[7] * st[1] * (ARFloat) 10.0 + mat_a->m[8]
            * (ARFloat) 10.0;
    Matrix::free(mat_a);
    world[1][0] = world[0][0] + dir[0];
    world[1][1] = world[0][1] + dir[1];
    world[1][2] = world[0][2] + dir[2];

    for (i = 0; i < 2; i++) {
        h = cpara[2][0] * world[i][0] + cpara[2][1] * world[i][1] + cpara[2][2] * world[i][2];
        if (h == 0.0)
            return -1;
        camera[i][0] = (cpara[0][0] * world[i][0] + cpara[0][1] * world[i][1] + cpara[0][2] * world[i][2]) / h;
        camera[i][1] = (cpara[1][0] * world[i][0] + cpara[1][1] * world[i][1] + cpara[1][2] * world[i][2]) / h;
    }

    v[0][0] = ed[0] - st[0];
    v[0][1] = ed[1] - st[1];
    v[1][0] = camera[1][0] - camera[0][0];
    v[1][1] = camera[1][1] - camera[0][1];

    if (v[0][0] * v[1][0] + v[0][1] * v[1][1] < 0) {
        dir[0] = -dir[0];
        dir[1] = -dir[1];
        dir[2] = -dir[2];
    }

    return 0;
}

static int check_rotation(ARFloat rot[2][3]) {
    ARFloat v1[3], v2[3], v3[3];
    ARFloat ca, cb, k1, k2, k3, k4;
    ARFloat a, b, c, d;
    ARFloat p1, q1, r1;
    ARFloat p2, q2, r2;
    ARFloat p3, q3, r3;
    ARFloat p4, q4, r4;
    ARFloat w;
    ARFloat e1, e2, e3, e4;
    int f;

    v1[0] = rot[0][0];
    v1[1] = rot[0][1];
    v1[2] = rot[0][2];
    v2[0] = rot[1][0];
    v2[1] = rot[1][1];
    v2[2] = rot[1][2];
    v3[0] = v1[1] * v2[2] - v1[2] * v2[1];
    v3[1] = v1[2] * v2[0] - v1[0] * v2[2];
    v3[2] = v1[0] * v2[1] - v1[1] * v2[0];
    w = (ARFloat) sqrt(v3[0] * v3[0] + v3[1] * v3[1] + v3[2] * v3[2]);
    if (w == 0.0)
        return -1;
    v3[0] /= w;
    v3[1] /= w;
    v3[2] /= w;

    cb = v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
    if (cb < 0)
        cb *= -1.0;
    ca = ((ARFloat) sqrt(cb + 1.0) + (ARFloat) sqrt(1.0 - cb)) * (ARFloat) 0.5;

    if (v3[1] * v1[0] - v1[1] * v3[0] != 0.0) {
        f = 0;
    } else {
        if (v3[2] * v1[0] - v1[2] * v3[0] != 0.0) {
            w = v1[1];
            v1[1] = v1[2];
            v1[2] = w;
            w = v3[1];
            v3[1] = v3[2];
            v3[2] = w;
            f = 1;
        } else {
            w = v1[0];
            v1[0] = v1[2];
            v1[2] = w;
            w = v3[0];
            v3[0] = v3[2];
            v3[2] = w;
            f = 2;
        }
    }
    if (v3[1] * v1[0] - v1[1] * v3[0] == 0.0)
        return -1;
    k1 = (v1[1] * v3[2] - v3[1] * v1[2]) / (v3[1] * v1[0] - v1[1] * v3[0]);
    k2 = (v3[1] * ca) / (v3[1] * v1[0] - v1[1] * v3[0]);
    k3 = (v1[0] * v3[2] - v3[0] * v1[2]) / (v3[0] * v1[1] - v1[0] * v3[1]);
    k4 = (v3[0] * ca) / (v3[0] * v1[1] - v1[0] * v3[1]);

    a = k1 * k1 + k3 * k3 + 1;
    b = k1 * k2 + k3 * k4;
    c = k2 * k2 + k4 * k4 - 1;

    d = b * b - a * c;
    if (d < 0)
        return -1;
    r1 = (-b + (ARFloat) sqrt(d)) / a;
    p1 = k1 * r1 + k2;
    q1 = k3 * r1 + k4;
    r2 = (-b - (ARFloat) sqrt(d)) / a;
    p2 = k1 * r2 + k2;
    q2 = k3 * r2 + k4;
    if (f == 1) {
        w = q1;
        q1 = r1;
        r1 = w;
        w = q2;
        q2 = r2;
        r2 = w;
        w = v1[1];
        v1[1] = v1[2];
        v1[2] = w;
        w = v3[1];
        v3[1] = v3[2];
        v3[2] = w;
        f = 0;
    }
    if (f == 2) {
        w = p1;
        p1 = r1;
        r1 = w;
        w = p2;
        p2 = r2;
        r2 = w;
        w = v1[0];
        v1[0] = v1[2];
        v1[2] = w;
        w = v3[0];
        v3[0] = v3[2];
        v3[2] = w;
        f = 0;
    }

    if (v3[1] * v2[0] - v2[1] * v3[0] != 0.0) {
        f = 0;
    } else {
        if (v3[2] * v2[0] - v2[2] * v3[0] != 0.0) {
            w = v2[1];
            v2[1] = v2[2];
            v2[2] = w;
            w = v3[1];
            v3[1] = v3[2];
            v3[2] = w;
            f = 1;
        } else {
            w = v2[0];
            v2[0] = v2[2];
            v2[2] = w;
            w = v3[0];
            v3[0] = v3[2];
            v3[2] = w;
            f = 2;
        }
    }
    if (v3[1] * v2[0] - v2[1] * v3[0] == 0.0)
        return -1;
    k1 = (v2[1] * v3[2] - v3[1] * v2[2]) / (v3[1] * v2[0] - v2[1] * v3[0]);
    k2 = (v3[1] * ca) / (v3[1] * v2[0] - v2[1] * v3[0]);
    k3 = (v2[0] * v3[2] - v3[0] * v2[2]) / (v3[0] * v2[1] - v2[0] * v3[1]);
    k4 = (v3[0] * ca) / (v3[0] * v2[1] - v2[0] * v3[1]);

    a = k1 * k1 + k3 * k3 + 1;
    b = k1 * k2 + k3 * k4;
    c = k2 * k2 + k4 * k4 - 1;

    d = b * b - a * c;
    if (d < 0)
        return -1;
    r3 = (-b + (ARFloat) sqrt(d)) / a;
    p3 = k1 * r3 + k2;
    q3 = k3 * r3 + k4;
    r4 = (-b - (ARFloat) sqrt(d)) / a;
    p4 = k1 * r4 + k2;
    q4 = k3 * r4 + k4;
    if (f == 1) {
        w = q3;
        q3 = r3;
        r3 = w;
        w = q4;
        q4 = r4;
        r4 = w;
        w = v2[1];
        v2[1] = v2[2];
        v2[2] = w;
        w = v3[1];
        v3[1] = v3[2];
        v3[2] = w;
        f = 0;
    }
    if (f == 2) {
        w = p3;
        p3 = r3;
        r3 = w;
        w = p4;
        p4 = r4;
        r4 = w;
        w = v2[0];
        v2[0] = v2[2];
        v2[2] = w;
        w = v3[0];
        v3[0] = v3[2];
        v3[2] = w;
        f = 0;
    }

    e1 = p1 * p3 + q1 * q3 + r1 * r3;
    if (e1 < 0)
        e1 = -e1;
    e2 = p1 * p4 + q1 * q4 + r1 * r4;
    if (e2 < 0)
        e2 = -e2;
    e3 = p2 * p3 + q2 * q3 + r2 * r3;
    if (e3 < 0)
        e3 = -e3;
    e4 = p2 * p4 + q2 * q4 + r2 * r4;
    if (e4 < 0)
        e4 = -e4;
    if (e1 < e2) {
        if (e1 < e3) {
            if (e1 < e4) {
                rot[0][0] = p1;
                rot[0][1] = q1;
                rot[0][2] = r1;
                rot[1][0] = p3;
                rot[1][1] = q3;
                rot[1][2] = r3;
            } else {
                rot[0][0] = p2;
                rot[0][1] = q2;
                rot[0][2] = r2;
                rot[1][0] = p4;
                rot[1][1] = q4;
                rot[1][2] = r4;
            }
        } else {
            if (e3 < e4) {
                rot[0][0] = p2;
                rot[0][1] = q2;
                rot[0][2] = r2;
                rot[1][0] = p3;
                rot[1][1] = q3;
                rot[1][2] = r3;
            } else {
                rot[0][0] = p2;
                rot[0][1] = q2;
                rot[0][2] = r2;
                rot[1][0] = p4;
                rot[1][1] = q4;
                rot[1][2] = r4;
            }
        }
    } else {
        if (e2 < e3) {
            if (e2 < e4) {
                rot[0][0] = p1;
                rot[0][1] = q1;
                rot[0][2] = r1;
                rot[1][0] = p4;
                rot[1][1] = q4;
                rot[1][2] = r4;
            } else {
                rot[0][0] = p2;
                rot[0][1] = q2;
                rot[0][2] = r2;
                rot[1][0] = p4;
                rot[1][1] = q4;
                rot[1][2] = r4;
            }
        } else {
            if (e3 < e4) {
                rot[0][0] = p2;
                rot[0][1] = q2;
                rot[0][2] = r2;
                rot[1][0] = p3;
                rot[1][1] = q3;
                rot[1][2] = r3;
            } else {
                rot[0][0] = p2;
                rot[0][1] = q2;
                rot[0][2] = r2;
                rot[1][0] = p4;
                rot[1][1] = q4;
                rot[1][2] = r4;
            }
        }
    }

    return 0;
}

} // namespace ARToolKitPlus
