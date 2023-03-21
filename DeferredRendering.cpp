
#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif


#include<iostream>
#include<string>
#include<fstream>
#include<vector>
#include<sstream>
#include<algorithm>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// Other Libs
#include <SOIL/SOIL.h>

// GLM Mathematics
#include <glm/glm.hpp>
#include<glm/vec3.hpp>
#include<glm/vec4.hpp>
#include<glm/mat4x4.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

// Other includes
#include "HeaderFiles/Shader2.h"
#include "HeaderFiles/OBJloader.h"
#include "HeaderFiles/Vertex.h"
#include "HeaderFiles/Texture.h"
#include "HeaderFiles/MyModelClass.h"
#include "HeaderFiles/Camera.h"
#include "HeaderFiles/Material.h"
#include "HeaderFiles/Light.h"
#include "HeaderFiles/ShadowPassClass.h"
#include "HeaderFiles/FillGBuffer.h"
#include "HeaderFiles/ScreenQuad.h"
#include "HeaderFiles/FillSSAO.h"

using namespace std;

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

// Window dimensions
const GLuint WIDTH = 1400, HEIGHT = 800;
GLFWwindow* window;

Camera* camera;

float dt = 0.0f;
float curTime = 0.0f;
float lastTime = 0.0f;

double lastMouseX = 0.0;
double lastMouseY = 0.0;
double mouseX = 0.0;
double mouseY = 0.0;
double mouseOffsetX = 0.0;
double mouseOffsetY = 0.0;
bool firstMouse = true;

glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix = glm::perspective(45.0f, (GLfloat)1400 / (GLfloat)800, 0.1f, 100.0f);

ShadowPassClass* shadowPass;
FillGBuffer* fillGBuffer;
ScreenQuad* screenQuad;
FillSSAO* fillSSAO;


void initWindow()
{
    // Init GLFW
    glfwInit();
    // Set all the required options for GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Create a GLFWwindow object that we can use for GLFW's functions
    window = glfwCreateWindow(WIDTH, HEIGHT, "Deferred Rendering", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    // Set the required callback functions
    glfwSetKeyCallback(window, key_callback);

    // Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
    glewExperimental = GL_TRUE;
    // Initialize GLEW to setup the OpenGL Function pointers
    glewInit();

    // Define the viewport dimensions
    glViewport(0, 0, WIDTH, HEIGHT);

    // Setup OpenGL options
    glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	GLenum err10;
    while ((err10 = glGetError()) != GL_NO_ERROR) {
        std::cerr << "OpenGL error 10: " << err10 << std::endl;
    }
}

void initCamera()
{
	
	camera = new Camera(glm::vec3(0.f, 2.f, 9.f), glm::vec3(0.f, 0.f, 1.f), glm::vec3(0.f, 1.f, 0.f));
	
	ViewMatrix = camera->getViewMatrix();
}

void updateKeyboardInput()
{
	//Camera
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		camera->move(dt, FORWARD);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		camera->move(dt, BACKWARD);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		camera->move(dt, LEFT);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		camera->move(dt, RIGHT);
	}
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
	{
		camera->move(dt,DOWN);
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		camera->move(dt,UP);
	}
}

void updateDt()
{
	curTime = static_cast<float>(glfwGetTime());
	dt = curTime - lastTime;
	lastTime = curTime;
}

void updateMouseInput()
{
	glfwGetCursorPos(window, &mouseX, &mouseY);

	if (firstMouse)
	{
		lastMouseX = mouseX;
		lastMouseY = mouseY;
		firstMouse = false;
	}

	//Calc offset
	mouseOffsetX = mouseX - lastMouseX;
	mouseOffsetY = lastMouseY - mouseY;

	//Set last X and Y
	lastMouseX = mouseX;
	lastMouseY = mouseY;

	//Move light
	/*if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS)
	{
		//pointLights[0]->setPosition(camera.getPosition());
	}*/
}

void updateInput()
{
	glfwPollEvents();

	updateKeyboardInput();
	//updateMouseInput();
	//camera->updateInput(dt, -1, mouseOffsetX, mouseOffsetY);
}

void updateUniforms(MyModelClass* model, vector<PointLight*> &pointLights, Shader2 &ourShader)
{

	shadowPass->setShaderUnifroms(ourShader);
	//View Matrix
	ViewMatrix = camera->getViewMatrix();
	ourShader.setVec3f(camera->getPosition(), "cameraPos");

	model->setViewMatrix(ViewMatrix);

	
	
	//Point Lights
	for (unsigned int i = 0; i < pointLights.size(); i++)
	{
		//pointLights[i]->sendToShader(ourShader);
		//ourShader.setVec3f(pointLights[i]->getPosition(), "lightPos0");
	}

}


int main()
{
    
    
    //Shaders
    vector<Shader2*> shaders;
    
    //Point Lights
    vector<PointLight*> pointLights;
    
    //Materials
    vector<Material*> materials; 

    //Textures
    vector<Texture*> textures; 
    
    //Models
    vector<MyModelClass*> myModels;
    
    //initwindow
    initWindow();

    // Build and compile our shader program
    shaders.push_back(new Shader2("Shaders/BaseVertexShader.vs", "Shaders/BaseFragmentShader.frag"));
    shaders.push_back(new Shader2("Shaders/BaseVertexShader.vs", "Shaders/AdvancedFragmentShader.frag"));


    initCamera();
    
	GLenum err10;
	 while ((err10 = glGetError()) != GL_NO_ERROR) {
        std::cerr << "OpenGL error 11: " << err10 << std::endl;
    }

	
    textures.push_back(new Texture("Textures/Cup1ColorFinal.png", GL_TEXTURE_2D));
    textures.push_back(new Texture("Textures/Cup2BaseColor.png", GL_TEXTURE_2D));
	textures.push_back(new Texture("Textures/MakeupColor.png", GL_TEXTURE_2D));
	textures.push_back(new Texture("Textures/Carpet/Rug_001_COLOR.png", GL_TEXTURE_2D));
	textures.push_back(new Texture("Textures/NailPolish.png", GL_TEXTURE_2D));
	textures.push_back(new Texture("Textures/Wood/WoodFineDark004_COL_4K.jpg", GL_TEXTURE_2D));


    while ((err10 = glGetError()) != GL_NO_ERROR) {
        std::cerr << "OpenGL error 11: " << err10 << std::endl;
    }

    materials.push_back(new Material(glm::vec3(0.7f), glm::vec3(1.f), glm::vec3(.8f), 0, 1, 1.0, glm::vec2(1.0f, 1.0f), 32.0)); 
    materials.push_back(new Material(glm::vec3(0.7f), glm::vec3(1.f), glm::vec3(.8f), 2, 3, 1.0, glm::vec2(1.0f, 1.0f), 32.0)); 
    materials.push_back(new Material(glm::vec3(0.7f), glm::vec3(2.f), glm::vec3(.6f), 4, 5, 1.0, glm::vec2(1.5f, 1.5f), 32.0)); 
	materials.push_back(new Material(glm::vec3(0.7f), glm::vec3(1.f), glm::vec3(1.f), 6, 7, 0.8, glm::vec2(1.0f, 1.0f), 32.0)); 
	materials.push_back(new Material(glm::vec3(0.7f), glm::vec3(2.f), glm::vec3(.6f), 8, 9, 1.0, glm::vec2(4.0f, 4.0f), 32.0)); 
	materials.push_back(new Material(glm::vec3(0.7f), glm::vec3(1.f), glm::vec3(1.f), 10, 11, 1.0, glm::vec2(1.0f, 1.0f), 32.0));
	materials.push_back(new Material(glm::vec3(0.7f), glm::vec3(1.f), glm::vec3(1.f), 16, 17, 1.0, glm::vec2(1.0f, 1.0f), 32.0));  
	materials.push_back(new Material(glm::vec3(0.7f), glm::vec3(1.f), glm::vec3(1.f), 18, 19, 1.0, glm::vec2(1.0f, 1.0f), 32.0));  

    while ((err10 = glGetError()) != GL_NO_ERROR) {
        std::cerr << "OpenGL error 10: " << err10 << std::endl;
    }

    pointLights.push_back(new PointLight(glm::vec3(12.f,10.0f,12.0f), 1.0f, glm::vec3(255.0f/256.0f,245.f/256.0f,182.f/256.0f)));
    pointLights.push_back(new PointLight(glm::vec3(0.f,5.0f,0.0f), 1.0f, glm::vec3(255.0f/256.0f,245.f/256.0f,182.f/256.0f)));
	pointLights.push_back(new PointLight(glm::vec3(-4.0f,5.0f,-4.0f), 1.3f, glm::vec3(255.0f/256.0f,0.f/256.0f,182.f/256.0f)));
	

    myModels.push_back(new MyModelClass(materials[0], textures[0], "Models/Cup1Model.txt",  ViewMatrix, ProjectionMatrix));
    myModels.push_back(new MyModelClass(materials[1], textures[1],  "Models/Cup1Model.txt", ViewMatrix, ProjectionMatrix));
    myModels.push_back(new MyModelClass(materials[2], textures[5], "Models/table top.txt", ViewMatrix, ProjectionMatrix));
	myModels.push_back(new MyModelClass(materials[3], textures[2],  "Models/makeupremover2.txt", ViewMatrix, ProjectionMatrix));
	myModels.push_back(new MyModelClass(materials[4], textures[3], "Models/GroundPlane2.txt", ViewMatrix, ProjectionMatrix));
	myModels.push_back(new MyModelClass(materials[5], textures[4],  "Models/NailPolish.txt", ViewMatrix, ProjectionMatrix));
	myModels.push_back(new MyModelClass(materials[6], textures[0], "Models/Pal1.txt",   ViewMatrix, ProjectionMatrix));
	myModels.push_back(new MyModelClass(materials[7], textures[0], "Models/Plush1.txt", ViewMatrix, ProjectionMatrix));
	

	//Transformations:
    
	//Cup 1
    myModels[0]->setOrigin(glm::vec3(-2.0f, 1.2f, -2.0f));

	//Cup 2
    myModels[1]->setOrigin(glm::vec3(3.0f, 1.2f, 0.f));

	//Table Top 
    myModels[2]->setOrigin(glm::vec3(0.0f, 0.0f, -4.0f));
	myModels[2]->setScale(glm::vec3(0.8f, 0.8f, 0.8f));

	//Makeup Remover Jar
	myModels[3]->setOrigin(glm::vec3(-0.2f, 0.15f, -4.8f));

	//Carpet Plane
	myModels[4]->setOrigin(glm::vec3(0.0f, 0.0f, -4.0f));
	myModels[4]->setScale(glm::vec3(2.0f, 1.0f, 2.0f));

	//Nail Polish Remover Bottle
	myModels[5]->setOrigin(glm::vec3(-1.5f, 0.0f, -4.0f));
	myModels[5]->setScale(glm::vec3(0.8f, 0.8f, 0.8f));

	//Makeup Pal
	myModels[6]->setOrigin(glm::vec3(-2.0f, -0.45f, 1.5f));
	myModels[6]->setScale(glm::vec3(0.6f, 0.6f, 0.6f));
	myModels[6]->setRotation(glm::vec3(0.0f, 90.f, 0.0f));

	//Owl Plush
	myModels[7]->setOrigin(glm::vec3(-1.0f, 0.0f, -1.f));
	myModels[7]->setScale(glm::vec3(0.6f, 0.6f, 0.6f));
	myModels[7]->setRotation(glm::vec3(0.0f, -45.f, -90.0f));


	//Shadow Pass
	shadowPass = new ShadowPassClass(pointLights[0], myModels);
	
	//for each model pass a refernece to the shadowPass depthmap
	for (int i = 0; i < myModels.size(); i++)
	{
		myModels[i]->setShadowTex(shadowPass->getDepthMap());
	}

	
	fillGBuffer = new FillGBuffer(myModels);

	fillSSAO = new FillSSAO(ProjectionMatrix, &ViewMatrix);

	screenQuad = new ScreenQuad(myModels);



    while ((err10 = glGetError()) != GL_NO_ERROR) {
        std::cerr << "OpenGL error 10: " << err10 << std::endl;
    }
	
    // Render loop
    while (!glfwWindowShouldClose(window))
    {
		

        //update functions
        updateDt();
       	updateInput();

		
	
		//shaders[2]->Use();

		glClearColor(0.0,0.0,0.0,1.0);                                  // keep it black so it doesn't leak into g-buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		ViewMatrix = camera->getViewMatrix();

		//shadowPass->setShaderUnifroms(ourShader);
		//shadowPass->render();

		fillGBuffer -> RenderToGBuffer();
		fillSSAO    -> GenerateSSAO(fillGBuffer);
		fillSSAO    -> BlurSSAO();

		fillGBuffer -> BindAllGBufferTextures(screenQuad->getScreenQuadShader(), fillSSAO->getSSAOTexture());
		

		screenQuad  -> PassShaderUniforms(camera, pointLights);
		screenQuad  -> renderScreenQuad();
	
	
		glfwSwapBuffers(window);
		glBindVertexArray(0);
		glUseProgram(0);
		
		glBindTexture(GL_TEXTURE_2D, 0);
        
    }
    
    
    // Properly de-allocate all resources once they've outlived their purpose
    glfwTerminate();
    
    for (size_t i = 0; i < shaders.size(); i++)
		delete shaders[i];
	
	for (size_t i = 0; i < textures.size(); i++)
		delete textures[i];

	for (size_t i = 0; i < materials.size(); i++)
		delete materials[i];

	for (size_t i = 0; i < myModels.size(); i++)
		delete myModels[i];

	for (size_t i = 0; i < pointLights.size(); i++)
		delete pointLights[i];
    
    
    return 0;
}



// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

