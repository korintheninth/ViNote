#include "../libs/vinote.h"

void initWidgets() {
	widgets = malloc(sizeof(Widget) * 1);
	if (!widgets) {
		fprintf(stderr, "Failed to allocate memory for widgets\n");
		return;
	}
	Widget empty_widget = {
		.x = -1,
		.y = -1,
		.width = -1,
		.height = -1,
	};
	widgets[0] = empty_widget;
}

int widgetCount() {
	int count = 0;
	while (widgets[count].width != -1) {
		count++;
	}
	return count;
}

void addWidget(Widget new_widget) {
	int count = widgetCount();
	
	widgets = realloc(widgets, sizeof(Widget) * (count + 2));
	if (widgets == NULL) {
		fprintf(stderr, "Failed to allocate memory for widgets\n");
		return;
	}
	
	widgets[count] = new_widget;
	Widget empty_widget = {
		.x = -1,
		.y = -1,
		.width = -1,
		.height = -1,
	};
	widgets[count + 1] = empty_widget;
}

void freeWidget(Widget *widget) {
	if (widget == NULL) return;
	
	if (widget->text) {
		free(widget->text);
	}
	if (widget->fontPath) {
		free(widget->fontPath);
	}
	if (widget->font.glyphs) {
		freeGlyphs(widget->font.glyphs);
	}
	if (widget->textureID) {
		glDeleteTextures(1, &widget->textureID);
	}
	free(widget);
}

void freeWidgets() {
	for (int i = 0; widgets[i].width != -1; i++)
		freeWidget(&widgets[i]);
}

void printWidgetInfo(Widget *widget) {
	if (widget == NULL) {
		fprintf(stderr, "Widget is NULL\n");
		return;
	}
	
	printf("Widget Info:\n");
	printf("Position: (%d, %d)\n", widget->x, widget->y);
	printf("Size: %dx%d\n", widget->width, widget->height);
	printf("Scale: %.2f\n", widget->scale);
	printf("Color: (%.2f, %.2f, %.2f)\n", widget->color[0], widget->color[1], widget->color[2]);
	printf("Text: %s\n", widget->text);
	printf("Font Path: %s\n", widget->fontPath);
}

void renderWidgets(GLuint textShaderProgram, GLuint textureShaderProgram) {
	for (int i = 0; widgets[i].width != -1; i++) {
		Widget *widget = &widgets[i];
		
		if (widget->textureID) {
			textureRenderParameters params = {
				.shaderProgram = textureShaderProgram,
				.x = widget->x,
				.y = widget->y,
				.width = widget->width,
				.height = widget->height,
				.color = {widget->color[0], widget->color[1], widget->color[2]},
				.textureID = widget->textureID
			};
			renderTexture(params);
		}
		
		if (widget->text && widget->font.glyphs) {
			textRenderParameters textParams = {
				.shaderProgram = textShaderProgram,
				.glyphs = widget->font.glyphs,
				.text = widget->text,
				.lineHeight = widget->font.lineHeight,
				.descender = widget->font.descender,
				.ascender = widget->font.ascender,
				.x = widget->x + widget->textOffsetX,
				.y = widget->y + widget->textOffsetY,
				.scale = widget->scale,
				.color = {widget->color[0], widget->color[1], widget->color[2]}
			};
			renderText(textParams);
		}
	}
}

Widget *testWidget() {
	Widget *widget = malloc(sizeof(Widget));
	if (!widget) {
		fprintf(stderr, "Failed to allocate memory for widget\n");
		return NULL;
	}
	
	widget->x = 100;
	widget->y = 100;
	widget->width = 200;
	widget->height = 100;
	widget->scale = 1.0f;
	widget->color[0] = 0.5f;
	widget->color[1] = 0.5f;
	widget->color[2] = 0.5f;
	widget->text = strdup("Test Widget");
	if (!widget->text) {
		fprintf(stderr, "Failed to allocate memory for widget text\n");
		free(widget);
		return NULL;
	}
	
	widget->fontPath = strdup("fonts/ARIAL.TTF");
	if (!widget->fontPath) {
		fprintf(stderr, "Failed to allocate memory for font path\n");
		free(widget->text);
		free(widget);
		return NULL;
	}
	
	widget->font.fontSize = 24;
	getGlyphs(widget->fontPath, widget->font.fontSize, &widget->font);
	if (widget->font.glyphs == NULL) {
		fprintf(stderr, "Failed to load glyphs for font: %s\n", widget->fontPath);
		free(widget->text);
		free(widget->fontPath);
		free(widget);
		return NULL;
	}
	widget->textOffsetX = 10;
	widget->textOffsetY = 10;
	widget->onClick = testDialog;
	widget->textureID = loadTexture("textures/image.png");
	
	return widget;
}
