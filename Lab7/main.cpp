#include <GLAD/glad.h>
#include <GLFW/glfw3.h>

#include <GLM/glm.hpp>
#include <GLM/mat4x4.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include <cmath>

// вершинный шейдер
const char* vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec3 inPosition;  // Входной атрибут: позиция вершины
        layout (location = 1) in vec3 inNormal;    // Входной атрибут: нормаль вершины

        out vec3 FragPos;  // Позиция вершины в мировых координатах
        out vec3 Normal;    // Нормаль вершины в мировых координатах

        uniform mat4 model;       // Матрица модели
        uniform mat4 view;        // Матрица вида
        uniform mat4 projection;  // Матрица проекции

        void main()
        {
            FragPos = vec3(model * vec4(inPosition, 1.0));  // Пересчитываем позицию вершины в мировых координатах
            Normal = mat3(transpose(inverse(model))) * inNormal;  // Пересчитываем нормаль вершины в мировых координатах
            gl_Position = projection * view * vec4(FragPos, 1.0);  // Рассчитываем конечную позицию вершины
        }
    )";

// фрагментный шейдер
const char* fragmentShaderSource = R"(
        #version 330 core
        in vec3 FragPos;  // Позиция вершины в мировых координатах
        in vec3 Normal;   // Нормаль вершины в мировых координатах

        out vec4 FragColor;  // Выходной цвет фрагмента

        // Структура для хранения свойств материала объекта
        struct Material {
            vec3 ambient;   // Фоновое освещение
            vec3 diffuse;   // Диффузное отражение
            vec3 specular;  // Зеркальное отражение
            float shininess; // Блеск материала
        };

        // Структура для хранения свойств источника света
        struct Light {
            vec3 position;  // Позиция источника света
            vec3 ambient;   // Фоновое освещение от источника света
            vec3 diffuse;   // Диффузное отражение от источника света
            vec3 specular;  // Зеркальное отражение от источника света
        };

        uniform Material material;  // Свойства материала
        uniform Light light;        // Свойства источника света
        uniform vec3 viewPos;       // Позиция камеры

        void main()
        {
            // Рассчитываем составляющую фонового освещения
            vec3 ambient = light.ambient * material.ambient;
    
            // Нормализуем нормаль вершины
            vec3 norm = normalize(Normal);
            // Рассчитываем направление света и диффузную составляющую освещения
            vec3 lightDir = normalize(light.position - FragPos);
            float diff = max(dot(norm, lightDir), 0.0);
            vec3 diffuse = light.diffuse * (diff * material.diffuse);
    
            // Рассчитываем направление взгляда и зеркальную составляющую освещения
            vec3 viewDir = normalize(viewPos - FragPos);
            vec3 reflectDir = reflect(-lightDir, norm);
            float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
            vec3 specular = light.specular * (spec * material.specular);
    
            // Общий результат освещения - сумма фонового, диффузного и зеркального освещения
            vec3 result = ambient + diffuse + specular;
            FragColor = vec4(result, 0.5);  // Устанавливаем цвет фрагмента с прозрачностью
        }
    )";


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


float vertices[] = {
    // Позиции вершин          // Нормали
    -0.5f, -0.5f, -0.5f,       0.0f, 0.0f, -1.0f,
     0.5f, -0.5f, -0.5f,       0.0f, 0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,       0.0f, 0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,       0.0f, 0.0f, -1.0f,
    -0.5f,  0.5f, -0.5f,       0.0f, 0.0f, -1.0f,
    -0.5f, -0.5f, -0.5f,       0.0f, 0.0f, -1.0f,

    // Передняя грань
    -0.5f, -0.5f, 0.5f,        0.0f, 0.0f, 1.0f,
     0.5f, -0.5f, 0.5f,        0.0f, 0.0f, 1.0f,
     0.5f,  0.5f, 0.5f,        0.0f, 0.0f, 1.0f,
     0.5f,  0.5f, 0.5f,        0.0f, 0.0f, 1.0f,
    -0.5f,  0.5f, 0.5f,        0.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, 0.5f,        0.0f, 0.0f, 1.0f,

    // Левая грань
    -0.5f,  0.5f,  0.5f,       -1.0f, 0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,       -1.0f, 0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,       -1.0f, 0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,       -1.0f, 0.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,       -1.0f, 0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,       -1.0f, 0.0f, 0.0f,

    // Правая грань
     0.5f,  0.5f,  0.5f,       1.0f, 0.0f, 0.0f,
     0.5f,  0.5f, -0.5f,       1.0f, 0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,       1.0f, 0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,       1.0f, 0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,       1.0f, 0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,       1.0f, 0.0f, 0.0f,

     // Нижняя грань
     -0.5f, -0.5f, -0.5f,       0.0f, -1.0f, 0.0f,
      0.5f, -0.5f, -0.5f,       0.0f, -1.0f, 0.0f,
      0.5f, -0.5f,  0.5f,       0.0f, -1.0f, 0.0f,
      0.5f, -0.5f,  0.5f,       0.0f, -1.0f, 0.0f,
     -0.5f, -0.5f,  0.5f,       0.0f, -1.0f, 0.0f,
     -0.5f, -0.5f, -0.5f,       0.0f, -1.0f, 0.0f,

     // Верхняя грань
     -0.5f, 0.5f, -0.5f,        0.0f, 1.0f, 0.0f,
      0.5f, 0.5f, -0.5f,        0.0f, 1.0f, 0.0f,
      0.5f, 0.5f,  0.5f,        0.0f, 1.0f, 0.0f,
      0.5f, 0.5f,  0.5f,        0.0f, 1.0f, 0.0f,
     -0.5f, 0.5f,  0.5f,        0.0f, 1.0f, 0.0f,
     -0.5f, 0.5f, -0.5f,        0.0f, 1.0f, 0.0f
};

// Размеры окна
glm::ivec2 g_windowSize(640, 480);

int main(void)
{
    if (!glfwInit()) 
    {
        std::cerr << "GLFW initialization failed!" << std::endl;
        return -1;
    }
        
    GLFWwindow* window;

    // Устанавливаем требуемую версию OpenGL и профиль
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

    // Инициализация GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "GLAD initialization failed!" << std::endl;
        return -1;
    }

    // Выводим информацию о рендерере и версии OpenGL
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;


    // Компиляция шейдеров
    GLuint vertexShader = CompileShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Установка режима рендеринга для прозрачных объектов
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Создание буферов вершин и настройка вершинных атрибутов
    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Атрибуты позиции вершин
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Атрибуты нормали вершин
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Использование шейдерной программы
    glUseProgram(shaderProgram);
    // Получаем uniform-переменные из шейдера
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

    // Устанавливаем свойства материала
    glUniform3f(materialAmbientLoc, 0.0f, 0.1f, 0.06f);
    glUniform3f(materialDiffuseLoc, 0.0f, 0.50980392f, 0.50980392f);
    glUniform3f(materialSpecularLoc, 0.50196078f, 0.50196078f, 0.50196078f);
    glUniform1f(materialShininessLoc, 32.0f);

    // Устанавливаем свойства источника света
    glUniform3f(lightPositionLoc, 1.2f, 1.0f, 2.0f);
    glUniform3f(lightAmbientLoc, 0.2f, 0.2f, 0.2f);
    glUniform3f(lightDiffuseLoc, 0.5f, 0.5f, 0.5f);
    glUniform3f(lightSpecularLoc, 1.0f, 1.0f, 1.0f);

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Устанавливаем матрицы вида и проекции
        glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

        // Устанавливаем uniform-переменные матриц в шейдере
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

        // Отрисовываем куб
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(1.0f, 0.5f, 0.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        // Позиция камеры
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
