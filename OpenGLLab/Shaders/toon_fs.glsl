#version 330 core
out vec4 FragColor;

uniform vec3 viewPos; 
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;

in vec3 FragPos;
in vec2 TexCoord;
in vec3 Normal;

// texture samplers
uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{

	// ambient
    float ambientStrength = 0.01;
    vec3 ambient = ambientStrength * lightColor;
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

	// specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;  
	vec3 result = (ambient + diffuse + specular) * objectColor;
    
    vec4 texColor = texture(texture1, TexCoord);
    if(texColor.a < 0.1)
        discard;

	// linearly interpolate between both textures (80% container, 20% awesomeface)
	//FragColor = mix(texColor, texture(texture2, TexCoord), 0.1) * vec4(result, 0.5);
	FragColor = vec4(texColor, 1.0);

}

