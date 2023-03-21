#version 330

#define SAMPLE_RATE 0.025

struct Material
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	sampler2D shadowMap;
	sampler2D diffuseTex;
	float alpha;
	vec2 textureScale;
	float shininess;

};

struct PointLight
{
	vec3 position;
	float intensity;
	vec3 color;
	float constant;
	float linear;
	float quadratic;
};

//input from vertex shader
in vec3 vs_position;
in vec3 vs_color;
in vec2 vs_texcoord;
in vec3 vs_normal;
in vec4 vs_fragPosLightSpace;

//output of fragment shader
out vec4 color;

//Uniforms

uniform Material material;
uniform PointLight pointLight;
uniform vec3 lightPos0;
uniform vec3 cameraPos;

//Functions

float ShadowCalculation(vec4 fragPosLightSpace)
{
	//return 1.0;

    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;

	if(projCoords.z > 1.0)
	{
		 float shadow = 0.0;
		 return shadow;
	}
    

    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(material.shadowMap, projCoords.xy).r;  
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;


	float bias = max(0.05 * (1.0 - dot(vs_normal, normalize(lightPos0 - vs_position))), 0.005);  

    // check whether current frag pos is in shadow
    //float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;


	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(material.shadowMap, 0);
	for(int x = -1; x <= 1; ++x)
	{
	for(int y = -1; y <= 1; ++y)
	{
		float pcfDepth = texture(material.shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
		shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
	}    
	}
	shadow /= 9.0;

    return shadow;
}  


vec3 calculateAmbient(Material material)
{
	vec3 AmbientColor = vec3(pointLight.color * material.ambient);
	return AmbientColor;
}

vec3 calculateDiffuse(Material material, vec3 vs_position, vec3 vs_normal, vec3 lightPos0)
{
	vec3 posToLightDirVec = normalize(lightPos0 - vs_position);
	float diffuse = clamp(dot(posToLightDirVec, normalize(vs_normal)), 0, 1);
	vec3 diffuseFinal = material.diffuse * diffuse;

	return diffuseFinal;
}

vec3 calculateSpecular(Material material, vec3 vs_position, vec3 vs_normal, vec3 lightPos0, vec3 cameraPos)
{
	vec3 lightDir = normalize(lightPos0 - vs_position);
    vec3 viewDir = normalize(cameraPos - vs_position);
    // Calculate the halfway vector by adding the light direction and view direction, and normalizing the result.
    vec3 halfwayDir = normalize(lightDir + viewDir);
	// Compute the angle between the surface normal and the halfway vector, and clamp the result to zero or greater.
    float specAngle = max(dot(vs_normal, halfwayDir), 0.0);
	// Raise the angle to the power of the material shininess to get the specular constant.
    float specularConstant = pow(specAngle, material.shininess);
    vec3 specularFinal = material.specular * specularConstant * pointLight.color;
    return specularFinal;
}



//--------------------------------------------------------------------------------------------------------------------------------
//Main
//--------------------------------------------------------------------------------------------------------------------------------




void main()
{
	
	//ShadowCalculation
	float shadow = ShadowCalculation(vs_fragPosLightSpace);


	//Ambient light
	vec3 ambientFinal = calculateAmbient(material);

	//Diffuse light
	vec3 diffuseFinal = calculateDiffuse(material, vs_position, vs_normal, pointLight.position);

	//Specular light
	vec3 specularFinal = calculateSpecular(material, vs_position, vs_normal, pointLight.position, cameraPos);

	//Attenuation
	float distance = length(pointLight.position - vs_position);
	//constant linear quadratic
	float attenuation = pointLight.constant / (1.f + pointLight.linear * distance + pointLight.quadratic * (distance * distance));

	//Final light
	ambientFinal *= attenuation;
	diffuseFinal *= attenuation;
	specularFinal *= attenuation;

	
	
	///Final Color

	//color = vec4(specularFinal.xyz, 1.0);

	color = texture(material.diffuseTex, vec2(vs_texcoord.x * material.textureScale.x,vs_texcoord.y * material.textureScale.y )) * ((vec4(ambientFinal, 1.0) + (1.0-shadow) * (vec4(diffuseFinal.xyz, 1.0) + vec4(specularFinal.xyz, 1.0))) * pointLight.intensity); 
	//color = texture(material.diffuseTex, vec2(vs_texcoord.x * material.textureScale.x, vs_texcoord.y * material.textureScale.y )) * ((vec4(ambientFinal, 1.0) +  (vec4(diffuseFinal.xyz, 1.0) + vec4(specularFinal.xyz, 1.0))) * pointLight.intensity); 
	//color = vec4(color.x,color.y,color.z,material.alpha);
	//color = vec4(specularFinal.xyz, 1.0);
}
