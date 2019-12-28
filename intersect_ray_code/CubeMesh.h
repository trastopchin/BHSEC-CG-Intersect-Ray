#ifndef CUBEMESH_H
#define CUBEMESH_H

#include "shapes/Mesh.h"

class CubeMesh : public Mesh
{
public:
    CubeMesh(unsigned int param1, unsigned int param2, float param3);
    virtual ~CubeMesh();

    void update() override;
    Intersection intersect(glm::vec3 P, glm::vec3 d) override;
};

#endif // CUBEMESH_H
