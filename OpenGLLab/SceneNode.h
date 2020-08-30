#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Model.h"
#include "Camera.h"

class SceneNode {
public:
	SceneNode* parent;
	string name;
	Model model;
	std::vector<SceneNode*> children;
	glm::vec3 transform;
	glm::vec3 modelScale;
	glm::vec3 worldTransform;
	glm::vec3 worldScale;
	float scale = 1.0f;
	bool isVisible = true;

	SceneNode(Model nodemodel, string name = "GameObject", SceneNode* parent = NULL) {
		this->model = nodemodel;
		this->name = name;
		modelScale = glm::vec3(1, 1, 1);
		transform = glm::vec3(0, 0, 0);
		worldTransform = glm::vec3(0, 0, 0);
		worldScale = glm::vec3(1, 1, 1);
		this->parent = parent;
	}

	bool SetVisible(bool state) {
		this->isVisible = state;
		return this->isVisible;
	}

	void DeleteSceneNode(void) {
		for (unsigned int i = 0; i < children.size(); ++i) {
			delete children[i];
		}
	}

	void AddChild(SceneNode* s) {
		children.push_back(s);
		s->parent = this;
	}

	//bug
	void DeleteChild(SceneNode* s) {
		if (s->parent = this) {
			swap(s, *(std::end(children) - 1));//µÈÍ¬ÓÚ swap(demo[1],demo[4])
			children.pop_back();
		}
		else {
			
		}
		
	}

	void Draw(Camera camera, Shader shader) {
		if (this->isVisible) {//only visible go can be drawn
			for (vector <SceneNode*>::const_iterator i = this->GetChildIteratorStart(); i != this->GetChildIteratorEnd(); ++i) {
				shader.use();
				glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
				glm::mat4 view = camera.GetViewMatrix();
				shader.setMat4("projection", projection);
				shader.setMat4("view", view);
				glm::mat4 modelmat4 = glm::mat4(1.0f);
				modelmat4 = glm::scale(modelmat4, (*i)->modelScale * worldScale);	// it's a bit too big for our scene, so scale it down
				modelmat4 = glm::translate(modelmat4, (*i)->transform + worldTransform / ((*i)->scale)); // translate it down so it's at the center of the scene
				shader.setMat4("model", modelmat4);
				((*i)->model).Draw(shader);
				//cout << ((*i)->name) << endl;
			}
		}
		else {

		}
	}

	std::vector<SceneNode*>::const_iterator GetChildIteratorStart() {
		return children.begin();
	}

	std::vector<SceneNode*>::const_iterator GetChildIteratorEnd() {
		return children.end();
	}


};