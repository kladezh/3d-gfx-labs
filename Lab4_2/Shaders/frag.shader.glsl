#version 330

precision mediump float;
uniform sampler2D uSampler;
in vec2 vTexCoord;
in vec3 vNormal;  // Use per-triangle normal
out vec4 fragColor;

void main()
{
    vec3 ambientLightIntensity = vec3(0.2, 0.2, 0.2);
    vec3 sunLightIntensity = vec3(0.7, 0.7, 0.7);
    vec3 sunLightDirection = normalize(vec3(-20.0, 20.0, 20.0));

    vec2 flipped = vec2(vTexCoord.x, 1 - vTexCoord.y);
    vec4 texel = texture(uSampler, flipped);

    // Use the per-triangle normal for flat shading
    vec3 lightIntensity = ambientLightIntensity + sunLightIntensity * max(dot(vNormal, sunLightDirection), 0.0);
    fragColor = vec4(texel.rgb * lightIntensity, texel.a);
}
