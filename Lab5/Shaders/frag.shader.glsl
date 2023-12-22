#version 330

// Устанавливаем точность для вычислений с плавающей запятой
precision mediump float;


uniform sampler2D uSampler;


in vec2 vTexCoord;      // Координаты текстуры
in vec3 vNormal;        // Нормаль объекта
in vec3 vViewDirection; // Вектор обзора


out vec4 fragColor;

void main()
{
    // Интенсивности освещения
    vec3 ambientLightIntensity = vec3(0.7, 0.7, 0.7);
    vec3 sunLightIntensity = vec3(0.7, 0.7, 0.7);
    
    // Направление света от солнца
    vec3 sunLightDirection = normalize(vec3(-20.0, 20.0, 20.0));

    // Корректируем координаты текстуры для вертикального отражения
    vec2 flipped = vec2(vTexCoord.x, 1 - vTexCoord.y);

    // Получаем цвет пикселя из текстуры
    vec4 texel = texture(uSampler, flipped);

    // Рассчитываем toon-shading 
    float tunShading = max(dot(vNormal, vViewDirection), 0.0);

    // Рассчитываем освещение с учетом toon-shading
    vec3 lightIntensity = ambientLightIntensity + sunLightIntensity * max(dot(vNormal, sunLightDirection), 0.0) * tunShading;

    // Устанавливаем итоговый цвет фрагмента с учетом освещенности и текстуры
    fragColor = vec4(texel.rgb * lightIntensity, texel.a);
}
