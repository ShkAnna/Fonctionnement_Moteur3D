#ifndef __OUR_GL_H__
#define __OUR_GL_H__

#include "tgaimage.h"
#include "geom.h"

extern vector<vector<float>> ModelView;
extern vector<vector<float>> Viewport;
extern vector<vector<float>> Projection;

struct IShader {
    virtual ~IShader();
    virtual Vertex4D vertex(Triangle iface, int nthvert) = 0;
    virtual bool fragment(Vertex3D bary, TGAColor &color) = 0;
};

int getWidth();
int getHeight();
void viewport(int x, int y, int w, int h, int depth);
void projection(float coeff);
void lookAt(const Vertex3D& from, const Vertex3D& to);
void line(Vertex3D vx1,  Vertex3D vx2, TGAImage &image, TGAColor color);

Vertex3D getVertex(Triangle face, vector<Vertex3D> vertices, int k);
Vertex3D getTextures(Triangle face, vector<Vertex3D> textures, int k);
Vertex3D getNormales(Triangle face, vector<Vertex3D> normales, int k);
void drawTriangle(Vertex4D v1, Vertex4D v2, Vertex4D v3, IShader &shader,
float *zbuffer, TGAImage &image);

#endif