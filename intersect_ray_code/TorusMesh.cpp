#include "TorusMesh.h"

TorusMesh::TorusMesh(unsigned int param1, unsigned int param2, float param3) : Mesh(param1, param2, param3)
{
    update();
}

TorusMesh::~TorusMesh()
{

}

void TorusMesh::update()
{
    unsigned int s = m_param1+2;
    unsigned int t = m_param2+2;

    float theta;
    float dtheta = 2*M_PI/t;
    float phi;
    float dphi = 2*M_PI/s;

    float r = m_param3/100/2;
    float R = .5;

    glm::vec3 normal = zero;


    for(unsigned int i = 0; i < s; i++){
        for(unsigned int j = 0; j < t+1; j++){
            phi = i*dphi;
            theta = j*dtheta;

            float x = (R + r*cos(theta))*cos(phi);
            float y = r*sin(theta);
            float z = (R + r*cos(theta))*sin(phi);

            glm::vec3 center = {R*cos(phi), 0 , R*sin(phi)};
            normal = glm::normalize(glm::vec3(x,y,z)-center);

            appendVertexData(x,y,z);
            appendVertexData(normal);

            if(j == 0){ // repeat first vertex
                appendVertexData(x,y,z);
                appendVertexData(normal);
            }

            x = (R + r*cos(theta+dtheta))*cos(phi+dphi);
            y =  r*sin(theta+dtheta);
            z = (R + r*cos(theta+dtheta))*sin(phi+dphi);

            center = {R*cos(phi+dphi), 0 , R*sin(phi+dphi)};
            normal = glm::normalize(glm::vec3(x,y,z)-center);

            appendVertexData(x,y,z);
            appendVertexData(normal);

            if(j == t){ // repeat last vertex
                appendVertexData(x,y,z);
                appendVertexData(normal);
            }
        }
    }
    initializeVAO();
}

Intersection TorusMesh::intersect(glm::vec3 P, glm::vec3 d)
{

}

/*
 * Sources:
 * Torus wiki on parametrically defining a torus equations
 */
