#include <vector>
#include <cmath>
#include "geom.h"
using namespace std;

vector<vector<float>> matrix(int l, int c) {
    vector<vector<float>> id(l);
    for (int i=0; i<l; i++) {
        id[i].resize(c);
        for (int j=0; j<c; j++) {
            id[i][j] = 0;
        }
    }
    return id;
}

vector<vector<float>> identity(int size) {
    vector<vector<float>> id(size);
    for (int i=0; i<size; i++) {
        id[i].resize(size);
        for (int j=0; j<size; j++) {
            id[i][j] = (i==j ? 1.f : 0.f);
        }
    }
    return id;
}

vector<vector<float>> transpose(vector<vector<float>> m) {
    vector<vector<float>> t = matrix(cols(m),rows(m));
    for (int i = 0; i < rows(m); ++i)
      for (int j = 0; j < cols(m); ++j) {
         t[j][i] = m[i][j];
      }
    return t;
}

int rows(vector<vector<float>> m) {
    return m.size();  
}
int cols(vector<vector<float>> m) {
    return m[0].size();
}
vector<vector<float>> operator*(vector<vector<float>> m1, vector<vector<float>> m2) {
    vector<vector<float>> m = matrix(rows(m1),cols(m2));
    for(int i = 0; i < rows(m1); ++i) {
        for(int j = 0; j < cols(m2); ++j) {
            for(int k = 0; k < cols(m1); ++k) {
                m[i][j] += m1[i][k] * m2[k][j];
            }
        }     
    }
    return m;
}
/*Vertex3D operator*(vector<vector<float>> m1, Vertex3D m2) {
    Vertex3D v;
    v.x = m1[0][0] * m2.x + m1[1][0]* m2.y+m1[2][0]*m2.z;
    v.y = m1[0][1] * m2.x + m1[1][1]* m2.y+m1[2][1]*m2.z;
    v.z = m1[0][2] * m2.x + m1[1][2]* m2.y+m1[2][2]*m2.z;
    return v;
}*/
vector<vector<float>> v3tom(Vertex3D v) {
    vector<vector<float>> m = matrix(3,1);
    m[0][0] = v.x;
    m[1][0] = v.y;
    m[2][0] = v.z;
    return m;
}
Vertex3D m3tov(vector<vector<float>>  m) {
    return {m[0][0], m[1][0], m[2][0]};
}
Vertex4D m4tov4(vector<vector<float>>  m) {
    return {m[0][0], m[1][0], m[2][0], m[3][0]};
}

vector<vector<float>> vtom(Vertex3D v) {
    vector<vector<float>> m = matrix(4,1);
    m[0][0] = v.x;
    m[1][0] = v.y;
    m[2][0] = v.z;
    m[3][0] = 1.f;
    return m;
}

Vertex3D mtov(vector<vector<float>>  m) {
    return {m[0][0]/m[3][0], m[1][0]/m[3][0], m[2][0]/m[3][0]};
}
Vertex3D v4tov3(Vertex4D v) {
    return {v.x/v.w, v.y/v.w, v.z/v.w};
}
vector<vector<float>> v4tom(Vertex4D v) {
    vector<vector<float>> m = matrix(4,1);
    m[0][0] = v.x;
    m[1][0] = v.y;
    m[2][0] = v.z;
    m[3][0] = v.w;
    return m;
}
//dot product
float operator*(Vertex3D v1, Vertex3D v2) {
    return v1.x*v2.x+v1.y*v2.y+v1.z*v2.z;
}
//cross product
Vertex3D operator^(Vertex3D v1, Vertex3D v2) {
    return {v1.y*v2.z - v1.z*v2.y, v1.z*v2.x - v1.x*v2.z, v1.x*v2.y - v1.y*v2.x};
}
Vertex3D operator-(Vertex3D v1, Vertex3D v2) {
    return {v1.x-v2.x, v1.y-v2.y, v1.z-v2.z};
}
Vertex4D operator-(Vertex4D v1, Vertex4D v2) {
    return {v1.x-v2.x, v1.y-v2.y, v1.z-v2.z, v1.w-v2.w};
}
Vertex3D operator+(Vertex3D v1, Vertex3D v2) {
    return {v1.x+v2.x, v1.y+v2.y, v1.z+v2.z};
}
Vertex3D operator*(Vertex3D v1, float alpha) {
    return {v1.x*alpha, v1.y*alpha, v1.z*alpha};
}
Vertex3D operator/(Vertex3D v1, float alpha) {
    return {v1.x/alpha, v1.y/alpha, v1.z/alpha};
}

Vertex3D normal(Vertex3D v){
    float magnitude = sqrt(v*v);
    return {v.x/magnitude, v.y/magnitude, v.z/magnitude};
}

Vertex3D barycentric(Vertex3D p, Vertex3D v1, Vertex3D v2, Vertex3D v3) {
    float u,v,w, den;
    Vertex3D v12 = v2-v1, v13 = v3-v1, v1p = p-v1;
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
vector<vector<float>> fill3m3(vector<vector<float>> &m, Vertex3D r1, Vertex3D r2, Vertex3D r3) {
    m[0][0] = r1.x; 
    m[0][1] = r1.y; 
    m[0][2] = r1.z; 
    m[1][0] = r2.x; 
    m[1][1] = r2.y; 
    m[1][2] = r2.z; 
    m[2][0] = r3.x; 
    m[2][1] = r3.y; 
    m[2][2] = r3.z; 
    return m;
}

float det(vector<vector<float>> A, int n) { 
    float D = 0.f;
    if (n == 1) 
        return A[0][0]; 
    else if (n == 2) {
        return (A[0][0]*A[1][1]-A[0][1]*A[1][0]);
    }
    vector<vector<float>> temp = matrix(rows(A),rows(A));
  
    for (int j = 0; j < n; j++) {
        int subi = 0;
        for (int k = 1; k < n; k++) {
            int subj = 0;
        for (int l = 0; l < n; l++) {
            if (l == j) continue;
            temp[subi][subj] = A[k][l];
            subj++;
        }
        subi++;
        }
        D += A[0][j]*pow(-1,j)*det(temp, n-1); 
    } 
    return D; 
}

vector<vector<float>> cofactor(vector<vector<float>> A,int n,float determ) {
	vector<vector<float>> b = matrix(n,n),c = matrix(n,n), d = matrix(n,n);
	int l,h,m,k,i,j;
	for (h=0;h<n;h++)
		for (l=0;l<n;l++) {
			m=0;
			k=0;
			for (i=0;i<n;i++)
				for (j=0;j<n;j++)
					if (i != h && j != l) {
						b[m][k]=A[i][j];
						if (k<(n-2))
							k++;
						else {
							k=0;
							m++;
						}
					}
			c[h][l] = (float) pow(-1,(h+l))*det(b,(n-1));	// c = cofactor Matrix
		}
	//c = transpose(c);
    for (i=0;i<n;i++)
		for (j=0;j<n;j++) {
            d[i][j] = c[i][j]/determ;
        }
    return d;	
}


bool inverse(vector<vector<float>> A, vector<vector<float>>& inverse) { 
    float determ = det(A, rows(A)); 
    if (determ == 0.f) { 
        return false; 
    } else if(rows(A) == 1) {
		inverse[0][0] = 1.f;
    } else { 
        inverse = cofactor(A,rows(A),determ);
    }
    return true; 
} 

// Inverse de matrice
vector<vector<float>> inverseM(vector<vector<float>> matr) {
	float s0 = matr[0][0] * matr[1][1] - matr[1][0] * matr[0][1];
	float s1 = matr[0][0] * matr[1][2] - matr[1][0] * matr[0][2];
	float s2 = matr[0][0] * matr[1][3] - matr[1][0] * matr[0][3];
	float s3 = matr[0][1] * matr[1][2] - matr[1][1] * matr[0][2];
	float s4 = matr[0][1] * matr[1][3] - matr[1][1] * matr[0][3];
	float s5 = matr[0][2] * matr[1][3] - matr[1][2] * matr[0][3];

	float c5 = matr[2][2] * matr[3][3] - matr[3][2] * matr[2][3];
	float c4 = matr[2][1] * matr[3][3] - matr[3][1] * matr[2][3];
	float c3 = matr[2][1] * matr[3][2] - matr[3][1] * matr[2][2];
	float c2 = matr[2][0] * matr[3][3] - matr[3][0] * matr[2][3];
	float c1 = matr[2][0] * matr[3][2] - matr[3][0] * matr[2][2];
	float c0 = matr[2][0] * matr[3][1] - matr[3][0] * matr[2][1];

	// Should check for 0 determinant
	float invdet = 1.0 / (s0 * c5 - s1 * c4 + s2 * c3 + s3 * c2 - s4 * c1 + s5 * c0);

	vector<vector<float>> inv = matrix(rows(matr), cols(matr));

	inv[0][0] = (matr[1][1] * c5 - matr[1][2] * c4 + matr[1][3] * c3) * invdet;
	inv[0][1] = (-matr[0][1] * c5 + matr[0][2] * c4 - matr[0][3] * c3) * invdet;
	inv[0][2] = (matr[3][1] * s5 - matr[3][2] * s4 + matr[3][3] * s3) * invdet;
	inv[0][3] = (-matr[2][1] * s5 + matr[2][2] * s4 - matr[2][3] * s3) * invdet;

	inv[1][0] = (-matr[1][0] * c5 + matr[1][2] * c2 - matr[1][3] * c1) * invdet;
	inv[1][1] = (matr[0][0] * c5 - matr[0][2] * c2 + matr[0][3] * c1) * invdet;
	inv[1][2] = (-matr[3][0] * s5 + matr[3][2] * s2 - matr[3][3] * s1) * invdet;
	inv[1][3] = (matr[2][0] * s5 - matr[2][2] * s2 + matr[2][3] * s1) * invdet;

	inv[2][0] = (matr[1][0] * c4 - matr[1][1] * c2 + matr[1][3] * c0) * invdet;
	inv[2][1] = (-matr[0][0] * c4 + matr[0][1] * c2 - matr[0][3] * c0) * invdet;
	inv[2][2] = (matr[3][0] * s4 - matr[3][1] * s2 + matr[3][3] * s0) * invdet;
	inv[2][3] = (-matr[2][0] * s4 + matr[2][1] * s2 - matr[2][3] * s0) * invdet;

	inv[3][0] = (-matr[1][0] * c3 + matr[1][1] * c1 - matr[1][2] * c0) * invdet;
	inv[3][1] = (matr[0][0] * c3 - matr[0][1] * c1 + matr[0][2] * c0) * invdet;
	inv[3][2] = (-matr[3][0] * s3 + matr[3][1] * s1 - matr[3][2] * s0) * invdet;
	inv[3][3] = (matr[2][0] * s3 - matr[2][1] * s1 + matr[2][2] * s0) * invdet;

	return inv;
}