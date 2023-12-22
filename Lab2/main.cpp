#include <GLAD/glad.h>
#include <GLFW/glfw3.h>

#include <GLM/glm.hpp>
#include <GLM/mat4x4.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>

#include <iostream>


// вершинный шейдер
const char* vertexShaderSource = R"(
        #version 460
        uniform mat4 projection;
        uniform mat4 view;
        layout(location = 0) in vec3 vertex_position;
        layout(location = 1) in vec3 vertex_color;
        out vec3 color;
        void main() {
            color = vertex_color;
            gl_Position = projection * view * vec4(vertex_position, 1.0);
        }
    )";

// фрагментный шейдер
const char* fragmentShaderSource = R"(
        #version 460
        in vec3 color;
        out vec4 frag_color;
        void main() {
            frag_color = vec4(color, 1.0);
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

// Массив координат вершин для двух треугольников
GLfloat point[] = {
    // Первый треугольник
    -0.5f,  0.7f, 0.0f,  // Верхний левый угол
    0.5f,  0.7f, 0.0f,   // Верхний правый угол
    0.0f,  0.1f, 0.0f,   // Нижний центральный угол

    // Второй треугольник
    -0.2f, -0.3f, 0.0f,  // Нижний правый угол
    0.2f, -0.3f, 0.0f,   // Нижний левый угол
    0.0f, -0.9f, 0.0f    // Нижний центральный угол
};

// Массив цветов для вершин
GLfloat colors[] = {
    1.0f, 0.0f, 0.0f,  // Верхний правый угол (красный)
    1.0f, 1.0f, 0.0f,  // Нижний правый угол (желтый)
    0.0f, 0.0f, 1.0f,  // Верхний левый угол (синий)

    1.0f, 1.0f, 0.0f,  // Нижний правый угол (желтый)
    0.0f, 0.0f, 1.0f,  // Нижний левый угол (синий)
    1.0f, 0.0f, 0.0f   // Верхний левый угол (красный)
};

// Управление камерой
glm::vec3 cameraPosition(0.0f, 0.0f, 3.0f);  // начальная позиция камеры
float cameraSpeed = 0.005f;  // скорость перемещения камеры
float rotationSpeed = 0.005f;  // скорость поворота камеры
float horizontalAngle = 3.14f;  // начальный угол, смотрим на -z
float verticalAngle = 0.0f;  // начальный вертикальный угол

// Размеры окна
glm::ivec2 g_windowSize(640, 480);

// Callback-функция для изменения размеров окна
void glfwWindowSizeCallback(GLFWwindow* window, int width, int height)
{
    g_windowSize.x = width;
    g_windowSize.y = height;
    glViewport(0, 0, width, height);
}

// Callback-функция для обработки нажатия клавиш
void glfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

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

    // Устанавливаем callback-функции
    glfwSetWindowSizeCallback(window, glfwWindowSizeCallback);
    glfwSetKeyCallback(window, glfwKeyCallback);

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    // Инициализация GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "GLAD initialization failed!" << std::endl;
        return -1;
    }

    // Компиляция шейдеров
    GLuint vertexShader = CompileShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);


    // Выводим информацию о рендерере и версии OpenGL
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
    
    // Выводим информацию о управлении
    std::cout << R"(
Camera Movement:
    W - move forward
    S - move back
    D - rotate to right
    A - rotate to left
    E - move up
    Q - move down
    )" << std::endl;

    glClearColor(0, 0, 0, 0); // Устанавливаем цвет очистки экрана

    // Создание программы и привязка к ней шейдеров
    GLuint shader_program = glCreateProgram();
    glAttachShader(shader_program, vertexShader);
    glAttachShader(shader_program, fragmentShader);
    glLinkProgram(shader_program);

    // Удаление шейдеров после линковки программы
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    GLuint points_vbo = 0; // Буфер вершин для первого треугольника
    glGenBuffers(1, &points_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(point), point, GL_STATIC_DRAW);

    GLuint colors_vbo = 0; // Буфер цветов для вершин
    glGenBuffers(1, &colors_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, colors_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);

    GLuint vao = 0;
    glGenVertexArrays(1, &vao); // Генерация VAO
    glBindVertexArray(vao);     // Привязка VAO

    // Активация и настройка массива вершин
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    // Активация и настройка массива цветов
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, colors_vbo);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    // Главный цикл приложения
    while (!glfwWindowShouldClose(window))
    {
        // Очистка буфера изображения
        glClear(GL_COLOR_BUFFER_BIT);

        // Обработка пользовательского ввода
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        {
            // Движение вперёд
            cameraPosition += cameraSpeed * glm::normalize(glm::vec3(cos(verticalAngle) * sin(horizontalAngle), sin(verticalAngle), cos(verticalAngle) * cos(horizontalAngle)));
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        {
            // Движение назад
            cameraPosition -= cameraSpeed * glm::normalize(glm::vec3(cos(verticalAngle) * sin(horizontalAngle), sin(verticalAngle), cos(verticalAngle) * cos(horizontalAngle)));
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            // Поворот влево
            horizontalAngle -= rotationSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        {
            // Поворот вправо
            horizontalAngle += rotationSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        {
            // Поворот вверх
            verticalAngle += rotationSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        {
            // Поворот вниз
            verticalAngle -= rotationSpeed;
        }

        // Вычисление направления камеры
        glm::vec3 direction(
            cos(verticalAngle) * sin(horizontalAngle),
            sin(verticalAngle),
            cos(verticalAngle) * cos(horizontalAngle)
        );

        // Вычисление правого и верхнего векторов
        glm::vec3 right = glm::vec3(
            sin(horizontalAngle - 3.14f / 2.0f),
            0,
            cos(horizontalAngle - 3.14f / 2.0f)
        );

        glm::vec3 up = glm::cross(right, direction);

        // Обновление матрицы вида
        glm::mat4 view = glm::lookAt(
            cameraPosition,
            cameraPosition + direction,
            up
        );

        // Обновление матрицы проекции
        glm::mat4 projection = glm::perspective(
            glm::radians(45.0f),
            (float)g_windowSize.x / (float)g_windowSize.y,
            0.1f,
            100.0f
        );

        // Использование шейдерной программы
        glUseProgram(shader_program);
        glUniformMatrix4fv(glGetUniformLocation(shader_program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(shader_program, "view"), 1, GL_FALSE, glm::value_ptr(view));

        // Отрисовка треугольников
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Переключение буферов и обработка событий
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}