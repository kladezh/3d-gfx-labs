#version 330

layout(location=0) in vec3 aPosition;
layout(location=1) in vec2 aTexCoord;
layout(location = 2) in vec3 aNormal;

uniform mat4 uMvpMatrix;
uniform mat4 uModelMatrix;

out vec2 vTexCoord;
out vec3 vNormal;

void main()
{
    gl_Position = uMvpMatrix * vec4(aPosition, 1.0);
    vNormal = (uModelMatrix * vec4(aNormal, 0.0)).xyz;
    vTexCoord = aTexCoord;
}
