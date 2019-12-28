#include "RayScene.h"

/* Raytracer Notes:
 * For the primary ray loop, to determine the closest object intersection in terms of t, do not normalize the direction vector of the ray
 */

RayScene::RayScene(Scene &scene) :
    Scene(scene)
{
    // TODO [INTERSECT]
    // Remember that any pointers or OpenGL objects (e.g. texture IDs) will
    // be deleted when the old scene is deleted (assuming you are managing
    // all your memory properly to prevent memory leaks).  As a result, you
    // may need to re-allocate some things here.

    //debugIntersect();

    std::cout << "Rayscene: " << std::endl;
    std::cout << "numbers of objects: " << m_sceneObjects.size() << std::endl;
    std::cout << "number of lights: " << m_sceneLights.size() << std::endl;
    std::cout << "number of primitives: " << m_primitiveMap.size() << std::endl;
    std::cout << std::endl;
}

RayScene::~RayScene()
{
}

void RayScene::render(Canvas2D *canvas, Camera *camera, int width, int height, int tile)
{  
    std::cout << "starting rendering" << std::endl;

    glm::vec4 cameraSpaceEye = {0,0,0,1};
    glm::mat4x4 cameraViewMatrix = camera->getViewMatrix();
    glm::mat4x4 cameraScaleMatrix = camera->getScaleMatrix();
    glm::vec4 worldSpaceEye = cameraSpacePointToWorldSpace(cameraSpaceEye, cameraViewMatrix, cameraScaleMatrix);

    for(int x = 0; x < width; x++){
        for(int y = 0; y < height; y++){ // primary camera ray loop; for each point on the film plane generate a ray

            glm::vec4 cameraSpacePoint = screenSpacePixelToCameraSpacePoint(x, y, width, height); // camera space point on film plane; camera space ray head
            glm::vec4 worldSpacePoint = cameraSpacePointToWorldSpace(cameraSpacePoint, cameraViewMatrix, cameraScaleMatrix); // world space ray head
            Ray worldSpaceRay = generateRay(worldSpaceEye, worldSpacePoint); // world space ray

            int depth = 1;
            if(settings.useReflection == true){
                depth = 4;
            }

            glm::vec3 illumination = rayTrace(worldSpaceRay, depth);

            float r = 255*glm::clamp(illumination.r, 0.f, 1.f);
            float g = 255*glm::clamp(illumination.g, 0.f, 1.f);
            float b = 255*glm::clamp(illumination.b, 0.f, 1.f);

            canvas->data()[x+y*width] = BGRA(r,g,b);
        }
        if(x%tile == 0 && x != 0){
            canvas->update();
            QCoreApplication::processEvents();
        }
    }
    canvas->update();
    QCoreApplication::processEvents();
    std::cout << "finished rendering" << std::endl;
}

glm::vec3 RayScene::rayTrace(Ray worldSpaceRay, int depth)
{
    glm::vec3 illumination = glm::vec3(0,0,0); // create illumination placeholder

    if(depth != 0){

        SceneObject object; // SceneObject to store intersected SceneObject
        Intersection intersection; // Intersection to store intersected Intersection

        objectIntersectionRayCast(worldSpaceRay, object, intersection); // gets the SceneObject and Intersection for the worldSpaceRay

        if(intersection.t != -1){

            glm::vec4 worldSpaceNormal = objectSpaceNormalToWorldSpaceNormal(intersection.getNormal(), object.matrix); // convert the object space intersection normal to world space
            glm::vec4 worldSpaceIntersection = worldSpaceRay.evaluate(intersection.t); // find world space intersection by evaluating worldSpaceRay at intersection.t

            glm::vec4 normalOffsetIntersection = worldSpaceIntersection + (float).0001 * worldSpaceNormal; // use normal offset in case of tangent rays

            glm::vec3 n = worldSpaceNormal.xyz();  // n is the world space normal at the intersection (normal)
            n = glm::normalize(n);

            glm::vec3 V = worldSpaceRay.evaluate(0).xyz() - worldSpaceIntersection.xyz();  // V is the vector connecting the intersection point to the camera or recursive eye (incoming ray)
            V = glm::normalize(V);

            CS123SceneMaterial material = object.primitive.material;

            for(CS123SceneLightData light : m_sceneLights){ // for each light, calculate the illumination contribution
                // do lighting computations in vec3, because otherwise glm::reflect functions weirdly and incorrect R -> incorrect specular highlights -> incorrect specular reflection

                bool occlusion = false; // assume no occlusion at first

                glm::vec3 L; // L is the vector connecting intersection point to the light (shadow ray)
                switch(light.type){

                case(LightType::LIGHT_POINT):
                    if(settings.usePointLights){
                        L  = light.pos.xyz() - worldSpaceIntersection.xyz();

                        if(settings.useShadows){ // if rendering shadows, create a shadow ray and check that there is no occlusion

                            Ray worldSpaceShadowRay = generateRay(normalOffsetIntersection, light.pos);

                            if(occlusionRayCast(worldSpaceShadowRay)){ // if there is an occlusion, set occlusion = true
                                occlusion = true;
                            }
                        }
                    }
                    break;

                case(LightType::LIGHT_DIRECTIONAL): // L is the literal point light from a direction
                    if(settings.useDirectionalLights){
                        L = -light.dir.xyz(); // NEGATE L : because L is in direction of light

                        if(settings.useShadows){
                            // the head of the ray is the tail + 1000 * -light.dir because I want a line segment to check within it an occluder
                            Ray worldSpaceShadowRay = generateRay(normalOffsetIntersection, normalOffsetIntersection + 1000.f * -light.dir);

                            if(occlusionRayCast(worldSpaceShadowRay)){
                                occlusion = true;
                            }
                        }
                    }
                    break;
                }
                L = glm::normalize(L);

                glm::vec3 R = glm::reflect(-L,n); // R is the reflection of -L about n (specular reflection ray)
                R = glm::normalize(R);

                if(!occlusion){ // if no occlusion, current light contributes to direct phong illumination
                    illumination += calculatePhongIllumination(material, light, n, L, R, V);
                }
            }

            if(glm::length(material.cReflective) == 0){
                return illumination;
            }

            glm::vec4 reflectionVector = glm::vec4(glm::reflect(-V, n),0);  // reflection vector is reflection of V about the normal
            Ray worldSpaceSpecularReflectionRay = Ray(normalOffsetIntersection, reflectionVector);

            illumination += m_global.ks * material.cReflective.xyz() * rayTrace(worldSpaceSpecularReflectionRay, depth-1);
        }
    }
    return illumination;
}

void RayScene::objectIntersectionRayCast(Ray worldSpaceRay, SceneObject &object, Intersection &intersection)
{
    intersection.t = -1;

    for(SceneObject &currentObject : m_sceneObjects){

        glm::vec4 objectSpaceTail = worldSpacePointToObjectSpace(worldSpaceRay.tail, currentObject.matrix);
        glm::vec4 objectSpaceHead = worldSpacePointToObjectSpace(worldSpaceRay.head, currentObject.matrix);

        Ray objectSpaceRay = generateRay(objectSpaceTail, objectSpaceHead);

        Intersection currentIntersection = m_primitiveMap[currentObject.primitive.type]->intersect(objectSpaceRay.p.xyz(), objectSpaceRay.d.xyz());

        if(Mesh::smallestNonNegative(currentIntersection.t, intersection.t) == currentIntersection.t){
            object = currentObject;
            intersection = currentIntersection;
        }
    }
}

bool RayScene::occlusionRayCast(Ray worldSpaceRay)
{
    for(SceneObject &currentObject: m_sceneObjects){

        glm::vec4 objectSpaceTail = worldSpacePointToObjectSpace(worldSpaceRay.tail, currentObject.matrix);
        glm::vec4 objectSpaceHead = worldSpacePointToObjectSpace(worldSpaceRay.head, currentObject.matrix);

        Ray objectSpaceRay = generateRay(objectSpaceTail, objectSpaceHead);

        Intersection currentIntersection = m_primitiveMap[currentObject.primitive.type]->intersect(objectSpaceRay.p.xyz(), objectSpaceRay.d.xyz());

        if(currentIntersection.t < 1 && currentIntersection.t > 0){
            return true;
        }
    }
    return false;
}

float RayScene::calculateSimpleIllumination(float objectAmbientColor, float objectDiffuseColor, float wavelengthIntensity, glm::vec4 n, glm::vec4 L)
{
    return  m_global.ka*objectAmbientColor + glm::dot(wavelengthIntensity,(m_global.kd * objectDiffuseColor * glm::clamp( glm::dot(n, L), 0.f, 1.f)) );
}

glm::vec3 RayScene::calculatePhongIllumination(CS123SceneMaterial material, CS123SceneLightData light, glm::vec3 n, glm::vec3 L, glm::vec3 R, glm::vec3 V)
{
    glm::vec3 objectAmbientColor = material.cAmbient.xyz();
    glm::vec3 objectDiffuseIntensity = material.cDiffuse.xyz();
    glm::vec3 objectSpecularIntensity = material.cSpecular.xyz();
    float objectShininess = material.shininess;
    float lightAttenuation = 1/powf(glm::length(L), 2);
    lightAttenuation = 1;
    glm::vec3 lightIntensity = light.color.xyz();

    glm::vec3 ambient = m_global.ka * objectAmbientColor;
    glm::vec3 diffuse = m_global.kd * objectDiffuseIntensity * glm::clamp( glm::dot(n, L), 0.f, 1.f);
    glm::vec3 specular = m_global.ks * objectSpecularIntensity * glm::pow( glm::clamp( glm::dot(R, V), 0.f, 1.f), objectShininess);

    return ambient + lightAttenuation * lightIntensity * (diffuse + specular);
}

glm::vec4 RayScene::screenSpacePixelToCameraSpacePoint(int x, int y, int xmax, int ymax)
{
    glm::vec4 cameraSpacePoint;
    cameraSpacePoint.x = 2.0*x/xmax - 1;
    cameraSpacePoint.y = 1 - 2.0*y/ymax;
    cameraSpacePoint.z = -1;
    cameraSpacePoint.w = 1;

    return cameraSpacePoint;
}

Ray RayScene::generateRay(glm::vec4 point1, glm::vec4 point2)
{
    glm::vec4 direction =  point2 - point1;

    return Ray(point1, direction);
}

glm::vec4 RayScene::cameraSpacePointToWorldSpace(glm::vec4 cameraSpacePoint, glm::mat4x4 cameraViewMatrix, glm::mat4x4 cameraScaleMatrix)
{
    glm::mat4x4 inverseViewMatrix = glm::inverse(cameraViewMatrix);
    glm::mat4x4 inverseScaleMatrix = glm::inverse(cameraScaleMatrix);

    return  inverseViewMatrix * inverseScaleMatrix * cameraSpacePoint;
}

glm::vec4 RayScene::worldSpacePointToObjectSpace(glm::vec4 worldSpacePoint, glm::mat4x4 compositeMatrix)
{
    return glm::inverse(compositeMatrix) * worldSpacePoint;
}


glm::vec4 RayScene::objectSpaceNormalToWorldSpaceNormal(glm::vec4 objectSpaceNormal, glm::mat4x4 compositeMatrix)
{
    return glm::transpose( glm::inverse(compositeMatrix) ) * objectSpaceNormal;
}

void RayScene::debugIntersect()
{
    Mesh *mesh;
    glm::vec3 P;
    glm::vec3 d;
    glm::vec3 xd = {-1,0,0};
    glm::vec3 yd = {0,-1,0};
    glm::vec3 zd = {0,0,-1};
    glm::vec3 xp = {5,0,0};
    glm::vec3 yp = {0,5,0};
    glm::vec3 zp = {0,0,5};


    // cube testing
    std::cout << "<<<< Cube Testing >>>>" << std::endl;
    mesh = new CubeMesh(0,0,0);
    mesh->debugRay(zp, zd); // {0, 0, 0.5}, {0, 0, 1}
    mesh->debugRay(xp, xd); // {0.5, 0, 0}, {1, 0, 0}
    mesh->debugRay(yp, yd); // {0, 0.5, 0}, {0, 1, 0}

    // top y offset intersection
    std::cout << "top y offset intersection" << std::endl;
    P = {.25,5,0};
    mesh->debugRay(P,yd); // {0.25, 0.5, 0}, {0, 1, 0}

    // back z offset intersection
    std::cout << "back z offset intersection" << std::endl;
    P = {-.25,0,-5};
    d = -zd;
    mesh->debugRay(P,d); // {-0.25, 0, -0.5}, {0, 0, -1}

    // special angle pi/4 in xy plane (body intersection)
    std::cout << "special angle pi/4 in xy plane (body intersection)" << std::endl;
    P = {1,.75,0};
    d = {-1, -1, 0};
    mesh->debugRay(P, d); // {0.5, 0.25, 0}, {1, 0, 0}
    std::cout << std::endl;

    // sphere testing
    std::cout << "<<<< Sphere Testing >>>>" << std::endl;
    mesh = new SphereMesh(0,0,0);

    mesh->debugRay(zp, zd); // {0, 0, 0.5}, {0, 0, 1}
    mesh->debugRay(xp, xd); // {0.5, 0, 0}, {1, 0, 0}
    mesh->debugRay(yp, yd); // {0, 0.5, 0}, {0, 1, 0}

    // special angle pi/4 in xy plane (body intersection)
    std::cout << "special angle pi/4 in xy plane (body intersection)" << std::endl;
    P = {1,1,0};
    d = {-1, -1, 0};
    std::cout << "(root2)/4 = " << pow(2, .5)/4.0 << std::endl;
    std::cout << "1/(root2) = " << 1.0/pow(2, .5) << std::endl;
    mesh->debugRay(P, d); // {0.353553, 0.353553, 0}, {0.707107, 0.707107, 0}
    std::cout << std::endl;


    // cylinder testing ----------------
    std::cout << "<<<< Cylinder Testing >>>>" << std::endl;
    mesh = new CylinderMesh(0,0,0);
    mesh->debugRay(zp, zd); // {0, 0, 0.5}, {0, 0, 1}
    mesh->debugRay(xp, xd); // {0.5, 0, 0}, {1, 0, 0}
    mesh->debugRay(yp, yd); // {0, 0.5, 0}, {0, 1, 0}

    // special angle pi/4 in xy plane (body intersection)
    std::cout << "special angle pi/4 in xy plane (body intersection)" << std::endl;
    P = {1,.75,0};
    d = {-1, -1, 0};
    mesh->debugRay(P, d); // {0.5, 0.25, 0}, {1, 0, 0}

    // top cap offset intersection
    std::cout << "top cap offset intersection" << std::endl;
    P = {.25,5,0};
    mesh->debugRay(P,yd); // {0.25, 0.5, 0}, {0, 1, 0}

    // bottom cap offset intersection
    std::cout << "bottom cap offset intersection" << std::endl;
    P = {0,-5,.25};
    d = -yd;
    mesh->debugRay(P, d); // {0, -0.5, .25}, {0, -1, 0}
    std::cout << std::endl;

    // cone testing ----------------
    std::cout << "<<<< Cone Testing >>>>" << std::endl;
    mesh = new ConeMesh(0,0,0);

    glm::vec3 nvec3 = {2,1,0};
    nvec3 = glm::normalize(nvec3);
    std::cout << "Normalized vec3 with x:y = 2:1; ";
    printVec(nvec3);

    mesh->debugRay(zp, zd); // {0, 0, 0.25}, {0, 0.447214, 0.894427}
    mesh->debugRay(xp, xd); // {0.25, 0, 0}, {0.894427, 0.447214, 0}
    mesh->debugRay(yp, yd); // {0, 0.5, 0}, {0, 1, 0}

    // special angle pi/4 in xy plane (body intersection)
    std::cout << "special angle pi/4 in xy plane (body intersection)" << std::endl;
    P = {1,1,0};
    d = {-1, -1, 0};
    std::cout << "1/6 = " << 1.0/6.0 << std::endl;
    mesh->debugRay(P, d); // {0.166667, 0.166667, 0}, {0.894427, 0.447214, 0}

    // cap intersection
    std::cout << "cap intersection" << std::endl;
    P = {0,-1,.25};
    d = -yd;
    mesh->debugRay(P, d); // {0, -0.5, 0.25}, {0, -1, 0}

    // Arnav check
    std::cout << "Arnav check" << std::endl;
    P = {5,5,5};
    d = {-1,-1,-1};
    mesh->debugRay(P, d); // {0.130602, 0.130602, 0.130602}, {0.632455, 0.447214, 0.632455}
    std::cout << std::endl;

    delete mesh;
}

void RayScene::printVec(glm::vec3 &vec)
{
    std::cout << "{" << vec.x << ", " << vec.y << ", " <<  vec.z << "}" << std::endl;
}

void RayScene::printVec(glm::vec4 &vec)
{
    std::cout << "{" << vec.x << ", " << vec.y << ", " <<  vec.z << ", " << vec.w << "}" << std::endl;
}

void RayScene::printMat(glm::mat4x4 &mat)
{
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            std::cout << mat[i][j] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}
