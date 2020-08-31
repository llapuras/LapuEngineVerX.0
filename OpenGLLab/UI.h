#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_internal.h"
#include "ImageLoader.h"
#include "SceneNode.h"



SceneNode* ui_root = NULL;

int scenelistcount=0;
bool node_clicked = -1;

bool LoadTextureFromFile(char const* filename, GLuint* out_texture, int* out_width, int* out_height);
ImageLoader uiImg;

//global variables
//-----------------------------------
//bool
bool show_demo_window = true;
bool show_another_window = false;
bool show_Mouse = true;
bool isShowShadow = true;
bool autoMove = false;

bool transform_isvisible = true;

float pos[3] = { 1.0f,1.0f,1.0f};
float scale[3] = { 1.0f,1.0f,1.0f};
float rotation[3] = { 1.0f,1.0f,1.0f };


//water
float DRAG_MULT = .001f;

ImGuiWindowFlags nothingleft_flags =
ImGuiWindowFlags_NoTitleBar
| ImGuiWindowFlags_NoScrollbar
| ImGuiWindowFlags_NoNav
| ImGuiWindowFlags_NoBackground
| ImGuiWindowFlags_NoBringToFrontOnFocus;

// 20.03.04 - new 
bool show_Menu = true;

class UI {
public:

	UI(GLFWwindow* window) {
		ImGui::CreateContext();
		IMGUI_CHECKVERSION();
		ImGuiIO& io = ImGui::GetIO();  (void)io;
		io.MouseDrawCursor = true;
		ImGui::StyleColorsDark();
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 330");

		ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	};

	~UI() {
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void Render(GLFWwindow* window) {

		int display_w, display_h;
		glfwMakeContextCurrent(window);
		glfwGetFramebufferSize(window, &display_w, &display_h);
		ImVec2 windowSize = ImVec2(display_w, display_h);
		ImVec2 boardSize = ImVec2(display_w*0.2f, display_h*0.98f);

		ImGui_ImplOpenGL3_NewFrame();
	    ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		const float DISTANCE = 10.0f;
		static int corner = 10;	//corner offset
		ImGuiIO& io = ImGui::GetIO();
		ImVec2 window_pos = ImVec2((corner & 1) ? io.DisplaySize.x - DISTANCE : DISTANCE, (corner & 2) ? io.DisplaySize.y - DISTANCE : DISTANCE);
		ImVec2 window_pos_pivot = ImVec2((corner & 1) ? 1.0f : 0.0f, (corner & 2) ? 1.0f : 0.0f);
		ImVec2 window_pos2 = ImVec2(io.DisplaySize.x - boardSize.x - DISTANCE, (corner & 2) ? io.DisplaySize.y - DISTANCE : DISTANCE);
		ImVec2 window_pos_pivot2 = ImVec2((corner & 1) ? 1.0f : 0.0f, (corner & 2) ? 1.0f : 0.0f);

		//Main UI Board
		ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
		ImGui::SetNextWindowSize(boardSize);
		SceneTreeBoard(); 
		static bool selection[] = { false, false, false, false, false };


		ImGui::SetNextWindowPos(window_pos2, ImGuiCond_Always, window_pos_pivot2);
		ImGui::SetNextWindowSize(boardSize);
		InspectorBoard();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	};

	void DrawSceneTree(SceneNode* xnode) {
		if (xnode != nullptr) {		
			//cout << xnode->GetChildCounts() << endl;
			if (xnode->GetChildCounts() > 1) {
				bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)xnode->name, ImGuiTreeNodeFlags_DefaultOpen, xnode->name, xnode->name);
				if (ImGui::IsItemClicked()) {
					node_clicked = xnode->name;
					cout << xnode->name << endl;
				}
				if (node_open)
				{
					for (vector <SceneNode*>::const_iterator i = xnode->GetChildIteratorStart(); i != xnode->GetChildIteratorEnd(); ++i)
					{
						DrawSceneTree(*i);
					}
					ImGui::TreePop();
				}
			}
			else {	
				ImGui::TreeNodeEx(xnode->name, ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen);
				if (ImGui::IsItemClicked()) {
					node_clicked = xnode->name;
					cout << xnode->name << endl;
				}
			}
		}		
	}
	

	bool SceneTreeBoard() {
		int obj_selected = -1;
		if (ImGui::IsItemClicked())
			cout<<"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaas"<<endl;

		ImGui::Begin("Scene Tree", NULL);
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return false;

		//search box
		static ImGuiTextFilter filter;
		filter.Draw();
		const char* lines[] = { "aaaa" };//需要改成tree里的名字合集
		for (int i = 0; i < IM_ARRAYSIZE(lines); i++)
			if (filter.PassFilter(lines[i]))
				ImGui::BulletText("%s", lines[i]);

		ImGui::Separator();

		//scene list
		DrawSceneTree(ui_root);




		ImGui::End();

	}

	bool InspectorBoard() {

		ImGui::Begin("Transform", NULL);
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return false;
		
		//visible
		ImGui::Checkbox("is visible", &transform_isvisible);

		ImGui::DragFloat3("Position", (float*)&pos, 0.01f, -200.0f, 200.0f, "%.0f");
		ImGui::DragFloat3("Scale", (float*)&scale, 0.01f, -200.0f, 200.0f, "%.0f");
		ImGui::DragFloat3("Rotation", (float*)&rotation, 1.0f, -360.0f, 360, "%.0f");


		//object info
		ImGui::Separator();

		ImGui::Text("Info");

		if (transform_isvisible) {
			ImGui::Text("Active");
		}
		else {
			ImGui::Text("Not Active");
		}

		ImGui::Separator();
		
		ImGui::End();
	}


	bool GolfGameMainMenu() {
		
		ImGui::Begin("Progress Indicators", NULL, nothingleft_flags);
		const ImU32 col = ImGui::GetColorU32(ImGuiCol_ButtonHovered);
		const ImU32 bg = ImGui::GetColorU32(ImGuiCol_Button);

		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return false;

		bool ret = uiImg.loadTexture("Images/GameStart.jpg");
		IM_ASSERT(ret);
		ImGui::Image((void*)(intptr_t)uiImg.image_texture, ImVec2(uiImg.image_width, uiImg.image_height));

		if (ImGui::Button("Game Start", ImVec2(760, 45))) {
			//isShow = false;
			GolfGameInGame();
		};
		if (ImGui::Button("Options", ImVec2(760, 45))) {
		};
		if (ImGui::Button("Exit Game", ImVec2(760, 45))) {
		};

		ImGui::End();
	}

	bool GolfGameInGame() {		
		ImGui::Begin("In Game", NULL, nothingleft_flags);

		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return false;

		//LoadImage("../Images/GameStart.jpg");
		int my_image_width = 0;
		int my_image_height = 0;
		GLuint my_image_texture = 0;
		bool ret = uiImg.loadTexture("Images/GameStart.jpg");
		IM_ASSERT(ret);
		ImGui::Image((void*)(intptr_t)my_image_texture, ImVec2(my_image_width, my_image_height));

		if (ImGui::Button("Back to Menu", ImVec2(760, 45))) {

		};

		ImGui::End();

	}

	// progress bar animation
	bool BufferingBar(const char* label, float value, const ImVec2& size_arg, const ImU32& bg_col, const ImU32& fg_col) {
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);

		ImVec2 pos = window->DC.CursorPos;
		ImVec2 size = size_arg;
		size.x -= style.FramePadding.x * 2;

		const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
		ImGui::ItemSize(bb, style.FramePadding.y);
		if (!ImGui::ItemAdd(bb, id))
			return false;

		// Render
		const float circleStart = size.x * 0.7f;
		const float circleEnd = size.x;
		const float circleWidth = circleEnd - circleStart;

		window->DrawList->AddRectFilled(bb.Min, ImVec2(pos.x + circleStart, bb.Max.y), bg_col);
		window->DrawList->AddRectFilled(bb.Min, ImVec2(pos.x + circleStart * value, bb.Max.y), fg_col);

		const float t = g.Time;
		const float r = size.y / 2;
		const float speed = 1.5f;

		const float a = speed * 0;
		const float b = speed * 0.333f;
		const float c = speed * 0.666f;

		const float o1 = (circleWidth + r) * (t + a - speed * (int)((t + a) / speed)) / speed;
		const float o2 = (circleWidth + r) * (t + b - speed * (int)((t + b) / speed)) / speed;
		const float o3 = (circleWidth + r) * (t + c - speed * (int)((t + c) / speed)) / speed;

		window->DrawList->AddCircleFilled(ImVec2(pos.x + circleEnd - o1, bb.Min.y + r), r, bg_col);
		window->DrawList->AddCircleFilled(ImVec2(pos.x + circleEnd - o2, bb.Min.y + r), r, bg_col);
		window->DrawList->AddCircleFilled(ImVec2(pos.x + circleEnd - o3, bb.Min.y + r), r, bg_col);
	
	}

	// loading spinner animation
	bool Spinner(const char* label, float radius, int thickness, const ImU32& color) {
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);

		ImVec2 pos = window->DC.CursorPos;
		ImVec2 size((radius) * 2, (radius + style.FramePadding.y) * 2);

		const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
		ImGui::ItemSize(bb, style.FramePadding.y);
		if (!ImGui::ItemAdd(bb, id))
			return false;

		// Render
		window->DrawList->PathClear();

		int num_segments = 30;
		int start = abs(ImSin(g.Time * 1.8f) * (num_segments - 5));

		const float a_min = IM_PI * 2.0f * ((float)start) / (float)num_segments;
		const float a_max = IM_PI * 2.0f * ((float)num_segments - 3) / (float)num_segments;

		const ImVec2 centre = ImVec2(pos.x + radius, pos.y + radius + style.FramePadding.y);

		for (int i = 0; i < num_segments; i++) {
			const float a = a_min + ((float)i / (float)num_segments) * (a_max - a_min);
			window->DrawList->PathLineTo(ImVec2(centre.x + ImCos(a + g.Time * 8) * radius,
				centre.y + ImSin(a + g.Time * 8) * radius));
		}

		window->DrawList->PathStroke(color, false, thickness);
	}

	void RenderImage(const char* path) {
		int my_image_width = 0;
		int my_image_height = 0;
		GLuint my_image_texture = 0;
		bool ret = uiImg.loadTexture(path);
		IM_ASSERT(ret);
		ImGui::Image((void*)(intptr_t)my_image_texture, ImVec2(my_image_width, my_image_height));
	}
};

//use stb_image.h
//- for loading image
bool LoadTextureFromFile(const char* filename, GLuint* out_texture, int* out_width, int* out_height)
{
	// Load from file
	int image_width = 0;
	int image_height = 0;
	unsigned char* image_data = stbi_load(filename, &image_width, &image_height, NULL, 4);
	if (image_data == NULL)
		return false;

	// Create a OpenGL texture identifier
	GLuint image_texture;
	glGenTextures(1, &image_texture);
	glBindTexture(GL_TEXTURE_2D, image_texture);

	// Setup filtering parameters for display
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Upload pixels into texture
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
	stbi_image_free(image_data);

	*out_texture = image_texture;
	*out_width = image_width;
	*out_height = image_height;

	return true;
}