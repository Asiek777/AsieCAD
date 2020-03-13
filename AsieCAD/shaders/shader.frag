#version 330 core
out vec4 FragColor;

in vec3 Normal;  
in vec3 FragPos;  


uniform vec3 lightDirection;	
uniform vec3 lightAmbient;
uniform vec3 lightDiffuse;
uniform vec3 lightSpecular;
  

uniform vec3 viewPos; 
uniform vec3 objectColor;

void main()
{

    // diffuse 
    vec3 norm = normalize(Normal);
    float diff = max(dot(norm, lightDirection), 0.0);
    vec3 diffuse = diff * lightDiffuse;
    
    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDirection, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = lightSpecular * spec;  
        
    vec3 result = (lightAmbient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
} 