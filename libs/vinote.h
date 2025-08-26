#ifndef VINOTE_H
#define VINOTE_H

#include <stdio.h>
#include <stdlib.h>
#include "../../Ck/libs/ck.h"
#include "external/glew-2.1.0/include/GL/glew.h"
#include "external/glfw-3.4.bin.WIN64/include/GLFW/glfw3.h"

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

#endif
