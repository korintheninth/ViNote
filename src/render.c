#include "../libs/vinote.h"
#define STB_IMAGE_IMPLEMENTATION
#include "../libs/external/stb_image.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "../libs/external/stb_image_resize2.h"

GLuint loadTexture(const char* filename) {
	int width, height, channels;
	
	stbi_set_flip_vertically_on_load(1);
	
	unsigned char* data = stbi_load(filename, &width, &height, &channels, 0);
	if (!data) {
		fprintf(stderr, "Failed to load image: %s\n", filename);
		return 0;
	}

	GLenum format;
	if (channels == 1)
		format = GL_RED;
	else if (channels == 3)
		format = GL_RGB;
	else if (channels == 4)
		format = GL_RGBA;
	else {
		fprintf(stderr, "Unsupported number of channels: %d\n", channels);
		stbi_image_free(data);
		return 0;
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		format,
		width,
		height,
		0,
		format,
		GL_UNSIGNED_BYTE,
		data
	);
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		fprintf(stderr, "OpenGL error: %d\n", error);
		glDeleteTextures(1, &texture);
		stbi_image_free(data);
		return 0;
	}

	stbi_image_free(data);

	return texture;
}


void initBuffers(GLuint *VAO, GLuint *VBO) {
	glGenVertexArrays(1, VAO);
	glBindVertexArray(*VAO);
	glGenBuffers(1, VBO);
	glBindBuffer(GL_ARRAY_BUFFER, *VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
}

GLuint loadShader(const char* vertexPath, const char* fragmentPath) {
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	GLuint shaderProgram = glCreateProgram();
	
	char* vertexCode = readFile(vertexPath);
	glShaderSource(vertexShader, 1, (const char**)&vertexCode, NULL);
	glCompileShader(vertexShader);
	
	GLint success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n%s\n%s\n\n", infoLog, vertexPath);
	}

	char* fragmentCode = readFile(fragmentPath);
	glShaderSource(fragmentShader, 1, (const char**)&fragmentCode, NULL);
	glCompileShader(fragmentShader);
	
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n%s\n%s\n\n", infoLog, fragmentPath);
	}

	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		printf("ERROR::SHADER::PROGRAM::LINKING_FAILED\n%s\n", infoLog);
	}

	free(vertexCode);
	free(fragmentCode);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return shaderProgram;
}

void renderText(textRenderParameters params) {
	glUseProgram(params.shaderProgram);

	int width, height;
	glfwGetFramebufferSize(glfwGetCurrentContext(), &width, &height);
	float screenSize[] = { (float)width, (float)height };
	glUniform2fv(glGetUniformLocation(params.shaderProgram, "screenSize"), 1, screenSize);
	glUniform3fv(glGetUniformLocation(params.shaderProgram, "textColor"), 1, params.color);

	glActiveTexture(GL_TEXTURE0);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	int lineCount = 0;
	for (const char* p = params.text; *p; p++) {
		if (*p == '\n') {
			lineCount++;
		}
	}
	params.y += lineCount * params.lineHeight * params.scale;
	float lineStart = params.x;
	for (const char* p = params.text; *p; p++) {
		if (*p == '\n') {
			params.x = lineStart;
			params.y -= params.lineHeight * params.scale;
			continue;
		}
		Glyph glyph = params.glyphs[*p];

		float xpos = params.x + glyph.bearingX * params.scale;
		float ypos = params.y - (glyph.height - glyph.bearingY + params.descender) * params.scale;

		float w = glyph.width * params.scale;
		float h = glyph.height * params.scale;

		GLfloat vertices[6][4] = {
			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },
			{ xpos,     ypos,       0.0f, 1.0f },

			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos + w, ypos + h,   1.0f, 0.0f },
			{ xpos + w, ypos,       1.0f, 1.0f }
		};
		
		glBindTexture(GL_TEXTURE_2D, glyph.textureID);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		params.x += (glyph.advance) * params.scale;
	}
}

void drawGrid(GLuint shaderProgram, int gridSpacing, int lineWidth, float gridColor[3]) {
	int width, height;
	glfwGetFramebufferSize(glfwGetCurrentContext(), &width, &height);

	
	glUseProgram(shaderProgram);
	glUniform2f(glGetUniformLocation(shaderProgram, "screenSize"), (float)width, (float)height);
	glUniform1f(glGetUniformLocation(shaderProgram, "gridSpacing"), (float)gridSpacing);
	glUniform1f(glGetUniformLocation(shaderProgram, "lineWidth"), (float)lineWidth);
	glUniform3fv(glGetUniformLocation(shaderProgram, "gridColor"), 1, gridColor);
	
	glActiveTexture(GL_TEXTURE0);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	GLfloat quadVertices[6][4] = {
		{ 0.0f, (float)height, 	0.0f, 0.0f}, 
		{ (float)width, 0.0f, 	1.0f, 1.0f},
		{ 0.0f, 0.0f, 			0.0f, 1.0f},

		{ 0.0f, (float)height,			0.0f, 0.0f},
		{ (float)width, (float)height, 	1.0f, 0.0f},
		{ (float)width, 0.0f, 			1.0f, 1.0f}
	};

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(quadVertices), quadVertices);

	glDrawArrays(GL_TRIANGLES, 0, 6);
}

GLFWcursor *setCustomCursor(const char* filename, GLFWwindow* window, int cursorWidth, int cursorHeight) {
	int width, height, channels;
	unsigned char* data = stbi_load(filename, &width, &height, &channels, 4);
	if (!data) {
		fprintf(stderr, "Failed to load image: %s\n", filename);
		return NULL;
	}

	unsigned char* resized_data = malloc(cursorWidth * cursorHeight * 4);
	if (!resized_data) {
		fprintf(stderr, "Failed to allocate memory for resized image\n");
		stbi_image_free(data);
		return NULL;
	}

	GLFWimage image;
	image.width = cursorWidth;
	image.height = cursorHeight;
	image.pixels = resized_data;
	if (!stbir_resize_uint8_srgb(data, width, height, 0, resized_data, image.width, image.height, 0, 4)) {
		fprintf(stderr, "Failed to resize image\n");
		free(resized_data);
		stbi_image_free(data);
		return NULL;
	}

	GLFWcursor* cursor = glfwCreateCursor(&image, image.width / 2, image.height / 2);
	if (!cursor) {
		fprintf(stderr, "Failed to create cursor\n");
		stbi_image_free(data);
		free(resized_data);
		return NULL;
	}

	glfwSetCursor(window, cursor);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	
	stbi_image_free(data);
	free(resized_data);
	return cursor;
}

void renderTexture(textureRenderParameters params) {

	glUseProgram(params.shaderProgram);

	int width, height;
	glfwGetFramebufferSize(glfwGetCurrentContext(), &width, &height);
	float screenSize[] = { (float)width, (float)height };
	glUniform2fv(glGetUniformLocation(params.shaderProgram, "screenSize"), 1, screenSize);
	glUniform3fv(glGetUniformLocation(params.shaderProgram, "textureColor"), 1, params.color);

	glActiveTexture(GL_TEXTURE0);


	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);



	float xpos = params.x;
	float ypos = params.y;

	float w = params.width;
	float h = params.height;

	GLfloat vertices[6][4] = {
		{ xpos,     ypos + h,   0.0f, 0.0f },
		{ xpos + w, ypos,       1.0f, 1.0f },
		{ xpos,     ypos,       0.0f, 1.0f },

		{ xpos,     ypos + h,   0.0f, 0.0f },
		{ xpos + w, ypos + h,   1.0f, 0.0f },
		{ xpos + w, ypos,       1.0f, 1.0f }
	};
		
	glBindTexture(GL_TEXTURE_2D, params.textureID);


	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

	glDrawArrays(GL_TRIANGLES, 0, 6);
}
