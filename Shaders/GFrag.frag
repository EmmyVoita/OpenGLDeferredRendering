#version 330 core

struct Material
{
	sampler2D shadowMap;
	sampler2D diffuseTex;
	//sampler2D normalMap;
	vec2 textureScale;
};

uniform Material material;

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;



//uniform sampler2D texture_specular1;

void main()
{    
    // store the fragment position vector in the first gbuffer texture
    gPosition = FragPos;
    // also store the per-fragment normals into the gbuffer\
    
    gNormal = normalize(Normal);
    //gNormal.b = 1.0 - gNormal.b;
    // and the diffuse per-fragment color
    gAlbedoSpec.rgb = texture(material.diffuseTex, TexCoords * material.textureScale).rgb;
    // store specular intensity in gAlbedoSpec's alpha component
    //gAlbedoSpec.a = texture(texture_specular1, TexCoords).r;
    gAlbedoSpec.a = 1.0;
}  

