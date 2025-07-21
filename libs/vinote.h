#ifndef VINOTE_H
#define VINOTE_H

#include <stdio.h>
#include <stdlib.h>
#include "external/freetype/ft2build.h" 
#include FT_FREETYPE_H
#include "external/glew-2.1.0/include/GL/glew.h"
#include "external/glfw-3.4.bin.WIN64/include/GLFW/glfw3.h"

typedef struct Glyph {
	GLuint textureID;
	int width;
	int height;
	int bearingX;
	int bearingY;
	unsigned int advance;
} Glyph;

typedef struct Font {
	Glyph* glyphs;
	int fontSize;
	int lineHeight;
	int ascender;
	int descender;
} Font;

typedef struct Container {
	int width;
	int height;
	int x;
	int y;
	float scale;
	float color[3];
	char* text;
	char* fontPath;
	Font font;
	char *type;
} Container;

typedef struct textRenderParameters {
	GLuint shaderProgram;
	Glyph* glyphs;
	const char* text;
	int lineHeight;
	int descender;
	int ascender;
	float x;
	float y;
	float scale;
	float color[3];
} textRenderParameters;

typedef struct textureRenderParameters {
	GLuint shaderProgram;
	float x;
	float y;
	int width;
	int height;
	float color[3];
	GLuint textureID;
} textureRenderParameters;

typedef struct config {
	float gridColor[3];
	int gridSpacing;
	int gridLineWidth;
	int cursorSize;
	int snapToGrid;
	int upKey;
	int downKey;
	int leftKey;
	int rightKey;
	int snapKey;
	int insertKey;
	int deleteKey;
	int selectKey;
	int unselectKey;
	int saveKey;
} config;

typedef struct Widget {
	int x;
	int y;
	int width;
	int height;
	float scale;
	float color[3];
	char* text;
	char* fontPath;
	Font font;
	int textOffsetX;
	int textOffsetY;
	void (*onClick)(struct Widget* self);
	GLuint textureID;
} Widget;

typedef struct function {
	char *containerType;
	char *name;
	void (*func)(void* self);
} function;

extern Container* containers;
extern Widget* widgets;
extern config appConfig;
extern int snapPos[2];

GLFWwindow* openWindow(int width, int height, const char* title, GLFWmonitor* monitor, GLFWwindow* share);
void closeWindow(GLFWwindow* window);

void getGlyphs(const char* fontPath, int fontSize, Font *font);

void initBuffers(GLuint *VAO, GLuint *VBO);
GLuint loadShader(const char* vertexPath, const char* fragmentPath);
void drawGrid(GLuint shaderProgram, int gridSpacing, int lineWidth, float gridColor[3]);
GLuint loadTexture(const char* filename);
GLFWcursor *setCustomCursor(const char* filename, GLFWwindow* window, int curosrWidth, int cursorHeight);
void renderText(textRenderParameters params);
void renderTexture(textureRenderParameters params);

void initContainers();
void createContainer(int x, int y, int fontSize, float scale, float color[3],
						const char* text, const char* fontPath);
void destroyContainer(int index);
void renderContainers(GLuint textShaderProgram, GLuint textureShaderProgram, GLuint containerTexture);
int containerCount();
int containerWidth(Container *container);
int containerHeight(Container *container);

char* readFile(const char* filename);
void freeContainers();
void freeGlyphs(Glyph* glyphs);
void freeResources(GLuint *VAO, GLuint *VBO, GLFWwindow* window, GLFWcursor* cursor);
int textWidth(const char* text, Glyph* glyphs, float scale);
int checkHitbox(int mouseX, int mouseY);

void errorCallback(int error, const char* description);
void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void charCallback(GLFWwindow* window, unsigned int codepoint);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos);
void cursorEnterCallback(GLFWwindow* window, int entered);

void saveFile(const char *filename);
void loadFile(const char *filename);
void loadConfig(config *appConfig);

void initWidgets();
void addWidget(Widget new_widget);
void freeWidget(Widget *widget);
void freeWidgets();
void renderWidgets(GLuint textShaderProgram, GLuint textureShaderProgram);
Widget *testWidget();

void testDialog();
#endif
