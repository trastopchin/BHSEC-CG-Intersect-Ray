#ifndef CONEMESH_H
#define CONEMESH_H

#include "shapes/Mesh.h"

class ConeMesh : public Mesh
{
public:
    ConeMesh(unsigned int param1, unsigned int param2, float param3);
    virtual ~ConeMesh();

    void update() override;
    Intersection intersect(glm::vec3 P, glm::vec3 d) override;
};

#endif // CONEMESH_H
