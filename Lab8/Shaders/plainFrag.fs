#version 330 core
// ������� ��� ���������� ����
float calcShadow(vec4 fragPosLightSpaceES, float bias);

// ���� ���������
out vec4 FragColor ;

// ������� ���������
in vec3 normES ;

// ���������� ���������� ���������
in vec2 textES ;

// ������� ���������
in vec3 posES ;

// �������� ��������� ���������
in float visibility;

// ������� ��������� � ������������ �����
in vec4 FragPosLightSpaceES ;

// ��������� ��� ������������� �����
struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
}; 

// �������� �������
uniform sampler2D texture1;

// �����
uniform sampler2D scene;

// �������� �����
uniform sampler2D shadowMap;

// ������������ ����
uniform DirLight dirLight;

// ������� ������������ �����
uniform mat4 lightSpaceMatrix;

// ������� ������
uniform vec3 viewPos ;

// ���� ����
uniform vec3 sky;

// ���� ��� ����������� �����
uniform int showShadow;

// ��������������� ������ �������
uniform int scale;

// ���� �� ���������
vec3 col = vec3(0.5);

void main()
{

  //vec4 fragPosLightSpace = lightSpaceMatrix * vec4(posES, 1.0);

  // �������� ��� �����
  float bias = 0.001;

  // ���� �� ������ ������
  float height = posES.y/scale; // ������������� ������ ��� y ������� ������� � ������������ ��
  vec4 green = vec4(0.3, 0.35, 0.15, 0.0); // ������� ����
  vec4 darkGreen = vec4(0.35, 0.55, 0.25, 0.0); // �����-������� ����
  vec4 blue = vec4(0.2, 0.2, 0.7, 0.0); // ����� ����
  vec4 gray = vec4(0.5, 0.4, 0.5, 0.0); // ����� ����

  
  // �������� ��������� ��� ��������� ����� �� ������ ������
  if(height > 0.6) // ���� ������ ������ 6, �� ��������� �����-������� � ����� ����� � ������������� ������ �� 0,6 �� 1,0
  col = vec3(mix(darkGreen, gray, smoothstep(0.6, 1.0, height)).rgb);
  else if(height > 0.3)// ���� ������ ������ 6, �� ��������� ������� � �����-������� ����� � ������������� ������ �� 0,3 �� 0,6
  col = vec3(mix(green, darkGreen, smoothstep(0.3, 0.6, height)).rgb);
  else if(height < 0.3) // ���� ������ ������ 3, �� ��������� ����� � ������� ����� � ������������� ������ �� 0,0 �� 0,3
  col = vec3(mix(blue, green, smoothstep(0.0, 0.3, height)).rgb);   


  // ������ ��������� ������-�����
  vec3 ambient = dirLight.ambient * col;  
  vec3 lightDir = normalize(dirLight.direction - posES);
  vec3 norm = normalize(normES) ;
  float diffFactor = max(dot(lightDir, norm), 0.0);
  vec3 diffuse = diffFactor * dirLight.diffuse * col;
  vec3 viewDir = normalize(viewPos - posES);

  // ���������� ���������
  vec3 reflectDir = reflect(-dirLight.direction, norm);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
  
  // �������������� �����������
  float specFactor = 0.0;
  vec3 halfDir = normalize(lightDir + viewDir);
  specFactor = pow(max(dot(halfDir, norm), 0.0), 64.0);  
  vec3 specular = specFactor * dirLight.specular * col;

  // ��������� ����� ���������
  float shadow ;
  if  (showShadow == 0)
    shadow = 0;
  else  
    shadow = calcShadow(FragPosLightSpaceES, bias); 
  
  FragColor = vec4(ambient + (1.0-shadow) * (diffuse + specular), 1.0f);
  FragColor = mix(vec4(sky,1.0), FragColor, visibility);

}

// ������� ��� ���������� ����
float calcShadow(vec4 FragPosLightSpaceES, float bias)  
{
    float shadow = 0.0 ; 

    // ���������� ������������� ������� �������� � ��������� [-1,1]
    vec3 projCoords = FragPosLightSpaceES.xyz / FragPosLightSpaceES.w;

    // ����������� � �������� [0,1]
    projCoords = projCoords * 0.5 + 0.5;

    // �������� �������� �� �������� ����� (���������� float; 
    float closestDepth = texture(shadowMap, projCoords.xy).r;

    // �������� ������� �������� ��������� �� ����������� �����
    float currentDepth = projCoords.z;

    // ���������, ��������� �� ������� ������� ��������� � ����
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0); 
    for  (int i = -1 ; i < 2; i++){
        for(int j = -1 ; j < 2; j++){
            float pcf = texture(shadowMap, projCoords.xy + vec2(i, j) * texelSize).r;
            if(currentDepth - bias > pcf)
            shadow += 1;
        }
    }
    shadow = shadow/9;
  
    // ���� ������� ��������� ��������� �� ��������� �����, �� ���� �� �����������
    if  (projCoords.z > 1.0)
        shadow = 0.0;    
  
    return shadow* .65;
}