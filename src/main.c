#include "../libs/vinote.h"

#define WIDTH  800
#define HEIGHT  600

Container* containers;
Widget* widgets;
config appConfig = {
	.gridColor = {0.8f, 0.8f, 0.8f},
	.gridSpacing = 10,
	.gridLineWidth = 1,
	.cursorSize = 20,
	.snapToGrid = 0,
	.upKey = 0,
	.downKey = 0,
	.leftKey = 0,
	.rightKey = 0,
	.snapKey = 0,
	.insertKey = 0,
	.deleteKey = 0,
	.selectKey = 0,
	.unselectKey = 0,
	.saveKey = 0
};
int snapPos[2] = {0, 0};

int main(void)
{
	loadConfig(&appConfig);
	GLFWwindow* window = openWindow(WIDTH, HEIGHT, "ViNote", NULL, NULL);
	if (!window) {
		return -1;
	}
	
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetCharCallback(window, charCallback);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetCursorPosCallback(window, cursorPositionCallback);
	glfwSetCursorEnterCallback(window, cursorEnterCallback);

	glfwMaximizeWindow(window);
	
	GLFWcursor *cursor = setCustomCursor("textures/cursor.png", window, appConfig.cursorSize, appConfig.cursorSize);

	GLuint cursorTexture = loadTexture("textures/cursor.png");
	GLuint containerTexture = loadTexture("textures/container.png");
	GLuint textshaderProgram = loadShader("src/shaders/textvertexshader.glsl", "src/shaders/textfragmentshader.glsl");
	GLuint gridshaderProgram = loadShader("src/shaders/gridvertexshader.glsl", "src/shaders/gridfragmentshader.glsl");
	GLuint textureshaderProgram = loadShader("src/shaders/texturevertexshader.glsl", "src/shaders/texturefragmentshader.glsl");
	GLuint VAO, VBO;
	initBuffers(&VAO, &VBO);
	
	initContainers();
	initWidgets();

	Widget *TestWidget = testWidget();
	if (TestWidget) {
		addWidget(*TestWidget);
	}

	while (!glfwWindowShouldClose(window)) {
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		drawGrid(gridshaderProgram, appConfig.gridSpacing, appConfig.gridLineWidth, appConfig.gridColor);
		renderContainers(textshaderProgram, textureshaderProgram, containerTexture);
		renderWidgets(textshaderProgram, textureshaderProgram);
		if (appConfig.snapToGrid) {
			textureRenderParameters params = {
				textureshaderProgram,
				snapPos[0] - appConfig.cursorSize/2,
				snapPos[1] - appConfig.cursorSize/2,
				appConfig.cursorSize,
				appConfig.cursorSize,
				{1.0f, 0.0f, 0.0f},
				cursorTexture
			};
			renderTexture(params);
		}
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteProgram(textshaderProgram);
	glDeleteProgram(gridshaderProgram);
	glDeleteProgram(textureshaderProgram);
	glDeleteTextures(1, &cursorTexture);
	glDeleteTextures(1, &containerTexture);
	freeResources(&VAO, &VBO, window, cursor);

	return 0;
}