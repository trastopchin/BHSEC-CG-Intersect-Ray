#include "SphereMesh.h"

SphereMesh::SphereMesh(unsigned int param1, unsigned int param2, float param3) : Mesh(param1, param2, param3)
{
    update();
}

SphereMesh::~SphereMesh()
{

}

void SphereMesh::update()
{
    unsigned int s = m_param1+2;
    unsigned int t = m_param2+1;

    float theta;
    float dtheta = M_PI/t;
    float phi;
    float dphi = -2*M_PI/s;

    float r = .5;

    glm::vec3 normal = zero;

    for(unsigned int i = 0; i < s; i++){ // the amount of wedges
        for(unsigned int j = 0; j < t; j++){ // the vertices per wedge
            phi = i*dphi;
            theta = j*dtheta;

            float x = r*sin(theta)*cos(phi);
            float y = r*cos(theta);
            float z = r*sin(theta)*sin(phi);

            normal = glm::normalize(glm::vec3(x,y,z));

            appendVertexData(x,y,z);
            appendVertexData(normal);

            if(j == 0){ // repeat if first vertex in wedge
                appendVertexData(x,y,z);
                appendVertexData(normal);
            }

            x = r*sin(theta+dtheta)*cos(phi+dphi);
            y = r*cos(theta+dtheta);
            z = r*sin(theta+dtheta)*sin(phi+dphi);

            normal = glm::normalize(glm::vec3(x,y,z));

            appendVertexData(x,y,z);
            appendVertexData(normal);

            if(j == t-1){ // repeat last vertex (at bottom)
                appendVertexData(x,y,z);
                appendVertexData(normal);
            }

        }
    }
    initializeVAO();
}

Intersection SphereMesh::intersect(glm::vec3 P, glm::vec3 d)
{
    Intersection i;

    float a = d.x*d.x + d.y*d.y + d.z*d.z;
    float b = 2*P.x*d.x + 2*P.y*d.y + 2*P.z*d.z;
    float c = P.x*P.x + P.y*P.y + P.z*P.z -.25;

    // sphere body intersection
    i.t = quadraticIntersect(a, b, c);

    // return function bc no intersectiondo not need to calculate point and normal
    if(i.t == -1){
        return i;
    }

    // calculate intersection point and normal
    i.point = P + i.t*d;
    i.normal = glm::normalize(i.point);

    return i;
}

/*
 * Wikipedia spherical coordinate page for cartesian conversion!!!
 */
