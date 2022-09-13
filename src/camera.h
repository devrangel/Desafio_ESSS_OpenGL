#pragma once

#include "GLFW/glfw3.h"
#include "glm/vec3.hpp"
#include "glm/ext/matrix_transform.hpp"

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 200.0f;
const float SENSITIVY = 0.05f;
const float ZOOM = 45.0f;

// TODO: needs improvements
class Camera
{
public:
	float m_Zoom;

	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 worldUp;

	Camera(glm::vec3 cameraPos, glm::vec3 cameraFront, glm::vec3 cameraUp, float lastX, float lastY, float yaw = YAW, float pitch = PITCH)
		: position(cameraPos)
		, front(cameraFront)
		, worldUp(cameraUp)
		, m_DeltaTime(0.0f)
		, m_LastFrame(0.0f)
		, m_MovementSpeed(SPEED)
		, m_MouseSensity(SENSITIVY)
		, m_Zoom(ZOOM)
		, m_Yaw(yaw)
		, m_Pitch(pitch)
		, m_FirstMouse(true)
		, m_MouseButton(false)
		, m_LastX(lastX)
		, m_LastY(lastY)
	{
		this->updateCameraVectors();
	}

	glm::mat4 getViewMatrix() const
	{
		return glm::lookAt(this->position, this->position + this->front, this->up);
	}

	void processKeyboard(GLFWwindow* window)
	{
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, true);
		}

		// Calculate de delta time in order to make a smooth moviment
		float currentFrame = (float)glfwGetTime();
		this->m_DeltaTime = currentFrame - this->m_LastFrame;
		this->m_LastFrame = currentFrame;

		const float cameraSpeed = this->m_MovementSpeed * m_DeltaTime;

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			position += cameraSpeed * front;

		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			position -= cameraSpeed * front;

		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			position -= glm::normalize(glm::cross(front, up)) * cameraSpeed;

		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			position += glm::normalize(glm::cross(front, up)) * cameraSpeed;
	}

	void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
	{
		if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			this->m_MouseButton = true;
		}
		else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			this->m_MouseButton = false;
		}
	}

	void mouseCallback(GLFWwindow* window, double xPosIn, double yPosIn)
	{
		if (this->m_MouseButton)
		{
			float xpos = static_cast<float>(xPosIn);
			float ypos = static_cast<float>(yPosIn);

			if (this->m_FirstMouse)
			{
				this->m_LastX = xpos;
				this->m_LastY = ypos;
				this->m_FirstMouse = false;
			}

			float xoffset = xpos - this->m_LastX;
			float yoffset = this->m_LastY - ypos;

			this->m_LastX = xpos;
			this->m_LastY = ypos;

			xoffset *= this->m_MouseSensity;
			yoffset *= this->m_MouseSensity;

			this->m_Yaw += xoffset;
			this->m_Pitch += yoffset;

			if (this->m_Pitch > 89.0f)
			{
				this->m_Pitch = 89.0f;
			}
			if (this->m_Pitch < -89.0f)
			{
				this->m_Pitch = -89.0f;
			}

			this->updateCameraVectors();
		}
	}

	void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
	{
		this->m_Zoom -= static_cast<float>(yoffset);
		if (this->m_Zoom < 1.0f)
		{
			this->m_Zoom = 1.0f;
		}
		if (this->m_Zoom > 90.0f)
		{
			this->m_Zoom = 90.0f;
		}
	}

private:
	float m_DeltaTime;
	float m_LastFrame;
	float m_MouseSensity;

	float m_MovementSpeed;

	float m_Yaw;
	float m_Pitch;

	bool m_FirstMouse;
	bool m_MouseButton;

	float m_LastX;
	float m_LastY;

	void updateCameraVectors()
	{
		glm::vec3 frontVec {};
		frontVec.x = cos(glm::radians(this->m_Yaw)) * cos(glm::radians(this->m_Pitch));
		frontVec.y = sin(glm::radians(this->m_Pitch));
		frontVec.z = sin(glm::radians(this->m_Yaw)) * cos(glm::radians(this->m_Pitch));

		this->front = glm::normalize(frontVec);
		glm::vec3 right = glm::normalize(glm::cross(this->front, this->worldUp));
		this->up = glm::normalize(glm::cross(right, this->front));
	}
};
