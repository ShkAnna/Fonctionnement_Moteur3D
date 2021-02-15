#ifndef __GEOM_H__
#define __GEOM_H__
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <limits>
#include <fstream>
#include <algorithm>
using namespace std;

struct Vertex2D { float x, y; };
struct Vertex3D { float x, y, z; };
struct Vertex4D { float x, y, z, w; };
struct Triangle { int v1, v2, v3, vt1, vt2, vt3, vn1, vn2, vn3; };

vector<vector<float>> matrix(int l,int c);
vector<vector<float>> identity(int size);
int rows(vector<vector<float>> m);
int cols(vector<vector<float>> m);
vector<vector<float>> operator*(vector<vector<float>> m1, vector<vector<float>> m2);
vector<vector<float>> vtom(Vertex3D v);
Vertex3D mtov(vector<vector<float>>  m);
Vertex3D v4tov3(Vertex4D v);
float operator*(Vertex3D v1, Vertex3D v2);
Vertex3D operator^(Vertex3D v1, Vertex3D v2);
Vertex3D operator-(Vertex3D v1, Vertex3D v2);
Vertex3D operator+(Vertex3D v1, Vertex3D v2);
Vertex3D operator*(Vertex3D v1, float alpha);
Vertex3D normal(Vertex3D v);
Vertex3D barycentric(Vertex3D p, Vertex3D v1, Vertex3D v2, Vertex3D v3);
Vertex2D getUV(Vertex3D v, Vertex2D xymin, Vertex2D xymax);
vector<vector<float>> transpose(vector<vector<float>> m);

#endif