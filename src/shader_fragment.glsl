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
uniform int object_id;

// Parâmetros da axis-aligned bounding box (AABB) do modelo
uniform vec4 bbox_min;
uniform vec4 bbox_max;

// Variáveis para acesso das imagens de textura
uniform sampler2D TextureImage0;
uniform sampler2D TextureImage1;
uniform sampler2D TextureImage2;

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
        vec3 Ka = vec3(0.0, 0.0, 0.0); // Refletância ambiente
        float q = 1; // Expoente especular para o modelo de iluminação de Blinn-Phong


        if ( object_id == MAPA )
        {
            // PREENCHA AQUI as coordenadas de textura da esfera, computadas com
            // projeção esférica EM COORDENADAS DO MODELO. Utilize como referência
            // o slides 134-150 do documento Aula_20_Mapeamento_de_Texturas.pdf.
            // A esfera que define a projeção deve estar centrada na posição
            // "bbox_center" definida abaixo.

            // Você deve utilizar:
            //   função 'length( )' : comprimento Euclidiano de um vetor
            //   função 'atan( , )' : arcotangente. Veja https://en.wikipedia.org/wiki/Atan2.
            //   função 'asin( )'   : seno inverso.
            //   constante M_PI
            //   variável position_model

            vec4 bbox_center = (bbox_min + bbox_max) / 2.0;
            vec4 pl = bbox_center + normalize(position_model - bbox_center);
            vec4 vp = pl - bbox_center;
            float theta = atan(vp.x, vp.z);
            float rho = asin(vp.y);

            U = (theta + M_PI) / (2 * M_PI);
            V = (rho + M_PI_2) / M_PI;
            Ks = vec3(0.0, 0.0, 0.0);
            Ka = vec3(0.5, 0.5, 0.5);
            Kd = texture(TextureImage0, vec2(U,V)).rgb;
            q = 1;
        }
        else if ( object_id == HAND )
        {
            // PREENCHA AQUI as coordenadas de textura do coelho, computadas com
            // projeção planar XY em COORDENADAS DO MODELO. Utilize como referência
            // o slides 99-104 do documento Aula_20_Mapeamento_de_Texturas.pdf,
            // e também use as variáveis min*/max* definidas abaixo para normalizar
            // as coordenadas de textura U e V dentro do intervalo [0,1]. Para
            // tanto, veja por exemplo o mapeamento da variável 'p_v' utilizando
            // 'h' no slides 158-160 do documento Aula_20_Mapeamento_de_Texturas.pdf.
            // Veja também a Questão 4 do Questionário 4 no Moodle.

            U = texcoords.x;
            V = texcoords.y;
            Ks = vec3(0.0, 0.0, 0.0);
            Ka = vec3(0.1, 0.1, 0.1);
            Kd = texture(TextureImage2, vec2(U,V)).rgb;
            q = 1;
        }
        else if ( object_id == CUBO )
        {
            // Coordenadas de textura do plano, obtidas do arquivo OBJ.
            U = texcoords.x;
            V = texcoords.y;

            Ks = vec3(0.0, 0.0, 0.0);
            Ka = vec3(0.5, 0.5, 0.5);
            Kd = texture(TextureImage1, vec2(U,V)).rgb;
            q = 1;
        }
        else if ( object_id == TROPHY )
        {

            Ks = vec3(0.8, 0.8, 0.8);
            Ka = vec3(0.1, 0.1, 0.1);
            Kd = vec3(0.54, 0.45, 0.0);
            q = 80.0;
            h = l = v;
        }
        else if ( object_id == SPHERE )
        {
            Ks = vec3(0.8, 0.8, 0.8);
            Ka = vec3(0.1, 0.1, 0.1);
            Kd = vec3(0.54, 0.45, 0.0);
            q = 80.0;
            h = l = v;
        }
        // Espectro da fonte de iluminação
        vec3 I = vec3(1.0,1.0,1.0);

        // Espectro da luz ambiente
        vec3 Ia = vec3(0.01, 0.01, 0.01);

        // Termo difuso utilizando a lei dos cossenos de Lambert
        vec3 lambert_diffuse_term = Kd * I * max(0, dot(n, l));

        // Termo ambiente
        vec3 ambient_term = Ka * Ia;

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

