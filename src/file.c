#include"../libs/vinote.h"

void saveFile(const char *filename) {
	FILE *file = fopen(filename, "wb+");
	if (file == NULL) {
		fprintf(stderr, "Failed to open file for writing: %s\n", filename);
		return;
	}
	for (int i = 0; i < containerCount(containers); i++) {
		Container *container = &containers[i];
		fprintf(file, "%d\x1F%d\x1F%f\x1F%f\x1F%f\x1F%f\x1F%d\x1F%s\x1F%s\x1E",
				container->x, container->y,container->scale,
				container->color[0], container->color[1], container->color[2],
				container->font.fontSize, container->text, container->fontPath);
	}
	fclose(file);
}

void loadFile(const char *filename) {
	char *fileContent = readFile(filename);
	if (fileContent == NULL) {
		fprintf(stderr, "Failed to read file: %s\n", filename);
		return;
	}

	char *contextOuter;
	char *line = strtok_s(fileContent, "\x1E", &contextOuter);

	while (line != NULL) {
		int x, y, fontSize;
		float scale, color[3];
		char *text, *fontPath;

		char *contextInner;
		char *token = strtok_s(line, "\x1F", &contextInner);
		if (token == NULL) break;
		x = atoi(token);

		token = strtok_s(NULL, "\x1F", &contextInner);
		if (token == NULL) break;
		y = atoi(token);

		token = strtok_s(NULL, "\x1F", &contextInner);
		if (token == NULL) break;
		scale = atof(token);

		token = strtok_s(NULL, "\x1F", &contextInner);
		if (token == NULL) break;
		color[0] = atof(token);

		token = strtok_s(NULL, "\x1F", &contextInner);
		if (token == NULL) break;
		color[1] = atof(token);

		token = strtok_s(NULL, "\x1F", &contextInner);
		if (token == NULL) break;
		color[2] = atof(token);

		token = strtok_s(NULL, "\x1F", &contextInner);
		if (token == NULL) break;
		fontSize = atoi(token);

		token = strtok_s(NULL, "\x1F", &contextInner);
		if (token == NULL) break;
		text = token;

		token = strtok_s(NULL, "\x1F", &contextInner);
		if (token == NULL) break;
		fontPath = token;

		token = strtok_s(NULL, "\x1F", &contextInner);
		if (token != NULL) {
			fprintf(stderr, "Unexpected token after font path: %s\n", token);
			break;
		}
		createContainer(x, y, fontSize, scale, color, text, fontPath);
		
		line = strtok_s(NULL, "\x1E", &contextOuter);
	}
	free(fileContent);
}

int *configPointer(config *appConfig, const char *key) {
	if (strcmp(key, "upKey") == 0) {
		return &appConfig->upKey;
	} else if (strcmp(key, "downKey") == 0) {
		return &appConfig->downKey;
	} else if (strcmp(key, "leftKey") == 0) {
		return &appConfig->leftKey;
	} else if (strcmp(key, "rightKey") == 0) {
		return &appConfig->rightKey;
	} else if (strcmp(key, "snapKey") == 0) {
		return &appConfig->snapKey;
	} else if (strcmp(key, "insertKey") == 0) {
		return &appConfig->insertKey;
	} else if (strcmp(key, "deleteKey") == 0) {
		return &appConfig->deleteKey;
	} else if (strcmp(key, "selectKey") == 0) {
		return &appConfig->selectKey;
	} else if (strcmp(key, "unselectKey") == 0) {
		return &appConfig->unselectKey;
	} else if (strcmp(key, "saveKey") == 0) {
		return &appConfig->saveKey;
	}
	fprintf(stderr, "Unknown config key: %s\n", key);
	return NULL;
}

void loadConfig(config *appConfig) {
	char *fileContent = readFile(".config");
	if (fileContent == NULL) {
		fprintf(stderr, "Failed to read config file\n");
		return;
	}

	char *contextOuter;
	char *line = strtok_s(fileContent, "\n", &contextOuter);
	while (line != NULL) {
		char *key = strtok(line, " ");
		char *value = strtok(NULL, " ");
		if (key && value) {
			if (strcmp(key, "gridColor") == 0) {
				sscanf(value, "%f,%f,%f", &appConfig->gridColor[0], &appConfig->gridColor[1], &appConfig->gridColor[2]);
			} else if (strcmp(key, "gridSpacing") == 0) {
				appConfig->gridSpacing = atoi(value);
			} else if (strcmp(key, "gridLineWidth") == 0) {
				appConfig->gridLineWidth = atoi(value);
			} else if (strcmp(key, "cursorSize") == 0) {
				appConfig->cursorSize = atoi(value);
			} else if (strcmp(key, "snapToGrid") == 0) {
				appConfig->snapToGrid = atoi(value);
			} else{
				int *keyPtr = configPointer(appConfig, key);
				if (keyPtr == NULL) {
					fprintf(stderr, "Unknown config key: %s\n", key);
					line = strtok_s(NULL, "\n", &contextOuter);
					continue;
				}
				char *button = strtok(value, "+");
				if (button == NULL) {
					fprintf(stderr, "No button specified for key: %s\n", key);
					line = strtok_s(NULL, "\n", &contextOuter);
					continue;
				}
				while (button != NULL) {
					if (button[strlen(button) - 1] == 13) 
						button[strlen(button) - 1] = '\0';
					if (strlen(button) == 1)
						*keyPtr += button[0];
					else if (strcmp(button, "CTRL") == 0)
						*keyPtr += GLFW_MOD_CONTROL * 65536;
					else if (strcmp(button, "SHIFT") == 0)
						*keyPtr += GLFW_MOD_SHIFT * 65536;
					else if (strcmp(button, "ALT") == 0)
						*keyPtr += GLFW_MOD_ALT * 65536;
					else if (strcmp(button, "ENTER") == 0)
						*keyPtr += GLFW_KEY_ENTER;
					else if (strcmp(button, "ESCAPE") == 0)
						*keyPtr += GLFW_KEY_ESCAPE;
					else if (strcmp(button, "BACKSPACE") == 0)
						*keyPtr += GLFW_KEY_BACKSPACE;
					else if (strcmp(button, "DELETE") == 0)
						*keyPtr += GLFW_KEY_DELETE;
					else if (strcmp(button, "TAB") == 0)
						*keyPtr += GLFW_KEY_TAB;
					else {
						fprintf(stderr, "Unknown key: %s\n", button);
						printf("len: %d, key: %s\n", (int)strlen(button), button);
						printf("button ends with: %d\n", button[strlen(button) - 1]);
					}
					button = strtok(NULL, "+");
				}
			}
		}
		line = strtok_s(NULL, "\n", &contextOuter);
	}
	free(fileContent);
}

void saveConfig(config *appConfig) {
	FILE *file = fopen(".config", "w");
	if (file == NULL) {
		fprintf(stderr, "Failed to open config file for writing\n");
		return;
	}
	fprintf(file, "gridColor %.2f,%.2f,%.2f\n", appConfig->gridColor[0], appConfig->gridColor[1], appConfig->gridColor[2]);
	fprintf(file, "gridSpacing %d\n", appConfig->gridSpacing);
	fprintf(file, "gridLineWidth %d\n", appConfig->gridLineWidth);
	fprintf(file, "cursorSize %d\n", appConfig->cursorSize);
	fprintf(file, "snapToGrid %d\n", appConfig->snapToGrid);
	fprintf(file, "upKey %d\n", appConfig->upKey);
	fprintf(file, "downKey %d\n", appConfig->downKey);
	fprintf(file, "leftKey %d\n", appConfig->leftKey);
	fprintf(file, "rightKey %d\n", appConfig->rightKey);
	fprintf(file, "snapKey %d\n", appConfig->snapKey);
	fprintf(file, "insertKey %d\n", appConfig->insertKey);
	fprintf(file, "deleteKey %d\n", appConfig->deleteKey);
	fprintf(file, "selectKey %d\n", appConfig->selectKey);
	fprintf(file, "unselectKey %d\n", appConfig->unselectKey);
	fprintf(file, "saveKey %d\n", appConfig->saveKey);
	fclose(file);
}