#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "stb_image.h"
#include "Camera.h"
#include "UI.h"
#include "Shader.h"
#include "Model.h"
#include "Light.h"
#include "SkyBox.h"
#include "Heightmap.h"
#include "SceneNode.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void ResetPos(GLFWwindow* window);

// camera
Camera camera(glm::vec3(0.0f, -5.0f, 20.0f));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;


void AddGameObject(string path) {
	Model newgo("Model/land/land.obj");

}

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwGetPrimaryMonitor()
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LapuEngine", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	glEnable(GL_DEPTH_TEST);

	//environment shader
	Shader environshader("Shaders/environmentvs.glsl", "Shaders/environmentfs.glsl");
	Shader modelshader("Shaders/modelload_vs.glsl", "Shaders/modelload_fs.glsl");
	Shader watershader("Shaders/watervs.glsl", "Shaders/waterfs.glsl");
	Shader heightmapshader("Shaders/heightmapvs.glsl", "Shaders/heightmapfs.glsl");
	Shader shader("Shaders/shadowmapvs.glsl", "Shaders/shadowmapfs.glsl");
	Shader simpleDepthShader("Shaders/shadowvs.glsl", "Shaders/shadowfs.glsl");

	Light lit;
	DirectionLight dirLit;
	PointLight poiLit;
	SpotLight spotLit;

	UI ui(window);
	Heightmap land = Heightmap();
	Heightmap land2 = Heightmap();
	Heightmap water = Heightmap();
	SkyBox sky = SkyBox();


	//EnvirMap Scene
	Model castle("Model/land/monu9.obj");
	Model board("Model/land/land.obj");
	SceneNode* root2 = new SceneNode(board, "root2");
	SceneNode* castlex = new SceneNode(castle, "bush");
	root2->AddChild(castlex);

	
	//No Shadow Scene
	Light light01;
	DirectionLight dirLight;
	PointLight pointLight;
	PointLight pointLight02;
	SpotLight spotLight01;
	Model landx("Model/land/land.obj");
	Model bush("Model/land/bush.obj");
	Model campfire("Model/land/campfire.obj");
	Model mushroom("Model/land/mushroom.obj");
	Model tree("Model/land/tree.obj");
	Model lamp("Model/land/lamp01.obj");
	Model piano("Model/POD.obj");

	SceneNode* root = new SceneNode(landx, "root");
	SceneNode* bush_child = new SceneNode(bush, "bush");
	SceneNode* tree_child = new SceneNode(tree, "tree");
	SceneNode* tree_child01 = new SceneNode(tree, "tree01");
	SceneNode* campfire_child = new SceneNode(campfire, "campfire");
	SceneNode* mushroom_child01 = new SceneNode(mushroom, "mushroom01");
	SceneNode* mushroom_child02 = new SceneNode(mushroom, "mushroom02");
	SceneNode* mushroom_child03 = new SceneNode(mushroom, "mushroom03");
	SceneNode* lamp_child = new SceneNode(lamp, "lamp");

	SceneNode* pianox = new SceneNode(piano, "piano");

	//root->AddChild(bush_child);
	//root->AddChild(tree_child);
	//root->AddChild(tree_child01);
	//root->AddChild(campfire_child);
	//root->AddChild(mushroom_child01);
	//root->AddChild(mushroom_child02);
	//root->AddChild(mushroom_child03);
	//root->AddChild(lamp_child);

	Model monu9("Model/land/monu9.obj");
	SceneNode* castle_child = new SceneNode(monu9, "land");
	

	tree_child->transform = glm::vec3(-2, 0, -3);
	tree_child01->transform = glm::vec3(3, 0, -2);
	bush_child->transform = glm::vec3(6, 0, -4);
	campfire_child->transform = glm::vec3(2, 0, -11);
	mushroom_child01->transform = glm::vec3(-2, 0, -10);
	mushroom_child02->transform = glm::vec3(4, 0, -5);
	mushroom_child03->transform = glm::vec3(3, 0, -2);
	lamp_child->transform = glm::vec3(8.6f, 1.14f, -2.3f);
	root->worldScale = glm::vec3(0.4, 0.4, 0.4);

	// Configure depth map FBO
	const GLuint SHADOW_WIDTH = 1500, SHADOW_HEIGHT = 1500;
	GLuint depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);
	// - Create depth texture
	GLuint depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// Set texture samples
	shader.use();
	shader.setInt("diffuseTexture", 0);
	shader.setInt("shadowMap", 1);
	// Shadow Light source
	glm::vec3 lightPos(-2, 4.0f, -1.0f);

	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	glm::mat4 view = camera.GetViewMatrix();
	glm::mat4 model = glm::mat4(1.0f);

	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		if (autoMove) {
			camera.AutoMove();
		}
		else {
			ResetPos(window);
			//camera.Position = glm::vec3(posx, posy, posz);
		}
		
		processInput(window);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		//draw shadow scene
		lightPos = glm::vec3(-4.2, 8.4, 9);
		glm::mat4 lightProjection, lightView;
		glm::mat4 lightSpaceMatrix;
		GLfloat near_plane = 0.1f, far_plane = 27.5f;
		lightProjection = glm::ortho(-5.0f, 10.0f, -5.0f, 5.0f, near_plane, far_plane);
		lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
		lightSpaceMatrix = lightProjection * lightView;
		simpleDepthShader.use();
		simpleDepthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		glActiveTexture(GL_TEXTURE0);

		pianox->modelScale = glm::vec3(scale[0], scale[1], scale[2]);
		pianox->scale = 2;
		pianox->transform = glm::vec3(pos[0], pos[1], pos[2]);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		
		shader.use();
		projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		view = camera.GetViewMatrix();
		shader.setMat4("projection", projection);
		shader.setMat4("view", view);
		shader.setVec3("viewPos", camera.Position);
		shader.setVec3("lightPos", lightPos);
		shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
		glActiveTexture(GL_TEXTURE0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMap);

		if (transform_isvisible) {
			root->SetVisible(transform_isvisible);
			root->AddChild(pianox);
		}
		else {
			//root->DeleteChild(castle_child);		
			root->SetVisible(transform_isvisible);
		}

		//root->worldTransform = glm::vec3(-20, -26, 23);
		//root->Draw(camera, simpleDepthShader);

		if (isShowShadow) {
			root->worldTransform = glm::vec3(-20, -26, 23);
			root->Draw(camera, shader);
		}

		watershader.setInt("DRAG_MULT", DRAG_MULT);

		//Height map and water effect
		//water.RenderWater(camera, watershader);

		//draw no shadow scene
		modelshader.use();
		modelshader.setVec3("viewPos", camera.Position);
		modelshader.setFloat("material.shininess", 32.0f);

		//dir light
		dirLight.direction = glm::vec3(-1, 1, -1);
		dirLight.diffuse = glm::vec3(-1, -0.04, 0.9);
		light01.RenderDirLight(modelshader, dirLight);
		//campfire
		pointLight.position = glm::vec3(3.4f, -9, -6);
		pointLight.ambient = glm::vec3(1.0f, 0, 0);
		pointLight.diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
		//pointLight.position = glm::vec3(litposx, litposy, litposz);
		light01.RenderPointLight(modelshader, pointLight);
		//spot light
		//spotLight01.position = glm::vec3(litposx, litposy, litposz);
		spotLight01.position = glm::vec3(5.4, -5.4, -3);
		spotLight01.direction = glm::vec3(0, -1, 0);
		light01.RenderSpotLight(modelshader, spotLight01);

		model = glm::mat4(1.0f);
		view = camera.GetViewMatrix();
		projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		modelshader.setMat4("model", model);
		modelshader.setMat4("view", view);
		modelshader.setMat4("projection", projection);
		modelshader.setVec3("cameraPos", camera.Position);
		root->worldTransform = glm::vec3(4, -26, -3);
		root->Draw(camera, modelshader);

		sky.DrawSkyBox(camera, sky.shader);
		ui.Render(window);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	//error↓
	//ui.Clear();
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);

	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		camera.ProcessKeyRotate(ROTATE_RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		camera.ProcessKeyRotate(ROTATE_LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		camera.ProcessKeyRotate(ROTATE_UP, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		camera.ProcessKeyRotate(ROTATE_DOWN, deltaTime);

	//if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	//{
	//	if (heightScale > 0.0f)
	//		heightScale -= 0.0005f;
	//	else
	//		heightScale = 0.0f;
	//}
	//else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
	//{
	//	if (heightScale < 1.0f)
	//		heightScale += 0.0005f;
	//	else
	//		heightScale = 1.0f;
	//}
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	//if (firstMouse)
	//{
	//	lastX = xpos;
	//	lastY = ypos;
	//	firstMouse = false;
	//}

	//float xoffset = xpos - lastX;
	//float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	//lastX = xpos;
	//lastY = ypos;

	//camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

void ResetPos(GLFWwindow* window) {
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
}
