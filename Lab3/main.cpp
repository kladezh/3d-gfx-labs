#include <GLAD/glad.h>
#include <GLFW/glfw3.h>

#include <GLM/glm.hpp>
#include <GLM/mat4x4.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>

#include <iostream>


// ��������� ������
const char* vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec3 inPosition;  // ������� �������: ������� �������
        layout (location = 1) in vec3 inNormal;    // ������� �������: ������� �������

        out vec3 FragPos;  // ������� ������� � ������� �����������
        out vec3 Normal;    // ������� ������� � ������� �����������

        uniform mat4 model;       // ������� ������
        uniform mat4 view;        // ������� ����
        uniform mat4 projection;  // ������� ��������

        void main()
        {
            FragPos = vec3(model * vec4(inPosition, 1.0));  // ������������� ������� ������� � ������� �����������
            Normal = mat3(transpose(inverse(model))) * inNormal;  // ������������� ������� ������� � ������� �����������
            gl_Position = projection * view * vec4(FragPos, 1.0);  // ������������ �������� ������� �������
        }
    )";

// ����������� ������
const char* fragmentShaderSource = R"(
        #version 330 core
        in vec3 FragPos;  // ������� ������� � ������� �����������
        in vec3 Normal;   // ������� ������� � ������� �����������

        out vec4 FragColor;  // �������� ���� ���������

        // ��������� ��� �������� ������� ��������� �������
        struct Material {
            vec3 ambient;   // ������� ���������
            vec3 diffuse;   // ��������� ���������
            vec3 specular;  // ���������� ���������
            float shininess; // ����� ���������
        };

        // ��������� ��� �������� ������� ��������� �����
        struct Light {
            vec3 position;  // ������� ��������� �����
            vec3 ambient;   // ������� ��������� �� ��������� �����
            vec3 diffuse;   // ��������� ��������� �� ��������� �����
            vec3 specular;  // ���������� ��������� �� ��������� �����
        };

        uniform Material material;  // �������� ���������
        uniform Light light;        // �������� ��������� �����
        uniform vec3 viewPos;       // ������� ������

        void main()
        {
            // ������������ ������������ �������� ���������
            vec3 ambient = light.ambient * material.ambient;
    
            // ����������� ������� �������
            vec3 norm = normalize(Normal);
            // ������������ ����������� ����� � ��������� ������������ ���������
            vec3 lightDir = normalize(light.position - FragPos);
            float diff = max(dot(norm, lightDir), 0.0);
            vec3 diffuse = light.diffuse * (diff * material.diffuse);
    
            // ������������ ����������� ������� � ���������� ������������ ���������
            vec3 viewDir = normalize(viewPos - FragPos);
            vec3 reflectDir = reflect(-lightDir, norm);
            float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
            vec3 specular = light.specular * (spec * material.specular);
    
            // ����� ��������� ��������� - ����� ��������, ���������� � ����������� ���������
            vec3 result = ambient + diffuse + specular;
            FragColor = vec4(result, 1.0);  // ������������� ���� ���������
        }
    )";


// ������� ��� ���������� � �������� ������ � �������
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


float vertices[] = {
    // ������� ������          // �������
    -0.5f, -0.5f, -0.5f,       0.0f, 0.0f, -1.0f,
     0.5f, -0.5f, -0.5f,       0.0f, 0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,       0.0f, 0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,       0.0f, 0.0f, -1.0f,
    -0.5f,  0.5f, -0.5f,       0.0f, 0.0f, -1.0f,
    -0.5f, -0.5f, -0.5f,       0.0f, 0.0f, -1.0f,

    // �������� �����
    -0.5f, -0.5f, 0.5f,        0.0f, 0.0f, 1.0f,
     0.5f, -0.5f, 0.5f,        0.0f, 0.0f, 1.0f,
     0.5f,  0.5f, 0.5f,        0.0f, 0.0f, 1.0f,
     0.5f,  0.5f, 0.5f,        0.0f, 0.0f, 1.0f,
    -0.5f,  0.5f, 0.5f,        0.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, 0.5f,        0.0f, 0.0f, 1.0f,

    // ����� �����
    -0.5f,  0.5f,  0.5f,       -1.0f, 0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,       -1.0f, 0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,       -1.0f, 0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,       -1.0f, 0.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,       -1.0f, 0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,       -1.0f, 0.0f, 0.0f,

    // ������ �����
     0.5f,  0.5f,  0.5f,        1.0f, 0.0f, 0.0f,
     0.5f,  0.5f, -0.5f,        1.0f, 0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,        1.0f, 0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,        1.0f, 0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,        1.0f, 0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,        1.0f, 0.0f, 0.0f,

     // ������ �����
     -0.5f, -0.5f, -0.5f,        0.0f, -1.0f, 0.0f,
      0.5f, -0.5f, -0.5f,        0.0f, -1.0f, 0.0f,
      0.5f, -0.5f,  0.5f,        0.0f, -1.0f, 0.0f,
      0.5f, -0.5f,  0.5f,        0.0f, -1.0f, 0.0f,
     -0.5f, -0.5f,  0.5f,        0.0f, -1.0f, 0.0f,
     -0.5f, -0.5f, -0.5f,        0.0f, -1.0f, 0.0f,

     // ������� �����
     -0.5f,  0.5f, -0.5f,        0.0f, 1.0f, 0.0f,
      0.5f,  0.5f, -0.5f,        0.0f, 1.0f, 0.0f,
      0.5f,  0.5f,  0.5f,        0.0f, 1.0f, 0.0f,
      0.5f,  0.5f,  0.5f,        0.0f, 1.0f, 0.0f,
     -0.5f,  0.5f,  0.5f,        0.0f, 1.0f, 0.0f,
     -0.5f,  0.5f, -0.5f,        0.0f, 1.0f, 0.0f
};

// ������� ������
glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 3.0f);  // ��������� ������� ������
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);    // ������ �����������, � ������� ������� ������
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);         // ������ ����������� "�����" ��� ������

float yaw = -90.0f;  // ���� �������� �� ����������� (yaw) � ��������� ��������� -90 ��������
float pitch = 0.0f;  // ���� �������� �� ��������� (pitch) � ��������� ��������� 0 ��������

const float sensitivity = 0.1f;  // ���������������� ��� ���������� �������

const float cameraSpeed = 0.05f;  // �������� �������� ������

void processInput(GLFWwindow* window) {
    float deltaTime = 0.1f; // ��������� ��� ��� ���������� ��������� 

    // �������� ����� (����� �����������, � ������� ������� ������)
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPosition += cameraSpeed * deltaTime * cameraFront;

    // �������� ����� (������ �����������, � ������� ������� ������)
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPosition -= cameraSpeed * deltaTime * cameraFront;

    // �������� ����� (�� ����������������� ����������� � ������� ����������� � ������� "�����")
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPosition -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed * deltaTime;

    // �������� ������ (�� ����������������� ����������� � ������� ����������� � ������� "�����")
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPosition += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed * deltaTime;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    static float lastX = 400, lastY = 300;  // ��������� ���������� �������
    static bool firstMouse = true;  // ���� ������� �������� ����

    if (firstMouse) {
        lastX = float(xpos);
        lastY = float(ypos);
        firstMouse = false;
    }

    // ��������� ��������� ��������� �������
    float xoffset = float(xpos) - lastX;
    float yoffset = lastY - float(ypos);

    lastX = float(xpos);
    lastY = float(ypos);

    xoffset *= sensitivity;  // ������������ ��������� ��������� �������
    yoffset *= sensitivity;

    yaw += xoffset;  // ��������� ���� �������� �� ����������� (yaw)
    pitch += yoffset;  // ��������� ���� �������� �� ��������� (pitch)

    // ������������ ���� �������� �� ��������� (pitch) ��� �������������� �����������
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    // ��������� ����� ����������� "������" (cameraFront) �� ������ ����� yaw � pitch
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

// ������� ����
glm::ivec2 g_windowSize(1024, 768);

int main(void)
{
    if (!glfwInit())
    {
        std::cerr << "GLFW initialization failed!" << std::endl;
        return -1;
    }

    GLFWwindow* window;


    window = glfwCreateWindow(g_windowSize.x, g_windowSize.y, "Hello OpenGL", NULL, NULL);
    if (!window)
    {
        std::cerr << "Window creating failed!" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // ������������� GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "GLAD initialization failed!" << std::endl;
        return -1;
    }

    // ������� ���������� � ��������� � ������ OpenGL
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;

    // ���������� ��������
    GLuint vertexShader = CompileShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    // ���������� �������� � ��������� ���������
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // �������� � �������� ������ ������
    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // ��������� ���������� ��������� ������
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
    glEnable(GL_DEPTH_TEST);  // �������� ����� ������� ��� ����� �������

    // ������������� callback ��� ��������� �������� ����
    glfwSetCursorPosCallback(window, mouse_callback);
    // ������������� ��������� ��������� �������
    glfwSetCursorPos(window, 400, 300);

    // �������� ���� ���������
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);  // ��������� ����������������� �����

        // ������� ������ �����
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);  // ���������� ��������� ���������

        // ������������� ������� ��������, ���� � ������
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
        glm::mat4 view = glm::lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp);

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

        glm::mat4 model = glm::mat4(1.0f);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

        // ������������� ��������� ����� � ���������
        glUniform3f(glGetUniformLocation(shaderProgram, "light.position"), 1.2f, 1.0f, 2.0f);
        glUniform3f(glGetUniformLocation(shaderProgram, "light.ambient"), 0.2f, 0.2f, 0.2f);
        glUniform3f(glGetUniformLocation(shaderProgram, "light.diffuse"), 0.5f, 0.5f, 0.5f);
        glUniform3f(glGetUniformLocation(shaderProgram, "light.specular"), 1.0f, 1.0f, 1.0f);

        glUniform3f(glGetUniformLocation(shaderProgram, "material.ambient"), 1.0f, 0.5f, 0.31f);
        glUniform3f(glGetUniformLocation(shaderProgram, "material.diffuse"), 1.0f, 0.5f, 0.31f);
        glUniform3f(glGetUniformLocation(shaderProgram, "material.specular"), 0.5f, 0.5f, 0.5f);
        glUniform1f(glGetUniformLocation(shaderProgram, "material.shininess"), 32.0f);

        // ������������� ������� ������
        glUniform3f(glGetUniformLocation(shaderProgram, "viewPos"), cameraPosition.x, cameraPosition.y, cameraPosition.z);

        // ������ ���
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // ����������� �������
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();
    return 0;
}
