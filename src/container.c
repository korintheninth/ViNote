#include "../libs/vinote.h"

void initContainers() {
	containers = malloc(sizeof(Container) * 1);
	if (!containers) {
		fprintf(stderr, "Failed to allocate memory for containers\n");
		return;
	}
	Container empty_container = {.width = -1, .height = -1};
	containers[0] = empty_container;
}

int containerCount() {
	int count = 0;
	while (containers[count].width != -1) {
		count++;
	}
	return count;
}

void createContainer(int x, int y, int fontSize, float scale, float color[3],
						const char* text, const char* fontPath) {
	Container new_container;
	int count = containerCount(containers);

	new_container.x = x;
	new_container.y = y;
	new_container.scale = scale;
	new_container.color[0] = color[0];
	new_container.color[1] = color[1];
	new_container.color[2] = color[2];
	new_container.fontPath = malloc(strlen(fontPath) + 1);
	if (new_container.fontPath == NULL) {
		fprintf(stderr, "Failed to allocate memory for font path\n");
		return;
	}
	new_container.text = malloc(strlen(text) + 1);
	if (new_container.text == NULL) {
		fprintf(stderr, "Failed to allocate memory for container text\n");
	}
	strcpy(new_container.text, text);
	strcpy(new_container.fontPath, fontPath);
	new_container.font.fontSize = fontSize;
	getGlyphs(new_container.fontPath, new_container.font.fontSize, &new_container.font);
	if (new_container.font.glyphs == NULL) {
		fprintf(stderr, "Failed to load glyphs for font: %s\n", new_container.fontPath);
		free(new_container.fontPath);
		free(new_container.text);
		return;
	}
	new_container.width = containerWidth(&new_container);
	new_container.height = containerHeight(&new_container);
	
	containers = realloc(containers, sizeof(Container) * (count + 2));
	if (containers == NULL) {
		fprintf(stderr, "Failed to allocate memory for containers\n");
		free(new_container.text);
		return;
	}
	
	containers[count] = new_container;
	Container empty_container = {.width = -1, .height = -1};
	containers[count + 1] = empty_container;

	return;
}

void destroyContainer(int index) {
	if (containers[index].text != NULL) {
		free(containers[index].text);
	}
	if (containers[index].fontPath != NULL) {
		free(containers[index].fontPath);
	}
	if (containers[index].font.glyphs != NULL) {
		freeGlyphs(containers[index].font.glyphs);
	}
	for (int i = index; containers[i].width != -1; i++) {
		containers[i] = containers[i + 1];
	}
}

void renderContainers(GLuint textShaderProgram, GLuint textureShaderProgram, GLuint containerTexture) {
	for (int i = 0; i < containerCount(containers); i++) {
		textRenderParameters params = {
			textShaderProgram,
			containers[i].font.glyphs,
			containers[i].text,
			containers[i].font.lineHeight,
			containers[i].font.descender,
			containers[i].font.ascender,
			containers[i].x + 10,
			containers[i].y - containers[i].height,
			containers[i].scale,
			{containers[i].color[0],
			containers[i].color[1],
			containers[i].color[2]}
		};
		textureRenderParameters textureParams = {
			textureShaderProgram,
			containers[i].x,
			containers[i].y - containers[i].height,
			containers[i].width,
			containers[i].height,
			{0.8f, 0.8f, 0.8f},
			containerTexture
		};
		renderTexture(textureParams);
		renderText(params);
	}
}

int containerWidth(Container *container) {
	int width = 0;
	char *copy = strdup(container->text);
	char *line = strtok(copy, "\n");
	while (line != NULL) {
		int lineWidth = textWidth(line, container->font.glyphs, container->scale);
		if (lineWidth > width) {
			width = lineWidth;
		}
		line = strtok(NULL, "\n");
	}
	free(copy);
	return width + 20;
}

int containerHeight(Container *container) {
	int height = 0;
	for (char *ptr = container->text; *ptr != '\0'; ptr++) {
		if (*ptr == '\n') {
			height += container->font.lineHeight;
		}
	}
	return height + container->font.lineHeight;
}

void freeContainers() {
	for (int i = 0; containers[i].width != -1; i++) {
		if (containers[i].text) {
			free(containers[i].text);
		}
		if (containers[i].fontPath) {
			free(containers[i].fontPath);
		}
		if (containers[i].font.glyphs) {
			freeGlyphs(containers[i].font.glyphs);
		}
	}
	free(containers);
}