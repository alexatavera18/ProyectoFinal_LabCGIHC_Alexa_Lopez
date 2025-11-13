// Nombre: Alexa Fernanda López Tavera
// Proyecto Final CGIHC "Galería de arte interactiva con OpenGL"
// Fecha de Entrega: 12 de Noviembre, 2025
// No. de cuenta: 319023024

#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>
#include <iomanip>
#include <limits>

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

// ===================== Config de luces =====================
constexpr int MAX_POINT_LIGHTS = 128;
glm::vec3 lightPosArr[MAX_POINT_LIGHTS];
glm::vec3 lightColorArr[MAX_POINT_LIGHTS];
int usedLights = 0;

bool hallwayOnlyMode = false;

// Atenuación ajustada para ambiente de galería (más suave y natural)
const float kC = 1.0f, kL = 0.09f, kQ = 0.032f;

// ---- Rectángulos (centros de rieles de reflectores) ----
const int NUM_RECTS = 5;
struct RectData {
    glm::vec3 center;
    float halfX;
    float halfZ;
};

RectData reflectorRects[NUM_RECTS] = {
    { glm::vec3(15.0f, 11.5f, -37.0f), 6.0f, 0.8f }, // cuarto 1 izquierdo
    { glm::vec3(35.0f, 11.5f, -37.0f), 6.0f, 0.8f }, // cuarto 1 derecho
    { glm::vec3(35.0f, 11.5f, -25.0f), 6.0f, 0.8f }, // pasillo central
    { glm::vec3(12.0f, 11.5f, -8.0f),  6.0f, 0.8f }, // cuarto 2 izquierdo
    { glm::vec3(45.0f, 11.5f, -8.0f),  6.0f, 0.8f }  // cuarto 2 derecho
};

float rectYDrop[NUM_RECTS] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
int selReflector = 0;

int bulbsPerLane = 8;
int lanesPerRail = 2;
float lanePaddingZ = 0.1f;

// Variable para animación del cuadro CirculoMedio
float circuloMedioScale = 1.0f;

// ===================== Prototipos =====================
void KeyCallback(GLFWwindow* w, int key, int sc, int action, int mods);
void MouseCallback(GLFWwindow* w, double x, double y);
void DoMovement();
void animacion();
void updateAnim(float dt);
void SetupStaticLights();

// ===================== Ventana / Cámara =====================
const GLuint WIDTH = 800, HEIGHT = 600;
int SCREEN_WIDTH, SCREEN_HEIGHT;

Camera camera(glm::vec3(0.0f, 1.2f, 4.5f));
GLfloat lastX = WIDTH / 2.0f, lastY = HEIGHT / 2.0f;
bool keys[1024]{};
bool firstMouse = true;

// ===================== Estado original del proyecto =====================
bool pajarosanim = false;
float tPajaro = 0.0f;
const float M_PI_ = 3.14159265f;

glm::vec3 P1PAj(0), P2PAj(0), P3PAj(0), P4PAj(0);
glm::vec3 P1Base(0), P2Base(0), P3Base(0), P4Base(0);
float rotarCuerpo1 = 1.0f;
float rotaralas = 0.0f;
bool mvPajaro = false;
bool ALASPAJ = false;
bool ALASMV = false;

// Candil/colgante (animable)
float SubirBase = 0.0f;
float girarCandiles = 1.0f;
float escalarCandiles = 0.0f;

// ===================== Keyframes SOLO colgante =====================
struct CandilFrame { float subir, girar, escalar; };
std::vector<CandilFrame> candilKeys;
bool  playCandil = false;
float candilT = 0.0f;
float candilDur = 4.0f;
size_t candilSegActual = 0;

// ===================== Tiempos =====================
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

// ===================== Geometría cubo (opcional) =====================
float cubeVerts[] = {
    -0.5f,-0.5f,-0.5f, 0,0,-1,  0.5f,-0.5f,-0.5f, 0,0,-1,  0.5f,0.5f,-0.5f, 0,0,-1,
     0.5f,0.5f,-0.5f, 0,0,-1, -0.5f,0.5f,-0.5f, 0,0,-1, -0.5f,-0.5f,-0.5f, 0,0,-1,
    -0.5f,-0.5f, 0.5f, 0,0, 1,  0.5f,-0.5f, 0.5f, 0,0, 1,  0.5f,0.5f, 0.5f, 0,0, 1,
     0.5f,0.5f, 0.5f, 0,0, 1, -0.5f,0.5f, 0.5f, 0,0, 1, -0.5f,-0.5f, 0.5f, 0,0, 1,
    -0.5f,0.5f, 0.5f,-1,0,0, -0.5f,0.5f,-0.5f,-1,0,0, -0.5f,-0.5f,-0.5f,-1,0,0,
    -0.5f,-0.5f,-0.5f,-1,0,0, -0.5f,-0.5f, 0.5f,-1,0,0, -0.5f,0.5f, 0.5f,-1,0,0,
     0.5f,0.5f, 0.5f, 1,0,0,  0.5f,0.5f,-0.5f, 1,0,0,  0.5f,-0.5f,-0.5f, 1,0,0,
     0.5f,-0.5f,-0.5f, 1,0,0,  0.5f,-0.5f, 0.5f, 1,0,0,  0.5f,0.5f, 0.5f, 1,0,0,
    -0.5f,-0.5f,-0.5f,0,-1,0,  0.5f,-0.5f,-0.5f,0,-1,0,  0.5f,-0.5f, 0.5f,0,-1,0,
     0.5f,-0.5f, 0.5f,0,-1,0, -0.5f,-0.5f, 0.5f,0,-1,0, -0.5f,-0.5f,-0.5f,0,-1,0,
    -0.5f,0.5f,-0.5f,0, 1,0,  0.5f,0.5f,-0.5f,0, 1,0,  0.5f,0.5f, 0.5f,0, 1,0,
     0.5f,0.5f, 0.5f,0, 1,0, -0.5f,0.5f, 0.5f,0, 1,0, -0.5f,0.5f,-0.5f,0, 1,0
};

// ===================== Main =====================
int main() {
    // Init
    glfwInit();
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "ProyectoFinal_Galeria_Alexa_Lopez", nullptr, nullptr);
    if (!window) { std::cout << "Failed to create GLFW window\n"; glfwTerminate(); return EXIT_FAILURE; }
    glfwMakeContextCurrent(window);
    glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);

    glfwSetKeyCallback(window, KeyCallback);
    glfwSetCursorPosCallback(window, MouseCallback);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) { std::cout << "Failed to initialize GLEW\n"; return EXIT_FAILURE; }

    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    // Alpha + profundidad
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);

    // ======== Shaders ========
    Shader lightingShader("Shader/lighting.vs", "Shader/lighting.frag");
    Shader lampShader("Shader/lamp.vs", "Shader/lamp.frag");

    // ======== Luces ========
    SetupStaticLights();

    // ======== Modelos ========
    Model Reflectores((char*)"Reflector/reflector.obj");

    // Cuarto 1
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
    Model Circulo1((char*)"FiguraR1/model.obj");
    Model Circulo2((char*)"FiguraR2/model.obj");

    glm::vec3 ALA_IZQ_PIVOT(-0.06f, 0.02f, 0.00f);
    glm::vec3 ALA_DER_PIVOT(0.06f, 0.02f, 0.00f);

    // Cuarto intermedio
    Model Escalera((char*)"Escalera/Escalera.obj");
    Model Cuadro((char*)"CuadroInterm/CuadroInterm.obj");
    Model Pedestal((char*)"Pedestal/Pedestal.obj");
    Model EsculturaPed((char*)"Pedestal/EsculturaInterna.obj");

    // Cuarto 2
    Model Banca((char*)"Banca/Banca.obj");
    Model CirculoMedio((char*)"CuadroCirculoMedio/CuadroCirculoMedio.obj");
    Model CuadroNegro((char*)"CuadroNegro/CuadroNegro.obj");
    Model CuatroCirculos((char*)"CuatroCirculos/cuadroscirculos.obj");
    Model CuadroVerde((char*)"CuadroVerde/agujeroverde.obj");
    Model CuadroEspiral((char*)"Espiral/espiral.obj");
    Model CuadroSol((char*)"sol/sol.obj");
    Model CuadroOnda((char*)"CuadroOnda/onda.obj");
    Model cortinillas((char*)"Cortina/Cortinillas.obj");
    Model base((char*)"Cortina/Base.obj");

    // ======== VAO/VBO cubo (debug opcional) ========
    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVerts), cubeVerts, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    lightingShader.Use();
    glUniform1i(glGetUniformLocation(lightingShader.Program, "material.diffuse"), 0);
    glUniform1i(glGetUniformLocation(lightingShader.Program, "material.specular"), 1);
    glUniform1f(glGetUniformLocation(lightingShader.Program, "alphaMul"), 1.0f);

    glm::mat4 projection = glm::perspective(camera.GetZoom(), (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 200.0f);

    // ======== Loop ========
    while (!glfwWindowShouldClose(window)) {
        GLfloat currentFrame = (GLfloat)glfwGetTime();
        deltaTime = currentFrame - lastFrame; lastFrame = currentFrame;

        animacion();
        glfwPollEvents();
        DoMovement();
        updateAnim(deltaTime);

        // Fondo
        glClearColor(0.06f, 0.06f, 0.08f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        lightingShader.Use();

        // Cámara
        GLint viewPosLoc = glGetUniformLocation(lightingShader.Program, "viewPos");
        glUniform3f(viewPosLoc, camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);

        // Luz direccional (ambiente general suave pero visible)
        glm::vec3 sunDir = glm::normalize(glm::vec3(-0.3f, -0.7f, -0.2f));
        glm::vec3 sunColor = glm::vec3(1.00f, 0.96f, 0.90f);
        float ambI = 0.12f, diffI = 0.35f, specI = 0.04f; // Un poco más de luz

        glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.direction"), sunDir.x, sunDir.y, sunDir.z);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.ambient"),
            sunColor.r * ambI, sunColor.g * ambI, sunColor.b * ambI);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.diffuse"),
            sunColor.r * diffI, sunColor.g * diffI, sunColor.b * diffI);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.specular"),
            sunColor.r * specI, sunColor.g * specI, sunColor.b * specI);

        // Sube TODAS las luces generadas con intensidades medias
        glUniform1i(glGetUniformLocation(lightingShader.Program, "numLights"), usedLights);
        for (int i = 0; i < usedLights; ++i) {
            std::string b = "pointLights[" + std::to_string(i) + "].";
            glUniform3fv(glGetUniformLocation(lightingShader.Program, (b + "position").c_str()), 1, &lightPosArr[i][0]);
            glm::vec3 c = lightColorArr[i];

            // Intensidades un poco más altas para mejor visibilidad
            glUniform3f(glGetUniformLocation(lightingShader.Program, (b + "ambient").c_str()),
                c.r * 0.015f, c.g * 0.015f, c.b * 0.015f);
            glUniform3f(glGetUniformLocation(lightingShader.Program, (b + "diffuse").c_str()),
                c.r * 0.40f, c.g * 0.40f, c.b * 0.40f);
            glUniform3f(glGetUniformLocation(lightingShader.Program, (b + "specular").c_str()),
                c.r * 0.25f, c.g * 0.25f, c.b * 0.25f);

            glUniform1f(glGetUniformLocation(lightingShader.Program, (b + "constant").c_str()), kC);
            glUniform1f(glGetUniformLocation(lightingShader.Program, (b + "linear").c_str()), kL);
            glUniform1f(glGetUniformLocation(lightingShader.Program, (b + "quadratic").c_str()), kQ);
        }
        // Matrices
        glm::mat4 view = camera.GetViewMatrix();
        GLint modelLoc = glGetUniformLocation(lightingShader.Program, "model");
        GLint viewLoc = glGetUniformLocation(lightingShader.Program, "view");
        GLint projLoc = glGetUniformLocation(lightingShader.Program, "projection");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        // ------------------- Dibujo -------------------
        glm::mat4 model(1.0f);
        glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);

        // Reflectores cuarto 1
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(15.0f, 12.5f, -37.0f));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
        model = glm::scale(model, glm::vec3(0.5f, 0.3f, 0.2f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        Reflectores.Draw(lightingShader);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(35.0f, 12.5f, -37.0f));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
        model = glm::scale(model, glm::vec3(0.5f, 0.3f, 0.2f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        Reflectores.Draw(lightingShader);

        // Sala / Galería
        model = glm::mat4(1.0f);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        Galeria.Draw(lightingShader);

        // Banca piedra
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(20.0f, -0.65f, -47.0f));
        model = glm::rotate(model, glm::radians(-155.0f), glm::vec3(0, 1, 0));
        model = glm::scale(model, glm::vec3(0.4f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        BancaPiedra.Draw(lightingShader);

        // Escultura piedra
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(37.0f, -0.1f, -48.0f));
        model = glm::scale(model, glm::vec3(1.5f));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0, 1, 0));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        EsculturaPiedra.Draw(lightingShader);

        // Lavadero
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(17.0f, 0.75f, -42.0f));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0, 1, 0));
        model = glm::scale(model, glm::vec3(0.7f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        Lavadero.Draw(lightingShader);

        // Planta que se inclina
        float sway = sinf(currentFrame * 0.8f) * glm::radians(6.0f);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(28.0f, 0.3f, -44.0f));
        model = glm::rotate(model, sway, glm::vec3(0, 0, 1));
        model = glm::scale(model, glm::vec3(2.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        Planta.Draw(lightingShader);

        glm::mat4 Mplanta = model;
        glm::mat4 Marbol = glm::translate(Mplanta, glm::vec3(-0.5f, -0.5f, 0.5f));
        Marbol = glm::scale(Marbol, glm::vec3(2.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(Marbol));
        Arbol.Draw(lightingShader);

        // Banca piedra 2
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(50.0f, -0.65f, -43.0f));
        model = glm::rotate(model, glm::radians(60.0f), glm::vec3(0, 1, 0));
        model = glm::scale(model, glm::vec3(0.8f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        BancaPiedra.Draw(lightingShader);

        // ——— Cuadros cuarto 1 ———
        // Persiana (Cuadro de Rayas)
        float persiana = 0.75f + 0.25f * sinf(currentFrame * 1.8f);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(16.0f, 2.3f, -48.7f));
        model = glm::rotate(model, glm::radians(-270.0f), glm::vec3(0, 1, 0));
        model = glm::scale(model, glm::vec3(3.0f * persiana, 4.0f, 1.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        CuadroRayas.Draw(lightingShader);

        // Cuadro circular
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(26.0f, 7.0f, -35.0f));
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 1, 0));
        model = glm::scale(model, glm::vec3(3.5f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        CuadroCircular.Draw(lightingShader);

        // Círculos pared fondo
        float zoom1 = 1.0f + 0.04f * sinf(currentFrame * 3.0f);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(45.0f, 6.3f, -50.7f));
        model = glm::scale(model, glm::vec3(7.0f * zoom1, 5.0f * zoom1, 1.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        Circulo1.Draw(lightingShader);

        float zoom2 = 1.0f + 0.05f * sinf(currentFrame * 2.2f + 1.0f);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(65.0f, 8.3f, -43.7f));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0, 1, 0));
        model = glm::scale(model, glm::vec3(7.0f * zoom2, 5.0f * zoom2, 1.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        Circulo2.Draw(lightingShader);

        // Bancas
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(62.0f, 0.0f, -42.5f));
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 1, 0));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        Banca.Draw(lightingShader);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(35.0f, 0.0f, -37.7f));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        Banca.Draw(lightingShader);

        // ================= Pájaros =================
        auto drawBird = [&](glm::vec3 base, glm::vec3 Aj) {
            glm::mat4 m = glm::mat4(1.0f);
            m = glm::translate(m, base + Aj);
            m = glm::rotate(m, glm::radians(-90.0f * rotarCuerpo1), glm::vec3(0, 1, 0));
            m = glm::scale(m, glm::vec3(0.70f));
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(m));
            Pajaro.Draw(lightingShader);

            glm::mat4 L = m;
            glm::mat4 L_izq = glm::translate(L, ALA_IZQ_PIVOT);
            L_izq = glm::rotate(L_izq, glm::radians(-90 * rotaralas), glm::vec3(1, 0, 0));
            L_izq = glm::translate(L_izq, -ALA_IZQ_PIVOT);
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(L_izq));
            PajaroAlaIzq.Draw(lightingShader);

            glm::mat4 L_der = glm::translate(L, ALA_DER_PIVOT);
            L_der = glm::rotate(L_der, glm::radians(90 * rotaralas), glm::vec3(1, 0, 0));
            L_der = glm::translate(L_der, -ALA_DER_PIVOT);
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(L_der));
            PajaroAlaDer.Draw(lightingShader);
            };
        drawBird(glm::vec3(10.0f, 3.0f, -42.0f), P1PAj);
        drawBird(glm::vec3(10.0f, 1.2f, -41.0f), P2PAj);
        drawBird(glm::vec3(10.0f, 2.5f, -44.0f), P3PAj);
        drawBird(glm::vec3(10.0f, 1.6f, -46.0f), P4PAj);

        // --------- Cuarto intermedio ---------
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(35.0f, 12.5f, -25.0f));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
        model = glm::scale(model, glm::vec3(0.3f, 0.2f, 0.2f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        Reflectores.Draw(lightingShader);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(13.6f, -0.3f, -31.7f));
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 1, 0));
        model = glm::scale(model, glm::vec3(2.5f, 1.5f, 1.5f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        Escalera.Draw(lightingShader);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(50.0f, 5.0f, -27.4f));
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 1, 0));
        model = glm::scale(model, glm::vec3(2.5f, 2.5f, 2.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        Cuadro.Draw(lightingShader);

        // Pedestal + objeto que flota
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(28.0f, -0.6f, -28.0f));
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 1, 0));
        model = glm::scale(model, glm::vec3(2.5f, 3.0f, 2.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        Pedestal.Draw(lightingShader);

        glm::mat4 Mped = model;
        glm::mat4 Mesc = glm::translate(Mped, glm::vec3(0.0f, -1.6f + 0.3f * sinf(currentFrame * 1.6f), 0.0f));
        Mesc = glm::scale(Mesc, glm::vec3(1.8f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(Mesc));
        EsculturaPed.Draw(lightingShader);

        // --------- Cuarto 2 ---------
        // Reflectores
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(12.0f, 12.5f, -8.0f));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
        model = glm::scale(model, glm::vec3(0.5f, 0.3f, 0.2f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        Reflectores.Draw(lightingShader);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(45.0f, 12.5f, -8.0f));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
        model = glm::scale(model, glm::vec3(0.5f, 0.3f, 0.2f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        Reflectores.Draw(lightingShader);

        // Cortinillas (colgante) 
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(35.0f, 8.0f, -15.0f));
        glm::vec3 sColgante(4.0f, 4.0f, 4.0f);
        sColgante.x += escalarCandiles;
        model = glm::scale(model, sColgante);
        model = glm::rotate(model, glm::radians(-45.0f * girarCandiles), glm::vec3(0, 1, 0));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        cortinillas.Draw(lightingShader);

        // Base del colgante (sube/baja con SubirBase)
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(35.0f, 8.0f + SubirBase, -15.0f));
        model = glm::scale(model, glm::vec3(4.0f));
        model = glm::rotate(model, glm::radians(-45.0f), glm::vec3(0, 1, 0));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        base.Draw(lightingShader);

        // Bancas
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(15.6f, 0.0f, -12.7f));
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 1, 0));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        Banca.Draw(lightingShader);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(37.6f, 0.0f, -15.7f));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        Banca.Draw(lightingShader);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(60.6f, 0.0f, -10.7f));
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 1, 0));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        Banca.Draw(lightingShader);

        // 7) Cuadro de círculos concéntricos CON ANIMACIÓN DE ZOOM POR LUZ
        glm::vec3 cuadroPos(7.4f, 5.3f, -18.5f);

        // Detectar si alguna luz está cerca (dentro del rango de iluminación efectiva)
        bool isIlluminated = false;
        float minDist = std::numeric_limits<float>::max();

        for (int i = 0; i < usedLights; ++i) {
            float dist = glm::length(lightPosArr[i] - cuadroPos);
            minDist = std::min(minDist, dist);

            // Si hay una luz a menos de 8 unidades, el cuadro está "iluminado"
            if (dist < 8.0f) {
                isIlluminated = true;
                break;
            }
        }

        // Animar escala basado en iluminación (transición suave)
        float targetScale = isIlluminated ? 1.0f : 0.96f;
        float scaleSpeed = 1.5f;

        if (circuloMedioScale < targetScale) {
            circuloMedioScale += scaleSpeed * deltaTime;
            if (circuloMedioScale > targetScale) circuloMedioScale = targetScale;
        }
        else if (circuloMedioScale > targetScale) {
            circuloMedioScale -= scaleSpeed * deltaTime;
            if (circuloMedioScale < targetScale) circuloMedioScale = targetScale;
        }

        // Efecto de "respiración" sutil adicional
        float breathe = 1.0f + (isIlluminated ? 0.015f * sinf(currentFrame * 2.0f) : 0.0f);

        model = glm::mat4(1.0f);
        model = glm::translate(model, cuadroPos);
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 0, 1));
        model = glm::scale(model, glm::vec3(4.5f * circuloMedioScale * breathe,
            5.5f * circuloMedioScale * breathe,
            1.5f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        CirculoMedio.Draw(lightingShader);

        // Negro
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(40.6f, 5.0f, -25.7f));
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 0, 1));
        model = glm::scale(model, glm::vec3(4.5f, 5.5f, 1.5f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        CuadroNegro.Draw(lightingShader);

        // 9) Cuadro 4 círculos — giro CONTINUO
        float angleFull = currentFrame * 1.2f;
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(45.6f, 9.5f, -6.0f));
        model = glm::rotate(model, angleFull, glm::vec3(0, 0, 1));
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 1, 0));
        model = glm::scale(model, glm::vec3(4.5f, 4.5f, 1.5f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        CuatroCirculos.Draw(lightingShader);

        // 8) Cuadro de onda
        float wave = sinf(currentFrame * 2.3f) * glm::radians(10.0f);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(25.6f, 3.5f, -6.0f));
        model = glm::rotate(model, wave, glm::vec3(1, 0, 0));
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 1, 0));
        model = glm::scale(model, glm::vec3(5.5f, 2.5f, 1.5f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        CuadroOnda.Draw(lightingShader);

        // Otros cuadros
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(25.6f, 5.0f, -25.7f));
        model = glm::scale(model, glm::vec3(1.5f, 2.5f, 1.5f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        CuadroVerde.Draw(lightingShader);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(7.4f, 3.3f, -12.0f));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 0, 1));
        model = glm::scale(model, glm::vec3(1.5f, 2.5f, 1.5f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        CuadroEspiral.Draw(lightingShader);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(57.6f, 5.0f, -25.7f));
        model = glm::scale(model, glm::vec3(1.5f, 2.5f, 1.5f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        CuadroSol.Draw(lightingShader);

        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}

// ===================== Controles =====================
void DoMovement() {
    if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP])    camera.ProcessKeyboard(FORWARD, deltaTime);
    if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN])  camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT])  camera.ProcessKeyboard(LEFT, deltaTime);
    if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT]) camera.ProcessKeyboard(RIGHT, deltaTime);

    // --- Ajustar caída de cada riel seleccionado ---
    const float prismSpeed = 2.0f;
    bool changedRect = false;
    if (keys[GLFW_KEY_R]) { rectYDrop[selReflector] -= prismSpeed * deltaTime; changedRect = true; }
    if (keys[GLFW_KEY_F]) { rectYDrop[selReflector] += prismSpeed * deltaTime; changedRect = true; }
    if (changedRect) {
        rectYDrop[selReflector] = glm::clamp(rectYDrop[selReflector], 0.0f, 5.0f);
        SetupStaticLights();
    }

    // --- Candil en vivo ---
    const float liftSpeed = 5.0f, scaleSpeed = 2.0f, rotSpeed = 60.0f;
    if (keys[GLFW_KEY_B]) SubirBase += liftSpeed * deltaTime;
    if (keys[GLFW_KEY_N]) SubirBase -= liftSpeed * deltaTime;
    if (keys[GLFW_KEY_V]) escalarCandiles += scaleSpeed * deltaTime;
    if (keys[GLFW_KEY_C]) escalarCandiles -= scaleSpeed * deltaTime;
    if (keys[GLFW_KEY_Z]) girarCandiles += rotSpeed * deltaTime / 45.0f;
    if (keys[GLFW_KEY_X]) girarCandiles -= rotSpeed * deltaTime / 45.0f;

    SubirBase = glm::clamp(SubirBase, 0.0f, 12.0f);
    escalarCandiles = glm::clamp(escalarCandiles, 0.0f, 6.0f);
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) glfwSetWindowShouldClose(window, GL_TRUE);

    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS)   keys[key] = true;
        else if (action == GLFW_RELEASE) keys[key] = false;
    }

    if (action == GLFW_PRESS) {
        // Selección de riel para R/F
        if (key == GLFW_KEY_F1) selReflector = 0;
        if (key == GLFW_KEY_F2) selReflector = 1;
        if (key == GLFW_KEY_F3) selReflector = 2;
        if (key == GLFW_KEY_F4) selReflector = 3;
        if (key == GLFW_KEY_F5) selReflector = 4;

        // Modo oscuro
        if (key == GLFW_KEY_M) {
            hallwayOnlyMode = !hallwayOnlyMode;
            std::cout << "[LIGHTS] Modo nocturno "
                << (hallwayOnlyMode ? "ACTIVADO (luces apagadas)\n"
                    : "DESACTIVADO (luces normales)\n");
            SetupStaticLights();
        }
        }

        // PÁJAROS: L
        if (key == GLFW_KEY_L) {
            PlaySound(TEXT("sonidos/volar.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT | SND_NOSTOP);
            if (!pajarosanim) { pajarosanim = true; mvPajaro = true; }
        }

        // CANDIL: O guarda, K reproduce
        if (key == GLFW_KEY_O) {
            CandilFrame f{ SubirBase, girarCandiles, escalarCandiles };

            std::string cambio = "Snapshot inicial";
            if (!candilKeys.empty()) {
                const CandilFrame& prev = candilKeys.back();
                std::vector<std::string> msgs;
                if (f.subir > prev.subir)   msgs.push_back("Subir base"); else if (f.subir < prev.subir)   msgs.push_back("Bajar base");
                if (f.escalar > prev.escalar) msgs.push_back("Expandir");   else if (f.escalar < prev.escalar) msgs.push_back("Contraer");
                if (f.girar > prev.girar)   msgs.push_back("Girar +");    else if (f.girar < prev.girar)   msgs.push_back("Girar -");
                cambio.clear();
                for (size_t i = 0; i < msgs.size(); ++i) { cambio += (i ? ", " : "") + msgs[i]; }
                if (cambio.empty()) cambio = "Sin cambio";
            }

            candilKeys.push_back(f);
            std::cout << std::fixed << std::setprecision(2)
                << "[KF] Guardado (" << candilKeys.size() - 1 << "→" << candilKeys.size()
                << "): " << cambio
                << " | subir=" << f.subir
                << " girar=" << f.girar
                << " escalar=" << f.escalar << std::endl;

            PlaySound(TEXT("sonidos/pajaros.mp3"), NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT | SND_NOSTOP);
        }
        if (key == GLFW_KEY_K) {
            if (candilKeys.size() >= 2) {
                playCandil = true;
                candilT = 0.0f;
                candilSegActual = 0;
                std::cout << "[KF] Reproducción de keyframes (" << (candilKeys.size() - 1) << " segmentos, dur=" << candilDur << "s)\n";
                if (candilKeys.size() >= 2) {
                    std::cout << "    Keyframe 1: "
                        << "A(" << candilKeys[0].subir << "," << candilKeys[0].girar << "," << candilKeys[0].escalar
                        << ") → B(" << candilKeys[1].subir << "," << candilKeys[1].girar << "," << candilKeys[1].escalar << ")\n";
                }
                PlaySound(TEXT("sonidos/cristal.mp3"), NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT | SND_NOSTOP);
            }
            else {
                std::cout << "[KF] Necesitas al menos 2 keyframes para reproducir.\n";
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

// ===================== Animación existente (pájaros) =====================
void animacion() {
    if (pajarosanim) {
        tPajaro += 0.005f;
        if (tPajaro >= 1.0f) { tPajaro = 1.0f; pajarosanim = false; }

        float angle = tPajaro * 2.0f * M_PI_;
        float theta = angle;

        P1PAj = P1Base + glm::vec3(-12.0f * cosf(angle) + 12.0f, 0.5f * sinf(theta), 2.0f * sinf(angle));
        P2PAj = P2Base + glm::vec3(-8.0f * cosf(angle) + 8.0f, 0.3f * sinf(theta), 3.0f * sinf(angle));
        P3PAj = P3Base + glm::vec3(-8.0f * cosf(angle) + 8.0f, 2.3f * sinf(angle), 0.5f * sinf(angle));
        P4PAj = P4Base + glm::vec3(-2.0f * cosf(angle) + 2.0f, 2.0f * sinf(theta), 0.5f * sinf(angle));

        rotarCuerpo1 -= 0.02f;
        if (ALASMV) { if (rotaralas <= 1) rotaralas += 0.1f; else ALASMV = false; }
        else { if (rotaralas > 0) rotaralas -= 0.1f; else ALASMV = true; }
    }
    else {
        tPajaro = 0.0f;
        P1PAj = P1Base; P2PAj = P2Base; P3PAj = P3Base; P4PAj = P4Base;
        rotaralas = 0; rotarCuerpo1 = 1.0f;
    }
}

// ===================== Reproductor de keyframes (solo candil) =====================
static float Lerp(float a, float b, float t) { return a + (b - a) * t; }

void updateAnim(float dt) {
    if (!playCandil) return;
    if (candilKeys.size() < 2) { playCandil = false; return; }

    candilT += dt;
    float T = std::min(candilT / candilDur, 1.0f);

    size_t segs = candilKeys.size() - 1;
    float segLen = 1.0f / float(segs);
    size_t i = std::min<size_t>((size_t)std::floor(T / segLen), segs - 1);
    float localT = (T - i * segLen) / segLen;

    if (i != candilSegActual) {
        candilSegActual = i;
        std::cout << "    Keyframe " << (i + 1) << " corriendo: "
            << "A(" << candilKeys[i].subir << "," << candilKeys[i].girar << "," << candilKeys[i].escalar
            << ") → B(" << candilKeys[i + 1].subir << "," << candilKeys[i + 1].girar << "," << candilKeys[i + 1].escalar << ")\n";
    }

    const CandilFrame& A = candilKeys[i];
    const CandilFrame& B = candilKeys[i + 1];

    SubirBase = Lerp(A.subir, B.subir, localT);
    girarCandiles = Lerp(A.girar, B.girar, localT);
    escalarCandiles = Lerp(A.escalar, B.escalar, localT);

    if (T >= 1.0f) {
        playCandil = false;
        PlaySound(NULL, 0, 0);
        std::cout << "[KF] Reproducción finalizada\n";
    }
}

// ===================== Malla de luces por riel  =====================
void SetupStaticLights() {
    usedLights = 0;
    if (hallwayOnlyMode) {
        std::cout << "[LIGHTS] Modo nocturno: 0 luces generadas\n";
        return;
    }

    auto addPoint = [&](const glm::vec3& pos, const glm::vec3& col = glm::vec3(0.95f, 0.92f, 0.85f)) {
        if (usedLights >= MAX_POINT_LIGHTS) return;
        lightPosArr[usedLights] = pos;
        lightColorArr[usedLights] = col;
        ++usedLights;
        };

    for (int r = 0; r < NUM_RECTS; ++r) {
        const RectData& R = reflectorRects[r];

        float y = R.center.y - rectYDrop[r];

        float x0 = R.center.x - R.halfX;
        float x1 = R.center.x + R.halfX;
        float z0 = R.center.z - (R.halfZ - lanePaddingZ);
        float z1 = R.center.z + (R.halfZ - lanePaddingZ);

        for (int l = 0; l < lanesPerRail; ++l) {
            float tLane = (lanesPerRail == 1) ? 0.5f : (float)l / (lanesPerRail - 1);
            float z = z0 + (z1 - z0) * tLane;

            for (int b = 0; b < bulbsPerLane; ++b) {
                float tBulb = (bulbsPerLane == 1) ? 0.5f : (float)b / (bulbsPerLane - 1);
                float x = x0 + (x1 - x0) * tBulb;

                glm::vec3 col = glm::vec3(0.95f, 0.92f, 0.85f);
                addPoint(glm::vec3(x, y, z), col);
            }
        }
    }

    if (usedLights >= MAX_POINT_LIGHTS) {
        std::cout << "[WARN] Se alcanzó MAX_POINT_LIGHTS=" << MAX_POINT_LIGHTS << "\n";
    }

    std::cout << "[LIGHTS] Luces generadas: " << usedLights
        << " a Y=" << reflectorRects[0].center.y << "\n";
}