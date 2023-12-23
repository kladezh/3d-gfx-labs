#include <GLAD/glad.h>
#include <GLFW/glfw3.h>

#include <GLM/glm.hpp>
#include <GLM/mat4x4.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include <cmath>

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
            FragColor = vec4(result, 0.5);  // ������������� ���� ��������� � �������������
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
     0.5f,  0.5f,  0.5f,       1.0f, 0.0f, 0.0f,
     0.5f,  0.5f, -0.5f,       1.0f, 0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,       1.0f, 0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,       1.0f, 0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,       1.0f, 0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,       1.0f, 0.0f, 0.0f,

     // ������ �����
     -0.5f, -0.5f, -0.5f,       0.0f, -1.0f, 0.0f,
      0.5f, -0.5f, -0.5f,       0.0f, -1.0f, 0.0f,
      0.5f, -0.5f,  0.5f,       0.0f, -1.0f, 0.0f,
      0.5f, -0.5f,  0.5f,       0.0f, -1.0f, 0.0f,
     -0.5f, -0.5f,  0.5f,       0.0f, -1.0f, 0.0f,
     -0.5f, -0.5f, -0.5f,       0.0f, -1.0f, 0.0f,

     // ������� �����
     -0.5f, 0.5f, -0.5f,        0.0f, 1.0f, 0.0f,
      0.5f, 0.5f, -0.5f,        0.0f, 1.0f, 0.0f,
      0.5f, 0.5f,  0.5f,        0.0f, 1.0f, 0.0f,
      0.5f, 0.5f,  0.5f,        0.0f, 1.0f, 0.0f,
     -0.5f, 0.5f,  0.5f,        0.0f, 1.0f, 0.0f,
     -0.5f, 0.5f, -0.5f,        0.0f, 1.0f, 0.0f
};

// ������� ����
glm::ivec2 g_windowSize(640, 480);

int main(void)
{
    if (!glfwInit()) 
    {
        std::cerr << "GLFW initialization failed!" << std::endl;
        return -1;
    }
        
    GLFWwindow* window;

    // ������������� ��������� ������ OpenGL � �������
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

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

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // ��������� ������ ���������� ��� ���������� ��������
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // �������� ������� ������ � ��������� ��������� ���������
    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // �������� ������� ������
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // �������� ������� ������
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // ������������� ��������� ���������
    glUseProgram(shaderProgram);
    // �������� uniform-���������� �� �������
    GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
    GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");
    GLuint projectionLoc = glGetUniformLocation(shaderProgram, "projection");
    GLuint materialAmbientLoc = glGetUniformLocation(shaderProgram, "material.ambient");
    GLuint materialDiffuseLoc = glGetUniformLocation(shaderProgram, "material.diffuse");
    GLuint materialSpecularLoc = glGetUniformLocation(shaderProgram, "material.specular");
    GLuint materialShininessLoc = glGetUniformLocation(shaderProgram, "material.shininess");
    GLuint lightPositionLoc = glGetUniformLocation(shaderProgram, "light.position");
    GLuint lightAmbientLoc = glGetUniformLocation(shaderProgram, "light.ambient");
    GLuint lightDiffuseLoc = glGetUniformLocation(shaderProgram, "light.diffuse");
    GLuint lightSpecularLoc = glGetUniformLocation(shaderProgram, "light.specular");
    GLuint viewPosLoc = glGetUniformLocation(shaderProgram, "viewPos");

    // ������������� �������� ���������
    glUniform3f(materialAmbientLoc, 0.0f, 0.1f, 0.06f);
    glUniform3f(materialDiffuseLoc, 0.0f, 0.50980392f, 0.50980392f);
    glUniform3f(materialSpecularLoc, 0.50196078f, 0.50196078f, 0.50196078f);
    glUniform1f(materialShininessLoc, 32.0f);

    // ������������� �������� ��������� �����
    glUniform3f(lightPositionLoc, 1.2f, 1.0f, 2.0f);
    glUniform3f(lightAmbientLoc, 0.2f, 0.2f, 0.2f);
    glUniform3f(lightDiffuseLoc, 0.5f, 0.5f, 0.5f);
    glUniform3f(lightSpecularLoc, 1.0f, 1.0f, 1.0f);

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // ������������� ������� ���� � ��������
        glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

        // ������������� uniform-���������� ������ � �������
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

        // ������������ ���
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(1.0f, 0.5f, 0.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        // ������� ������
        glUniform3f(viewPosLoc, 0.0f, 0.0f, 3.0f);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}
