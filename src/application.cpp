#include "application.h"
#include "abstract/disposable.h"
#include "camera.h"
#include "gameobject.h"

#include <glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
using glm::mat4;
using glm::perspective;

#include <stdexcept>
#include <unordered_map>
using std::runtime_error;
using std::unordered_map;

Application* Application::instance;
static GLFWwindow *window;

// Camera-specific variables
static Camera *camera;
static double lastX, lastY;
static unordered_map<int, bool> buttons;
static unordered_map<int, bool> keys;
static mat4 projection;

void cursor_position_callback(GLFWwindow*, double xpos, double ypos)
{
    auto xoffset = xpos - lastX;
    auto yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    if (!buttons[GLFW_MOUSE_BUTTON_RIGHT])
        return;

    camera->ProcessMouse(xoffset, yoffset);
}

void framebuffer_size_callback(GLFWwindow*, int width, int height)
{
    projection = perspective(45.f, (GLfloat)width / (GLfloat)height, .1f, 1000.f);
    glViewport(0, 0, width, height);
}

void key_callback(GLFWwindow*, int key, int, int action, int)
{
    switch (action)
    {
    case GLFW_PRESS:
        keys[key] = true;
        break;
    case GLFW_RELEASE:
        keys[key] = false;
        break;
    }
}

void mouse_button_callback(GLFWwindow*, int button, int action, int)
{
    switch (action)
    {
    case GLFW_PRESS:
        buttons[button] = true;
        break;
    case GLFW_RELEASE:
        buttons[button] = false;
        break;
    }
}

Application::Application(const char *name, int width, int height, float rotateSpeed, float moveSpeed)
    : deltaTime(0.f)
{
    if (instance)
    {
        throw runtime_error("You cannot create more than one instance of an application class");
    }
    instance = this;

    if (!glfwInit())
    {
        throw runtime_error("Failed to initialize GLFW");
    }

    window = glfwCreateWindow(width, height, name, nullptr, nullptr);
    if (!window)
    {
        throw runtime_error("Failed to create GLFW window");
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        throw runtime_error("Failed to initialize GLAD");
    }

    camera = new Camera(rotateSpeed, moveSpeed);

    glfwGetCursorPos(window, &lastX, &lastY);

    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    // Setting up the projection matrix
    framebuffer_size_callback(window, width, height);
}

Application::~Application()
{
    instance = nullptr;

    for (size_t i = 0; i < Disposable::allocated.size(); i++)
    {
        delete Disposable::allocated[i];
    }
    delete camera;

    buttons.clear();
    keys.clear();

    glfwDestroyWindow(window);
    glfwTerminate();
}

const Camera* Application::GetCamera()
{
    return camera;
}

double Application::GetDeltaTime()
{
    return instance->deltaTime;
}

mat4 Application::GetProjectionMatrix()
{
    return projection;
}

int Application::exec()
{
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    glCullFace(GL_FRONT);

    double lastFrame = 0;

    while (!glfwWindowShouldClose(window))
    {
        auto currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glfwPollEvents();

        if (keys[GLFW_KEY_W])
            camera->ProcessKeyboard(Direction::Forward);
        if (keys[GLFW_KEY_S])
            camera->ProcessKeyboard(Direction::Backward);
        if (keys[GLFW_KEY_A])
            camera->ProcessKeyboard(Direction::Left);
        if (keys[GLFW_KEY_D])
            camera->ProcessKeyboard(Direction::Right);

        glClearColor(1.f, 1.f, 1.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for (auto object : GameObject::instances)
        {
            object->Do();
        }

        glfwSwapInterval(0);
        glfwSwapBuffers(window);
    }

    return EXIT_SUCCESS;
}
