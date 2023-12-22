#include <GLAD/glad.h>
#include <GLFW/glfw3.h>

#include <GLM/glm.hpp>
#include <GLM/mat4x4.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>

#include <iostream>


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
            FragColor = vec4(result, 1.0);  // Устанавливаем цвет фрагмента
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
     0.5f,  0.5f,  0.5f,        1.0f, 0.0f, 0.0f,
     0.5f,  0.5f, -0.5f,        1.0f, 0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,        1.0f, 0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,        1.0f, 0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,        1.0f, 0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,        1.0f, 0.0f, 0.0f,

     // Нижняя грань
     -0.5f, -0.5f, -0.5f,        0.0f, -1.0f, 0.0f,
      0.5f, -0.5f, -0.5f,        0.0f, -1.0f, 0.0f,
      0.5f, -0.5f,  0.5f,        0.0f, -1.0f, 0.0f,
      0.5f, -0.5f,  0.5f,        0.0f, -1.0f, 0.0f,
     -0.5f, -0.5f,  0.5f,        0.0f, -1.0f, 0.0f,
     -0.5f, -0.5f, -0.5f,        0.0f, -1.0f, 0.0f,

     // Верхняя грань
     -0.5f,  0.5f, -0.5f,        0.0f, 1.0f, 0.0f,
      0.5f,  0.5f, -0.5f,        0.0f, 1.0f, 0.0f,
      0.5f,  0.5f,  0.5f,        0.0f, 1.0f, 0.0f,
      0.5f,  0.5f,  0.5f,        0.0f, 1.0f, 0.0f,
     -0.5f,  0.5f,  0.5f,        0.0f, 1.0f, 0.0f,
     -0.5f,  0.5f, -0.5f,        0.0f, 1.0f, 0.0f
};

// Позиция камеры
glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 3.0f);  // Начальная позиция камеры
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);    // Вектор направления, в который смотрит камера
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);         // Вектор направления "вверх" для камеры

float yaw = -90.0f;  // Угол поворота по горизонтали (yaw) с начальным значением -90 градусов
float pitch = 0.0f;  // Угол поворота по вертикали (pitch) с начальным значением 0 градусов

const float sensitivity = 0.1f;  // Чувствительность для управления камерой

const float cameraSpeed = 0.05f;  // Скорость движения камеры

void processInput(GLFWwindow* window) {
    float deltaTime = 0.1f; // Временной шаг для управления скоростью 

    // Движение вперёд (вдоль направления, в котором смотрит камера)
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPosition += cameraSpeed * deltaTime * cameraFront;

    // Движение назад (против направления, в котором смотрит камера)
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPosition -= cameraSpeed * deltaTime * cameraFront;

    // Движение влево (по перпендикулярному направлению к вектору направления и вектору "вверх")
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPosition -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed * deltaTime;

    // Движение вправо (по перпендикулярному направлению к вектору направления и вектору "вверх")
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPosition += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed * deltaTime;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    static float lastX = 400, lastY = 300;  // Начальные координаты курсора
    static bool firstMouse = true;  // Флаг первого движения мыши

    if (firstMouse) {
        lastX = float(xpos);
        lastY = float(ypos);
        firstMouse = false;
    }

    // Вычисляем изменение положения курсора
    float xoffset = float(xpos) - lastX;
    float yoffset = lastY - float(ypos);

    lastX = float(xpos);
    lastY = float(ypos);

    xoffset *= sensitivity;  // Масштабируем изменение положения курсора
    yoffset *= sensitivity;

    yaw += xoffset;  // Обновляем угол поворота по горизонтали (yaw)
    pitch += yoffset;  // Обновляем угол поворота по вертикали (pitch)

    // Ограничиваем угол поворота по вертикали (pitch) для предотвращения переворотов
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    // Вычисляем новое направление "вперед" (cameraFront) на основе углов yaw и pitch
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

// Размеры окна
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

    // Связывание шейдеров в шейдерную программу
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Создание и загрузка буфера вершин
    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Настройка указателей атрибутов вершин
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
    glEnable(GL_DEPTH_TEST);  // Включаем буфер глубины для теста глубины

    // Устанавливаем callback для обработки движения мыши
    glfwSetCursorPosCallback(window, mouse_callback);
    // Устанавливаем начальное положение курсора
    glfwSetCursorPos(window, 400, 300);

    // Основной цикл отрисовки
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);  // Обработка пользовательского ввода

        // Очистка буфера кадра
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);  // Используем шейдерную программу

        // Устанавливаем матрицы проекции, вида и модели
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
        glm::mat4 view = glm::lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp);

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

        glm::mat4 model = glm::mat4(1.0f);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

        // Устанавливаем параметры света и материала
        glUniform3f(glGetUniformLocation(shaderProgram, "light.position"), 1.2f, 1.0f, 2.0f);
        glUniform3f(glGetUniformLocation(shaderProgram, "light.ambient"), 0.2f, 0.2f, 0.2f);
        glUniform3f(glGetUniformLocation(shaderProgram, "light.diffuse"), 0.5f, 0.5f, 0.5f);
        glUniform3f(glGetUniformLocation(shaderProgram, "light.specular"), 1.0f, 1.0f, 1.0f);

        glUniform3f(glGetUniformLocation(shaderProgram, "material.ambient"), 1.0f, 0.5f, 0.31f);
        glUniform3f(glGetUniformLocation(shaderProgram, "material.diffuse"), 1.0f, 0.5f, 0.31f);
        glUniform3f(glGetUniformLocation(shaderProgram, "material.specular"), 0.5f, 0.5f, 0.5f);
        glUniform1f(glGetUniformLocation(shaderProgram, "material.shininess"), 32.0f);

        // Устанавливаем позицию камеры
        glUniform3f(glGetUniformLocation(shaderProgram, "viewPos"), cameraPosition.x, cameraPosition.y, cameraPosition.z);

        // Рисуем куб
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Освобождаем ресурсы
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();
    return 0;
}
