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

struct Vertex3D { double x, y, z; };
struct Triangle { int v1, v2, v3; };

constexpr unsigned int str2int(const char* str, int h = 0)
{
    return !str[h] ? 5381 : (str2int(str, h+1) * 33) ^ str[h];
}

void getFacesAndVertices( string filename, vector<Vertex3D> &vertices, vector<Triangle> &faces)
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


int main(int argc, char** argv) {
    vector<Vertex3D> vertices;
    vector<Triangle> faces;
    string filename = "obj/african_head.obj";
    TGAImage image(width, height, TGAImage::RGB);    
    getFacesAndVertices(filename, vertices, faces);
    
    cout << "Vertices:\n";
    for(Vertex3D v : vertices) cout << v.x << "  " << v.y << "  " << v.z << '\n';

    cout << "\n\n";

    //cout << "Triangles\n";
    //for(Triangle t : faces) cout << t.v1 << "  " << t.v2 << "  " << t.v3 << '\n';

    return 0;
}
