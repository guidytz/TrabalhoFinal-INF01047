#include <string>
#include <map>
#include <cmath>
#include <iostream>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

#include "utils.h"
#include "matrices.h"
#include "collisions.h"

// Considerando que os planos de colisão só acontecem nos eixos x e z
bool checkCubePlaneCollision(GameObject objA, GameObject plane) {
    if (plane.bbox.z == INFINITY) { // testa colisão com os planos perpendiculares ao eixo x
        if (plane.bbox.x > 0 && (objA.position_center.x + objA.bbox.x) > plane.bbox.x) return true;
        if (plane.bbox.x < 0 && (objA.position_center.x - objA.bbox.x) < plane.bbox.x) return true;
    } else if (plane.bbox.x == INFINITY) { // testa colisão com os planos perpendiculares ao eixo z
        if (plane.bbox.z > 0 && (objA.position_center.z + objA.bbox.z) > plane.bbox.z) return true;
        if (plane.bbox.z < 0 && (objA.position_center.z - objA.bbox.z) < plane.bbox.z) return true;
    }
    return false;
}

bool checkCubeCubeCollision(GameObject objA, GameObject objB) {
    // Checa se há intersecção no eixo x
    if (std::abs(objA.position_center.x - objB.position_center.x) < objA.bbox.x + objB.bbox.x) {
        // Checa se há intersecção no eixo z
        if (std::abs(objA.position_center.z - objB.position_center.z) < objA.bbox.z + objB.bbox.z) {
            // Checa se há intersecção no eixo y
            if (std::abs(objA.position_center.y - objB.position_center.y) < objA.bbox.y + objB.bbox.y) {
                return true;
            }
        }
    }
    return false;
}

glm::vec4 getClosestPointToCenter(GameObject objA, GameObject objB) {
    float AxMin = objA.position_center.x - objA.bbox.x;
    float AyMin = objA.position_center.y - objA.bbox.y;
    float AzMin = objA.position_center.z - objA.bbox.z;

    float AxMax = objA.position_center.x + objA.bbox.x;
    float AyMax = objA.position_center.y + objA.bbox.y;
    float AzMax = objA.position_center.z + objA.bbox.z;

    float x = std::fmax(AxMin, std::fmin(AxMax, objB.position_center.x));
    float y = std::fmax(AyMin, std::fmin(AyMax, objB.position_center.y));
    float z = std::fmax(AzMin, std::fmin(AzMax, objB.position_center.z));

    return glm::vec4(x, y, z, 1.0f);
}

bool checkCubeSphereCollision(GameObject objA, GameObject sphere) {
    glm::vec4 closest_point = getClosestPointToCenter(objA, sphere);
    float dist_to_center = glm::distance(closest_point, sphere.position_center);
    return dist_to_center < sphere.radius;
}

std::vector<std::string> collided(GameObject objA, std::map<std::string, GameObject> gameObjectCollection) {
    std::vector<std::string> collided_with;
    for (auto& objB : gameObjectCollection) {
        bool collision = false;
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
        }
        if (collision) {
            collided_with.push_back(objB.second.name);
        }
    }
    return collided_with;
} 

XZDirection closest_direction(glm::vec4 dir_vec) {
    // Caso o vetor seja nulo, não é possível calcular a direção principal
    // Além disso, não faz sentido calculcular direção de um ponto
    if (norm(dir_vec) == 0.0f || dir_vec.w != 0.0f) return NONE;

    dir_vec = dir_vec / norm(dir_vec);
    glm::vec4 compass[] = {
        glm::vec4(1.0f, 0.0f, 0.0f, 0.0f),
        glm::vec4(-1.0f, 0.0f, 0.0f, 0.0f),
        glm::vec4(0.0f, 0.0f, 1.0f, 0.0f),
        glm::vec4(0.0f, 0.0f, -1.0f, 0.0f)
    };
    XZDirection best_direction = NONE;
    float maximum = 0.0f;
    for (int i = 0; i < 4; i++) {
        float dot_prod = dotproduct(dir_vec, compass[i]);
        if (dot_prod > maximum) {
            maximum = dot_prod;
            if (i == 0) {
                best_direction = EAST;
            } else if (i == 1) {
                best_direction = WEST;
            } else if (i == 2) {
                best_direction = SOUTH;
            } else {
                best_direction = NORTH;
            }
        }
    }
    return best_direction;
}

glm::vec4 updateMovementDirection(GameObject movingObj, 
                                  std::string collidedName,
                                  glm::vec4 move_direction,
                                  std::map<std::string, GameObject> gameObjectCollection) 
{
    // Caso o objeto colidido seja ele mesmo, não continuar computação
    if (movingObj.name.compare(collidedName) == 0) return move_direction;

    if (collidedName.compare("planeEast") == 0 || collidedName.compare("planeWest") == 0){
        // caso tenha colidido com os planos leste ou oeste, desconsiderar movimento no eixo x
        move_direction.x = 0.0f;
    } else if (collidedName.compare("planeNorth") == 0 || collidedName.compare("planeSouth") == 0){
        // caso tenha colidido com os planos norte ou sul, desconsiderar movimento no eixo z
        move_direction.z = 0.0f;
    } else if (collidedName.find("cube") != std::string::npos ||
                collidedName.find("wall") != std::string::npos) {
        glm::vec4 closest_cube_point = getClosestPointToCenter(gameObjectCollection[collidedName], movingObj);
        glm::vec4 cube_dir = closest_cube_point - movingObj.position_center;
        XZDirection direction = closest_direction(cube_dir);
        if (direction == WEST || direction == EAST) { // Caso a colisão tenha acontecido na direção do eixo x
            glm::vec4 x_dir(move_direction.x, 0.0f, 0.0f, 0.0f);
            x_dir *= 100; // necessário por conta do valor pequeno no vetor de direção
            if (norm(x_dir) == 0.0f) return move_direction;// movimento nessa direção já é zero!

            XZDirection movement_dir = closest_direction(x_dir);
            // Testa se a direção do deslocamento no eixo x é a mesma do objeto colidido
            if (direction == movement_dir) {
                move_direction.x = 0;
            }
        } else if (direction == NORTH || direction == SOUTH) { // Caso a colisão tenha acontecido na direção do eixo z
            glm::vec4 z_dir(0.0f, 0.0f, move_direction.z, 0.0f);
            z_dir *= 100; // necessário por conta do valor pequeno no vetor de direção
            if (norm(z_dir) == 0.0f) return move_direction; // movimento nessa direção já é zero!

            XZDirection movement_dir = closest_direction(z_dir);
            // Testa se a direção do deslocamento no eixo z é a mesma do objeto colidido
            if (movement_dir == direction) {
                move_direction.z = 0.0f;
            }
        } 
    }
    return move_direction;
} 