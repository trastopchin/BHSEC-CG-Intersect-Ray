#ifndef TORUSMESH_H
#define TORUSMESH_H

#include "shapes/Mesh.h"

class TorusMesh : public Mesh
{
public:
    TorusMesh(unsigned int param1, unsigned int param2, float param3);
    virtual ~TorusMesh();

    void update() override;
    Intersection intersect(glm::vec3 P, glm::vec3 d) override;
};

#endif // TORUSMESH_H
