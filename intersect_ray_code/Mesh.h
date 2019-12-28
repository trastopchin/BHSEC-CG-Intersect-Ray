#ifndef MESH_H
#define MESH_H

#include <iostream>

#include <memory>
#include <vector>

#include "math.h"
#include "glm.hpp"

#include "gl/datatype/VAO.h"
#include "gl/datatype/VBO.h"
#include "gl/datatype/VBOAttribMarker.h"
#include "gl/shaders/ShaderAttribLocations.h"

using namespace CS123::GL;

struct Intersection{
    float t;
    glm::vec3 point;
    glm::vec3 normal;

    glm::vec4 getPoint(){
        return glm::vec4(point, 1);
    }
    glm::vec4 getNormal(){
        return glm::vec4(normal, 0);
    }
};

class Mesh
{
public:
    Mesh(unsigned int param1, unsigned int param2, float param3);
    virtual ~Mesh();

    void draw();
    virtual void update() = 0;

    virtual Intersection intersect(glm::vec3 P, glm::vec3 d) = 0;
    void debugRay(glm::vec3 P, glm::vec3 d);

    static float smallestNonNegative(float a, float b);

protected:
    std::vector<float> m_vertexData;
    std::vector<VBOAttribMarker> m_vertexAttributes;
    std::unique_ptr<VAO> m_vao;
    int m_param1;
    int m_param2;
    float m_param3;

    glm::vec3 zero;

    void initializeVAO();

    void appendVertexData(float x, float y, float z);
    void appendVertexData(glm::vec3 vector);

    void printVec(glm::vec3 vec);
    float quadraticIntersect(float a, float b, float c);
};

#endif // MESH_H
