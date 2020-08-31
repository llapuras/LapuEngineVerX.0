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
	const char* name;
	Model model;
	int childcount = 0;
	int allchildcount = 0;
	std::vector<SceneNode*> children;
	glm::vec3 transform;
	glm::vec3 modelScale;
	glm::vec3 worldTransform;
	glm::vec3 worldScale;
	glm::vec3 rotation;
	float scale = 1.0f;
	bool isVisible = true;

	const char* namelist = nullptr;

	SceneNode(const char* name = "GameObject", SceneNode* parent = NULL) {
		this->model = Model();
		this->name = name;
		modelScale = glm::vec3(1, 1, 1);
		transform = glm::vec3(0, 0, 0);
		rotation = glm::vec3(0, 0, 0);
		worldTransform = glm::vec3(0, 0, 0);
		worldScale = glm::vec3(1, 1, 1);
		this->parent = parent;
	}

	SceneNode(Model nodemodel, const char* name = "GameObject", SceneNode* parent = NULL) {
		this->model = nodemodel;
		this->name = name;
		modelScale = glm::vec3(1, 1, 1);
		transform = glm::vec3(0, 0, 0);
		worldTransform = glm::vec3(0, 0, 0);
		rotation = glm::vec3(0, 0, 0);
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

	//in all childs range
	SceneNode* FindNodeInChildsByName(const char* name) {
		for (vector <SceneNode*>::const_iterator i = this->GetChildIteratorStart(); i != this->GetChildIteratorEnd(); ++i) {
			if ((*i)->name == name) {
				cout << (*i)->name << endl;
				return *i;
			}
			else {
				(*i)->FindNodeInChildsByName(name);
			}
		}
		return this;
	}

	void Draw(Camera camera, Shader shader) {
		if (this->isVisible) {//only visible go can be drawn
			for (vector <SceneNode*>::const_iterator i = this->GetChildIteratorStart(); i != this->GetChildIteratorEnd(); ++i) {
				if ((*i)->isVisible) {
					shader.use();
					glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
					glm::mat4 view = camera.GetViewMatrix();
					shader.setMat4("projection", projection);
					shader.setMat4("view", view);
					glm::mat4 modelmat4 = glm::mat4(1.0f);
					modelmat4 = glm::scale(modelmat4, (*i)->modelScale * worldScale);	// it's a bit too big for our scene, so scale it down
					modelmat4 = glm::translate(modelmat4, (*i)->transform + worldTransform / ((*i)->scale)); // translate it down so it's at the center of the scene
					//rotation
					modelmat4 = glm::rotate(modelmat4, glm::radians((*i)->rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));//xaxis
					modelmat4 = glm::rotate(modelmat4, glm::radians((*i)->rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));//yaxis
					modelmat4 = glm::rotate(modelmat4, glm::radians((*i)->rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));//zaxis
					shader.setMat4("model", modelmat4);
					((*i)->model).Draw(shader);

					//draw childs
					(*i)->Draw(camera, shader);
				}
			}
		}
		else {
			cout<<"aaaaaaaaaaaaaaaaa"<<endl;
			return;
		}
	}

	bool IsExit(const char* name) {
		for (vector <SceneNode*>::const_iterator i = this->GetChildIteratorStart(); i != this->GetChildIteratorEnd(); ++i) {
			if((*i)->name == name)
				return true;
			else
				return false;
		}
	}

	//get all childs count(include 1st one)
	int GetAllChildsCounts() {
		for (vector <SceneNode*>::const_iterator i = this->GetChildIteratorStart(); i != this->GetChildIteratorEnd(); ++i) {
			allchildcount++;
			allchildcount+=(*i)->GetAllChildsCounts();
		}
		return allchildcount;
	}
	
	//get 1st layer childs count
	int GetChildCounts() {
		childcount = 0;
		for (vector <SceneNode*>::const_iterator i = this->GetChildIteratorStart(); i != this->GetChildIteratorEnd(); ++i) {
			childcount++;
		}
		return childcount;
	}

	void GetObjectNameList() {
		//namelist.
	}

	std::vector<SceneNode*>::const_iterator GetChildIteratorStart() {
		return children.begin();
	}

	std::vector<SceneNode*>::const_iterator GetChildIteratorEnd() {
		return children.end();
	}


};