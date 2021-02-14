#include <vector>
#include <iostream>

#include "tgaimage.h"
#include "geom.h"
#include "our_gl.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const int width  = 800;
const int height = 800;

int getWidth() { return width; }
int getHeight() { return width; }

vector<vector<float>> ModelView;
vector<vector<float>> Viewport;
vector<vector<float>> Projection;

IShader::~IShader() {}

void viewport(int x, int y, int w, int h, int depth) {
    //translation*scaling
    Viewport = identity(4);
    Viewport[0][0] = w/2.f;
    Viewport[1][1] = -h/2.f;
    Viewport[2][2] = depth/2.f;

    Viewport[0][3] = x+w/2.f;
    Viewport[1][3] = y+h/2.f;
    Viewport[2][3] = depth/2.f;
}
void projection(float coeff) {
    Projection = identity(4);
    Projection[3][2] = coeff;
}

void lookAt(const Vertex3D& from, const Vertex3D& to) 
{ 
    ModelView = identity(4); 
    Vertex3D tmp {0,1,0};
    Vertex3D forward = normal(from-to); 
    Vertex3D right = normal(normal(tmp)^forward); 
    Vertex3D up = normal(forward^right); 
    
    ModelView[0][0] = right.x; 
    ModelView[0][1] = right.y; 
    ModelView[0][2] = right.z; 
    ModelView[1][0] = up.x; 
    ModelView[1][1] = up.y; 
    ModelView[1][2] = up.z; 
    ModelView[2][0] = forward.x; 
    ModelView[2][1] = forward.y; 
    ModelView[2][2] = forward.z; 
 
    ModelView[3][0] = -to.x; 
    ModelView[3][1] = -to.y; 
    ModelView[3][2] = -to.z;
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
    return {vertices[index].x,vertices[index].y, vertices[index].z};

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

Vertex3D getNormales(Triangle face, vector<Vertex3D> normales, int k) {
    int index;
    switch(k) {
        case 0: index = face.vn1-1;break;
        case 1: index = face.vn2-1;break;
        case 2: index = face.vn3-1;break;
    }
    return {(normales[index].x), (normales[index].y), normales[index].z};
}

void drawTriangle(Triangle face, Vertex4D v1T, Vertex4D v2T, Vertex4D v3T, IShader &shader,
TGAImage& zbuffer, TGAImage &image, vector<Vertex3D> textures) {
    float z,w,shadow;
    Vertex3D tP;

    Vertex3D t1 = getTextures(face, textures, 0);
    Vertex3D t2 = getTextures(face, textures, 1);
    Vertex3D t3 = getTextures(face, textures, 2);

    //float i1 = getNormales(face, normales, 0)*l;
    //float i2 = getNormales(face, normales, 1)*l;
    //float i3 = getNormales(face, normales, 2)*l;

    Vertex2D xymin {INFINITY,  INFINITY};
    Vertex2D xymax {-INFINITY, -INFINITY};
    Vertex3D v1 = v4tov3(v1T);
    Vertex3D v2 = v4tov3(v2T);
    Vertex3D v3 = v4tov3(v3T);
    xymax.x = max({v1.x, v2.x, v3.x, xymax.x});
    xymax.y = max({v1.y, v2.y, v3.y, xymax.y});
    xymin.x = min({v1.x, v2.x, v3.x, xymin.x});
    xymin.y = min({v1.y, v2.y, v3.y, xymin.y});
    TGAColor color;
    for (int x=xymin.x; x<=xymax.x; x++) {
        for (int y=xymin.y; y<=xymax.y; y++) {
            //Vertex3D v1T = v4tov3(v1);
            //Vertex3D v2T = v4tov3(v2);
            //Vertex3D v3T = v4tov3(v3);
            //cout<<"v1 et v1T "<<v1T.x<<" "<<v1T.w<<" "<<v1T.x/v1T.w<<" "<<v1.x<<"\n";
            Vertex3D bary = barycentric({x,y}, v1, v2, v3);
            
            z = 0;
            z = v1T.z*bary.x+v2T.z*bary.y+v3T.z*bary.z;
            w = v1T.w*bary.x+v2T.w*bary.y+v3T.w*bary.z;
            //cout<<"w "<<v1T.w<<" "<<v2T.w<<" "<<v3T.w<<"\n";
            //tP = t1*bary.x+t2*bary.y+t3*bary.z;
            //shadow = i1*bary.x+i2*bary.y+i3*bary.z;
            //cout<<"Z/w "<<int(z/w+5.)<<"\n";
            int frag_depth = std::min(255, int(z/w));
            //cout<<"frag_depth "<<frag_depth<<"\n";
            //TGAColor color = textureImg.get(tP.x*textureImg.get_width(),tP.y*textureImg.get_height());
            if (bary.x < 0 || bary.y < 0 || bary.z < 0||zbuffer.get(x, y)[0]>frag_depth) { continue; }
            cout<<"Bary "<<bary.x<<" "<<bary.y<<" "<<bary.z<<"\n";
            bool discard = shader.fragment(bary, color);
            if(!discard ){
            //if (zbuffer[int(x+y*width)] < z) {//&& !discard){
                //zbuffer[int(x+y*width)] = z; 
                //image.set(x, y, TGAColor(255*shadow, 255*shadow, 255*shadow,255*shadow));
                zbuffer.set(x, y, TGAColor(frag_depth));
                image.set(x, y, color);
            }
           
        }
    }
}