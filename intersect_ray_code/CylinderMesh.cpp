#include "CylinderMesh.h"

CylinderMesh::CylinderMesh(unsigned int param1, unsigned int param2, float param3) : Mesh(param1, param2, param3)
{
    update();
}

CylinderMesh::~CylinderMesh(){

}

void CylinderMesh::update()
{
    unsigned int s = m_param2;
    unsigned int t = m_param1+2;

    float r = .5;
    float dd = r/s;
    float dphi = 2*M_PI/t;

    glm::vec3 normal = {0,1,0};
    float y = .5;

    for(unsigned int i = 0; i < t+1; i++){
        for(unsigned int j = 0; j < s+1; j++){
            float phi = i*dphi;
            float d = j*dd;

            appendVertexData(d*cos(phi), y, d*sin(phi));
            appendVertexData(normal);

            appendVertexData(d*cos(phi + dphi), y, d*sin(phi +dphi));
            appendVertexData(normal);

            if(j == s && i != t){
                appendVertexData(d*cos(phi + dphi), y, d*sin(phi +dphi));
                appendVertexData(normal);

                appendVertexData(0,y,0);
                appendVertexData(zero);
            }
            if(j == s && i == t){
                appendVertexData(d*cos(phi + dphi), y, d*sin(phi +dphi));
                appendVertexData(normal);
            }
        }
    }

    normal = {0,-1,0};
    y = -.5;
    for(unsigned int i = 0; i < t+1; i++){
        for(unsigned int j = 0; j < s+1; j++){
            float phi = i*dphi;
            float d = j*dd;

            if(i == 0 && j == 0){
                appendVertexData(d*cos(phi + dphi), y, d*sin(phi +dphi));
                appendVertexData(normal);
            }

            appendVertexData(d*cos(phi + dphi), y, d*sin(phi +dphi));
            appendVertexData(normal);

            appendVertexData(d*cos(phi), y, d*sin(phi));
            appendVertexData(normal);

            if(j == s && i != t){ // zero triangle
                appendVertexData(d*cos(phi), y, d*sin(phi));
                appendVertexData(normal);

                appendVertexData(0,y,0);
                appendVertexData(zero);
            }
            if(j == s && i == t){
                appendVertexData(d*cos(phi), y, d*sin(phi));
                appendVertexData(zero);
            }
        }
    }

    float dy = 1.0/s;
    for(unsigned int i = 0; i < s; i++){
        for(unsigned int j = 0; j < t+1; j++){
            float phi = j*dphi;
            float x1 = r*cos(phi);
            float z1 = r*sin(phi);
            float y1 = .5 - dy*i;
            float y2 = .5 - dy*(i+1);

            normal = glm::normalize(glm::vec3(x1,0,z1));

            appendVertexData(x1, y1, z1);
            appendVertexData(normal);

            appendVertexData(x1, y2, z1);
            appendVertexData(normal);
        }
    }

    initializeVAO();
}

Intersection CylinderMesh::intersect(glm::vec3 P, glm::vec3 d)
{
    Intersection i;

    float a = d.x*d.x + d.z*d.z;
    float b = 2*P.x*d.x + 2*P.z*d.z;
    float c = P.x*P.x + P.z*P.z -.25;

    // cylinder infinite cylinder
    float t1 = quadraticIntersect(a, b, c);
    float y1 = P.y + t1*d.y;
    if(y1 > .5 || y1 < -.5){
        t1 = -1;
    }

    // cylinder top cap
    float t2 = (.5 - P.y)/d.y;
    float x2 = P.x + t2*d.x;
    float z2 = P.z + t2*d.z;
    if(x2*x2 + z2*z2 > .25){
        t2 = -1;
    }

    // cylinder bottom cap
    float t3 = (-.5 - P.y)/d.y;
    float x3 = P.x + t3*d.x;
    float z3 = P.z + t3*d.z;
    if(x3*x3 + z3*z3 > .25){
        t3 = -1;
    }

    i.t = smallestNonNegative(t1, smallestNonNegative(t2, t3));

    // return function bc no intersectiondo not need to calculate point and normal
    if(i.t == -1){
        return i;
    }

    // calculate intersection point and normal
    i.point = P + i.t*d;

    if(i.t == t1){
        i.normal.x = i.point.x;
        i.normal.y = 0;
        i.normal.z = i.point.z;
        i.normal = glm::normalize(i.normal);
    }
    else if(i.t == t2){
        i.normal = {0,1,0};
    }
    else if(i.t == t3){
        i.normal = {0,-1,0};
    }

    return i;
}
