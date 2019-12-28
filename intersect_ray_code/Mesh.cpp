#include "Mesh.h"
/*
 * Notes:
 * ShaderAttrib just defines the in (layout = x) for each shader
 * VBOAttribMarker constructer takes 3 arguements not 5 bc assumes the last two
 * VAO constructer takes a vbo and the amount of vertices stored in it
 *
 * Triangle Strip:
 * A   C
 *
 * B   D
 */

Mesh::Mesh(unsigned int param1, unsigned int param2, float param3):
    m_param1(param1),
    m_param2(param2),
    m_param3(param3)
{
    zero = {0,0,0};
    m_vertexAttributes.push_back(VBOAttribMarker(ShaderAttrib::POSITION, 3, 0));
    m_vertexAttributes.push_back(VBOAttribMarker(ShaderAttrib::NORMAL, 3, 3*sizeof(float)));
}

Mesh::~Mesh(){

}

void Mesh::draw(){
    m_vao->bind();
    m_vao->draw();
    m_vao->unbind();
}

void Mesh::initializeVAO(){
    VBO vbo(m_vertexData.data(), m_vertexData.size(), m_vertexAttributes, VBO::GEOMETRY_LAYOUT::LAYOUT_TRIANGLE_STRIP);
    m_vao = std::make_unique<VAO>(vbo, m_vertexData.size()/6);
}

void Mesh::appendVertexData(float x, float y, float z){
    m_vertexData.push_back(x);
    m_vertexData.push_back(y);
    m_vertexData.push_back(z);
}


void Mesh::appendVertexData(glm::vec3 vector){
    m_vertexData.push_back(vector.x);
    m_vertexData.push_back(vector.y);
    m_vertexData.push_back(vector.z);
}

/* For raycasts resulting in quadratics or multiple intersections:
 *
 * For quadratics: check descriminant D
 * D < 0 -> no intersection
 * D = 0 -> one intersection (tangent multiplicity)
 * D > 0 -> two intersection (two real roots)
 *
 * Want smallest non-negative real t (of all calculated intersections)
 */

float Mesh::smallestNonNegative(float a, float b)
{
    if(a <= b && a >= 0){
        return a;
    }
    else if(b >= 0){
        return b;
    }

    if(b <= a && b >= 0){
        return b;
    }
    else if(a >= 0){
        return a;
    }

    return -1;
}

float Mesh::quadraticIntersect(float a, float b, float c)
{
    float D = b*b - 4.0*a*c;
    if(D >= 0){
        if(D == 0){
            float root = -b/(2*a);
            if(root > 0){
                return root;
            }
        }
        if(D > 0){
            float root1 = (-b + pow(D, .5))/(2*a);
            float root2 = (-b - pow(D, .5))/(2*a);
            return smallestNonNegative(root1, root2);
        }
    }
    return -1;
}

void Mesh::printVec(glm::vec3 vec)
{
    std::cout << "{" << vec.x << ", " << vec.y << ", " <<  vec.z << "}" << std::endl;
}

void Mesh::debugRay(glm::vec3 P, glm::vec3 d)
{
    std::cout << "----------------" << std::endl;
    Intersection i = intersect(P, d);
    std::cout << "t = " << i.t << std::endl;
    if(i.t != -1){
        //glm::vec3 intersect = P + t*d;
        //printVec(intersect);
        std::cout << "point: ";
        printVec(i.point);
        std::cout << "normal: ";
        printVec(i.normal);
    }
    else{
        std::cout << "Ray Fail" << std::endl;
    }
    std::cout << "----------------" << std::endl;
}

/*
 * Sources:
 * http://www.corehtml5.com/trianglestripfundamentals.php
 */
