#version 330

// Входные атрибуты (атрибуты вершин) и их местоположения
layout(location=0) in vec3 aPosition; // Местоположение вершины
layout(location=1) in vec2 aTexCoord; // Координаты текстуры
layout(location = 2) in vec3 aNormal; // Нормаль к поверхности

// Униформы (переменные, общие для всех вершин)
uniform mat4 uMvpMatrix;  // Матрица проекции, вида и модели
uniform mat4 uModelMatrix; // Матрица модели

// Выходные переменные для передачи во фрагментный шейдер
out vec2 vTexCoord; // Координаты текстуры
out vec3 vNormal;   // Нормаль к поверхности

void main()
{
    // Преобразуем позицию вершины в координаты экрана (clip space) с учетом матрицы проекции, вида и модели
    gl_Position = uMvpMatrix * vec4(aPosition, 1.0);

    // Передаем нормаль к поверхности, преобразованную с учетом матрицы модели
    vNormal = (uModelMatrix * vec4(aNormal, 0.0)).xyz;

    // Передаем координаты текстуры
    vTexCoord = aTexCoord;
}
