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


// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
bool keys[1024];
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;
float rot = 0.0f;
bool anim = false;
bool opening = true;

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
    Shader shader("Shaders/modelLoading.vs", "Shaders/modelLoading.frag");

    // Load models
    Model plant((char*)"Models/Room/Plant/plant.obj");
    Model bed((char*)"Models/Room/Bed/bed.obj");
    Model armchair((char*)"Models/Room/Armchair/armchair.obj");
    Model wardrobe((char*)"Models/Room/Wardrobe/wardrobe.obj");
    Model furniture((char*)"Models/Room/BedsideFurniture/furniture.obj");
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
        model = glm::translate(model, glm::vec3(-2.85, 0.1, 6.9f));
        model = glm::scale(model, glm::vec3(1.1, 1.25, 1));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        outside_door.Draw(shader);

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
        model = glm::scale(model, glm::vec3(1.15, 1.1, 1.0f));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        armchair.Draw(shader);

        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(2.67, 0.1, -2.2f));
        model = glm::scale(model, glm::vec3(1.1, 1.25, 1));
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
        model = glm::translate(model, glm::vec3(8.8, 0.1, -4.25));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0, 1, 0));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        lamp.Draw(shader);

        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(8, 0.1, -8.0f));
        model = glm::scale(model, glm::vec3(1.5, 2.25, 1.5));
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

