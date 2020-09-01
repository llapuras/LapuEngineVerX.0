#pragma once
#include <iostream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Camera.h"

enum LightType
{
	Dir,
	Point,
	Spot
};

struct DirectionLight {
	glm::vec3 direction = glm::vec3(-0.2f, -1.0f, -0.3f);
	glm::vec3 ambient = glm::vec3(0.2f, 0.2f, 0.2f);
	glm::vec3 diffuse = glm::vec3(0.4f, 0.4f, 0.4f);
	glm::vec3 specular = glm::vec3(0.5f, 0.5f, 0.5f);
	float intensity = 1.0f;
};

struct PointLight {
	glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 ambient = glm::vec3(0.0f, 0.4f, 0.9f);
	glm::vec3 diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
	glm::vec3 specular = glm::vec3(1.0f, 1.0f, 1.0f);
	float constant = 1.0f;
	float linear = 0.09f;
	float quadratic = 0.032f;
};

struct SpotLight {
	glm::vec3 position = glm::vec3(10, 5, -4);
	glm::vec3 ambient = glm::vec3(0.3f, 1.0f, 0.4f);
	glm::vec3 direction = glm::vec3(0, -1, 0);
	glm::vec3 diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 specular = glm::vec3(1.0f, 1.0f, 1.0f);;
	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
	float constant = 3.0f;
	float linear = 1.0f;
	float quadratic = 0.002f;
	float cutOff = glm::cos(glm::radians(12.5f));
	float outerCutOff = glm::cos(glm::radians(15.0f));
};

class Light {
public:

	void RenderDirLight(Shader shader, DirectionLight dirLight)
	{
		shader.setVec3("dirLight.direction", dirLight.direction);
		shader.setVec3("dirLight.ambient", dirLight.ambient);
		shader.setVec3("dirLight.diffuse", dirLight.diffuse);
		shader.setVec3("dirLight.specular", dirLight.specular);
		shader.setFloat("dirLight.intensity", dirLight.intensity);
	}

	void RenderPointLight(Shader shader, PointLight pointLight)
	{
		shader.setVec3("pointLight.position", pointLight.position);
		shader.setVec3("pointLight.ambient", pointLight.ambient);
		shader.setVec3("pointLight.diffuse", pointLight.diffuse);
		shader.setVec3("pointLight.specular", pointLight.specular);
		shader.setFloat("pointLight.constant", pointLight.constant);
		shader.setFloat("pointLight.linear", pointLight.linear);
		shader.setFloat("pointLight.quadratic", pointLight.quadratic);
	}

	void RenderSpotLight(Shader shader, SpotLight spotLight)
	{
		shader.setVec3("spotLight.position", spotLight.position);
		shader.setVec3("spotLight.direction", spotLight.direction);
		shader.setVec3("spotLight.ambient", spotLight.ambient);
		shader.setVec3("spotLight.diffuse", spotLight.diffuse);
		shader.setVec3("spotLight.specular", spotLight.specular);
		shader.setFloat("spotLight.constant", spotLight.constant);
		shader.setFloat("spotLight.linear", spotLight.linear);
		shader.setFloat("spotLight.quadratic", spotLight.quadratic);
		shader.setFloat("spotLight.cutOff", spotLight.cutOff);
		shader.setFloat("spotLight.outerCutOff", spotLight.outerCutOff);
	}
};


