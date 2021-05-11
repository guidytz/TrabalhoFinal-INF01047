#include <string>
#include <map>
#include <cmath>
#include <iostream>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

// Definição das estruturas para uso nos testes de colisão 
enum ObjectType 
{
    CUBE,
    PLANE,
    SPHERE
};

struct GameObject 
{
    std::string name;
    ObjectType type;
    glm::vec4 position_center;
    glm::vec3 bbox;
    float radius;
};


// Considerando que os planos de colisão só acontecem nos eixos x e z
bool checkCubePlaneCollision(GameObject objA, GameObject plane) {
    if (plane.bbox.z == INFINITY) {
        if (plane.bbox.x > 0 && (objA.position_center.x + objA.bbox.x) > plane.bbox.x) return true;
        if (plane.bbox.x < 0 && (objA.position_center.x - objA.bbox.x) < plane.bbox.x) return true;
    } else if (plane.bbox.x == INFINITY) {
        if (plane.bbox.z > 0 && (objA.position_center.z + objA.bbox.z) > plane.bbox.z) return true;
        if (plane.bbox.z < 0 && (objA.position_center.z - objA.bbox.z) < plane.bbox.z) return true;
    }
    return false;
}

bool checkCubeCubeCollision(GameObject objA, GameObject objB) {
    // Checa se há intersecção no eixo x
    if (abs(objA.position_center.x - objB.position_center.x) < objA.bbox.x + objB.bbox.x) {
        // Checa se há intersecção no eixo z
        if (abs(objA.position_center.z - objB.position_center.z) < objA.bbox.z + objB.bbox.z) {
            // Checa se há intersecção no eixo y
            if (abs(objA.position_center.y - objB.position_center.y) < objA.bbox.y + objB.bbox.y) {
                return true;
            }
        }
    }
    return false;
}

bool checkCubeSphereCollision(GameObject objA, GameObject sphere) {
    float AxMin = objA.position_center.x - objA.bbox.x;
    float AyMin = objA.position_center.y - objA.bbox.y;
    float AzMin = objA.position_center.z - objA.bbox.z;

    float AxMax = objA.position_center.x + objA.bbox.x;
    float AyMax = objA.position_center.y + objA.bbox.y;
    float AzMax = objA.position_center.z + objA.bbox.z;

    float x = fmax(AxMin, fmin(AxMax, sphere.position_center.x));
    float y = fmax(AyMin, fmin(AyMax, sphere.position_center.y));
    float z = fmax(AzMin, fmin(AzMax, sphere.position_center.z));

    glm::vec4 closest_point = glm::vec4(x, y, z, 1.0f);
    float dist_to_center = glm::distance(closest_point, sphere.position_center);
    return dist_to_center < sphere.radius;
}

std::vector<std::string> collided(GameObject objA, std::map<std::string, GameObject> gameObjectCollection) {
    std::vector<std::string> collided_with;
    bool collision = false;
    for (auto& objB : gameObjectCollection) {
        if (objA.name != objB.first) {
            if (objA.type == objB.second.type && objA.type == CUBE) {
                collision = checkCubeCubeCollision(objA, objB.second);
            } else if (objA.type != objB.second.type) {
                if (objA.type == CUBE && objB.second.type == PLANE) {
                    collision = checkCubePlaneCollision(objA, objB.second);
                } else if (objA.type == SPHERE && objB.second.type == CUBE) {
                    // Desconsiderando colisão entre mão e player
                    if (objA.name == "hand" && objB.second.name == "player") { 
                        collision = false;
                    } else {
                        collision = checkCubeSphereCollision(objB.second, objA);
                    }
                } else if (objA.type == CUBE && objB.second.type == SPHERE) {
                    // Desconsiderando colisão entre mão e player
                    if (objA.name == "player" && objB.second.name == "hand") { 
                        collision = false;
                    } else  {
                        collision = checkCubeSphereCollision(objA, objB.second);
                    }
                }
            }
            if (collision) {
                collided_with.push_back(objB.second.name);
            }
        }
    }
    return collided_with;
} 