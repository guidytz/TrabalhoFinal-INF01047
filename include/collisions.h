#ifndef __COLLISIONS_H
#define __COLLISIONS_H

// Definição das estruturas para uso nos testes de colisão 
enum ObjectType 
{
    CUBE,
    PLANE,
    SPHERE
};

// Definição das direções a serem usadas para teste de qual a direção principal da colisão
enum XZDirection {
    WEST,
    EAST,
    NORTH,
    SOUTH,
    NONE
};

// Estrutura que armazena informações necessárias para os testes de colisão dos objetos do jogo
struct GameObject 
{
    std::string name;
    ObjectType type;
    glm::vec4 position_center;
    glm::vec3 bbox;
    float radius;
};


// Definição das funções de teste de colisão a serem usadas no código da aplicação
std::vector<std::string> collided(GameObject objA, std::map<std::string, GameObject> gameObjectCollection);
XZDirection closest_direction(glm::vec4 dir_vec);
glm::vec4 getClosestPointToCenter(GameObject objA, GameObject objB);
glm::vec4 updateMovementDirection(GameObject movingObj, 
                                  std::string collidedName,
                                  glm::vec4 move_direction,
                                  std::map<std::string, GameObject> gameObjectCollection);

#endif // __COLLISIONS_H