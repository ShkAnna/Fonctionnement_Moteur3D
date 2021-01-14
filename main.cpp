#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <fstream>
#include "tgaimage.h"
using namespace std;

const TGAColor white = TGAColor(255, 255, 255, 255);
const int width  = 800;
const int height = 800;

struct Vertex2D { double x, y; };
struct Vertex3D { double x, y, z; };
struct Triangle { int v1, v2, v3; };

vector<Vertex3D> vertices;
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
    double x, y, z;
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
                //image.set((x*300+width/2), (y*300+height/2), white);
                vertices.push_back({x, y, z});
                break;

            case str2int("f "):
            case str2int("F "):
                ss >> sv1 >> sv2 >> sv3;
                v1 = stoi(sv1);  v2 = stoi(sv2);  v3 = stoi(sv3);
                faces.push_back({v1, v2, v3});
                break;
        }
    }
    in.close();
}

void line(Vertex2D vx1,  Vertex2D vx2, TGAImage &image, TGAColor color) {
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

Vertex2D getVertex(Triangle face, vector<Vertex3D> vertices, int k) {
    int index;
    switch(k) {
        case 0: index = face.v1-1;break;
        case 1: index = face.v2-1;break;
        case 2: index = face.v3-1;break;
    }
    return {vertices[index].x*300+width/2.,vertices[index].y*300+height/2.};
}

void drawTriangle(Triangle face, TGAImage &image, TGAColor color) {
    for(int j = 0; j < 3; j++) {
        Vertex2D vx1 = getVertex(face, vertices, j);
        Vertex2D vx2 = getVertex(face, vertices, (j+1)%3);
        line(vx1, vx2, image, color);
    }
}

int main(int argc, char** argv) {
    
    string filename = "obj/african_head.obj";
    TGAImage image(width, height, TGAImage::RGB);    
    getFacesAndVertices(filename, image);
    
    cout << "Vertices:\n";
    for(Vertex3D v : vertices) {
        cout << v.x << "  " << v.y << "  " << v.z << '\n';
    }

    for (size_t i = 0; i<faces.size(); i++) {
        drawTriangle(faces[i], image, white);
    }
	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("result.tga");

    return 0;
}
