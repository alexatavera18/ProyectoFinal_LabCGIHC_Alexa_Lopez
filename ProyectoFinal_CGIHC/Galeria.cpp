//Nombre: Alexa Fernanda López Tavera
//Proyecto Final CGIHC "Galería de arte interactiva con OpenGL"
//Fecha de Entrega: 10 de Noviembre, 2025
//No. de cuenta: 319023024

#include <iostream>
#include <cmath>
#include <algorithm>
#include <vector>
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "SOIL2/SOIL2.h"
#include "Shader.h"
#include "Camera.h"
#include "Model.h"





// Prototipos
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow* window, double xPos, double yPos);
void DoMovement();
void Animation() {} // <-- sin animaciones por ahora
void animacion(); //Funci�n para los frames 
void updateAnim(float deltaTime);

// Window
const GLuint WIDTH = 800, HEIGHT = 600;
int SCREEN_WIDTH, SCREEN_HEIGHT;

// Cámara
Camera  camera(glm::vec3(0.0f, 1.2f, 4.5f));
GLfloat lastX = WIDTH / 2.0f;
GLfloat lastY = HEIGHT / 2.0f;
bool keys[1024];
bool firstMouse = true;
bool pajarosanim = false;
float tPajaro = 0.0f;
// Factores pre-calculados
const float startFactor = 1.0f;   // -45°
const float factor3Turns = 25.0f;  // 3 vueltas y vuelve a -45°
const float factor3TurnsPlus180 = 29.0f;  // 3 vueltas + 180°

enum AnimState {
    PHASE1_SUBIR_ESCALAR_GIRAR_RAPIDO,
    PHASE2_GIRO_90_LENTO,
    PHASE3_GIRO_90_REGRESO_MAS_RAPIDO,
    PHASE4_BAJAR_Y_ENCOGER_RAPIDO,
    ANIM_FINISHED
};

AnimState animState = PHASE1_SUBIR_ESCALAR_GIRAR_RAPIDO;
float animTime = 0.0f;

// Ajusta estos valores a ojo hasta que te guste
const float baseAlturaTarget = 4.0f;   // cuánto sube la base
const float dur1 = 0.5f;               // fase 1 rápida
const float dur2 = 3.0f;               // fase 2 lenta
const float dur3 = dur2 / 3.0f;        // fase 3 = el doble de velocidad que fase 2
const float dur4 = dur1 / 1.5f;        // fase 4 = 1.5 veces más rápida que fase 1

// Luces (se quedan como en tu código)
glm::vec3 lightPos(0.0f, 0.0f, 0.0f);
bool active;
glm::vec3 pointLightPositions[] = {
    glm::vec3(0.0f,2.0f, 0.0f),
    glm::vec3(0.0f,0.0f, 0.0f),
    glm::vec3(0.0f,0.0f, 0.0f),
    glm::vec3(0.0f,0.0f, 0.0f)
};
glm::vec3 Light1 = glm::vec3(0);

// Cubito para visualizar la lámpara
float vertices[] = {
    -0.5f,-0.5f,-0.5f,  0.0f,0.0f,-1.0f,  0.5f,-0.5f,-0.5f,  0.0f,0.0f,-1.0f,
     0.5f, 0.5f,-0.5f,  0.0f,0.0f,-1.0f,  0.5f, 0.5f,-0.5f,  0.0f,0.0f,-1.0f,
    -0.5f, 0.5f,-0.5f,  0.0f,0.0f,-1.0f, -0.5f,-0.5f,-0.5f,  0.0f,0.0f,-1.0f,

    -0.5f,-0.5f, 0.5f,  0.0f,0.0f, 1.0f,  0.5f,-0.5f, 0.5f,  0.0f,0.0f, 1.0f,
     0.5f, 0.5f, 0.5f,  0.0f,0.0f, 1.0f,  0.5f, 0.5f, 0.5f,  0.0f,0.0f, 1.0f,
    -0.5f, 0.5f, 0.5f,  0.0f,0.0f, 1.0f, -0.5f,-0.5f, 0.5f,  0.0f,0.0f, 1.0f,

    -0.5f, 0.5f, 0.5f, -1.0f,0.0f,0.0f,  -0.5f, 0.5f,-0.5f, -1.0f,0.0f,0.0f,
    -0.5f,-0.5f,-0.5f, -1.0f,0.0f,0.0f,  -0.5f,-0.5f,-0.5f, -1.0f,0.0f,0.0f,
    -0.5f,-0.5f, 0.5f, -1.0f,0.0f,0.0f,  -0.5f, 0.5f, 0.5f, -1.0f,0.0f,0.0f,

     0.5f, 0.5f, 0.5f,  1.0f,0.0f,0.0f,   0.5f, 0.5f,-0.5f,  1.0f,0.0f,0.0f,
     0.5f,-0.5f,-0.5f,  1.0f,0.0f,0.0f,   0.5f,-0.5f, 0.5f,  1.0f,0.0f,0.0f,
     0.5f, 0.5f, 0.5f,  1.0f,0.0f,0.0f,

    -0.5f,-0.5f,-0.5f,  0.0f,-1.0f,0.0f,  0.5f,-0.5f,-0.5f,  0.0f,-1.0f,0.0f,
     0.5f,-0.5f, 0.5f,  0.0f,-1.0f,0.0f,  0.5f,-0.5f, 0.5f,  0.0f,-1.0f,0.0f,
    -0.5f,-0.5f, 0.5f,  0.0f,-1.0f,0.0f, -0.5f,-0.5f,-0.5f,  0.0f,-1.0f,0.0f,

    -0.5f, 0.5f,-0.5f,  0.0f, 1.0f,0.0f,  0.5f, 0.5f,-0.5f,  0.0f, 1.0f,0.0f,
     0.5f, 0.5f, 0.5f,  0.0f, 1.0f,0.0f,  0.5f, 0.5f, 0.5f,  0.0f, 1.0f,0.0f,
    -0.5f, 0.5f, 0.5f,  0.0f, 1.0f,0.0f, -0.5f, 0.5f,-0.5f,  0.0f, 1.0f,0.0f
};

// Tiempos
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;
glm::vec3 P1PAj(0, 0, 0);
glm::vec3 P2PAj(0, 0, 0);
glm::vec3 P3PAj(0, 0, 0);
glm::vec3 P4PAj(0, 0, 0);
float rotarCuerpo1 =1;
float rotaralas =0.0;

float SubirBase = 0.0;

float girarCandiles = 1.0;

float escalarCandiles = 0.0;

glm::vec3 P1Base = P1PAj;
glm::vec3 P2Base = P2PAj;
glm::vec3 P3Base = P3PAj;
glm::vec3 P4Base = P4PAj;


float M_PI = 3.1416;

bool mvPajaro=false;
bool ALASPAJ = false;
bool ALASMV = false;
int main() {
    // Init GLFW
    glfwInit();
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "ProyectoFinal_Galeria_Alexa_Lopez", nullptr, nullptr);
    if (!window) { std::cout << "Failed to create GLFW window\n"; glfwTerminate(); return EXIT_FAILURE; }
    glfwMakeContextCurrent(window);
    glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);

    glfwSetKeyCallback(window, KeyCallback);
    glfwSetCursorPosCallback(window, MouseCallback);
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) { std::cout << "Failed to initialize GLEW\n"; return EXIT_FAILURE; }

    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    Shader lightingShader("Shader/lighting.vs", "Shader/lighting.frag");
    Shader lampShader("Shader/lamp.vs", "Shader/lamp.frag");

    // ====== TUS MODELOS ======
    //Cuarto1
    Model Galeria((char*)"Galeria/GaleriaRGR.obj");
    Model BancaPiedra((char*)"BancaPiedra/Banca.obj");
    Model CuadroCircular((char*)"CuadroCircular/CuadroCirculo.obj");
    Model CuadroRayas((char*)"CuadroRayas/CuadroRayas.obj");
    Model EsculturaPiedra((char*)"EsculturaPiedra/EsculturaPiedra.obj");
    Model Lavadero((char*)"Lavadero/Lavadero.obj");
    Model Pajaro((char*)"Pajaro/Cuerpo.obj");
    Model PajaroAlaIzq((char*)"Pajaro/AlaDerecha.obj");
    Model PajaroAlaDer((char*)"Pajaro/AlaIzquierda.obj");
    Model Planta((char*)"Planta/Planta.obj");
    Model Arbol((char*)"Planta/arbolito.obj");

    glm::vec3 ALA_IZQ_PIVOT(-0.06f, 0.02f, 0.00f);
    glm::vec3 ALA_DER_PIVOT(0.06f, 0.02f, 0.00f);
    //CuartoInterm
    Model Escalera((char*)"Escalera/Escalera.obj");
    Model Cuadro((char*)"CuadroInterm/CuadroInterm.obj");
    Model Pedestal((char*)"Pedestal/Pedestal.obj");
    Model EsculturaPed((char*)"Pedestal/EsculturaInterna.obj");
    //Cuarto2
    Model Banca((char*)"Banca/Banca.obj");
    Model CirculoMedio((char*)"CuadroCirculoMedio/CuadroCirculoMedio.obj");
    Model CuadroNegro((char*)"CuadroNegro/CuadroNegro.obj");
    Model CuadroCirculo1((char*)"CuadrosCirculo/CuadroCirculo.obj");
    Model CuadroCirculo2((char*)"CuadrosCirculo1/CuadroCirculo1.obj");
    Model CuadroCirculo3((char*)"CuadrosCirculo2/CuadroCirculo2.obj");
    Model CuadroCirculo4((char*)"CuadrosCirculo3/CuadroCirculo3.obj");
    Model cortinillas((char*)"Cortina/Cortinillas.obj"); 
    Model base((char*)"Cortina/Base.obj");

   
    // ====== VAO/VBO para el cubito de la luz ======
    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    lightingShader.Use();
    glUniform1i(glGetUniformLocation(lightingShader.Program, "Material.difuse"), 0);
    glUniform1i(glGetUniformLocation(lightingShader.Program, "Material.specular"), 1);

    glm::mat4 projection = glm::perspective(camera.GetZoom(), (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 100.0f);

    while (!glfwWindowShouldClose(window)) {
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame; lastFrame = currentFrame;
        animacion();
        glfwPollEvents();
        DoMovement();
        Animation();
       updateAnim(deltaTime);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        lightingShader.Use();

        GLint viewPosLoc = glGetUniformLocation(lightingShader.Program, "viewPos");
        glUniform3f(viewPosLoc, camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);

        // Luz direccional (igual que tu código)
        glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.direction"), -0.2f, -1.0f, -0.3f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.ambient"), 0.6f, 0.6f, 0.6f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.diffuse"), 0.6f, 0.6f, 0.6f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.specular"), 0.3f, 0.3f, 0.3f);

        glm::vec3 lightColor;
        lightColor.x = fabsf(sinf(glfwGetTime() * Light1.x));
        lightColor.y = fabsf(sinf(glfwGetTime() * Light1.y));
        lightColor.z = sinf(glfwGetTime() * Light1.z);

        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].position"),
            pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].ambient"), lightColor.x, lightColor.y, lightColor.z);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].diffuse"), lightColor.x, lightColor.y, lightColor.z);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].specular"), 1.0f, 0.2f, 0.2f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].constant"), 1.0f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].linear"), 0.045f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].quadratic"), 0.075f);

        // Spotlight en cámara (igual que tu código)
        glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.position"), camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.direction"), camera.GetFront().x, camera.GetFront().y, camera.GetFront().z);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.ambient"), 0.2f, 0.2f, 0.8f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.diffuse"), 0.2f, 0.2f, 0.8f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.specular"), 0.0f, 0.0f, 0.0f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.constant"), 1.0f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.linear"), 0.3f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.quadratic"), 0.7f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.cutOff"), glm::cos(glm::radians(12.0f)));
        glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.outerCutOff"), glm::cos(glm::radians(18.0f)));

        glUniform1f(glGetUniformLocation(lightingShader.Program, "material.shininess"), 5.0f);

        glm::mat4 view = camera.GetViewMatrix();
        GLint modelLoc = glGetUniformLocation(lightingShader.Program, "model");
        GLint viewLoc = glGetUniformLocation(lightingShader.Program, "view");
        GLint projLoc = glGetUniformLocation(lightingShader.Program, "projection");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

      
        //Cuarto1
        glm::mat4 model(1.0f);
        glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);

        // Sala / Galería (paredes, piso)
        model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(1.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        Galeria.Draw(lightingShader);


        //Cortinillas
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(50.0f, 6.0f , -15.0f));
        model = glm::scale(model, glm::vec3(4.0f + 1.5*escalarCandiles,4.0f,4.0f+ escalarCandiles));
        model = glm::rotate(model, glm::radians(-45.0f * girarCandiles), glm::vec3(0, 1, 0));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        cortinillas.Draw(lightingShader);

        //BaseCortinillas
        model = glm::mat4(1.0f);

        model = glm::translate(model, glm::vec3(50.0f, 6.0f + SubirBase, -15.0f));
        model = glm::scale(model, glm::vec3(4.0f));
        model = glm::rotate(model, glm::radians(-45.0f), glm::vec3(0, 1, 0));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        base.Draw(lightingShader);

        // Banca de piedra (derecha/frontal)
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(20.0f, -0.65f, -47.0f));
        model = glm::rotate(model, glm::radians(-155.0f), glm::vec3(0, 1, 0));
        model = glm::scale(model, glm::vec3(0.4f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        BancaPiedra.Draw(lightingShader);

        // Escultura de piedra (centro-izquierda)
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(37.0f, -0.1f, -48.0f));
        model = glm::scale(model, glm::vec3(1.5f));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0, 1, 0));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        EsculturaPiedra.Draw(lightingShader);

        // Lavadero (mueble de madera al fondo-centro)
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(17.0f, 0.75f, -42.0f));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0, 1, 0));
        model = glm::scale(model, glm::vec3(0.7f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        Lavadero.Draw(lightingShader);

        // Planta 
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(28.0f, 0.3f, -44.0f));
        model = glm::scale(model, glm::vec3(2.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        Planta.Draw(lightingShader);

        glm::mat4 Mplanta = model;

        // Árbol
        glm::mat4 Marbol = Mplanta;
        Marbol = glm::translate(Marbol, glm::vec3(-0.5f, -0.5f, 0.5f));
        Marbol = glm::scale(Marbol, glm::vec3(2.0f));

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(Marbol));
        Arbol.Draw(lightingShader); 

        // Cuadro circular (pared izquierda)
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(26.0f, 7.0f, -35.0f));
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 1, 0)); 
        model = glm::scale(model, glm::vec3(3.5f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        CuadroCircular.Draw(lightingShader);

        // Cuadro de rayas (pared derecha)
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(16.0f, 2.3f, -48.7f));
        model = glm::rotate(model, glm::radians(-270.0f), glm::vec3(0, 1, 0));
        model = glm::scale(model, glm::vec3(3.0f,4.0f, 1.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        CuadroRayas.Draw(lightingShader);

        // Pájaro 1
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(10.0f+ P1PAj.x, 3.0f+ P1PAj.y, -42.0f+P1PAj.z));
        model = glm::rotate(model, glm::radians(-90.0f*rotarCuerpo1), glm::vec3(0, 1, 0));
        model = glm::scale(model, glm::vec3(0.70f));

        // cuerpo
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        Pajaro.Draw(lightingShader);

        glm::mat4 M1 = model; 

        glm::mat4 M1_izq = M1;
        M1_izq = glm::translate(M1_izq, ALA_IZQ_PIVOT);
        M1_izq = glm::translate(M1_izq, -ALA_IZQ_PIVOT);
        M1_izq = glm::rotate(M1_izq, glm::radians(-90 * rotaralas), glm::vec3(1, 0, 0));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(M1_izq));
        PajaroAlaIzq.Draw(lightingShader);

        glm::mat4 M1_der = M1;
        M1_der = glm::translate(M1_der, ALA_DER_PIVOT);
        M1_der = glm::translate(M1_der, -ALA_DER_PIVOT);
        M1_der = glm::rotate(M1_der, glm::radians(90 * rotaralas), glm::vec3(1, 0, 0));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(M1_der));
        PajaroAlaDer.Draw(lightingShader);

        // Pájaro 2
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(10.0f+ P2PAj.x, 1.2f+ P2PAj.y, -41.0f+P2PAj.z));
        model = glm::rotate(model, glm::radians(-90.0f*rotarCuerpo1), glm::vec3(0, 1, 0));
        model = glm::scale(model, glm::vec3(0.70f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        Pajaro.Draw(lightingShader);

        glm::mat4 M2 = model;

        glm::mat4 M2_izq = M2;
        M2_izq = glm::translate(M2_izq, ALA_IZQ_PIVOT);
        M2_izq = glm::translate(M2_izq, -ALA_IZQ_PIVOT);
        M2_izq = glm::rotate(M2_izq, glm::radians(-90 * rotaralas), glm::vec3(1, 0, 0));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(M2_izq));
        PajaroAlaIzq.Draw(lightingShader);

        glm::mat4 M2_der = M2;
        M2_der = glm::translate(M2_der, ALA_DER_PIVOT);
        M2_der = glm::translate(M2_der, -ALA_DER_PIVOT);
        M2_der = glm::rotate(M2_der, glm::radians(90 * rotaralas), glm::vec3(1, 0, 0));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(M2_der));
        PajaroAlaDer.Draw(lightingShader);

        // Pájaro 3
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(10.0f+ P3PAj.x, 2.5f+ P3PAj.y, -44.0f+ P3PAj.z));
        model = glm::rotate(model, glm::radians(-90.0f* rotarCuerpo1), glm::vec3(0, 1, 0));
        model = glm::scale(model, glm::vec3(0.70f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        Pajaro.Draw(lightingShader);

        glm::mat4 M3 = model;

        glm::mat4 M3_izq = M3;
        M3_izq = glm::translate(M3_izq, ALA_IZQ_PIVOT);
        M3_izq = glm::translate(M3_izq, -ALA_IZQ_PIVOT);

        M3_izq = glm::rotate(M3_izq, glm::radians(-90 * rotaralas), glm::vec3(1, 0, 0));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(M3_izq));
        PajaroAlaIzq.Draw(lightingShader);

        glm::mat4 M3_der = M3;
        M3_der = glm::translate(M3_der, ALA_DER_PIVOT);
        M3_der = glm::translate(M3_der, -ALA_DER_PIVOT);
        M3_der = glm::rotate(M3_der, glm::radians(90 * rotaralas), glm::vec3(1, 0, 0));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(M3_der));
        PajaroAlaDer.Draw(lightingShader);

        // Pájaro 4
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(10.0f + P4PAj.x, 1.60f+ P4PAj.y, -46.0f+ P4PAj.z));
        model = glm::rotate(model, glm::radians(-90.0f* rotarCuerpo1), glm::vec3(0, 1, 0));
        model = glm::scale(model, glm::vec3(0.70f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        Pajaro.Draw(lightingShader);

        glm::mat4 M4 = model;

        glm::mat4 M4_izq = M4;
        M4_izq = glm::translate(M4_izq, ALA_IZQ_PIVOT);
        M4_izq = glm::translate(M4_izq, -ALA_IZQ_PIVOT);

        M4_izq = glm::rotate(M4_izq, glm::radians(-90 * rotaralas), glm::vec3(1, 0, 0));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(M4_izq));
        PajaroAlaIzq.Draw(lightingShader);

        glm::mat4 M4_der = M4;
        M4_der = glm::translate(M4_der, ALA_DER_PIVOT);
        M4_der = glm::translate(M4_der, -ALA_DER_PIVOT);

        M4_der = glm::rotate(M4_der , glm::radians(90 * rotaralas), glm::vec3(1, 0, 0));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(M4_der));
        PajaroAlaDer.Draw(lightingShader);

        //Cuarto Intermedio
        
        // Escalera
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(13.6f, -0.3f, -31.7f));
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 1, 0));
        model = glm::scale(model, glm::vec3(2.5f, 1.5f, 1.5f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        Escalera.Draw(lightingShader);

		// Cuadro Intermedio (pared trasera)
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(50.0f, 5.0f, -27.4f));
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 1, 0));
        model = glm::scale(model, glm::vec3(2.5f, 2.5f, 2.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        Cuadro.Draw(lightingShader);

		// Pedestal (centro del cuarto)

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(28.0f, -0.6f, -28.0f));
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 1, 0));
        model = glm::scale(model, glm::vec3(2.5f, 3.0f, 2.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        Pedestal.Draw(lightingShader);

        glm::mat4 Mped = model;
        glm::mat4 Mesc = Mped;
        Mesc = glm::translate(Mesc, glm::vec3(0.0f, -1.6f, 0.0f));

        Mesc = glm::rotate(Mesc, glm::radians(0.0f), glm::vec3(0, 1, 0)); 
        Mesc = glm::scale(Mesc, glm::vec3(1.8f));                      
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(Mesc));
        EsculturaPed.Draw(lightingShader);

        //Cuarto2
        
        // Banca
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(35.6f, 0.0f, -15.7f));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        Banca.Draw(lightingShader);

        // CuadroCirculoMedio
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(7.4f, 5.3f, -16.0f));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 0, 1));
        model = glm::scale(model, glm::vec3(4.5f, 5.5f, 1.5f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        CirculoMedio.Draw(lightingShader);

        // Cuadro negro
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(35.6f, 5.0f, -25.7f));
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 0, 1));
        model = glm::scale(model, glm::vec3(4.5f, 5.5f, 1.5f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        CuadroNegro.Draw(lightingShader);

        // Cuadro circulo1
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(30.6f, -0.3f, -8.7f));
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 1, 0));
        model = glm::scale(model, glm::vec3(2.5f, 1.5f, 1.5f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        CuadroCirculo1.Draw(lightingShader);

        // Cuadro circulo2
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(30.6f, -0.3f, -8.7f));
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 1, 0));
        model = glm::scale(model, glm::vec3(2.5f, 1.5f, 1.5f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        CuadroCirculo2.Draw(lightingShader);

        // Cuadro circulo3
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(30.6f, -0.3f, -8.7f));
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 1, 0));
        model = glm::scale(model, glm::vec3(2.5f, 1.5f, 1.5f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        CuadroCirculo3.Draw(lightingShader);

        // Cuadro circulo4
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(30.6f, -0.3f, -8.7f));
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 1, 0));
        model = glm::scale(model, glm::vec3(2.5f, 1.5f, 1.5f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        CuadroCirculo4.Draw(lightingShader);

        // ================== Visualizador de luz ==================
        lampShader.Use();
        modelLoc = glGetUniformLocation(lampShader.Program, "model");
        viewLoc = glGetUniformLocation(lampShader.Program, "view");
        projLoc = glGetUniformLocation(lampShader.Program, "projection");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        glm::mat4 m = glm::mat4(1.0f);
        m = glm::translate(m, pointLightPositions[0]);
        m = glm::scale(m, glm::vec3(0.2f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(m));

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}

// ====== Controles ======
void DoMovement() {
    if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP])    camera.ProcessKeyboard(FORWARD, deltaTime);
    if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN])  camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT])  camera.ProcessKeyboard(LEFT, deltaTime);
    if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT]) camera.ProcessKeyboard(RIGHT, deltaTime);

    if (keys[GLFW_KEY_T]) pointLightPositions[0].x += 0.01f;
    if (keys[GLFW_KEY_G]) pointLightPositions[0].x -= 0.01f;
    if (keys[GLFW_KEY_Y]) pointLightPositions[0].y += 0.01f;
    if (keys[GLFW_KEY_H]) pointLightPositions[0].y -= 0.01f;
    if (keys[GLFW_KEY_U]) pointLightPositions[0].z -= 0.10f;
    if (keys[GLFW_KEY_J]) pointLightPositions[0].z += 0.01f;
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) glfwSetWindowShouldClose(window, GL_TRUE);

    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS)   keys[key] = true;
        else if (action == GLFW_RELEASE) keys[key] = false;
    }

    if (keys[GLFW_KEY_SPACE]) {
        active = !active;
        if (active) Light1 = glm::vec3(0.2f, 0.8f, 1.0f);
        else        Light1 = glm::vec3(0.0f);
    
    }
    if (keys[GLFW_KEY_L]) {
        PlaySound(TEXT("sonidos/volar.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT | SND_NOSTOP);

        if (!pajarosanim) {
            pajarosanim = true;
            mvPajaro = true;
           
        }
    }

    
    if (keys[GLFW_KEY_K]) {

        if (!ALASPAJ) {
            PlaySound(TEXT("sonidos/candiles.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT | SND_NOSTOP);
            ALASPAJ = true;
        }
    }
}

void MouseCallback(GLFWwindow* window, double xPos, double yPos) {
    if (firstMouse) { lastX = (float)xPos; lastY = (float)yPos; firstMouse = false; }
    GLfloat xOffset = (float)xPos - lastX;
    GLfloat yOffset = lastY - (float)yPos;
    lastX = (float)xPos; lastY = (float)yPos;
    camera.ProcessMouseMovement(xOffset, yOffset);
}

void animacion() {

    if (pajarosanim) {
        // Velocidad (ajusta a gusto)
        tPajaro += 0.005f;

        if (tPajaro >= 1.0f) {
            tPajaro = 1.0f;
            pajarosanim = false;  // terminó el ciclo, ya regresó
        }

        float angle = tPajaro * 2.0f * M_PI;
        float theta = tPajaro * 2.0f * M_PI;
        // --- Pájaro 1 (más ovalado)
        float rX1 = -12.0f;   // más ancho
        float rZ1 = 2.0f;   // menos profundo
        float offsetX1 = rX1 * cos(angle) - rX1;
        float offsetZ1 = rZ1 * sin(angle);
        float offsetY1 = 0.5f * sin(theta);
        P1PAj = P1Base + glm::vec3(offsetX1, offsetY1, offsetZ1);


        float rX2 = -8.0f;  // horizontal
        float rZ2 = 3.0f;  // profundidad
        float offsetX2 = rX2 * cos(angle) - rX2;
        float offsetZ2 = rZ2 * sin(angle);
        float offsetY2 = 0.3f * sin(theta);
        glm::vec3 offset2(offsetX2, offsetY2, offsetZ2);

        P2PAj = P2Base + offset2;


        float rX = -8.0f;  // horizontal
        float rZ = 0.5f;  // profundidad
        float offsetX = rX * cos(angle) - rX;
        float offsetZ = rZ * sin(angle);
        float offsetY = 2.3f * sin(angle); 
        glm::vec3 offset(offsetX, offsetY, offsetZ);

        P3PAj = P3Base + offset;

        float rX3 = -2.0f;  // horizontal
        float rZ3 = 0.5f;  // profundidad
        float offsetX3 = rX3 * cos(angle) - rX3;
        float offsetZ3 = rZ3 * sin(angle);
        float offsetY3 = 2.0f * sin(theta);
        glm::vec3 offset3(offsetX3, offsetY3, offsetZ3);

        P4PAj = P4Base + offset3;

        // Rotación según dirección del movimiento (tangente a la curva)
        // Derivadas:
        // dx/da = -rX*sin(angle)
        // dz/da =  rZ*cos(angle)
        float dirX = -rX * sin(angle);
        float dirZ = rZ * cos(angle);

        // Ángulo en grados (ajusta eje según tu modelo)
        rotarCuerpo1 -= 0.02;
        if (ALASMV) {
            if (rotaralas <= 1) {
                rotaralas += 0.1;
            }
            else {
                ALASMV = false;
            }
        }
        else {
            if (rotaralas > 0) {

                rotaralas -= 0.1;
            }
            else {
                ALASMV = true;
            }

        }
    }
    else {
        tPajaro = 0.0f;
        P1PAj = P1Base;
        P2PAj = P2Base;
        P3PAj = P3Base;
        P4PAj = P4Base;
        rotaralas = 0;
        rotarCuerpo1 = 1.0;
       
    }
    
    if (true) {
        
    }
    
}

void updateAnim(float deltaTime) {
    if (!ALASPAJ) return;

    animTime += deltaTime;

    switch (animState) {

    case PHASE1_SUBIR_ESCALAR_GIRAR_RAPIDO: {
        float t = glm::clamp(animTime / dur1, 0.0f, 1.0f);
        SubirBase = glm::mix(0.0f, baseAlturaTarget, t);
        escalarCandiles = glm::mix(0.0f, 4.0f, t);                 // 4 -> 8
        girarCandiles = glm::mix(startFactor, factor3Turns, t);  // 1 -> 25 (3 vueltas)

        if (animTime >= dur1) {
            animState = PHASE2_GIRO_90_LENTO;   // ahora será el giro de 180°
            animTime = 0.0f;
        }
        break;
    }

                                          // 2) Ya grande, gira 180° (lento) manteniendo escala
    case PHASE2_GIRO_90_LENTO: {
        float t = glm::clamp(animTime / dur2, 0.0f, 1.0f);

        SubirBase = baseAlturaTarget;
        escalarCandiles = 4.0f; // sigue al doble
        // 3 vueltas terminadas (25) -> +180° (29)
        girarCandiles = glm::mix(factor3Turns, factor3TurnsPlus180, t);

        if (animTime >= dur2) {
            animState = PHASE3_GIRO_90_REGRESO_MAS_RAPIDO;
            animTime = 0.0f;
        }
        break;
    }

                             // 3) Regresa esos 180° más rápido (si quieres que vuelva a la orientación anterior)
    case PHASE3_GIRO_90_REGRESO_MAS_RAPIDO: {
        float t = glm::clamp(animTime / dur3, 0.0f, 1.0f);

        SubirBase = baseAlturaTarget;
        escalarCandiles = 4.0f;
        // de 29 -> 25 (quita los 180° extra)
        girarCandiles = glm::mix(factor3TurnsPlus180, factor3Turns, t);

        if (animTime >= dur3) {
            animState = PHASE4_BAJAR_Y_ENCOGER_RAPIDO;
            animTime = 0.0f;
        }
        break;
    }

                                          // 4) Baja base + vuelve a tamaño original + vuelve al factor inicial
    case PHASE4_BAJAR_Y_ENCOGER_RAPIDO: {
        float t = glm::clamp(animTime / dur4, 0.0f, 1.0f);

        SubirBase = glm::mix(baseAlturaTarget, 0.0f, t);
        escalarCandiles = glm::mix(4.0f, 0.0f, t);                // 8 -> 4
        girarCandiles = glm::mix(factor3Turns, startFactor, t); // vuelve a -45°

        if (animTime >= dur4) {
            animState = PHASE1_SUBIR_ESCALAR_GIRAR_RAPIDO;
            ALASPAJ = false;   // listo para reactivar
            animTime = 0.0f;
            PlaySound(NULL, 0, 0);
        }
        break;
    }

    default:
        break;
    }
}


