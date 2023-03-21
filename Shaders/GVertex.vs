#version 330 core

layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_color;
layout (location = 2) in vec2 vertex_texcoord;
layout (location = 3) in vec3 vertex_normal;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

uniform bool invertedNormals;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    // transform the vertex position into clip-space
    gl_Position = projection * view * model * vec4(vertex_position, 1.0);
    
    // pass the fragment position to the fragment shader in view-space
    FragPos = vec3(view * model * vec4(vertex_position, 1.0));
    
    // pass the normal to the fragment shader in view-space
    Normal = mat3(transpose(inverse(view * model))) * (invertedNormals ? -vertex_normal : vertex_normal);
    
    // pass the texture coordinates to the fragment shader
    TexCoords = vertex_texcoord;
}

