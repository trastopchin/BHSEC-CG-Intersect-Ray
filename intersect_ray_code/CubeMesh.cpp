#include "CubeMesh.h"

CubeMesh::CubeMesh(unsigned int param1, unsigned int param2, float param3) : Mesh(param1, param2, param3)
{
    update();
}

CubeMesh::~CubeMesh()
{

}

void CubeMesh::update()
{
    unsigned int s = m_param1;
    unsigned int t = m_param2;

    float ds = 1.0/s;
    float dt = 1.0/t;

    glm::vec3 normal = {0,0,1};
    float z = .5;

    for(unsigned int i = 0; i < t; i++){
        for(unsigned int j = 0; j < s+1; j++){

            float x1 = -.5 + ds*j;
            float y1 = .5 - dt*i;
            float y2 = .5 - dt*(i+1);

            appendVertexData(x1, y1, z);
            appendVertexData(normal);

            appendVertexData(x1, y2, z);
            appendVertexData(normal);

            if(j == s && i != t-1){ // zero triangle
                appendVertexData(x1, y2, z);
                appendVertexData(zero);

                appendVertexData(-.5, y2, z);
                appendVertexData(zero);
            }
            if(j == s && i == t-1){ // duplicate last vertex empty
                appendVertexData(x1, y2, z);
                appendVertexData(zero);
            }
        }
    }

    normal = {0,0,-1};
    z = -.5;

    for(unsigned int i = 0; i < t; i++){
        for(unsigned int j = 0; j < s+1; j++){

            float x1 = .5 - ds*j;
            float y1 = .5 - dt*i;
            float y2 = .5 - dt*(i+1);

            if(i == 0 && j == 0){ // duplicate first vertex empty
                appendVertexData(x1, y1, z);
                appendVertexData(zero);
            }

            appendVertexData(x1, y1, z);
            appendVertexData(normal);

            appendVertexData(x1, y2, z);
            appendVertexData(normal);

            if(j == s && i != t-1){ // empty triangle
                appendVertexData(x1, y2, z);
                appendVertexData(zero);

                appendVertexData(.5, y2, z);
                appendVertexData(zero);
            }
            if(j == s && i == t-1){ // duplicate last vertex empty
                appendVertexData(x1, y2, z);
                appendVertexData(zero);
            }
        }
    }


    normal = {-1,0,0};
    float x = -.5;

    for(unsigned int i = 0; i < t; i++){
        for(unsigned int j = 0; j < s+1; j++){

            float z1 = -.5 + ds*j;
            float y1 = .5 - dt*i;
            float y2 = .5 - dt*(i+1);

            if(i == 0 && j == 0){ // duplicate first vertex empty
                appendVertexData(x, y1, z1);
                appendVertexData(zero);
            }

            appendVertexData(x, y1, z1);
            appendVertexData(normal);

            appendVertexData(x, y2, z1);
            appendVertexData(normal);


            if(j == s && i != t-1){ // empty triangle
                appendVertexData(x, y2, z1);
                appendVertexData(zero);

                appendVertexData(x, y2, -.5);
                appendVertexData(zero);
            }
            if(j == s && i == t-1){ // duplicate last vertex empty
                appendVertexData(x, y2, z1);
                appendVertexData(zero);
            }
        }
    }

    normal = {1,0,0};
    x = .5;

    for(unsigned int i = 0; i < t; i++){
        for(unsigned int j = 0; j < s+1; j++){

            float z1 = .5 - ds*j;
            float y1 = .5 - dt*i;
            float y2 = .5 - dt*(i+1);

            if(i == 0 && j == 0){ // duplicate first vertex empty
                appendVertexData(x, y1, z1);
                appendVertexData(zero);
            }

            appendVertexData(x, y1, z1);
            appendVertexData(normal);

            appendVertexData(x, y2, z1);
            appendVertexData(normal);


            if(j == s && i != t-1){ // empty triangle
                appendVertexData(x, y2, z1);
                appendVertexData(zero);

                appendVertexData(x, y2, .5);
                appendVertexData(zero);
            }
            if(j == s && i == t-1){ // duplicate last vertex empty
                appendVertexData(x, y2, z1);
                appendVertexData(zero);
            }
        }
    }

    normal = {0,1,0};
    float y = .5;

    for(unsigned int i = 0; i < t; i++){
        for(unsigned int j = 0; j < s+1; j++){

            float x1 = .5 - ds*j;
            float z1 = .5 - dt*i;
            float z2 = .5 - dt*(i+1);

            if(i == 0 && j == 0){ // duplicate first vertex empty
                appendVertexData(x1, y, z1);
                appendVertexData(zero);
            }

            appendVertexData(x1, y, z1);
            appendVertexData(normal);

            appendVertexData(x1, y, z2);
            appendVertexData(normal);

            if(j == s && i != t-1){ // zero triangle
                appendVertexData(x1, y, z2);
                appendVertexData(zero);

                appendVertexData(.5, y, z2);
                appendVertexData(zero);
            }
            if(j == s && i == t-1){ // duplicate last vertex empty
                appendVertexData(x1, y, z2);
                appendVertexData(zero);
            }
        }
    }

    normal = {0,-1,0};
    y = -.5;

    for(unsigned int i = 0; i < t; i++){
        for(unsigned int j = 0; j < s+1; j++){

            float x1 = -.5 + ds*j;
            float z1 = .5 - dt*i;
            float z2 = .5 - dt*(i+1);

            if(i == 0 && j == 0){ // duplicate first vertex empty
                appendVertexData(x1, y, z1);
                appendVertexData(zero);
            }

            appendVertexData(x1, y, z1);
            appendVertexData(normal);

            appendVertexData(x1, y, z2);
            appendVertexData(normal);

            if(j == s && i != t-1){ // zero triangle
                appendVertexData(x1, y, z2);
                appendVertexData(zero);

                appendVertexData(-.5, y, z2);
                appendVertexData(zero);
            }
            if(j == s && i == t-1){ // duplicate last vertex empty
                appendVertexData(x1, y, z2);
                appendVertexData(zero);
            }
        }
    }

    initializeVAO();
}

Intersection CubeMesh::intersect(glm::vec3 P, glm::vec3 d)
{
    Intersection i;
    glm::vec3 v;

    // x
    float t1 = (.5 - P.x)/d.x;
    v = P + t1*d;
    if(fabs(v.z) > .5 || fabs(v.y) > .5){
        t1 = -1;
    }

    float t2 = (-.5 - P.x)/d.x;
    v = P + t2*d;
    if(fabs(v.z) > .5 || fabs(v.y) > .5){
        t2 = -1;
    }

    // y
    float t3 = (.5 - P.y)/d.y;
    v = P + t3*d;
    if(fabs(v.z) > .5 || fabs(v.x) > .5){
        t3 = -1;
    }

    float t4 = (-.5 - P.y)/d.y;
    v = P + t4*d;
    if(fabs(v.z) > .5 || fabs(v.x) > .5){
        t4 = -1;
    }

    // z
    float t5 = (.5 - P.z)/d.z;
    v = P + t5*d;
    if(fabs(v.x) > .5 || fabs(v.y) > .5){
        t5 = -1;
    }

    float t6 = (-.5 - P.z)/d.z;
    v = P + t6*d;
    if(fabs(v.x) > .5 || fabs(v.y) > .5){
        t6 = -1;
    }

    i.t = smallestNonNegative( smallestNonNegative( smallestNonNegative(t1,t2), smallestNonNegative(t3,t4)), smallestNonNegative(t5,t6) );

    // return function bc no intersectiondo not need to calculate point and normal
    if(i.t == -1){
        return i;
    }

    // calculate intersection point and normal
    i.point = P + i.t*d;

    if(i.t == t1){
        i.normal = {1,0,0};
    }
    else if(i.t == t2){
        i.normal = {-1,0,0};
    }
    else if(i.t == t3){
        i.normal = {0,1,0};
    }
    else if(i.t == t4){
        i.normal = {0,-1,0};
    }
    else if(i.t == t5){
        i.normal = {0,0,1};
    }
    else if(i.t == t6){
        i.normal = {0,0,-1};
    }

    return i;
}

