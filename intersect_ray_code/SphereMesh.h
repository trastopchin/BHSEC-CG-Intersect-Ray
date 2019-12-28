#ifndef SPHEREMESH_H
#define SPHEREMESH_H

#include "shapes/Mesh.h"

class SphereMesh :  public Mesh
{
public:
    SphereMesh(unsigned int param1, unsigned int param2, float param3);
    virtual ~SphereMesh();

    void update() override;
    Intersection intersect(glm::vec3 P, glm::vec3 d) override;
};

#endif // SPHEREMESH_H
