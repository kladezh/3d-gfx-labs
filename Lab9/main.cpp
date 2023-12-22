#include <GLAD/glad.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <GLM/glm.hpp>
#include <GLM/ext.hpp>

#include <iostream>
#include <memory>
#include <vector>
#include <array>
#include <fstream>
#include <string>


struct Vertex
{
    glm::vec4 position; // Позиция вершины
};

struct Transform
{
    glm::mat4 MVP; // Матрица модели-вида-проекции (Model-View-Projection)
};

namespace buffer
{
    enum type
    {
        VERTEX,    // Тип буфера для вершин
        ELEMENT,   // Тип буфера для индексов (элементов)
        TRANSFORM, // Тип буфера для преобразований
        MAX       // Максимальное количество типов буферов
    };
}

std::string title{ "Tessellated Cube" }; // Заголовок окна
GLfloat alpha = 0.0f, beta = 0.0f; // Углы вращения
GLfloat zoom = 4.0f; // Уровень масштабирования

double cursorX; // Позиция X курсора мыши
double cursorY; // Позиция Y курсора мыши


// Function prototypes
void error_callback(int error, const char* description);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void cursor_position_callback(GLFWwindow* window, double x, double y);
void scroll_callback(GLFWwindow* window, double x, double y);

void checkShader(GLuint object);
void checkProgram(GLuint object);
GLuint createShader(std::string filename, GLenum type);
GLuint createProgram(const std::vector<GLuint>& shaders);


// Функция для компиляции и проверки ошибок в шейдере
GLuint CompileShader(GLenum type, const char* source)
{
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetShaderInfoLog(shader, sizeof(infoLog), NULL, infoLog);
        std::cerr << "Shader compilation error:\n" << infoLog << std::endl;
        return 0;
    }

    return shader;
}

// Размеры окна
glm::ivec2 g_windowSize(640, 480);

int main(void)
{
    if (!glfwInit())
    {
        std::cerr << "GLFW initialization failed!" << std::endl;
        return -1;
    }

    glfwSetErrorCallback(error_callback);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window;

    window = glfwCreateWindow(g_windowSize.x, g_windowSize.y, "Hello OpenGL", NULL, NULL);
    if (!window)
    {
        std::cerr << "Window creating failed!" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // Set the required callback functions
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // Инициализация GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "GLAD initialization failed!" << std::endl;
        return -1;
    }


    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    auto vs = createShader("shaders/cube.vert", GL_VERTEX_SHADER);
    auto tcs = createShader("shaders/cube.cont", GL_TESS_CONTROL_SHADER);
    auto tes = createShader("shaders/cube.eval", GL_TESS_EVALUATION_SHADER);
    auto fs = createShader("shaders/cube.frag", GL_FRAGMENT_SHADER);
    auto program = createProgram({ vs, tcs, tes, fs });

    GLuint pipeline = 0;
    glCreateProgramPipelines(1, &pipeline);
    glUseProgramStages(pipeline, GL_VERTEX_SHADER_BIT | GL_TESS_CONTROL_SHADER_BIT | GL_TESS_EVALUATION_SHADER_BIT | GL_FRAGMENT_SHADER_BIT, program);

    static const std::vector<Vertex> vertices = {
        Vertex{ glm::vec4(-1.0f,-1.0f,-1.0f, 1.0f) },
        Vertex{ glm::vec4(1.0f,-1.0f,-1.0f, 1.0f) },
        Vertex{ glm::vec4(1.0f, 1.0f,-1.0f, 1.0f) },
        Vertex{ glm::vec4(-1.0f, 1.0f,-1.0f, 1.0f) },
        Vertex{ glm::vec4(-1.0f,-1.0f, 1.0f, 1.0f) },
        Vertex{ glm::vec4(1.0f,-1.0f, 1.0f, 1.0f) },
        Vertex{ glm::vec4(1.0f, 1.0f, 1.0f, 1.0f) },
        Vertex{ glm::vec4(-1.0f, 1.0f, 1.0f, 1.0f) }
    };

    static const std::vector<GLushort> indices = {
        0, 4, 7, 3,
        5, 1, 2, 6,
        0, 1, 5, 4,
        7, 6, 2, 3,
        4, 5, 6, 7,
        0, 1, 2, 3
    };

    GLint alignment = 0;
    glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &alignment);
    GLint blockSize = glm::max(GLint(sizeof(Transform)), alignment);

    std::array<GLuint, buffer::MAX> buffers;
    glCreateBuffers(buffer::MAX, &buffers[0]);
    glNamedBufferStorage(buffers[buffer::VERTEX], vertices.size() * sizeof(Vertex), vertices.data(), 0);
    glNamedBufferStorage(buffers[buffer::ELEMENT], indices.size() * sizeof(GLushort), indices.data(), 0);
    glNamedBufferStorage(buffers[buffer::TRANSFORM], blockSize, nullptr, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);


    GLuint vao = 0;
    glCreateVertexArrays(1, &vao);

    glVertexArrayAttribBinding(vao, 0, 0);
    glVertexArrayAttribFormat(vao, 0, 4, GL_FLOAT, GL_FALSE, 0);
    glEnableVertexArrayAttrib(vao, 0);

    glVertexArrayVertexBuffer(vao, 0, buffers[buffer::VERTEX], 0, sizeof(Vertex));
    glVertexArrayElementBuffer(vao, buffers[buffer::ELEMENT]);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    while (!glfwWindowShouldClose(window))
    {
        {
            auto transform = static_cast<Transform*>(glMapNamedBufferRange(buffers[buffer::TRANSFORM],
                0, blockSize,
                GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT));

            auto aspectRatio = static_cast<float>(width) / static_cast<float>(height);
            glm::mat4 Projection = glm::perspective(glm::pi<float>() * 0.25f, aspectRatio, 0.1f, 1000.0f);
            glm::mat4 View = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -zoom));
            View = glm::rotate(View, glm::radians(beta), glm::vec3(1.0f, 0.0f, 0.0f));
            View = glm::rotate(View, glm::radians(alpha), glm::vec3(0.0f, 1.0f, 0.0f));
            glm::mat4 Model = glm::mat4(1.0f);

            transform->MVP = Projection * View * Model;

            glUnmapNamedBuffer(buffers[buffer::TRANSFORM]);
        }

        glClearBufferfv(GL_COLOR, 0, &glm::vec4(0.2f, 0.2f, 0.3f, 1.0f)[0]);
        glClearBufferfv(GL_DEPTH, 0, &glm::vec4(1.0f)[0]);

        glBindProgramPipeline(pipeline);
        glBindVertexArray(vao);
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, buffers[buffer::TRANSFORM]);

        // We work with 4 points per patch.
        glPatchParameteri(GL_PATCH_VERTICES, 4);

        glDrawElementsInstancedBaseVertex(GL_PATCHES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_SHORT, nullptr, 1, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteProgramPipelines(1, &pipeline);
    glDeleteProgram(program);
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(buffer::MAX, &buffers[0]);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}


void error_callback(int error, const char* description)
{
    std::cerr << "Error (" << error << "): " << description << "\n";
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

//========================================================================
// Callback function for mouse button events
//========================================================================

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button != GLFW_MOUSE_BUTTON_LEFT)
        return;

    if (action == GLFW_PRESS)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwGetCursorPos(window, &cursorX, &cursorY);
    }
    else
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}


//========================================================================
// Callback function for cursor motion events
//========================================================================

void cursor_position_callback(GLFWwindow* window, double x, double y)
{
    if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED)
    {
        alpha += (GLfloat)(x - cursorX) / 10.f;
        beta += (GLfloat)(y - cursorY) / 10.f;

        cursorX = x;
        cursorY = y;
    }
}


//========================================================================
// Callback function for scroll events
//========================================================================

void scroll_callback(GLFWwindow* window, double x, double y)
{
    zoom -= (float)y / 8.f;
    if (zoom < 0)
        zoom = 0;
}

GLuint createShader(std::string filename, GLenum type)
{
    // read the file in lambda expression
    auto source = [filename]() {
        std::string result;
        std::ifstream stream(filename.data());

        if (!stream.is_open()) {
            std::string str{ filename };
            std::cerr << "Could not open file: " << str << "\n";
            return result;
        }

        stream.seekg(0, std::ios::end);
        result.reserve((size_t)stream.tellg());
        stream.seekg(0, std::ios::beg);

        result.assign(std::istreambuf_iterator<char>{stream},
            std::istreambuf_iterator<char>{});

        return result;
    }();
    auto pSource = source.c_str();

    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &pSource, nullptr);
    glCompileShader(shader);
    checkShader(shader);

    return shader;
}

GLuint createProgram(const std::vector<GLuint>& shaders)
{
    GLuint program = glCreateProgram();
    glProgramParameteri(program, GL_PROGRAM_SEPARABLE, GL_TRUE);

    for (const auto& shader : shaders) {
        glAttachShader(program, shader);
    }

    glLinkProgram(program);
    checkProgram(program);

    for (const auto& shader : shaders) {
        glDetachShader(program, shader);
        glDeleteShader(shader);
    }

    return program;
}

void checkShader(GLuint object)
{
    GLint isCompiled{};
    glGetShaderiv(object, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE)
    {
        GLint maxLength{};
        glGetShaderiv(object, GL_INFO_LOG_LENGTH, &maxLength);
        if (maxLength > 0)
        {
            auto infoLog = std::make_unique<GLchar[]>(maxLength);
            glGetShaderInfoLog(object, maxLength, &maxLength, infoLog.get());
            glDeleteShader(object);
            std::cerr << "Error compiled:\n" << infoLog.get() << "\n";
        }
    }
}

void checkProgram(GLuint object)
{
    GLint isLinked{};
    glGetProgramiv(object, GL_LINK_STATUS, &isLinked);
    if (isLinked == GL_FALSE)
    {
        GLint maxLength{};
        glGetProgramiv(object, GL_INFO_LOG_LENGTH, &maxLength);
        if (maxLength > 0)
        {
            auto infoLog = std::make_unique<GLchar[]>(maxLength);
            glGetProgramInfoLog(object, maxLength, &maxLength, infoLog.get());
            glDeleteProgram(object);
            std::cerr << "Error linking:\n" << infoLog.get() << "\n";
        }
    }
}