#ifndef CYLINDERMESH_H
#define CYLINDERMESH_H

#include "shapes/Mesh.h"

class CylinderMesh : public Mesh
{
public:
    CylinderMesh(unsigned int param1, unsigned int param2, float param3);
    virtual ~CylinderMesh();

    void update() override;
    Intersection intersect(glm::vec3 P, glm::vec3 d) override;
};

#endif // CYLINDERMESH_H
