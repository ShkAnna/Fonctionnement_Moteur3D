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
#include "tgaimage.h"
using namespace std;

const TGAColor white = TGAColor(255, 255, 255, 255);
const int width  = 800;
const int height = 800;

struct Vertex2D { float x, y; };
struct Vertex3D { float x, y, z; };
struct Triangle { int v1, v2, v3; };

vector<Vertex3D> vertices;
vector<Vertex3D> textures;
vector<Vertex3D> normales;
vector<Vertex2D> vertices2D;
vector<Triangle> faces;

constexpr unsigned int str2int(const char* str, int h = 0)
{
    return !str[h] ? 5381 : (str2int(str, h+1) * 33) ^ str[h];
}

void getFacesAndVertices( string filename, TGAImage &image)
{
    faces.clear();   vertices.clear();  
    string line, str, str2;
    int v1, v2, v3;
    float x, y, z;
    string sv1, sv2, sv3;

    ifstream in( filename );
    while(getline(in, line)) {
        if(line.find_first_of("vVfF") == string::npos) continue;
        
        istringstream ss( line );
        ss >> str;
        str2 = line.substr(0, 2);

        switch(str2int(str2.c_str())) {
            case str2int("v "):
            case str2int("V "):
                ss >> x >> y >> z;
                vertices.push_back({x, y, z});
                break;

            case str2int("f "):
            case str2int("F "):
                ss >> sv1 >> sv2 >> sv3;
                v1 = stoi(sv1);  v2 = stoi(sv2);  v3 = stoi(sv3);
                faces.push_back({v1, v2, v3});
                break;
            case str2int("vt "):
                ss >> x >> y >> z;
                textures.push_back({x, y, z});
                break;
            case str2int("vn "):
                ss >> x >> y >> z;
                normales.push_back({x, y, z});
                break;
        }
    }
    in.close();
}

void line(Vertex3D vx1,  Vertex3D vx2, TGAImage &image, TGAColor color) {
    int x0 = vx1.x, x1 = vx2.x, y0 = vx1.y, y1 = vx2.y;
    bool steep = abs(x0-x1) < abs(y0-y1); 

    if (steep) { 
        swap(x0, y0); 
        swap(x1, y1); 
    } 
    if (x0>x1) { 
        swap(x0, x1); 
        swap(y0, y1); 
    } 

    int dx = x1-x0, dy = abs(y1-y0);
    int error = dx/2; 

    while(x0 <= x1) { 
        if (steep) { 
            image.set(y0, x0, color); 
        } else { 
            image.set(x0, y0, color); 
        } 
        error -= dy; 
        if (error < 0 ) { 
            y0 += (y1>y0 ? 1:-1); 
            error += dx; 
        }
        x0++; 
    } 
}

Vertex3D getVertex(Triangle face, vector<Vertex3D> vertices, int k) {
    int index;
    switch(k) {
        case 0: index = face.v1-1;break;
        case 1: index = face.v2-1;break;
        case 2: index = face.v3-1;break;
    }
    return {(vertices[index].x+1.f)*width/2.f, (vertices[index].y+1.f)*height/2.f, vertices[index].z*width};
}

float dotProduct3D(Vertex3D v1, Vertex3D v2) {
    return v1.x*v2.x+v1.y*v2.y+v1.z*v2.z;
}

Vertex3D crossProduct3D(Vertex3D v1, Vertex3D v2) {
    return {v1.y*v2.z - v1.z*v2.y, v1.z*v2.x - v1.x*v2.z, v1.x*v2.y - v1.y*v2.x};
}
Vertex3D diff(Vertex3D v1, Vertex3D v2) {
    return {v2.x-v1.x, v2.y-v1.y, v2.z-v1.z};
}
Vertex3D add(Vertex3D v1, Vertex3D v2) {
    return {v1.x+v2.x, v1.y+v2.y, v1.z+v2.z};
}
Vertex3D multip(Vertex3D v1, float alpha) {
    return {v1.x*alpha, v1.y*alpha, v1.z*alpha};
}

Vertex3D barycentric(Vertex3D p, Vertex3D v1, Vertex3D v2, Vertex3D v3) {
    float u,v,w, den;
    Vertex3D v12 = diff(v1,v2), v13 = diff(v1,v3), v1p = diff(v1,p);
    den = v12.x * v13.y - v13.x * v12.y;
    v = (v1p.x * v13.y - v13.x * v1p.y) / den;
    w = (v12.x * v1p.y - v1p.x * v12.y) / den;
    u = 1.0f - v - w;
    return {u,v,w} ;

}

void drawTriangle(Triangle face, TGAImage &image, TGAColor color) {
    Vertex3D v1 = getVertex(face, vertices, 0);
    Vertex3D v2 = getVertex(face, vertices, 1);
    Vertex3D v3 = getVertex(face, vertices, 2);

    Vertex2D xymin {numeric_limits<float>::max(),  numeric_limits<float>::max()};
    Vertex2D xymax {-numeric_limits<float>::max(), -numeric_limits<float>::max()};

    xymax.x = max({v1.x, v2.x, v3.x, xymax.x});
    xymax.y = max({v1.y, v2.y, v3.y, xymax.y});
    xymin.x = min({v1.x, v2.x, v3.x, xymin.x});
    xymin.y = min({v1.y, v2.y, v3.y, xymin.y});

    for (int x=xymin.x; x<=xymax.x; x++) {
        for (int y=xymin.y; y<=xymax.y; y++) {
            Vertex3D bary = barycentric({x,y}, v1, v2, v3);
            if (bary.x < 0 || bary.y < 0 || bary.z < 0) { continue; }
            image.set(x, y,color);
        }
    }
}

int main(int argc, char** argv) {
    
    string filename = "obj/african_head.obj";
    TGAImage image(width, height, TGAImage::RGB);
    Vertex3D l {0,0,-1}; //vecteur direction du lumiere 
    getFacesAndVertices(filename, image);
    
    cout << "Vertices:\n";
    for(Vertex3D v : vertices) {
        cout << v.x << "  " << v.y << "  " << v.z << '\n';
    }

    for (size_t i = 0; i<faces.size(); i++) {

    Vertex3D v1 = getVertex(faces[i], vertices, 0);
    Vertex3D v2 = getVertex(faces[i], vertices, 1);
    Vertex3D v3 = getVertex(faces[i], vertices, 2);
    
        Vertex3D n = crossProduct3D(diff(v1,v3),diff(v1,v2));
        float magnitude = sqrt(dotProduct3D(n,n));
        n = {n.x/magnitude, n.y/magnitude, n.z/magnitude};
        float intensity = dotProduct3D(multip(l,1),n);
        if (intensity>0) {
            drawTriangle(faces[i], image, TGAColor(intensity*255, intensity*255, intensity*255, intensity*255));
        }
    }
	image.flip_vertically();
	image.write_tga_file("result.tga");

    return 0;
}
