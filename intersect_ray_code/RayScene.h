#ifndef RAYSCENE_H
#define RAYSCENE_H

#include "Scene.h"
#include "Settings.h"
#include "CS123SceneData.h"
#include "CamtransCamera.h"

#include <iostream>
#include <vector>

#include <qcoreapplication.h>
#include "Canvas2D.h"
#include "BGRA.h"



/**
 * @class RayScene
 *
 *  Students will implement this class as necessary in the Ray project.
 */

struct Ray{
    glm::vec4 p; // point
    glm::vec4 d; // direction
    glm::vec4 tail;
    glm::vec4 head;

    Ray(glm::vec4 point, glm::vec4 direction){
        p = point;
        d = direction;
        tail = p;
        head = evaluate(1);
    }

    glm::vec4 evaluate(float t){
        return p + t*d;
    }
};

struct RenderConstants{
    glm::vec4 worldSpaceEye;

    void setWorldSpaceEye(glm::vec4 worldSpaceEye){
        this->worldSpaceEye = worldSpaceEye;
    }
};

class RayScene : public Scene
{
public:
    RayScene(Scene &scene);
    virtual ~RayScene();

    void render(Canvas2D *canvas, Camera *camera, int width, int height, int tile);

private:

    RenderConstants m_renderConstants;

    void printVec(glm::vec3 &vec);
    void printVec(glm::vec4 &vec);
    void printMat(glm::mat4x4 &mat);

    void debugIntersect();

    Ray generateRay(glm::vec4 point1, glm::vec4 point2);

    glm::vec4 screenSpacePixelToCameraSpacePoint(int x, int y, int xmax, int ymax);
    glm::vec4 cameraSpacePointToWorldSpace(glm::vec4 cameraSpacePoint, glm::mat4x4 cameraViewMatrix, glm::mat4x4 cameraScaleMatrix);
    glm::vec4 worldSpacePointToObjectSpace(glm::vec4 worldSpacePoint, glm::mat4x4 compositeMatrix);
    glm::vec4 objectSpaceNormalToWorldSpaceNormal(glm::vec4 objectSpaceNormal, glm::mat4x4 compositeMatrix);

    void objectIntersectionRayCast(Ray worldSpaceRay, SceneObject &object, Intersection &intersection);
    bool occlusionRayCast(Ray worldSpaceRay);

    float calculateSimpleIllumination(float objectAmbientColor, float objectDiffuseColor, float wavelengthIntensity, glm::vec4 n, glm::vec4 L);
    glm::vec3 calculatePhongIllumination(CS123SceneMaterial material, CS123SceneLightData light, glm::vec3 n, glm::vec3 L, glm::vec3 R, glm::vec3 V);

    glm::vec3 rayTrace(Ray worldSpaceRay, int depth);
};

#endif // RAYSCENE_H
