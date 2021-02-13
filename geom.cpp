#include <vector>
#include <cmath>
#include "geom.h"
using namespace std;

vector<vector<float>> matrix(int l, int c) {
    vector<vector<float>> id(l);
    for (int i=0; i<l; i++) {
        id[i].resize(c);
        for (int j=0; j<c; j++) {
            id[i][j] = 0;
        }
    }
    return id;
}

vector<vector<float>> identity(int size) {
    vector<vector<float>> id(size);
    for (int i=0; i<size; i++) {
        id[i].resize(size);
        for (int j=0; j<size; j++) {
            id[i][j] = (i==j ? 1.f : 0.f);
        }
    }
    return id;
}

int rows(vector<vector<float>> m) {
    return m.size();  
}
int cols(vector<vector<float>> m) {
    return m[0].size();
}
vector<vector<float>> operator*(vector<vector<float>> m1, vector<vector<float>> m2) {
    vector<vector<float>> m = matrix(rows(m1),cols(m2));
    for(int i = 0; i < rows(m1); ++i) {
        for(int j = 0; j < cols(m2); ++j) {
            for(int k = 0; k < cols(m1); ++k) {
                m[i][j] += m1[i][k] * m2[k][j];
            }
        }     
    }
    return m;
}

vector<vector<float>> vtom(Vertex3D v) {
    vector<vector<float>> m = matrix(4,1);
    m[0][0] = v.x;
    m[1][0] = v.y;
    m[2][0] = v.z;
    m[3][0] = 1;
    return m;
}

Vertex3D mtov(vector<vector<float>>  m) {
    return {m[0][0]/m[3][0], m[1][0]/m[3][0], m[2][0]/m[3][0]};
}
//dot product
float operator*(Vertex3D v1, Vertex3D v2) {
    return v1.x*v2.x+v1.y*v2.y+v1.z*v2.z;
}
//cross product
Vertex3D operator^(Vertex3D v1, Vertex3D v2) {
    return {v1.y*v2.z - v1.z*v2.y, v1.z*v2.x - v1.x*v2.z, v1.x*v2.y - v1.y*v2.x};
}
Vertex3D operator-(Vertex3D v1, Vertex3D v2) {
    return {v1.x-v2.x, v1.y-v2.y, v1.z-v2.z};
}
Vertex3D operator+(Vertex3D v1, Vertex3D v2) {
    return {v1.x+v2.x, v1.y+v2.y, v1.z+v2.z};
}
Vertex3D operator*(Vertex3D v1, float alpha) {
    return {v1.x*alpha, v1.y*alpha, v1.z*alpha};
}

Vertex3D normal(Vertex3D v){
    float magnitude = sqrt(v*v);
    return {v.x/magnitude, v.y/magnitude, v.z/magnitude};
}