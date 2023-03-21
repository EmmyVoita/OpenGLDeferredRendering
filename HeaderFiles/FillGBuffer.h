#pragma once
// GLM Mathematicsclear

#include <glm/glm.hpp>
#include<glm/vec3.hpp>
#include<glm/vec4.hpp>
#include<glm/mat4x4.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

#include "Shader2.h"

using namespace std;

class FillGBuffer
{
    private:

    const unsigned int SCR_WIDTH = 1400, SCR_HEIGHT = 800;
    GLuint gBuffer;
    GLuint gPosition, gNormal, gAlbedoSpec;
    vector<MyModelClass*> myModels;
    Shader2* gBufferShader;

    void Init()
    {

        GLenum err2;
        while ((err2 = glGetError()) != GL_NO_ERROR) {
            std::cerr << "OpenGL error 22: " << err2 << std::endl;
        }
       
        glGenFramebuffers(1, &gBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
        
        
        // - position color buffer
        glGenTextures(1, &gPosition);
        glBindTexture(GL_TEXTURE_2D, gPosition);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);  
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

         // Check for errors
        GLenum err10;
        while ((err10 = glGetError()) != GL_NO_ERROR) {
            std::cerr << "OpenGL error 1: " << err10 << std::endl;
        }
        
        // - normal color buffer
        glGenTextures(1, &gNormal);
        glBindTexture(GL_TEXTURE_2D, gNormal);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

          // Check for errors
        GLenum err11;
        while ((err11 = glGetError()) != GL_NO_ERROR) {
            std::cerr << "OpenGL error 2: " << err11 << std::endl;
        }
        
        // - color + specular color buffer
        glGenTextures(1, &gAlbedoSpec);
        glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);


        // then also add render buffer object as depth buffer and check for completeness.
        unsigned int rboDepth;
        glGenRenderbuffers(1, &rboDepth);
        glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
        
        // - tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
        GLenum attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
        glDrawBuffers(3, attachments);

        
        // - check if framebuffer is complete
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            std::cout << "Framebuffer is not complete!" << std::endl;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

    }

    void RenderModels()
    {
        gBufferShader->Use();

        for(int i = 0; i < myModels.size(); i++)
		{
            //gBufferShader->Use();     

            gBufferShader -> set1i(0, "invertedNormals");
            gBufferShader->Use();
			myModels[i]->RenderForGBuffer(*gBufferShader);
		}
    }

    public:

    FillGBuffer(vector<MyModelClass*> myModels)
    {

        GLenum err2;
        while ((err2 = glGetError()) != GL_NO_ERROR) {
            std::cerr << "OpenGL error 24: " << err2 << std::endl;
        }

        this->myModels = myModels;

        gBufferShader = new Shader2("Shaders/GVertex.vs", "Shaders/GFrag.frag");

        Init();
    }

    // In the geometry pass we need to render all objects of the scene and store these data components in the G-buffer.

	void RenderToGBuffer()
	{

        gBufferShader->Use();

        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);                     // bind the G-buffer framebuffer
        
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		RenderModels();

        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR) {
            std::cerr << "OpenGL error in RenderModels to gBuffer: " << err << std::endl;
        }
       
        glBindFramebuffer(GL_FRAMEBUFFER, 0);                           // unbind the G-buffer framebuffer
        glBindTexture(GL_TEXTURE_2D, 0);
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	}	

    void BindAllGBufferTextures(Shader2* screenQuadShader, unsigned int ssaoColorBufferBlur)
    {
        // set the shader program for rendering the screen quad
        
        screenQuadShader->Use();

        //update view matrix


        glUniform1i(glGetUniformLocation(screenQuadShader->Program, "gPosition"), 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gPosition);

        glUniform1i(glGetUniformLocation(screenQuadShader->Program, "gNormal"), 1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gNormal);

        glUniform1i(glGetUniformLocation(screenQuadShader->Program, "gAlbedoSpec"), 2);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);

        glUniform1i(glGetUniformLocation(screenQuadShader->Program, "ssao"), 3);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);


        GLenum err;
        // Check for errors
        while ((err = glGetError()) != GL_NO_ERROR) {
            std::cerr << "OpenGL error when binding textures to screenQuad: " << err << std::endl;
        }
    }

    void BindGBufferPositionNormal(Shader2* screenQuadShader)
    {
        screenQuadShader->Use();

        glUniform1i(glGetUniformLocation(screenQuadShader->Program, "gPosition"), 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gPosition);

        glUniform1i(glGetUniformLocation(screenQuadShader->Program, "gNormal"), 1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gNormal);

        GLenum err;
        // Check for errors
        while ((err = glGetError()) != GL_NO_ERROR) {
            std::cerr << "OpenGL error when binding textures to screenQuad: " << err << std::endl;
        }
    }

    unsigned int getGBuffer()
    {
        return gBuffer;
    }
};


