#version 330

precision mediump float;
uniform sampler2D uSampler;
in vec2 vTexCoord;  // Координаты текстуры
in vec3 vNormal;    // Нормаль к поверхности
out vec4 fragColor;

// Структура для хранения параметров света
struct Light {
    vec3 direction;  // Направление света
    vec3 intensity;  // Интенсивность света
};

void main()
{
    vec3 ambientLightIntensity = vec3(0.2, 0.2, 0.2); // Интенсивность окружающего света
    Light sunLight;
    sunLight.intensity = vec3(0.7, 0.7, 0.7); // Интенсивность солнечного света
    sunLight.direction = normalize(vec3(-20.0, 20.0, 20.0)); // Нормализованное направление солнечного света

    vec2 flipped = vec2(vTexCoord.x, 1 - vTexCoord.y); // Перевернутые координаты текстуры (OpenGL использует 0 в верхнем левом углу)
    vec4 texel = texture(uSampler, flipped); // Цвет пикселя из текстуры

    vec3 normal = normalize(vNormal); // Нормализация нормали
    vec3 viewDirection = normalize(-gl_FragCoord.xyz); // Нормализованное направление обзора (обратное направление камеры)
    vec3 lightDirection = normalize(sunLight.direction); // Нормализованное направление света

    float ambientStrength = 0.2; // Коэффициент окружающего освещения
    vec3 ambient = ambientStrength * ambientLightIntensity; // Вычисление окружающего освещения

    float diffuseStrength = max(dot(normal, lightDirection), 0.0); // Интенсивность диффузного освещения
    vec3 diffuse = diffuseStrength * sunLight.intensity; // Вычисление диффузного освещения

    float specularStrength = 10; // Коэффициент зеркального отражения
    float shininess = 500.0; // Фактор блика (настройте под свои нужды)
    vec3 reflectionDirection = reflect(-lightDirection, normal); // Направление отраженного света
    float specularStrengthCalc = pow(max(dot(viewDirection, reflectionDirection), 0.0), shininess); // Расчет интенсивности зеркального отражения
    vec3 specular = specularStrengthCalc * specularStrength * sunLight.intensity; // Вычисление зеркального отражения

    vec3 lightIntensity = ambient + diffuse + specular; // Общая интенсивность света
    fragColor = vec4(texel.rgb * lightIntensity, texel.a); // Итоговый цвет пикселя с учетом освещения
}
