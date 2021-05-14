#version 330 core

// Atributos de fragmentos recebidos como entrada ("in") pelo Fragment Shader.
// Neste exemplo, este atributo foi gerado pelo rasterizador como a
// interpolação da posição global e a normal de cada vértice, definidas em
// "shader_vertex.glsl" e "main.cpp".
in vec4 position_world;
in vec4 normal;

// Posição do vértice atual no sistema de coordenadas local do modelo.
in vec4 position_model;

// Coordenadas de textura obtidas do arquivo OBJ (se existirem!)
in vec2 texcoords;

// Matrizes computadas no código C++ e enviadas para a GPU
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// Variaveis associadas ao modelo de interpolacao de Gouraud feita no vertex shader
// caso tenha sido determinado na main
uniform bool use_gouraud_shading;
in vec3 gouraud_color;

// Identificador que define qual objeto está sendo desenhado no momento
#define MAPA    0
#define HAND    1
#define CUBO    2
#define TROPHY  3
#define OBJETIVO 4
uniform int object_id;

// Parâmetros da axis-aligned bounding box (AABB) do modelo
uniform vec4 bbox_min;
uniform vec4 bbox_max;

// Variáveis para acesso das imagens de textura
uniform sampler2D TextureImage0;
uniform sampler2D TextureImage1;
uniform sampler2D TextureImage2;
uniform sampler2D TextureImage3;

// O valor de saída ("out") de um Fragment Shader é a cor final do fragmento.
out vec3 color;

// Constantes
#define M_PI   3.14159265358979323846
#define M_PI_2 1.57079632679489661923

void main()
{
    if (use_gouraud_shading){
        // Caso use a interpolação de Gouraud, considerar a cor calculada no vertex shader
        color = gouraud_color;
    } else {
        // Caso não use interpolação de Gouraud, significa que é preciso fazer as operações necessárias para a
        // interpolação de Phong

        // Obtemos a posição da câmera utilizando a inversa da matriz que define o
        // sistema de coordenadas da câmera.
        vec4 origin = vec4(0.0, 0.0, 0.0, 1.0);
        vec4 camera_position = inverse(view) * origin;

        // O fragmento atual é coberto por um ponto que percente à superfície de um
        // dos objetos virtuais da cena. Este ponto, p, possui uma posição no
        // sistema de coordenadas global (World coordinates). Esta posição é obtida
        // através da interpolação, feita pelo rasterizador, da posição de cada
        // vértice.
        vec4 p = position_world;

        // Normal do fragmento atual, interpolada pelo rasterizador a partir das
        // normais de cada vértice.
        vec4 n = normalize(normal);

        // Vetor que define o sentido da fonte de luz em relação ao ponto atual.
        vec4 l = normalize(vec4(0.0,10.0,0.0,0.0) - p);

        // Vetor que define o sentido da câmera em relação ao ponto atual.
        vec4 v = normalize(camera_position - p);

        // half-vector para o modelo de iluminação de Blinn-Phong
        vec4 h = normalize(l + v);

        // Vetor que define o sentido da reflexão especular ideal.
        // vec4 r = normalize(-l + 2 * n * dot(n, l));

        // Coordenadas de textura U e V
        float U = 0.0;
        float V = 0.0;

        vec3 Kd = vec3(0.0, 0.0, 0.0); // Refletância difusa
        vec3 Ks = vec3(0.0, 0.0, 0.0); // Refletância especular
        float q = 1; // Expoente especular para o modelo de iluminação de Blinn-Phong


        if ( object_id == MAPA )
        {

            U = texcoords.x;
            V = texcoords.y;
            Ks = vec3(0.0, 0.0, 0.0);
            Kd = texture(TextureImage0, vec2(U,V)).rgb;
            q = 1;
        }
        else if ( object_id == HAND )
        {

            U = texcoords.x;
            V = texcoords.y;
            Ks = vec3(0.0, 0.0, 0.0);
            Kd = texture(TextureImage2, vec2(U,V)).rgb;
            q = 1;
        }
        else if ( object_id == CUBO )
        {
            U = texcoords.x;
            V = texcoords.y;

            Ks = vec3(0.0, 0.0, 0.0);
            Kd = texture(TextureImage1, vec2(U,V)).rgb;
            q = 1;
        }
        else if ( object_id == OBJETIVO )
        {
            U = texcoords.x;
            V = texcoords.y;

            Ks = vec3(0.0, 0.0, 0.0);
            Kd = vec3(0.0, 1.0, 0.3);
            q = 1;
        }
        else if ( object_id == TROPHY )
        {

            Ks = vec3(0.8, 0.8, 0.8);
            Kd = vec3(0.54, 0.45, 0.0);
            q = 80.0;
            h = l = v;
        }
        // Espectro da fonte de iluminação
        vec3 I = vec3(1.0,1.0,1.0);

        // Espectro da luz ambiente
        vec3 Ia = vec3(0.05, 0.05, 0.05);

        // Termo difuso utilizando a lei dos cossenos de Lambert
        vec3 lambert_diffuse_term = Kd * I * max(0, dot(n, l));

        // Termo ambiente usando o termo difuso com uma luz fraca ambiente
        vec3 ambient_term = Kd * Ia;

        // Termo especular utilizando o modelo de iluminação de Phong
        vec3 blinn_phong_specular_term  = Ks * I * pow(max(0, dot(n, h)), q);

        // // Equação de Iluminação
        // float lambert = max(0, dot(n,l));

        color = lambert_diffuse_term + ambient_term + blinn_phong_specular_term;

        // Cor final com correção gamma, considerando monitor sRGB.
        // Veja https://en.wikipedia.org/w/index.php?title=Gamma_correction&oldid=751281772#Windows.2C_Mac.2C_sRGB_and_TV.2Fvideo_standard_gammas
    }
    color = pow(color, vec3(1.0,1.0,1.0)/2.2);
}

