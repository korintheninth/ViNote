#include "../libs/vinote.h"

#define WIDTH  800
#define HEIGHT  600

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

int main(void)
{
	Ck *ck = initCK();
	Window *win = create_window(ck, WIDTH, HEIGHT, "ViNote");
	Context *ctx = create_context();
	win->context = ctx;


	loopCK(ck);
	
	return 0;
}