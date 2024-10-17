#version 330 core

out vec4 FragColor;

in vec3 crntPos;
in vec3 Normal;
in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D diffuse0;
uniform sampler2D specular0;

uniform vec4 lightColor;
uniform vec3 lightPos;
uniform vec3 camPos;

void main() {
    float ambient = 0.20f;
    
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - crntPos);
    float diffuse = max(dot(norm, lightDir), 0.0f);

    float specularLight = 0.50f;
    vec3 viewDir = normalize(camPos - crntPos);
    vec3 reflectionDir = reflect(-lightDir, norm);
    float specAmount = pow(max(dot(viewDir, reflectionDir), 0.0f), 16);
    float specular = specularLight * specAmount;

    FragColor = texture(diffuse0, TexCoord) * lightColor * (diffuse + ambient) + texture(specular0, TexCoord).r * specular;
}
