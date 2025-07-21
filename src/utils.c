#include "../libs/vinote.h"
#include <pthread.h>

char* readFile(const char* filename) {
	FILE* file = fopen(filename, "rb");
	if (!file) {
		fprintf(stderr, "Could not open file: %s\n", filename);
		return NULL;
	}

	// Seek to the end to determine the file size
	if (fseek(file, 0, SEEK_END) != 0) {
		fprintf(stderr, "Failed to seek to end of file: %s\n", filename);
		fclose(file);
		return NULL;
	}

	long length = ftell(file);
	if (length < 0) {
		fprintf(stderr, "Failed to determine file size: %s\n", filename);
		fclose(file);
		return NULL;
	}

	if (fseek(file, 0, SEEK_SET) != 0) {
		fprintf(stderr, "Failed to seek to beginning of file: %s\n", filename);
		fclose(file);
		return NULL;
	}

	char* buffer = malloc(length + 1);
	if (!buffer) {
		fprintf(stderr, "Failed to allocate memory for file content\n");
		fclose(file);
		return NULL;
	}

	size_t readLength = fread(buffer, 1, length, file);
	if (readLength != length) {
		fprintf(stderr, "Failed to read the entire file: %s\n", filename);
		free(buffer);
		fclose(file);
		return NULL;
	}

	buffer[length] = '\0';

	fclose(file);
	return buffer;
}

void freeResources(GLuint *VAO, GLuint *VBO, GLFWwindow* window, GLFWcursor* cursor) {
	if (cursor) {
		glfwDestroyCursor(cursor);
	}
	freeContainers(containers);
	freeWidgets(widgets);
	glDeleteVertexArrays(1, VAO);
	glDeleteBuffers(1, VBO);
	glfwDestroyWindow(window);
	glfwTerminate();
}

int textWidth(const char* text, Glyph* glyphs, float scale) {
	int width = 0;
	for (int i = 0; text[i] != '\0'; i++)
		width += (glyphs[text[i]].advance) * scale;
	return width;
}

int checkHitbox(int mouseX, int mouseY) {
	for (int i = 0; containers[i].width != -1; i++) {
		if (mouseX >= containers[i].x && mouseX <= containers[i].x + containers[i].width &&
			mouseY <= containers[i].y && mouseY >= containers[i].y - containers[i].height) {
			return i;
		}
	}
	for (int i = 0; widgets[i].width != -1; i++) {
		if (mouseX >= widgets[i].x && mouseX <= widgets[i].x + widgets[i].width &&
			mouseY >= widgets[i].y && mouseY <= widgets[i].y + widgets[i].height) {
			if (widgets[i].onClick) {
				pthread_t thread;
				if (pthread_create(&thread, NULL, (void* (*)(void*))widgets[i].onClick, (void*)&widgets[i]) != 0) {
					fprintf(stderr, "Failed to create thread for widget click handler\n");
					return -2;
				}
				pthread_detach(thread);
			}
			return -2;
		}
	}
	return -1;
}