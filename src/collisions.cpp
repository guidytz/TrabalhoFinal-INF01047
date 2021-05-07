#include <string>
#include <map>

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
    return false;
}

bool checkCubeSphereCollision(GameObject objA, GameObject sphere) {
    return false;
}

bool collided(GameObject objA, std::map<std::string, GameObject> gameObjectCollection) {
    bool collision = false;
    for (auto& objB : gameObjectCollection) {
        if (objA.name != objB.first) {
            if (objA.type == objB.second.type && objA.type == CUBE) {
                collision = checkCubeCubeCollision(objA, objB.second);
            } else if (objA.type != objB.second.type) {
                if (objA.type == CUBE && objB.second.type == PLANE) {
                    collision = checkCubePlaneCollision(objA, objB.second);
                } else if (objA.type == CUBE && objB.second.type == SPHERE) {
                    collision = checkCubeSphereCollision(objA, objB.second);
                }
            } else {
                return false;
            }
            if (collision) return true;
        }
    }
    return collision;
} 