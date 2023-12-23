#version 330 core
out vec4 fragColor ;

// Функция для линеаризации глубины
float LinearizeDepth(float depth);

in vec2 TextCoords;

// Текстуры сцены и карты теней
uniform sampler2D scene;
uniform sampler2D depthMap;

const float near_plane = 1;
const float far_plane = 100;

void main()
{  
  // Получаем глубину текущего фрагмента из текстуры карты теней
  float depth = texture(depthMap, TextCoords).r;
  
  // Линеаризуем глубину для корректного отображения
  fragColor = vec4(vec3(LinearizeDepth(depth)/ far_plane), 1.0);
  
  // Для отладки можно отобразить глубину просто так
  //fragColor = vec4(vec3(depth), 1.0);
}  

// Функция для линеаризации глубины
float LinearizeDepth(float depth)
{
  float z = depth * 2.0 - 1.0;
  return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z *(far_plane - near_plane)); 
}