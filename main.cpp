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
Vertex3D camera {1.2,-.8,3};//{0,0,3};
Vertex3D center {0,0,0};

vector<Vertex3D> vertices;
vector<Vertex3D> textures;
vector<Vertex3D> normales;
vector<Vertex2D> vertices2D;
vector<Triangle> faces;
TGAImage textureImg, textureImgNm, textureImgSp;
TGAImage image(getWidth(), getHeight(), TGAImage::RGB);
TGAImage depth(getWidth(), getHeight(), TGAImage::RGB);
float *shadowbuffer;

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

struct Shader : public IShader {
    Vertex3D vTexture[3];
    //Vertex3D vNm[3];
    Vertex3D triang[3];
    vector<vector<float>> mp = Projection*ModelView;
    vector<vector<float>> mvp = Viewport*Projection*ModelView;
    vector<vector<float>> mpT = transpose(mp);

    virtual Vertex4D vertex(Triangle face, int vId) {
        vTexture[vId] = getTextures(face, textures, vId);

        //Vertex3D nm =  getNormales(face, normales, vId);
        //vNm[vId] = mtov(mp*vtom(nm));

        Vertex3D gl_Vertex = getVertex(face, vertices, vId);
        vector<vector<float>> res = mvp*vtom(gl_Vertex);
        triang[vId] = mtov(res);
        return m4tov4(res);
    }

    virtual bool fragment(Vertex3D bary, TGAColor &color) {
        Vertex3D texture,tri,unishadow; 
        vector<vector<float>> mvpInv = matrix(4,4), mpIT = matrix(4,4);
        texture = vTexture[0]*bary.x+vTexture[1]*bary.y+vTexture[2]*bary.z;
        tri = triang[0]*bary.x+triang[1]*bary.y+triang[2]*bary.z;

        color = textureImgNm.get(texture.x*textureImgNm.get_width(),texture.y*textureImgNm.get_height());
        Vertex3D res;
        res.x = color[2]/255.f*2.f - 1.f;
        res.y = color[1]/255.f*2.f - 1.f;
        res.z = color[0]/255.f*2.f - 1.f;

        inverse(mvp, mvpInv);
        mvpInv = transpose(mvpInv);
        unishadow = mtov(mvp*mvpInv*vtom(tri));
        int idx = int(unishadow.x)+int(unishadow.y)*getWidth();
        float shadow = .3+.7*(shadowbuffer[idx] < unishadow.z+43.34);
        
        inverse(mp,mpIT);
        vector<vector<float>> modelv = matrix(4,4);
        inverse(ModelView,modelv);
        Vertex3D n = normal(mtov(modelv*vtom(res)));
        l = normal(mtov(mpIT*vtom(l)));
        Vertex3D r = normal(n*(n*l*2.f)-l);
        float tmpSp = textureImgSp.get(texture.x*textureImgSp.get_width(),texture.y*textureImgSp.get_height())[0]/1.f;
        float spec = pow(max(r.z, 0.0f),tmpSp);
        float diff = max(0.f, n*l);
        color = textureImg.get(texture.x*textureImg.get_width(),texture.y*textureImg.get_height());
        for (int i=0; i<3; i++) color[i] = min<float>(20 + color[i]*shadow*(1.2*diff + .6*spec), 255);
        return false;
    }
};

struct DepthShader : public IShader {
    Vertex3D triang[3];

    virtual Vertex4D vertex(Triangle face, int vId) {
        Vertex3D gl_Vertex = getVertex(face, vertices, vId);
        vector<vector<float>> res = Viewport*Projection*ModelView*vtom(gl_Vertex);
        triang[vId] = mtov(res);
        return m4tov4(res);
    }

    virtual bool fragment(Vertex3D bary, TGAColor &color) {
        Vertex3D tri = triang[0]*bary.x+triang[1]*bary.y+triang[2]*bary.z;

        color = TGAColor(255,255,255)*(tri.z/255.f);
        return false;
    }
};

struct ZShader : public IShader {
    Vertex3D triang[3];

    virtual Vertex4D vertex(Triangle face, int vId) {
        Vertex3D gl_Vertex = getVertex(face, vertices, vId);
        vector<vector<float>> res = Viewport*Projection*ModelView*vtom(gl_Vertex);
        triang[vId] = mtov(res);
        return m4tov4(res);
    }

    virtual bool fragment(Vertex3D bary, TGAColor &color) {
        color = TGAColor(0, 0, 0);
        return false;
    }
};

float max_elevation_angle(float *zbuffer, Vertex3D p, Vertex3D dir) {
    float maxangle = 0;
    for (float t = 0.; t < 1000.; t++) {
        Vertex3D cur = p + dir*t;
        if (cur.x >= getWidth() || cur.y >= getHeight() || cur.x < 0 || cur.y < 0) return maxangle;

        float distance = sqrt((p-cur)*(p-cur));
        if (distance < 1.f) continue;
        float elevation = zbuffer[int(cur.x)+int(cur.y)*getWidth()]-zbuffer[int(p.x)+int(p.y)*getWidth()];
        maxangle = max(maxangle, atanf(elevation/distance));
    }
    return maxangle;
}

int main(int argc, char** argv) {
    int width = getWidth();
    int height = getHeight();
    string filename = "obj/african_head.obj";
    float *zbuffer = new float[width*height];

    for (int y = 0; y < height; ++y) 
        for (int x = 0; x < width; ++x) 
            zbuffer[x+y*width] = -INFINITY;

    getFacesAndVertices(filename);
    textureImg.read_tga_file("obj/african_head_diffuse.tga");
    textureImgNm.read_tga_file("obj/african_head_nm_tangent.tga");
    textureImgSp.read_tga_file("obj/african_head_spec.tga");
    textureImg.flip_vertically();
    textureImgNm.flip_vertically();
    textureImgSp.flip_vertically();

    viewport(width/8, height/8, width*3/4, height*3/4, 255); 
    
    lookAt(camera, center);
    Vertex3D cc = camera-center;
    projection(-1.f/sqrt(cc*cc));
    
    ZShader zshader;
    for (size_t i = 0; i<faces.size(); i++) {
        Vertex4D v1 = zshader.vertex(faces[i], 0);
        Vertex4D v2 = zshader.vertex(faces[i], 1);
        Vertex4D v3 = zshader.vertex(faces[i], 2);
        
        drawTriangle(v1, v2, v3, zshader, zbuffer, image);
    }
    
    float ssao;
    for (int x=0; x<width; x++) {
        for (int y=0; y<height; y++) {
            if (zbuffer[x+y*width] < 0) continue;
            ssao = 0;
            for (float angle = 0; angle < M_PI*2-1e-4; angle += M_PI/4) {
                ssao += M_PI/2 - max_elevation_angle(zbuffer, {x, y, 0}, {cos(angle), sin(angle), 0});
            }
            ssao /= (M_PI/2)*8;
            image.set(x, y, TGAColor(ssao*255,ssao*255,ssao*255));
        }
    }

	image.write_tga_file("result.tga");

    return 0;
}
