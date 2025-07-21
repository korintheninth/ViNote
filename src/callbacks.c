#include "../libs/vinote.h"
#include <math.h>
#include <limits.h>

int active_container_index = -1;
int is_typing = 0;
int is_dragging = 0;
int last_mouse_x = 0;
int last_mouse_y = 0;

int is_command = 0;

void errorCallback(int error, const char* description) {
	fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	double mouseX, mouseY;
	if (appConfig.snapToGrid) {
		mouseX = snapPos[0];
		mouseY = snapPos[1];
	}
	else {
		glfwGetCursorPos(window, &mouseX, &mouseY);
		mouseY = height - mouseY;
	}
	if (button == GLFW_MOUSE_BUTTON_LEFT
		&& action == GLFW_PRESS) 
	{
		int index = checkHitbox((int)mouseX, (int)mouseY);
		if (index == -2) {
			return;
		}
		if (index != -1) {
			active_container_index = index;
			is_dragging = 1;
			last_mouse_x = (int)mouseX;
			last_mouse_y = (int)mouseY;
			is_typing = 1;
			return;
		}
		double xpos, ypos;
		if (appConfig.snapToGrid) {
			xpos = snapPos[0];
			ypos = snapPos[1];
		}
		else {
			glfwGetCursorPos(window, &xpos, &ypos);
			ypos = height - ypos;
		}
		createContainer((int)xpos, (int)ypos, 24, 1.0f, (float[]){0.0f, 0.0f, 0.0f}, "", "fonts/ARIAL.TTF");
		active_container_index = containerCount() - 1;
		is_typing = 1;
	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT
		&& action == GLFW_RELEASE) 
		is_dragging = 0;
}

void charCallback(GLFWwindow* window, unsigned int codepoint) {
	if (active_container_index != -1 && is_typing && !is_command) {
		Container* container = &containers[active_container_index];
		int len = strlen(container->text);
		container->text = realloc(container->text, len + 2);
		if (container->text == NULL) {
			fprintf(stderr, "Failed to allocate memory for container text\n");
			return;
		}
		container->text[len] = (char)codepoint;
		container->text[len + 1] = '\0';
		container->width = containerWidth(container);
	}
	is_command = 0;
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == appConfig.saveKey % 65536
		&& (mods & appConfig.saveKey / 65536 || (appConfig.saveKey / 65536 == 0 && mods == 0))
		&& action == GLFW_PRESS)
	{
		saveFile("notes.vn");
	}
	if (active_container_index != -1) {
		if (key == GLFW_KEY_BACKSPACE
			&& (action == GLFW_PRESS || action == GLFW_REPEAT))
		{
			Container* container = &containers[active_container_index];
			int len = strlen(container->text);
			if (len > 0) {
				container->text[len - 1] = '\0';
				container->text = realloc(container->text, len);
				if (container->text == NULL) {
					fprintf(stderr, "Failed to allocate memory for container text\n");
					return;
				}
				container->width = containerWidth(container);
				container->height = containerHeight(container);
			}
		}
		else if (key == GLFW_KEY_ENTER
			&& action == GLFW_PRESS
			&& is_typing)
		{
			Container* container = &containers[active_container_index];
			int len = strlen(container->text);
			container->text = realloc(container->text, len + 2);
			if (container->text == NULL) {
				fprintf(stderr, "Failed to allocate memory for container text\n");
				return;
			}
			container->text[len] = '\n';
			container->text[len + 1] = '\0';
			container->width = containerWidth(container);
			container->height = containerHeight(container);
		}
		else if (key == GLFW_KEY_DELETE
			&& action == GLFW_PRESS)
		{
			for (int i = active_container_index; containers[i].width != -1; i++) {
				containers[i] = containers[i + 1];
			}
			active_container_index = -1;
			is_typing = 0;
		}
		else if ((key == appConfig.unselectKey % 65536
			&& (mods & appConfig.unselectKey / 65536 || (appConfig.unselectKey / 65536 == 0 && mods == 0)))
			&& action == GLFW_PRESS
			|| (key == GLFW_KEY_ESCAPE
			&& action == GLFW_PRESS))
		{
			if (!is_typing)
				active_container_index = -1;
			is_typing = 0;
		}
	}
	else if (key == appConfig.selectKey % 65536
		&& (mods &  appConfig.selectKey / 65536 || (appConfig.selectKey / 65536 == 0 && mods == 0))
		&& action == GLFW_PRESS)
	{
		double xpos, ypos;
		if (appConfig.snapToGrid) {
			xpos = snapPos[0];
			ypos = snapPos[1];
		}
		else {
			int width, height;
			glfwGetWindowSize(window, &width, &height);
			glfwGetCursorPos(window, &xpos, &ypos);
			ypos = height - ypos;
		}
		int index = checkHitbox((int)xpos, (int)ypos);
		if (index == -2) {
			return;
		}
		active_container_index = index;
	}
	else if (key == appConfig.insertKey % 65536
		&& (mods & appConfig.insertKey / 65536 || (appConfig.insertKey / 65536 == 0 && mods == 0))
		&& action == GLFW_PRESS)
	{
		is_typing = 1;
		is_command = 1;
		double xpos, ypos;
		if (appConfig.snapToGrid) {
			xpos = snapPos[0];
			ypos = snapPos[1];
		}
		else {
			int width, height;
			glfwGetWindowSize(window, &width, &height);
			glfwGetCursorPos(window, &xpos, &ypos);
			ypos = height - ypos;
		}
		int index = checkHitbox((int)xpos, (int)ypos);
		if (index == -2) {
			return;
		}
		if (index != -1) {
			active_container_index = index;
			return;
		}
		createContainer((int)xpos, (int)ypos, 24, 1.0f, (float[]){0.0f, 0.0f, 0.0f}, "", "fonts/ARIAL.TTF");
		active_container_index = containerCount() - 1;
	}
	else if (key == appConfig.snapKey % 65536
		&& (mods & appConfig.snapKey / 65536 || (appConfig.snapKey / 65536 == 0 && mods == 0))
		&& action == GLFW_PRESS)
	{
		appConfig.snapToGrid = !appConfig.snapToGrid;
		if (appConfig.snapToGrid) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
		} else {
			int width, height;
			glfwGetWindowSize(window, &width, &height);
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			glfwSetCursorPos(window, snapPos[0], height - snapPos[1]);
		}
	}
	else if (appConfig.snapToGrid) {
		if (key == appConfig.leftKey % 65536
			&& (mods & appConfig.leftKey / 65536 || (appConfig.leftKey / 65536 == 0 && mods == 0))
			&& (action == GLFW_PRESS || action == GLFW_REPEAT))
			snapPos[0] -= appConfig.gridSpacing;
		else if (key == appConfig.rightKey % 65536
			&& (mods & appConfig.rightKey / 65536 || (appConfig.rightKey / 65536 == 0 && mods == 0))
			&& (action == GLFW_PRESS || action == GLFW_REPEAT))
			snapPos[0] += appConfig.gridSpacing;
		else if (key == appConfig.upKey % 65536
			&& (mods & appConfig.upKey / 65536 || (appConfig.upKey / 65536 == 0 && mods == 0))
			&& (action == GLFW_PRESS || action == GLFW_REPEAT))
			snapPos[1] += appConfig.gridSpacing;
		else if (key == appConfig.downKey % 65536
			&& (mods & appConfig.downKey / 65536 || (appConfig.downKey / 65536 == 0 && mods == 0))
			&& (action == GLFW_PRESS || action == GLFW_REPEAT))
			snapPos[1] -= appConfig.gridSpacing;
		int width, height;
		glfwGetWindowSize(window, &width, &height);
		glfwSetCursorPos(window, snapPos[0], height - snapPos[1]);
	}
}

void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos) {
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	int snapx = (int)round(xpos / (appConfig.gridSpacing)) * (appConfig.gridSpacing);
	int snapy = (int)round(ypos / (appConfig.gridSpacing)) * (appConfig.gridSpacing);
	snapPos[0] = snapx;
	snapPos[1] = height - snapy;

	if (is_dragging && active_container_index != -1) {
		Container* container = &containers[active_container_index];
		int dy;
		int dx;
		if (appConfig.snapToGrid) {
			dx = snapPos[0] - last_mouse_x;
			dy = snapPos[1] - last_mouse_y;
		}
		else {
			dx = (int)xpos - last_mouse_x;
			dy = height - (int)ypos - last_mouse_y;
		}
		container->x += dx;
		container->y += dy;
	}
	if (appConfig.snapToGrid) {
		last_mouse_x = snapPos[0];
		last_mouse_y = snapPos[1];
	}
	else {
		last_mouse_x = (int)xpos;
		last_mouse_y = height - (int)ypos;
	}
}

void cursorEnterCallback(GLFWwindow* window, int entered) {
	if (!entered) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
	else {
		if (appConfig.snapToGrid) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
		}
	}
}