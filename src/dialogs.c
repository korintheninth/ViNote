#include "../libs/vinote.h"

void testDialog() {
	int run = 1;
	GLFWwindow *mainWindow = glfwGetCurrentContext();
	GLFWwindow *dialog = openWindow(400, 200, "Test Dialog", NULL, mainWindow);
	if (!dialog) {
		fprintf(stderr, "Failed to create dialog window\n");
		return;
	}
	glfwMakeContextCurrent(dialog);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	
	GLuint VAO, VBO;
	initBuffers(&VAO, &VBO);

	GLuint textShaderProgram = loadShader("src/shaders/textvertexshader.glsl", "src/shaders/textfragmentshader.glsl");
	if (textShaderProgram == 0) {
		fprintf(stderr, "Failed to load text shader\n");
		glfwDestroyWindow(dialog);
		return;
	}
	Font dialogFont = {
		.glyphs = NULL,
		.fontSize = 24,
		.lineHeight = 24,
		.ascender = 24,
		.descender = 0
	};
	getGlyphs("fonts/ARIAL.TTF", dialogFont.fontSize, &dialogFont);
	if (dialogFont.glyphs == NULL) {
		fprintf(stderr, "Failed to load glyphs for dialog font\n");
		glDeleteProgram(textShaderProgram);
		glfwDestroyWindow(dialog);
		return;
	}
	while (run && !glfwWindowShouldClose(dialog)) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		textRenderParameters params = {
			.shaderProgram = textShaderProgram,
			.glyphs = dialogFont.glyphs,
			.text = "This is a test dialog",
			.lineHeight = dialogFont.lineHeight,
			.descender = dialogFont.descender,
			.ascender = dialogFont.ascender,
			.x = 10.0f,
			.y = 10.0f,
			.scale = 1.0f,
			.color = {0.0f, 0.0f, 0.0f}
		};
		renderText(params);
		glfwSwapBuffers(dialog);
		glfwPollEvents();

		if (glfwGetKey(dialog, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			run = 0;
		}
	}
	glDeleteProgram(textShaderProgram);
	glfwDestroyWindow(dialog);
	freeGlyphs(dialogFont.glyphs);
	glfwMakeContextCurrent(mainWindow);
}