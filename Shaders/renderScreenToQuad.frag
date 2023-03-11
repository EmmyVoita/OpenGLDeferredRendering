#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

void main()
{
     // sample the textures
    vec3 position = texture(gPosition, TexCoord).xyz;
    vec3 normal = texture(gNormal, TexCoord).xyz;
    vec4 albedoSpec = texture(gAlbedoSpec, TexCoord);


    FragColor = vec4(position,1);
    //FragColor = vec4(TexCoord,0,1);
}

