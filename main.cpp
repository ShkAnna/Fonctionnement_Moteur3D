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
#include "geom.h"
#include "our_gl.h"
using namespace std;

Vertex3D l =normal({1,1,1});//{1,-1,1} //vecteur direction du lumiere 
Vertex3D camera {1,1,3};//{0,0,3};
Vertex3D center {0,0,0};

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
    float x, y, z;
    string sv1, sv2, sv3, svt1, svt2, svt3, svn1, svn2, svn3;
    vector<string> termes;

    ifstream in( filename );
    while(getline(in, line)) {
        if(line.find_first_of("vVfF") == string::npos) continue;
        
        istringstream ss( line );
        ss >> str;
        str2 = line.substr(0, 2);
        switch(str2int(str2.c_str())) {
            case str2int("v "):
            {
                ss >> x >> y >> z;
                vertices.push_back({x, y, z});
                break;
            }
            case str2int("f "):
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
                ss >> x >> y >> z;
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

struct GouraudShader : public IShader {
    Vertex3D vIntensity;

    virtual Vertex4D vertex(Triangle face, int vId) {
        float i = getNormales(face, normales, vId)*l;
        switch(vId) {
            case 0 : 
                vIntensity.x = i;
                break;
            case 1 : 
                vIntensity.y = i;
                break;
            case 2 : 
                vIntensity.z = i;
                break;
        }
        Vertex3D gl_Vertex = getVertex(face, vertices, vId);
        vector<vector<float>> res = Viewport*Projection*ModelView*vtom(gl_Vertex);
        return {res[0][0],res[1][0],res[2][0],res[3][0]};
    }

    virtual bool fragment(Vertex3D bary, TGAColor &color) {
        float intensity = vIntensity*bary;
        //color = TGAColor(255, 255, 255)*intensity;
        if (intensity>.85) intensity = 1;
        else if (intensity>.60) intensity = .80;
        else if (intensity>.45) intensity = .60;
        else if (intensity>.30) intensity = .45;
        else if (intensity>.15) intensity = .30;
        else intensity = 0;
        color = TGAColor(255, 155, 0)*intensity;
        return false;
    }
};

int main(int argc, char** argv) {
    int width = getWidth();
    int height = getHeight();
    string filename = "obj/african_head.obj";
    TGAImage image(width, height, TGAImage::RGB);
    TGAImage zbuffer(width, height, TGAImage::GRAYSCALE);
    getFacesAndVertices(filename);
    textureImg.read_tga_file("obj/african_head_diffuse.tga");
    textureImg.flip_vertically();

    viewport(width/8, height/8, width*3/4, height*3/4, 255); //0.0 and 1.0 to enable the system to render to the entire range of depth values in the depth buffer
    projection(-1.f/camera.z);
    lookAt(camera, center);
    GouraudShader shader;

    for (size_t i = 0; i<faces.size(); i++) {
        Vertex4D v1 = shader.vertex(faces[i], 0);
        Vertex4D v2 = shader.vertex(faces[i], 1);
        Vertex4D v3 = shader.vertex(faces[i], 2);
        drawTriangle(faces[i], v1, v2, v3, shader, zbuffer, image, textures);
    }
	//image.flip_vertically(); //-h in viewport make this 
	image.write_tga_file("result.tga");

    return 0;
}
