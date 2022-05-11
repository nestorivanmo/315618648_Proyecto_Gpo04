// Std. Includes
#include <string>

// GLEW
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>


// Other Libs
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

// Function prototypes
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow* window, double xPos, double yPos);
void DoMovement();
void toggle_door(float& degrees, bool& open);


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
float outside_door_degrees = 0.0f, door_degrees = 0.0f;
float leaves_time;
bool outside_door_open = true, door_open = true;
bool anim_leaves = false, is_drawer_open = false;
float drawer_x_pos = 8.15f;


int main()
{
    // Init GLFW
    glfwInit();
    // Set all the required options for GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Create a GLFWwindow object that we can use for GLFW's functions
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Adaptacion,carga de modelos y camara sintetica", nullptr, nullptr);

    if (nullptr == window)
    {
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
    //glfwSetInputMode( window, GLFW_CURSOR, GLFW_CURSOR_DISABLED );

    // Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
    glewExperimental = GL_TRUE;
    // Initialize GLEW to setup the OpenGL Function pointers
    if (GLEW_OK != glewInit())
    {
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

    // Load models
    Model plant((char*)"Models/Room/Plant/plant.obj");
    Model leaf1((char*)"Models/Room/Plant/Leaves/L1.obj");
    Model leaf2((char*)"Models/Room/Plant/Leaves/L2.obj");
    Model leaf3((char*)"Models/Room/Plant/Leaves/L3.obj");
    Model leaf4((char*)"Models/Room/Plant/Leaves/L4.obj");
    Model leaf5((char*)"Models/Room/Plant/Leaves/L5.obj");
    Model bed((char*)"Models/Room/Bed/bed.obj");
    Model armchair((char*)"Models/Room/Armchair/armchair.obj");
    Model wardrobe((char*)"Models/Room/Wardrobe/wardrobe.obj");
    Model small_drawer_left((char*)"Models/Room/Wardrobe/Drawers/Small/small_drawer.obj");
    Model small_drawer_right((char*)"Models/Room/Wardrobe/Drawers/Small/small_drawer.obj");
    Model large_drawer_left((char*)"Models/Room/Wardrobe/Drawers/Large/large_drawer.obj");
    Model large_drawer_right((char*)"Models/Room/Wardrobe/Drawers/Large/large_drawer.obj");
    Model furniture((char*)"Models/Room/BedsideFurniture/furniture.obj");
    Model drawer((char*)"Models/Room/BedsideFurniture/Drawer/drawer.obj");
    Model lamp((char*)"Models/Room/Lamp/lamp.obj");
    Model mirror((char*)"Models/Room/Mirror/mirror.obj");
    Model mustard_painting((char*)"Models/Room/Paintings/Mustard/mustard_painting.obj");
    Model lime_painting((char*)"Models/Room/Paintings/Lime/lime_painting.obj");
    Model lemon_painting((char*)"Models/Room/Paintings/Lemon/lemon.obj");
    Model window_((char*)"Models/Room/Window/window.obj");
    Model door((char*)"Models/Room/Door/door.obj");

    Model house((char*)"Models/House/Facade/house.obj");
    Model outside_door((char*)"Models/House/Outside_Door/outside_door.obj");
    Model world_floor((char*)"Models/World/world_floor.obj");
    Model tiled_floor((char*)"Models/World/Tiled_floor/tiled_floor.obj");
    Model garage_floor((char*)"Models/World/Garage_floor/garage_floor.obj");

    Model cone((char*)"Models/Extra/cone.obj");

    glm::mat4 projection = glm::perspective(camera.GetZoom(), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);

    // Game loop
    while (!glfwWindowShouldClose(window))
    {
        // Set frame time
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Check and call events
        glfwPollEvents();
        DoMovement();

        // Clear the colorbuffer
        //glClearColor(0.623f, 0.627f, 1.0f, 1.0f);
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
        model = glm::scale(model, glm::vec3(50.0f, 1.0f, 50.0f));
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
        // CONOS --- QUITAR DESPUÉS
        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(0.3, 0.1, -3));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
        model = glm::scale(model, glm::vec3(1.15, 1.1, 1.0f));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        cone.Draw(shader);

        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(0.3, 0.1, -6));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
        model = glm::scale(model, glm::vec3(1.15, 1.1, 1.0f));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        cone.Draw(shader);

        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(-3, 0.1, -3));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
        model = glm::scale(model, glm::vec3(1.15, 1.1, 1.0f));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        cone.Draw(shader);

        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(-3, 0.1, -6));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
        model = glm::scale(model, glm::vec3(1.15, 1.1, 1.0f));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        cone.Draw(shader);
        // ----------------------------------------------

        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(2.2f, 1.25f, -2.2f));
        model = glm::scale(model, glm::vec3(1.0f, 1.25, 1));
        model = glm::rotate(model, glm::radians(door_degrees), glm::vec3(0, 1, 0));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        door.Draw(shader);

        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(3, 0.1, -9.75));
        model = glm::scale(model, glm::vec3(1.85f, 1.5f, 1.5f));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        wardrobe.Draw(shader);

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

        // Swap the bufferss
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}


// Moves/alters the camera positions based on user input
void DoMovement()
{
    // Camera controls
    if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP])
    {
        camera.ProcessKeyboard(FORWARD, deltaTime);
    }

    if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN])
    {
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }

    if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT])
    {
        camera.ProcessKeyboard(LEFT, deltaTime);
    }

    if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT])
    {
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }

    float delta = 0.02f;

    if (anim) {
        rot = (opening) ? rot + delta : rot - delta;
        if (rot >= 33.0f) opening = false;
        if (rot <= -33.0f) opening = true;
    }
}

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

// Is called whenever a key is pressed/released via GLFW
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (GLFW_KEY_ESCAPE == key && GLFW_PRESS == action)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
        {
            keys[key] = true;
        }
        else if (action == GLFW_RELEASE)
        {
            keys[key] = false;
        }
    }


    if (keys[GLFW_KEY_O]) {
        anim = !anim;
    }

    if (keys[GLFW_KEY_U]) {
        printf("U pressed - degrees:%f - open: %d\n", outside_door_degrees, outside_door_open);
        toggle_door(outside_door_degrees, outside_door_open);
    }

    if (keys[GLFW_KEY_I]) {
        printf("I pressed - degrees:%f - open: %d\n", door_degrees, door_open);
        toggle_door(door_degrees, door_open);
    }

    if (keys[GLFW_KEY_J]) {
        printf("J pressed - anim_leaves: %d\n", anim_leaves);
        anim_leaves = !anim_leaves;
    }

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

}

void MouseCallback(GLFWwindow* window, double xPos, double yPos)
{
    if (firstMouse)
    {
        lastX = xPos;
        lastY = yPos;
        firstMouse = false;
    }

    GLfloat xOffset = xPos - lastX;
    GLfloat yOffset = lastY - yPos;  // Reversed since y-coordinates go from bottom to left

    lastX = xPos;
    lastY = yPos;

    camera.ProcessMouseMovement(xOffset, yOffset);
}

