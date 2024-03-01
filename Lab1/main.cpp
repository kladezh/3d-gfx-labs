#include <GLAD/glad.h>
#include <GLFW/glfw3.h>

#include <GLM/glm.hpp>
#include <GLM/mat4x4.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>

#include <iostream>


// âåðøèííûé øåéäåð
const char* vertexShaderSource = R"(
        #version 460
        layout(location = 0) in vec3 vertex_position;
        layout(location = 1) in vec3 vertex_color;
        out vec3 color;
        void main() {
            color = vertex_color;
            gl_Position = vec4(vertex_position, 1.0);
        }
    )"; 

// ôðàãìåíòíûé øåéäåð
const char* fragmentShaderSource = R"(
        #version 460
        in vec3 color;
        out vec4 frag_color;
        void main() {
            frag_color = vec4(color, 1.0);
        }
    )";


// Ôóíêöèÿ äëÿ êîìïèëÿöèè è ïðîâåðêè îøèáîê â øåéäåðå
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


// Ìàññèâ êîîðäèíàò âåðøèí äëÿ äâóõ òðåóãîëüíèêîâ
GLfloat point[] = {
    // Ïåðâûé òðåóãîëüíèê
    -0.5f,  0.7f, 0.0f,  // Âåðõíèé ëåâûé óãîë
     0.5f,  0.7f, 0.0f,  // Âåðõíèé ïðàâûé óãîë
     0.0f,  0.1f, 0.0f,  // Íèæíèé öåíòðàëüíûé óãîë

     // Âòîðîé òðåóãîëüíèê
     -0.2f, -0.3f, 0.0f,  // Íèæíèé ïðàâûé óãîë
      0.2f, -0.3f, 0.0f,  // Íèæíèé ëåâûé óãîë
      0.0f, -0.9f, 0.0f   // Íèæíèé öåíòðàëüíûé óãîë
};

// Ìàññèâ öâåòîâ äëÿ âåðøèí
GLfloat colors[] = {
    1.0f, 0.0f, 0.0f,  // Âåðõíèé ïðàâûé óãîë (êðàñíûé)
    1.0f, 1.0f, 0.0f,  // Íèæíèé ïðàâûé óãîë (æåëòûé)
    0.0f, 0.0f, 1.0f,  // Âåðõíèé ëåâûé óãîë (ñèíèé)

    1.0f, 1.0f, 0.0f,  // Íèæíèé ïðàâûé óãîë (æåëòûé)
    0.0f, 0.0f, 1.0f,  // Íèæíèé ëåâûé óãîë (ñèíèé)
    1.0f, 0.0f, 0.0f   // Âåðõíèé ëåâûé óãîë (êðàñíûé)
};

static int shoulderAngle = 0, elbowAngle = 0;

// Ðàçìåðû îêíà
glm::ivec2 g_windowSize(640, 480);

// Callback-ôóíêöèÿ äëÿ èçìåíåíèÿ ðàçìåðîâ îêíà
void glfwWindowSizeCallback(GLFWwindow* window, int width, int height)
{
    g_windowSize.x = width;
    g_windowSize.y = height;
    glViewport(0, 0, width, height); // Óñòàíàâëèâàåì îáëàñòü ðèñîâàíèÿ OpenGL
}

// Callback-ôóíêöèÿ äëÿ îáðàáîòêè íàæàòèÿ êëàâèø
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

    // Óñòàíàâëèâàåì òðåáóåìóþ âåðñèþ OpenGL è ïðîôèëü
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

    // Óñòàíàâëèâàåì callback-ôóíêöèè
    glfwSetWindowSizeCallback(window, glfwWindowSizeCallback);
    glfwSetKeyCallback(window, glfwKeyCallback);

    // Èíèöèàëèçàöèÿ GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "GLAD initialization failed!" << std::endl;
        return -1;
    }

    // Âûâîäèì èíôîðìàöèþ î ðåíäåðåðå è âåðñèè OpenGL
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;

    glClearColor(0, 0, 0, 0); // Óñòàíàâëèâàåì öâåò î÷èñòêè ýêðàíà

    // Êîìïèëÿöèÿ øåéäåðîâ
    GLuint vertexShader = CompileShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    // Ñîçäàíèå ïðîãðàììû è ïðèâÿçêà ê íåé øåéäåðîâ
    GLuint shader_program = glCreateProgram();
    glAttachShader(shader_program, vertexShader);
    glAttachShader(shader_program, fragmentShader);
    glLinkProgram(shader_program);

    // Óäàëåíèå øåéäåðîâ ïîñëå ëèíêîâêè ïðîãðàììû
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    GLuint VBO_points = 0; // Áóôåð âåðøèí äëÿ ïåðâîãî òðåóãîëüíèêà
    glGenBuffers(1, &VBO_points);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_points);
    glBufferData(GL_ARRAY_BUFFER, sizeof(point), point, GL_STATIC_DRAW);

    GLuint VBO_colors = 0; // Áóôåð öâåòîâ äëÿ âåðøèí
    glGenBuffers(1, &VBO_colors);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_colors);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);

    GLuint VAO = 0;
    glGenVertexArrays(1, &VAO); // Ãåíåðàöèÿ VAO
    glBindVertexArray(VAO);     // Ïðèâÿçêà VAO

    // Àêòèâàöèÿ è íàñòðîéêà ìàññèâà âåðøèí
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_points);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    // Àêòèâàöèÿ è íàñòðîéêà ìàññèâà öâåòîâ
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_colors);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    // Îñíîâíîé öèêë îòðèñîâêè
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT); // Î÷èñòêà áóôåðà öâåòà

        glUseProgram(shader_program); // Èñïîëüçîâàíèå ïðîãðàììû

        glBindVertexArray(VAO); // Ïðèâÿçêà VAO
        glDrawArrays(GL_TRIANGLES, 0, 6); // Îòðèñîâêà òðåóãîëüíèêîâ

        glfwSwapBuffers(window); // Ïåðåêëþ÷åíèå áóôåðîâ

        glfwPollEvents(); // Ïðîâåðêà ñîáûòèé
    }

    // Îñâîáîæäàåì ðåñóðñû
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO_points);
    glDeleteBuffers(1, &VBO_colors);

    glfwTerminate();
    return 0;
}
