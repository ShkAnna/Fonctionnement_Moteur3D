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

constexpr unsigned int str2int(const char* str, int h = 0)
{
    return !str[h] ? 5381 : (str2int(str, h+1) * 33) ^ str[h];
}

void getFacesAndVertices( string filename, vector<Vertex3D> &vertices, vector<Triangle> &faces, TGAImage &image)
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
                image.set((x*300+width/2), (y*300+height/2), white);
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

void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) { 
    bool steep = false; 
    if (std::abs(x0-x1)<std::abs(y0-y1)) { 
        std::swap(x0, y0); 
        std::swap(x1, y1); 
        steep = true; 
    } 
    if (x0>x1) { 
        std::swap(x0, x1); 
        std::swap(y0, y1); 
    } 
    int dx = x1-x0; 
    int dy = y1-y0; 
    int derror2 = std::abs(dy)*2; 
    int error2 = 0; 
    int y = y0; 
    for (int x=x0; x<=x1; x++) { 
        if (steep) { 
            image.set(y, x, color); 
        } else { 
            image.set(x, y, color); 
        } 
        error2 += derror2; 
        if (error2 > dx) { 
            y += (y1>y0?1:-1); 
            error2 -= dx*2; 
        } 
    } 
} 


int main(int argc, char** argv) {
    vector<Vertex3D> vertices;
    vector<Vertex2D> vertices2D;
    vector<Triangle> faces;
    string filename = "obj/african_head.obj";
    TGAImage image(width, height, TGAImage::RGB);    
    getFacesAndVertices(filename, vertices, faces, image);
    
    cout << "Vertices:\n";
    for(Vertex3D v : vertices) {
        cout << v.x << "  " << v.y << "  " << v.z << '\n';
        
    }
    //image.set(v.x, v.y, white);  
    //image.write_tga_file("result.tga");
    //cout << "\n\n";
    //TGAImage image(100, 100, TGAImage::RGB);
	//image.set(52, 41, white);
	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    //line(13, 20, 80, 40, image, white); 
	image.write_tga_file("result.tga");
    //cout << "Triangles\n";
    //for(Triangle t : faces) cout << t.v1 << "  " << t.v2 << "  " << t.v3 << '\n';

    return 0;
}
