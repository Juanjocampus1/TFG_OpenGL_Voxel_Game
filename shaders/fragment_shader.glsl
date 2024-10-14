#version 330 core

out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

in vec3 Normal;
in vec3 crntPos;

uniform sampler2D texture1;
uniform vec4 lightColor;
uniform vec3 lightPos;
uniform vec3 camPos;

void main(){
    float ambient = 0.20f;
    
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - crntPos);
    float diffuse = max(dot(norm, lightDir), 0.0f);

    float specularLight = 0.50f;
    vec3 viewDir = normalize(camPos - crntPos);
    vec3 reflectionDir = reflect(-lightDir, norm);
    float specAmount = pow(max(dot(viewDir, reflectionDir), 0.0f), 32);
    float specular = specularLight * specAmount;

    FragColor = texture(texture1, TexCoord) * lightColor * (diffuse + ambient + specular);
}
