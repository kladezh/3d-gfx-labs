#version 330 core
// функци€ дл€ вычислени€ тени
float calcShadow(vec4 fragPosLightSpaceES, float bias);

// цвет фрагмента
out vec4 FragColor ;

// нормаль фрагмента
in vec3 normES ;

// текстурные координаты фрагмента
in vec2 textES ;

// позици€ фрагмента
in vec3 posES ;

// значение видимости фрагмента
in float visibility;

// позици€ фрагмента в пространстве света
in vec4 FragPosLightSpaceES ;

// структура дл€ направленного света
struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
}; 

// текстура объекта
uniform sampler2D texture1;

// сцена
uniform sampler2D scene;

// текстура теней
uniform sampler2D shadowMap;

// направленный свет
uniform DirLight dirLight;

// матрица пространства света
uniform mat4 lightSpaceMatrix;

// позици€ камеры
uniform vec3 viewPos ;

// цвет неба
uniform vec3 sky;

// флаг дл€ отображени€ теней
uniform int showShadow;

// масштабирование высоты объекта
uniform int scale;

// цвет по умолчанию
vec3 col = vec3(0.5);

void main()
{

  //vec4 fragPosLightSpace = lightSpaceMatrix * vec4(posES, 1.0);

  // смещение дл€ теней
  float bias = 0.001;

  // цвет на основе высоты
  float height = posES.y/scale; // устанавливаем высоту как y позиции объекта и масштабируем ее
  vec4 green = vec4(0.3, 0.35, 0.15, 0.0); // зеленый цвет
  vec4 darkGreen = vec4(0.35, 0.55, 0.25, 0.0); // темно-зеленый цвет
  vec4 blue = vec4(0.2, 0.2, 0.7, 0.0); // синий цвет
  vec4 gray = vec4(0.5, 0.4, 0.5, 0.0); // серый цвет

  
  // условные операторы дл€ установки цвета на основе высоты
  if(height > 0.6) // если высота больше 6, то смешиваем темно-зеленый и серый цвета с интерпол€цией Ёрмита от 0,6 до 1,0
  col = vec3(mix(darkGreen, gray, smoothstep(0.6, 1.0, height)).rgb);
  else if(height > 0.3)// если высота меньше 6, то смешиваем зеленый и темно-зеленый цвета с интерпол€цией Ёрмита от 0,3 до 0,6
  col = vec3(mix(green, darkGreen, smoothstep(0.3, 0.6, height)).rgb);
  else if(height < 0.3) // если высота меньше 3, то смешиваем синий и зеленый цвета с интерпол€цией Ёрмита от 0,0 до 0,3
  col = vec3(mix(blue, green, smoothstep(0.0, 0.3, height)).rgb);   


  // модель освещени€ Ѕлинна-‘онга
  vec3 ambient = dirLight.ambient * col;  
  vec3 lightDir = normalize(dirLight.direction - posES);
  vec3 norm = normalize(normES) ;
  float diffFactor = max(dot(lightDir, norm), 0.0);
  vec3 diffuse = diffFactor * dirLight.diffuse * col;
  vec3 viewDir = normalize(viewPos - posES);

  // зеркальное отражение
  vec3 reflectDir = reflect(-dirLight.direction, norm);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
  
  // комбинирование результатов
  float specFactor = 0.0;
  vec3 halfDir = normalize(lightDir + viewDir);
  specFactor = pow(max(dot(halfDir, norm), 0.0), 64.0);  
  vec3 specular = specFactor * dirLight.specular * col;

  // установка цвета фрагмента
  float shadow ;
  if  (showShadow == 0)
    shadow = 0;
  else  
    shadow = calcShadow(FragPosLightSpaceES, bias); 
  
  FragColor = vec4(ambient + (1.0-shadow) * (diffuse + specular), 1.0f);
  FragColor = mix(vec4(sky,1.0), FragColor, visibility);

}

// функци€ дл€ вычислени€ тени
float calcShadow(vec4 FragPosLightSpaceES, float bias)  
{
    float shadow = 0.0 ; 

    // производим перспективное деление значений в диапазоне [-1,1]
    vec3 projCoords = FragPosLightSpaceES.xyz / FragPosLightSpaceES.w;

    // преобразуем в диапазон [0,1]
    projCoords = projCoords * 0.5 + 0.5;

    // выбираем значение из текстуры теней (возвращает float; 
    float closestDepth = texture(shadowMap, projCoords.xy).r;

    // получаем глубину текущего фрагмента из перспективы света
    float currentDepth = projCoords.z;

    // провер€ем, находитс€ ли текуща€ позици€ фрагмента в тени
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0); 
    for  (int i = -1 ; i < 2; i++){
        for(int j = -1 ; j < 2; j++){
            float pcf = texture(shadowMap, projCoords.xy + vec2(i, j) * texelSize).r;
            if(currentDepth - bias > pcf)
            shadow += 1;
        }
    }
    shadow = shadow/9;
  
    // если позици€ фрагмента находитс€ за пределами света, то тень не учитываетс€
    if  (projCoords.z > 1.0)
        shadow = 0.0;    
  
    return shadow* .65;
}