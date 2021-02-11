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
struct Triangle { int v1, v2, v3, vt1, vt2, vt3, vn1, vn2, vn3; };

vector<Vertex3D> vertices;
vector<Vertex3D> textures;
vector<Vertex3D> normales;
vector<Vertex2D> vertices2D;
vector<Triangle> faces;
TGAImage textureImg;

constexpr unsigned int str2int(const char* str, int h = 0)
{
    return !str[h] ? 5381 : (str2int(str, h+1) * 33) ^ str[h];
}

string remplacerSlash(string s)
{
    //Remplace les '/' par des espaces.
    string ret="";
    for(unsigned int i=0;i<s.size();i++)
    {
        if(s[i]=='/')
            ret+=' ';
        else
            ret+=s[i];
    }
    return ret;
}
vector<string> splitSpace(string s)
{
    //Eclate une chaîne au niveau de ses espaces.
    vector<string> ret;
    string s1="";
    for(unsigned int i=0;i<s.size();i++)
    {
        if(s[i]==' '||i==s.size()-1)
        {
            if(i==s.size()-1)
                s1+=s[i];
            ret.push_back(s1);
            s1="";
        }
        else
            s1+=s[i];
    }
    return ret;
}

void getFacesAndVertices( string filename)
{
    faces.clear();   vertices.clear();  
    string line, str, str2;
    int v1, v2, v3;
    float x, y, z;
    string sv1, sv2, sv3, svt1, svt2, svt3, svn1, svn2, svn3;
    vector<string> termes;

    ifstream in( filename );
    while(getline(in, line)) {
        if(line.find_first_of("vVfF") == string::npos) continue;
        
        istringstream ss( line );
        //cout<<"line "<<line[0]<<'\n';
        ss >> str;
        str2 = line.substr(0, 2);
        //cout<<"str2 "<<str2;
        switch(str2int(str2.c_str())) {//str2int(str2.c_str())
            case str2int("v "):
            //case str2int("V "):
            {
                ss >> x >> y >> z;
                vertices.push_back({x, y, z});
                break;
            }
            case str2int("f "):
            //case str2int("F "):
            {
                line = remplacerSlash(line);
                istringstream ss( line );
                ss >> str;
                ss >> sv1 >> svt1 >> svn1 >> sv2 >> svt2 >> svn2 >> sv3 >> svt3 >> svn3;
                faces.push_back({stoi(sv1), stoi(sv2), stoi(sv3), stoi(svt1), 
                stoi(svt2), stoi(svt3), stoi(svn1), stoi(svn2), stoi(svn3)});
                break;
            }
            case str2int("vt"):
            {
                //cout<<"ss "<< line;
                ss >> x >> y >> z;
                //cout<<"x "<<x<<" y "<<y<<" z \n";
                textures.push_back({x, y, z});
                break;
            }
            case str2int("vn"):
            {
                ss >> x >> y >> z;
                normales.push_back({x, y, z});
                break;
            }
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
    return {(vertices[index].x+1.f)*width/2.f, (vertices[index].y+1.f)*height/2.f, vertices[index].z*width/2};
}

Vertex3D getTextures(Triangle face, vector<Vertex3D> textures, int k) {
    int index;
    switch(k) {
        case 0: index = face.vt1-1;break;
        case 1: index = face.vt2-1;break;
        case 2: index = face.vt3-1;break;
    }
    return {(textures[index].x), (textures[index].y), textures[index].z};
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

Vertex2D getUV(Vertex3D v, Vertex2D xymin, Vertex2D xymax) {
    v.x = (v.x - xymin.x) / (xymax.x - xymin.x);
    v.y = (v.y - xymin.y) / (xymax.y - xymin.y);
    return {v.x, v.y};
}

void drawTriangle(Triangle face, float *zbuffer, TGAImage &image, float intensity) {
    float z;
    Vertex3D tP;
    Vertex3D v1 = getVertex(face, vertices, 0);
    Vertex3D v2 = getVertex(face, vertices, 1);
    Vertex3D v3 = getVertex(face, vertices, 2);
    Vertex3D t1 = getTextures(face, textures, 0);
    Vertex3D t2 = getTextures(face, textures, 1);
    Vertex3D t3 = getTextures(face, textures, 2);

    Vertex2D xymin {INFINITY,  INFINITY};
    Vertex2D xymax {-INFINITY, -INFINITY};

    xymax.x = max({v1.x, v2.x, v3.x, xymax.x});
    xymax.y = max({v1.y, v2.y, v3.y, xymax.y});
    xymin.x = min({v1.x, v2.x, v3.x, xymin.x});
    xymin.y = min({v1.y, v2.y, v3.y, xymin.y});

    for (int x=xymin.x; x<=xymax.x; x++) {
        for (int y=xymin.y; y<=xymax.y; y++) {
            Vertex3D bary = barycentric({x,y}, v1, v2, v3);
            int x2 = (bary.x - xymin.x) / (xymax.x - xymin.x);
            int y2 = (bary.y - xymin.y) / (xymax.y - xymin.y);
            if (bary.x < 0 || bary.y < 0 || bary.z < 0) { continue; }
            z = 0;
            z = v1.z*bary.x+v2.z*bary.y+v3.z*bary.z;
            cout<<"x2 "<<x2<<" y2 "<<y2<<'\n';
           
            tP = add(add(multip(t1,bary.x),multip(t2,bary.y)),multip(t3,bary.z));
            TGAColor color = textureImg.get(tP.x*textureImg.get_width(),tP.y*textureImg.get_height());
           
            if (zbuffer[int(x+y*width)] < z) {
                zbuffer[int(x+y*width)] = z;
                image.set(x, y, TGAColor(color.r*intensity,color.g*intensity,color.b*intensity, color.a*intensity));
            }
           
        }
    }
}

int main(int argc, char** argv) {
    
    string filename = "obj/african_head.obj";
    TGAImage image(width, height, TGAImage::RGB);
    Vertex3D l {0,0,-1}; //vecteur direction du lumiere 
    getFacesAndVertices(filename);
    textureImg.read_tga_file("obj/african_head_diffuse.tga");
    textureImg.flip_vertically();

    float *zbuffer = new float[width*height];

    for (uint32_t y = 0; y < height; ++y) 
        for (uint32_t x = 0; x < width; ++x) 
            zbuffer[x+y*width] = -INFINITY;

    for (size_t i = 0; i<faces.size(); i++) {

        Vertex3D v1 = getVertex(faces[i], vertices, 0);
        Vertex3D v2 = getVertex(faces[i], vertices, 1);
        Vertex3D v3 = getVertex(faces[i], vertices, 2);
    
        Vertex3D n = crossProduct3D(diff(v1,v3),diff(v1,v2));
        float magnitude = sqrt(dotProduct3D(n,n));
        n = {n.x/magnitude, n.y/magnitude, n.z/magnitude};
        float intensity = dotProduct3D(multip(l,1),n);
        if (intensity>0) {
            drawTriangle(faces[i], zbuffer, image, intensity);
        }
    }
	image.flip_vertically();
	image.write_tga_file("result.tga");

    return 0;
}
