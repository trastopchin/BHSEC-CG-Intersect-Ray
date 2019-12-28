#include "ConeMesh.h"

ConeMesh::ConeMesh(unsigned int param1, unsigned int param2, float param3) : Mesh(param1, param2, param3)
{
    update();
}

ConeMesh::~ConeMesh()
{

}

void ConeMesh::update()
{
    const unsigned int s = m_param2;
    const unsigned int t = m_param1+2;

    const float dy = 1.0/s;
    const float dphi = 2.0*M_PI/t;

    glm::vec3 normal = zero;

    for(unsigned int i = 0; i < s; i++){
        for(unsigned int j = 0; j < t+1; j++){
            float phi = j*dphi;

            float y1 = dy*i -.5;
            float y2 = dy*(i+1) -.5;

            float r1 = -.5*y1 + .25;
            float r2 = -.5*y2 + .25;

            float x1 = r1*cos(phi);
            float z1 = r1*sin(phi);

            float x2 = r2*cos(phi);
            float z2 = r2*sin(phi);

            normal.x = 2*cos(phi);
            normal.z = 2*sin(phi);
            normal.y = 1;

            /*
             * How do I calculate this normal?
             */
            normal = glm::normalize(normal);

            appendVertexData(x1,y1,z1);
            appendVertexData(normal);

            appendVertexData(x2,y2,z2);
            appendVertexData(normal);

            if(i == s-1 && j == t){
                appendVertexData(x2,y2,z2);
                appendVertexData(normal);
            }
        }
    }

    const float dd = .5/s;
    normal = {0,-1,0};
    const float y = -.5;
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
        }
    }

    initializeVAO();
}

Intersection ConeMesh::intersect(glm::vec3 P, glm::vec3 d)
{
    Intersection i;

    float a = d.x*d.x + d.z*d.z - d.y*d.y/4.0;
    float b = 2*P.x*d.x + 2*P.z*d.z + d.y/4.0 - P.y*d.y/2.0;
    float c = P.x*P.x + P.z*P.z + P.y/4.0 - P.y*P.y/4.0 - 1.0/16.0;

    // cone body intersection
    float t1 = quadraticIntersect(a, b, c);
    float y1 = P.y + t1*d.y;
    if(y1 > .5 || y1 < -.5){
        t1 = -1;
    }

    // cone cap intersection
    float t2 = (-.5 - P.y)/d.y;
    float x2 = P.x + t2*d.x;
    float z2 = P.z + t2*d.z;
    if(x2*x2 + z2*z2 <= .25){
        i.t = smallestNonNegative(t2, quadraticIntersect(a, b, c));
    }
    else{
        i.t = t1;
    }

    // return function bc no intersection do not need to calculate point and normal
    if(i.t == -1){
        return i;
    }

    // calculate intersection point and normal
    i.point = P + i.t*d;

    if(i.t == t1){
        glm::vec3 levelSurfacePoint = i.point;
        levelSurfacePoint.y -= .5;
        levelSurfacePoint.y *= .5;
        i.normal.x = 2*levelSurfacePoint.x;
        i.normal.y = -levelSurfacePoint.y;
        i.normal.z = 2*levelSurfacePoint.z;

        i.normal = glm::normalize(i.normal);

        glm::vec3 zero = {0,0,0};
        if(levelSurfacePoint == zero){
            i.normal = {0,1,0};
        }
    }
    else if(i.t == t2){
        i.normal = {0,-1,0};
    }

    return i;
}
