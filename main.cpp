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
TGAImage textureImg, textureImgNm, textureImgSp;
TGAImage image(getWidth(), getHeight(), TGAImage::RGB);

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
    Vertex3D vTexture[3];
    Vertex3D vNm[3];
    Vertex3D triang[3];
    vector<vector<float>> mp = Projection*ModelView;
    vector<vector<float>> mvp = Viewport*Projection*ModelView;
    vector<vector<float>> mpT = transpose(mp);

    virtual Vertex4D vertex(Triangle face, int vId) {
        vTexture[vId] = getTextures(face, textures, vId);

        Vertex3D nm =  getNormales(face, normales, vId);
        vNm[vId] = mtov(mpT*vtom(nm));

        Vertex3D gl_Vertex = getVertex(face, vertices, vId);
        vector<vector<float>> res = mp*vtom(gl_Vertex);
        triang[vId] = mtov(res);//normal(mtov(mp*vtom(gl_Vertex)));

        return {res[0][0],res[1][0],res[2][0],res[3][0]};
    }

    virtual bool fragment(Vertex3D bary, TGAColor &color) {
        vector<vector<float>> A = identity(4), invA = matrix(4,4),darboux = matrix(3,3);
        Vertex3D texture, nm, u, v, i, j, tangent, bitangent, bitangenttest;
        texture = vTexture[0]*bary.x+vTexture[1]*bary.y+vTexture[2]*bary.z;
        nm = normal(vNm[0]*bary.x+vNm[1]*bary.y+vNm[2]*bary.z);

        Vertex3D pos1 = (triang[1]-triang[0]);
        Vertex3D pos2 = (triang[2]-triang[0]);

        Vertex3D uv1 = (vTexture[1]-vTexture[0]);
        Vertex3D uv2 = (vTexture[2]-vTexture[0]);
        u = {uv1.x, uv2.x,0};
        v = {uv1.y, uv2.y,0};

        fill3m3(A, pos1, pos2, nm);
        if(!inverse(A, invA)) {return true;}
        //inverse(A, invA);
        
        //float f = 1.f/(uv1.x*uv2.y-uv2.x*uv1.y);
        //tangent = normal((pos1*uv2.y+pos2*(-uv1.y))*f);
        //bitangent = normal((pos1*(-uv2.x)+pos2*uv1.x)*f);
        //cout<<"Tan "<<tangent.x<<" "<<tangent.y<<" "<<tangent.z<<"\n";
        color = textureImgNm.get(texture.x*textureImgNm.get_width(),texture.y*textureImgNm.get_height());
        Vertex3D res;
        res.x = color[2]/255.f*2.f-1.f;
        res.y = color[1]/255.f*2.f-1.f;
        res.z = color[0]/255.f*2.f-1.f;
        res = normal(res);
        
        /*Vertex3D T = normal(mtov(mp*vtom(tangent)));
        Vertex3D B = normal(mtov(mp*vtom(bitangent)));
        Vertex3D N = normal(mtov(mp*vtom(nm)));*/
        //T = normal(T-(N*(T*N)));
        //Vertex3D B = N^T;
        Vertex3D T = normal(mtov(invA*vtom(u)));
        Vertex3D B = normal(mtov(invA*vtom(v)));
        //Vertex3D N = normal(mtov(invA*vtom(nm)));
        
        vector<vector<float>> TBN = identity(4);//matrix(3,3);
        fill3m3(TBN,T, B, nm);
        //inverse(TBN,TBN);
        TBN = transpose(TBN);
        
        res = normal(mtov(mp*TBN*vtom(res)));
        //res = normal(mtov(mp*vtom(res)));
        cout<<"Res "<<res.x<<" "<<res.y<<" "<<res.z<<"\n";
        float intensity = max(0.f, res*l);
        color = textureImg.get(texture.x*textureImg.get_width(),texture.y*textureImg.get_height())*intensity;
        return false;
    }
};

int main(int argc, char** argv) {
    int width = getWidth();
    int height = getHeight();
    string filename = "obj/african_head.obj";
    //TGAImage zbuffer(width, height, TGAImage::GRAYSCALE);
    float *zbuffer = new float[width*height];

    for (uint32_t y = 0; y < height; ++y) 
        for (uint32_t x = 0; x < width; ++x) 
            zbuffer[x+y*width] = -INFINITY;
    getFacesAndVertices(filename);
    textureImg.read_tga_file("obj/african_head_diffuse.tga");//grid.tga");//
    textureImgNm.read_tga_file("obj/african_head_nm_tangent.tga");
    //textureImgSp.read_tga_file("obj/african_head_spec.tga");
    textureImg.flip_vertically();
    textureImgNm.flip_vertically();
    //textureImgSp.flip_vertically();

    viewport(width/8, height/8, width*3/4, height*3/4, 255); 
    projection(-1.f/camera.z);
    lookAt(camera, center);
    l = normal(mtov(Projection*ModelView*vtom(l)));
    GouraudShader shader;

    for (size_t i = 0; i<faces.size(); i++) {
        Vertex4D v1 = m4tov4(Viewport*v4tom(shader.vertex(faces[i], 0)));
        Vertex4D v2 = m4tov4(Viewport*v4tom(shader.vertex(faces[i], 1)));
        Vertex4D v3 = m4tov4(Viewport*v4tom(shader.vertex(faces[i], 2)));
        drawTriangle(v1, v2, v3, shader, zbuffer, image);
    }
	//image.flip_vertically(); //-h in viewport make this 
	image.write_tga_file("result.tga");

    return 0;
}
