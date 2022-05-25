// Archivo: Main.cpp
// Autor: Néstor I. Martínez Ostoa
// Descripción: Recreación de un espacio en OpenGL - Proyecto Final
// Materia: Computación Gráfica e Interacción Humano Computadora - FI, UNAM
// Fecha: Mayo 2022

// =============================================================================
// Libraries
// =============================================================================
#include <string>
#include <set>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "SOIL2/SOIL2.h"
#include "stb_image.h"

// GL includes
#include "Shader.h"
#include "Camera.h"
#include "Model.h"

// GLM Mathemtics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Properties
const GLuint WIDTH = 800, HEIGHT = 600;
int SCREEN_WIDTH, SCREEN_HEIGHT;

// =============================================================================
// Function prototypes
// =============================================================================
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow* window, double xPos, double yPos);
void DoMovement();
void wardrobe_lamp_animation();
void toggle_door(float& degrees, bool& open);
void open_wardrobe_drawers(vector<int> drawers_ids, vector<float>& drawers_z_pos);
void close_wardrobe_drawers(vector<float>& drawers_z_pos);
vector<int> generate_wardrobe_drawers_ids();
void saveFrame(void);
void resetElements(void);
void interpolation(void);


// Camera
Camera camera(glm::vec3(7.0f, 4.0f, 25.0f));
bool keys[1024];
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;
float rot = 0.0f;
bool anim = false;
bool opening = true;


// =============================================================================
// Variables
// =============================================================================

// Outside door
float outside_door_degrees = 0.0f, door_degrees = 0.0f;
bool outside_door_open = true, door_open = true;

// Plant + Leaves variables
float leaves_time;
bool anim_leaves = false, is_drawer_open = false;

// Wardrobe drawers 
float drawer_x_pos = 8.15f;
bool wd_drawers_are_open = false; // wd -> wardrobe
float wd_init_z = -9.33;
float wd_final_z = -8.83;
vector<float> wardrobe_drawers_z = { wd_init_z, wd_init_z, wd_init_z, wd_init_z, wd_init_z };

// Wardrobe lamp
float delta_mov = 0.2f;
bool lamp_in_wardrobe = true;
glm::vec3 PosIni(3.5f, 2.1f, -9.4f);
bool active;
float posX = PosIni.x, posY = PosIni.y, posZ = PosIni.z;
float lamp_wardrobe_degrees = 0.0f;

#define MAX_FRAMES 9
int i_max_steps = 10;
int i_curr_steps = 0;
typedef struct _frame {
    float posX;		
    float posY;		
    float posZ;		
    float incX;		
    float incY;		
    float incZ;		

} FRAME;

FRAME KeyFrame[MAX_FRAMES];
int FrameIndex = 0;			
bool play = false;
int playIndex = 0;

int main() {
    glfwInit();

    // Set all the required options for GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Create a GLFWwindow object that we can use for GLFW's functions
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "315618648 - Proyecto Final", nullptr, nullptr);

    if (nullptr == window) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(window);
    glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);

    // Set the required callback functions
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetCursorPosCallback(window, MouseCallback);

    // GLFW Options
    glewExperimental = GL_TRUE;
    if (GLEW_OK != glewInit()) {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return EXIT_FAILURE;
    }

    // Define the viewport dimensions
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    // OpenGL options
    glEnable(GL_DEPTH_TEST);

    // Setup and compile our shaders
    Shader lightingShader("Shaders/lighting.vs", "Shaders/lighting.frag");
    Shader shader("Shaders/modelLoading.vs", "Shaders/modelLoading.frag");
    Shader PlantAnim("Shaders/anim.vs", "Shaders/anim.frag");

    // =========================================================================
    // Model loading
    // =========================================================================
    Model house((char*)"Models/House/Facade/house.obj");
    Model world_floor((char*)"Models/World/world_floor.obj");
    Model tiled_floor((char*)"Models/World/Tiled_floor/tiled_floor.obj");
    Model garage_floor((char*)"Models/World/Garage_floor/garage_floor.obj");
    Model cone((char*)"Models/Extra/cone.obj");
    Model outside_door((char*)"Models/House/Outside_Door/outside_door.obj");
    Model door((char*)"Models/Room/Door/door.obj");
    Model armchair((char*)"Models/Room/Armchair/armchair.obj");
    Model bed((char*)"Models/Room/Bed/bed.obj");
    Model furniture((char*)"Models/Room/BedsideFurniture/furniture.obj");
    Model drawer((char*)"Models/Room/BedsideFurniture/Drawer/drawer.obj");
    Model lamp((char*)"Models/Room/Lamp/lamp.obj");
    Model plant((char*)"Models/Room/Plant/plant.obj");
    Model leaf1((char*)"Models/Room/Plant/Leaves/L1.obj");
    Model leaf2((char*)"Models/Room/Plant/Leaves/L2.obj");
    Model leaf3((char*)"Models/Room/Plant/Leaves/L3.obj");
    Model leaf4((char*)"Models/Room/Plant/Leaves/L4.obj");
    Model leaf5((char*)"Models/Room/Plant/Leaves/L5.obj");
    Model wardrobe((char*)"Models/Room/Wardrobe/wardrobe.obj");
    Model small_drawer_left((char*)"Models/Room/Wardrobe/Drawers/Small/small_drawer.obj");
    Model small_drawer_right((char*)"Models/Room/Wardrobe/Drawers/Small/small_drawer.obj");
    Model large_drawer_top((char*)"Models/Room/Wardrobe/Drawers/Large/large_drawer.obj");
    Model large_drawer_mid((char*)"Models/Room/Wardrobe/Drawers/Large/large_drawer.obj");
    Model large_drawer_bottom((char*)"Models/Room/Wardrobe/Drawers/Large/large_drawer.obj");
    Model wardrobe_lamp((char*)"Models/Room/Wardrobe/Lamp/wardrobe_lamp.obj");
    Model mirror((char*)"Models/Room/Mirror/mirror.obj");
    Model mustard_painting((char*)"Models/Room/Paintings/Mustard/mustard_painting.obj");
    Model lime_painting((char*)"Models/Room/Paintings/Lime/lime_painting.obj");
    Model lemon_painting((char*)"Models/Room/Paintings/Lemon/lemon.obj");
    Model window_((char*)"Models/Room/Window/window.obj");
    Model white_armchair((char*)"Models/LivingRoom/WhiteArmchair/white_armchair.obj");
    Model white_furniture((char*)"Models/LivingRoom/WhiteFurniture/white_furniture.obj");
    Model brown_furniture((char*)"Models/LivingRoom/BrownFurniture/brown_furniture.obj");
    Model cactus((char*)"Models/LivingRoom/Cactus/cactus.obj");
    Model living_room_painting ((char*)"Models/LivingRoom/Painting/painting.obj");
    Model living_room_frame((char*)"Models/LivingRoom/WallFrame/wall_frame.obj");
    Model tapestry((char*)"Models/LivingRoom/Tapestry/tapestry.obj");
    Model tv((char*)"Models/LivingRoom/TV/tv.obj");
    Model wall_clock((char*)"Models/LivingRoom/WallClock/wall_clock.obj");

    glm::mat4 projection = glm::perspective(camera.GetZoom(), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);

    //KeyFrames initialization
    for (int i = 0; i < MAX_FRAMES; i++) {
        KeyFrame[i].posX = 0;
        KeyFrame[i].incX = 0;
        KeyFrame[i].incY = 0;
        KeyFrame[i].incZ = 0;
    }

    // =========================================================================
    // Main Loop
    // =========================================================================
    while (!glfwWindowShouldClose(window)) {
        // Set frame time
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Check and call events
        glfwPollEvents();
        DoMovement();
        wardrobe_lamp_animation();

        // Clear the colorbuffer
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.Use();

        glm::mat4 view = camera.GetViewMatrix();
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));

        // Draw the loaded model
        glm::mat4 model(1);
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        house.Draw(shader);

        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(-3.55, 1.25f, 6.9f));
        model = glm::scale(model, glm::vec3(1.1, 1.4, 1));
        model = glm::rotate(model, glm::radians(outside_door_degrees), glm::vec3(0, 1, 0));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        outside_door.Draw(shader);

        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(-2.85, 0.05, 9.25f));
        model = glm::scale(model, glm::vec3(0.7, 1, 1));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        tiled_floor.Draw(shader);

        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(7.45, 0.05, 4.5f));
        model = glm::scale(model, glm::vec3(1.12, 1, 1.1));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        garage_floor.Draw(shader);
        
        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(0, -0.1, 0));
        model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        world_floor.Draw(shader);

        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(7.65, 0.1, -6));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
        model = glm::scale(model, glm::vec3(1.35, 1.25, 1.25));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        bed.Draw(shader);

        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(2.5, 0.15, -4.5));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        armchair.Draw(shader);


        // ----------------------------------------------
        // Sala de estar (segundo cuarto)
        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(-4.3, 0.1, -5));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        white_armchair.Draw(shader);

        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(-4.3, 0.1, -1.7));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        cactus.Draw(shader);

        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(-5.35, 2.3, -5));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        living_room_painting.Draw(shader);

        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(-3.0, 0.1, -5));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        tapestry.Draw(shader);

        
        // ----------------------------------------------

        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(2.2f, 1.25f, -2.2f));
        model = glm::scale(model, glm::vec3(1.0f, 1.25, 1));
        model = glm::rotate(model, glm::radians(door_degrees), glm::vec3(0, 1, 0));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        door.Draw(shader);

        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(3, 0.13, -9.4));
        model = glm::scale(model, glm::vec3(1.85f, 1.5f, 1.5f));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        wardrobe.Draw(shader);

        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(posX, posY, posZ));
        model = glm::scale(model, glm::vec3(1.85f, 1.5f, 1.5f));
        model = glm::rotate(model, glm::radians(lamp_wardrobe_degrees), glm::vec3(0, 0, 1));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        wardrobe_lamp.Draw(shader);

        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(2.53, 1.542, wardrobe_drawers_z[0]));
        model = glm::scale(model, glm::vec3(1.97f, 1.53f, 1.5f));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        small_drawer_left.Draw(shader);

        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(3.5, 1.542, wardrobe_drawers_z[1]));
        model = glm::scale(model, glm::vec3(1.97f, 1.53f, 1.5f));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        small_drawer_right.Draw(shader);

        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(3.0, 1.15, wardrobe_drawers_z[2]));
        model = glm::scale(model, glm::vec3(1.97f, 1.65f, 1.5f));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        large_drawer_top.Draw(shader);

        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(3.0, 0.75, wardrobe_drawers_z[3]));
        model = glm::scale(model, glm::vec3(1.97f, 1.65f, 1.5f));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        large_drawer_mid.Draw(shader);

        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(3.0, 0.35, wardrobe_drawers_z[4]));
        model = glm::scale(model, glm::vec3(1.97f, 1.65f, 1.5f));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        large_drawer_bottom.Draw(shader);

        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(8.25, 0.1, -3.15));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        furniture.Draw(shader);

        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(drawer_x_pos, 0.94, -3.13));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0, 1, 0));
        model = glm::scale(model, glm::vec3(0.98, 1, 1));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        drawer.Draw(shader);

        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(8.8, 0.1, -4.25));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0, 1, 0));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        lamp.Draw(shader);

        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(8.5, 0.1, -8.5f));
        model = glm::scale(model, glm::vec3(2.5, 1.85, 2.0f));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        plant.Draw(shader);

        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(3, 2.25, -9.85));
        model = glm::scale(model, glm::vec3(2, 1.5, 1));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        mirror.Draw(shader);

        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(5.25, 1.5, -9.85));
        model = glm::scale(model, glm::vec3(2.125, 1.75, 1));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        window_.Draw(shader);

        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(7.25, 2.4, -9.85));
        model = glm::scale(model, glm::vec3(2, 2, 1));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        mustard_painting.Draw(shader);

        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(8.4, 3.2, -9.85));
        model = glm::scale(model, glm::vec3(1.25, 1.5, 1));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        lemon_painting.Draw(shader);

        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(8.4, 2.5, -9.85));
        model = glm::scale(model, glm::vec3(1.25, 1.5, 1));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        lime_painting.Draw(shader);

        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(8.4, 1.9, -9.85));
        model = glm::scale(model, glm::vec3(1.25, 1.5, 1));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        lemon_painting.Draw(shader);

        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(7.1, 1.9, -9.85));
        model = glm::scale(model, glm::vec3(1.5, 1.5, 1));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        lime_painting.Draw(shader);

        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(9.3, 1.5, -5.75));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0, 1, 0));
        model = glm::scale(model, glm::vec3(2.125, 1.75, 1));        
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        window_.Draw(shader);

        GLint modelLoc = glGetUniformLocation(lightingShader.Program, "model");
        GLint viewLoc = glGetUniformLocation(lightingShader.Program, "view");
        GLint projLoc = glGetUniformLocation(lightingShader.Program, "projection");

        PlantAnim.Use();
        leaves_time = anim_leaves ? glfwGetTime() : 0.0f;
        modelLoc = glGetUniformLocation(PlantAnim.Program, "model");
        viewLoc = glGetUniformLocation(PlantAnim.Program, "view");
        projLoc = glGetUniformLocation(PlantAnim.Program, "projection");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(8.5, -0.3, -8.7f));
        model = glm::scale(model, glm::vec3(1.5, 2.25, 1.5));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniform1f(glGetUniformLocation(PlantAnim.Program, "time"), leaves_time);
        leaf1.Draw(PlantAnim);

        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(8.5, -0.3, -8.6f));
        model = glm::scale(model, glm::vec3(1.5, 2.25, 1.5));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniform1f(glGetUniformLocation(PlantAnim.Program, "time"), leaves_time);
        leaf2.Draw(PlantAnim);

        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(8.5, -0.3, -8.5f));
        model = glm::scale(model, glm::vec3(1.5, 2.25, 1.5));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniform1f(glGetUniformLocation(PlantAnim.Program, "time"), leaves_time);
        leaf3.Draw(PlantAnim);

        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(8.5, -0.3, -8.4f));
        model = glm::scale(model, glm::vec3(1.5, 2.25, 1.5));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniform1f(glGetUniformLocation(PlantAnim.Program, "time"), leaves_time);
        leaf4.Draw(PlantAnim);

        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(8.5, -0.3, -8.3f));
        model = glm::scale(model, glm::vec3(1.5, 2.25, 1.5));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniform1f(glGetUniformLocation(PlantAnim.Program, "time"), leaves_time);
        leaf5.Draw(PlantAnim);

        glfwSwapBuffers(window);
    }
    glfwTerminate();
    return 0;
}

// =============================================================================
// KeyFrame functions
// =============================================================================
void saveFrame(void) {
    printf("Frameindex %d\n", FrameIndex);

    KeyFrame[FrameIndex].posX = posX;
    KeyFrame[FrameIndex].posY = posY;
    KeyFrame[FrameIndex].posZ = posZ;

    FrameIndex++;
}

void resetElements(void) {
    lamp_wardrobe_degrees = 0.0f;
    posX = KeyFrame[0].posX;
    posY = KeyFrame[0].posY;
    posZ = KeyFrame[0].posZ;
}

void interpolation(void) {
    if (playIndex == 2) {
        lamp_wardrobe_degrees = -30.0f;
    } else if (playIndex == 4) {
        lamp_wardrobe_degrees = -60.0f;
    }
    else if (playIndex == 5) {
        lamp_wardrobe_degrees = -90.0f;
    }
    else if (playIndex == 6) {
        lamp_wardrobe_degrees = -120.0f;
    }

    KeyFrame[playIndex].incX = (KeyFrame[playIndex + 1].posX - KeyFrame[playIndex].posX) / i_max_steps;
    KeyFrame[playIndex].incY = (KeyFrame[playIndex + 1].posY - KeyFrame[playIndex].posY) / i_max_steps;
    KeyFrame[playIndex].incZ = (KeyFrame[playIndex + 1].posZ - KeyFrame[playIndex].posZ) / i_max_steps;
}

// =============================================================================
// Movement functions
// =============================================================================

void DoMovement() { // Moves/alters the camera positions based on user input   

    // Camera movement
    if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP])      camera.ProcessKeyboard(FORWARD, deltaTime);
    if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN])    camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT])    camera.ProcessKeyboard(LEFT, deltaTime);
    if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT])   camera.ProcessKeyboard(RIGHT, deltaTime);

    // Anim
    float delta = 0.02f;
    if (anim) {
        rot = (opening) ? rot + delta : rot - delta;
        if (rot >= 33.0f) opening = false;
        if (rot <= -33.0f) opening = true;
    }
}


// =============================================================================
// KeyCallback helper functions
// =============================================================================
void toggle_door(float& degrees, bool& to_open) {
    if (to_open) {
        while (degrees < 90.0f) degrees += 0.1f;
        degrees = 90.0f;
        to_open = false;
    }
    else {
        while (degrees > 0.0f) degrees -= 0.1;
        degrees = 0.0f;
        to_open = true;
    }
}

vector<int> generate_wardrobe_drawers_ids() {
    vector<int> ids;

    srand(time(0));
    int num_drawers_to_open = (rand() % 5);

    if (num_drawers_to_open == 0) num_drawers_to_open++;

    if (num_drawers_to_open == 5) {
        ids = { 0,1,2,3,4 };
        return ids;
    }

    set<int> ids_set;
    for (int i = 0; i < num_drawers_to_open; i++) {
        ids_set.insert(rand() % 5);
    }
    for (auto s : ids_set) {
        ids.push_back(s);
    }
    return ids;
}

void open_wardrobe_drawers(vector<int> drawers_ids, vector<float>& drawers_z_pos) {
    for (auto id : drawers_ids) {
        drawers_z_pos[id] = wd_final_z;
    }
}

void close_wardrobe_drawers(vector<float> & drawers_z_pos) {
    int num_drawers = 5;
    for (int i = 0; i < num_drawers; i++) {
        drawers_z_pos[i] = wd_init_z;
    }
}

void wardrobe_lamp_animation() {
    if (play) {
        if (i_curr_steps >= i_max_steps) { 
            playIndex++;
            if (playIndex > FrameIndex - 2) { 
                printf("termina anim\n");
                playIndex = 0;
                play = false;
            } else {
                i_curr_steps = 0;
                interpolation();                  
            }
        } else {
            //Draw animation
            posX += KeyFrame[playIndex].incX;
            posY += KeyFrame[playIndex].incY;
            posZ += KeyFrame[playIndex].incZ;
            i_curr_steps++;
        }
    }
}

// =============================================================================
// KeyCallback: called when user interacts with keyboard's keys
// =============================================================================
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    if (GLFW_KEY_ESCAPE == key && GLFW_PRESS == action) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    
    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS)           keys[key] = true;
        else if (action == GLFW_RELEASE)    keys[key] = false;
    }

    if (keys[GLFW_KEY_O])   anim = !anim;

    // Exterior door animation
    if (keys[GLFW_KEY_U]) {
        printf("U pressed - degrees:%f - open: %d\n", outside_door_degrees, outside_door_open);
        toggle_door(outside_door_degrees, outside_door_open);
    }

    // Room 1's door animation
    if (keys[GLFW_KEY_I]) {
        printf("I pressed - degrees:%f - open: %d\n", door_degrees, door_open);
        toggle_door(door_degrees, door_open);
    }

    // Plant's leaves animation
    if (keys[GLFW_KEY_J]) {
        printf("J pressed - anim_leaves: %d\n", anim_leaves);
        anim_leaves = !anim_leaves;
    }

    // Bedside furniture's white drawer animation
    if (keys[GLFW_KEY_K]) {
        printf("K pressed - drawer is open: %d\n", is_drawer_open);
        if (is_drawer_open) {
            while (drawer_x_pos < 8.15f) drawer_x_pos += 0.1f;
            is_drawer_open = false;
        } else {
            while (drawer_x_pos > 7.75f) drawer_x_pos -= 0.1f;
            is_drawer_open = true;
        }
    }

    // Wardrobe's drawers animation
    if (keys[GLFW_KEY_L]) {
        printf("L pressed - wardrobe drawers are open: %d\n", wd_drawers_are_open);

        if (wd_drawers_are_open) {
            close_wardrobe_drawers(wardrobe_drawers_z);
            wd_drawers_are_open = false;
        } else {
            auto drawers_ids = generate_wardrobe_drawers_ids();
            open_wardrobe_drawers(drawers_ids, wardrobe_drawers_z);
            wd_drawers_are_open = true;
        }
    }
    
    // Wardrobe's lamp
    if (keys[GLFW_KEY_N]) {
        // Projectile throw simulation
        KeyFrame[0].posX = 3.5f; KeyFrame[0].posY = 2.1f; KeyFrame[0].posZ = -9.4f;
        KeyFrame[1].posX = 3.9f; KeyFrame[1].posY = 2.1f; KeyFrame[1].posZ = -9.4f;
        KeyFrame[2].posX = 4.3f; KeyFrame[2].posY = 2.1f; KeyFrame[2].posZ = -9.4f;
        KeyFrame[3].posX = 4.7f; KeyFrame[3].posY = 1.9f; KeyFrame[3].posZ = -9.4f;
        KeyFrame[4].posX = 5.1f; KeyFrame[4].posY = 1.6f; KeyFrame[4].posZ = -9.4f;
        KeyFrame[5].posX = 5.5f; KeyFrame[5].posY = 1.1f; KeyFrame[5].posZ = -9.4f;
        KeyFrame[6].posX = 5.9f; KeyFrame[6].posY = 0.7f; KeyFrame[6].posZ = -9.4f;
        KeyFrame[7].posX = 6.3f; KeyFrame[7].posY = 0.3f; KeyFrame[7].posZ = -9.4f;
        KeyFrame[8].posX = 6.7f; KeyFrame[8].posY = 0.0f; KeyFrame[8].posZ = -9.4f;
        FrameIndex = 8;

        if (play == false && (FrameIndex > 1)) {
            resetElements();
            interpolation();
            play = true;
            playIndex = 0;
            i_curr_steps = 0;
        } else {
            play = false;
        }
    }
}

// =============================================================================
// MouseCallback: called when user interacts with the mouse
// =============================================================================

void MouseCallback(GLFWwindow* window, double xPos, double yPos) {
    if (firstMouse) {
        lastX = xPos;
        lastY = yPos;
        firstMouse = false;
    }

    GLfloat xOffset = xPos - lastX;
    GLfloat yOffset = lastY - yPos;

    lastX = xPos;
    lastY = yPos;

    camera.ProcessMouseMovement(xOffset, yOffset);
}

