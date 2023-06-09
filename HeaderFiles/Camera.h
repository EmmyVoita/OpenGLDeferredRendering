#pragma once

#include<iostream>

// GLEW
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// GLM Mathematics
#include <glm/glm.hpp>
#include<glm/vec3.hpp>
#include<glm/vec4.hpp>
#include<glm/mat4x4.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

enum direction {NONE, FORWARD, BACKWARD, LEFT, RIGHT, UP, DOWN};

class Camera
{
private:
 	
	glm::mat4 ViewMatrix;

	float keyDownTime;
	float speed;

	GLfloat movementSpeed;
	GLfloat sensitivity;

	glm::vec3 worldUp;
	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 right;
	glm::vec3 up;

	GLfloat pitch;
	GLfloat yaw;
	GLfloat roll;

	void updateCameraVectors()
	{
		this->front.x = cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
		this->front.y = sin(glm::radians(this->pitch));
		this->front.z = sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));

		this->front = glm::normalize(this->front);
		this->right = glm::normalize(glm::cross(this->front, this->worldUp));
		this->up = glm::normalize(glm::cross(this->right, this->front));
	}

public:
	Camera(glm::vec3 position, glm::vec3 direction, glm::vec3 worldUp)
	{
		this->ViewMatrix = glm::mat4(1.f);

		this->keyDownTime = 0.0;
		this->speed = 1.0;

		this->movementSpeed = 3.f;
		this->sensitivity = 50.f;

		this->worldUp = worldUp;
		this->position = position;
		this->right = glm::vec3(0.f);
		this->up = worldUp;

		this->pitch = -45.f;
		this->yaw = -90.f;
		this->roll = 0.f;

		this->updateCameraVectors();
	}

	~Camera() {}

	//Accessors
	const glm::mat4 getViewMatrix()
	{
		this->updateCameraVectors();

		this->ViewMatrix = glm::lookAt(this->position, this->position + this->front, this->up);

		return this->ViewMatrix;
	}

	const glm::vec3 getPosition() const
	{
		return this->position;
	}

	//Functions
	void move(const float& dt, const int direction)
	{

		bool isKeyDown = false;
    
		if (direction != NONE)
		{
			isKeyDown = true;
		}
		else
		{
			keyDownTime = 0.0f;
			isKeyDown = false;
			speed = 1.0f;
		}

		if (isKeyDown && keyDownTime < 1.5f)
		{
			keyDownTime += dt;
		}

		if (!isKeyDown) // check if the key is released
		{
			keyDownTime = 0.0f; // reset the keyDownTime
			speed = 1.0f;
		}
		
		if (keyDownTime >= 1.5f)
		{
			speed = 5.f;
		}

		cout << isKeyDown << endl;
		cout << keyDownTime << endl;
		cout << "direction: " << direction << endl;


		// Update position vector
		switch (direction)
		{
		case FORWARD:
			this->position += glm::vec3(0.f, 0.f, -1.f) * this->movementSpeed * speed * dt;
			break;
		case BACKWARD:
			this->position += glm::vec3(0.f, 0.f, 1.f) * this->movementSpeed * speed * dt;
			break;
		case LEFT:
			this->position += glm::vec3(-1.f, 0.f, 0.f) * this->movementSpeed * speed * dt;
			break;
		case RIGHT:
			this->position += glm::vec3(1.f, 0.f, 0.f) * this->movementSpeed * speed * dt;
			break;
		case UP:
			this->position += glm::vec3(0.f, 1.f, 0.f) * this->movementSpeed * speed * dt;
			break;
		case DOWN:
			this->position += glm::vec3(0.f, -1.f, 0.f) * this->movementSpeed * speed * dt;
			break;
		case NONE:
			break;
		default:
			break;
		}
	}

	void updateMouseInput(const float& dt, const double& offsetX, const double& offsetY)
	{
		//Update pitch yaw and roll
		this->pitch += static_cast<GLfloat>(offsetY) * this->sensitivity * dt;
		this->yaw += static_cast<GLfloat>(offsetX) * this->sensitivity * dt;

		if (this->pitch > 80.f)
			this->pitch = 80.f;
		else if (this->pitch < -80.f)
			this->pitch = -80.f;

		if (this->yaw > 360.f || this->yaw < -360.f)
			this->yaw = 0.f;
	}

	void updateInput(const float& dt, const int direction, const double& offsetX, const double& offsetY)
	{
		this->updateMouseInput(dt, offsetX, offsetY);
	}

};
