#version 330 core
out vec4 FragColor;


struct DirLight {
    vec3 direction;
	
    float ambient;
    float diffuse;
    float specular;
};

in vec3 FragPos;
in vec3 Normal;
in vec2 texCoord;

uniform vec3 viewPos;
uniform vec3 color;
float shininess = 128;
DirLight dirLight = {
    vec3(-1/sqrt(2), -1/sqrt(2), 0),
    0.1, 0.8, 0.3};


vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);

void main()
{    
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    

    vec3 result = CalcDirLight(dirLight, norm, viewDir); 

    FragColor = vec4(result, 1.0);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);

    float diff = max(dot(normal, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);

    vec3 ambient = light.ambient * color;
    vec3 diffuse = light.diffuse * diff * color;
    vec3 specular = light.specular * spec * color;
    return (ambient + diffuse + specular);
}
