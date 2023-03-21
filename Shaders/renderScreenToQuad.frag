#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D ssao;


struct PointLight
{
	vec3  Position;
	float Intensity;
	vec3  Color;
	float Constant;
	float Linear;
	float Quadratic;
};

uniform PointLight lights[10];
uniform int NR_LIGHTS = 3;
uniform vec3 cameraPos;

void main()
{
    
   // retrieve data from gbuffer
    vec3 FragPos = texture(gPosition, TexCoord).rgb;
    vec3 Normal = texture(gNormal, TexCoord).rgb;
    vec3 Diffuse = texture(gAlbedoSpec, TexCoord).rgb;
    float AmbientOcclusion = texture(ssao, TexCoord).r;

    // calculate lighting for each point light
    vec3 lighting = vec3(0.0);
    vec3 viewDir = normalize(cameraPos - FragPos);
    for (int i = 0; i < NR_LIGHTS; ++i) {
        vec3 lightDir = normalize(lights[i].Position - FragPos);

        // diffuse
        vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Diffuse * lights[i].Color * lights[i].Intensity;

        // specular
        vec3 reflectDir = reflect(-lightDir, Normal);
        vec3 halfwayDir = normalize(viewDir + lightDir);
        float spec = pow(max(dot(reflectDir, halfwayDir), 0.0), 32.0); // adjusted exponent
        vec3 specular = lights[i].Color * spec * lights[i].Intensity;

        // attenuation
        float distance = length(lights[i].Position - FragPos);
        float attenuation = 1.0 / (lights[i].Constant + lights[i].Linear * distance + lights[i].Quadratic * distance * distance);
        diffuse *= attenuation;
        specular *= attenuation;

        lighting += diffuse + specular;
    }

    // apply ambient occlusion
    vec3 ambient = vec3(0.8 * Diffuse) * AmbientOcclusion;
    lighting += ambient;

    FragColor = vec4(lighting, 1.0);
    //FragColor = vec4(1.0, 1.0, 1.0, 1.0) * AmbientOcclusion;
}
