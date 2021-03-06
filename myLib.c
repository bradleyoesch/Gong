/* BRADLEY OESCH
 * 10/28/13
 */

#include "myLib.h"
#include "dma.h"
#include <stdlib.h>

u16 *videoBuffer = (u16*)0x6000000;

/* Functions */
int getRandomNumber(int min, int max) {
	return min + rand() / (RAND_MAX / (max - min + 1) + 1);
}

void drawPixel(int r, int c, u16 color) {
	videoBuffer[OFFSET(r, c, W)] = color;
}

void drawRect(int r, int c, int width, int height, u16 color) {
	for (int row=0; row<height; row++) {
		DMA[3].src = &color;
		DMA[3].dst = &videoBuffer[OFFSET(r+row, c, W)];
		DMA[3].cnt = width | DMA_ON | DMA_SOURCE_FIXED;
	}
}

void fillPicture(const u16 *img) { //or u16 *img
	DMA[3].src = &img;
	DMA[3].dst = videoBuffer;
	DMA[3].cnt = (W*H) | DMA_ON;
}

void drawPicture(int r, int c, int width, int height, const u16 *img) {
	for (int i=0; i<height; i++) {
		DMA[3].src = img + r+i * width;
		DMA[3].dst = videoBuffer + r+i * W;
		DMA[3].cnt = width | DMA_ON;
	}
}

//only called when ball is on correct column
int hitsPaddle(BALL ball, PADDLE paddle) {
	int rangeLow = paddle.r - ball.s;
	int rangeHigh = paddle.r + paddle.h + ball.s;
	return (ball.r >= rangeLow && ball.r <= rangeHigh) ? -1 : 1;
}

PADDLE realignPaddle(PADDLE paddle) {
	if (paddle.r <= paddle.topBound)
		paddle.r = paddle.topBound;
	if (paddle.r >= paddle.botBound)
		paddle.r = paddle.botBound;
	return paddle;
}

BALL resetBall(BALL ball) {
	ball.r = getRandomNumber(TOP_MARGIN + 1, H - MARGIN - ball.s -1); //H / 2 - ball.s / 2;
	ball.c = W / 2 - ball.s / 2;
	ball.rD *= -1;
	ball.cD *= -1;
	return ball;
}

int checkScoreCondition(BALL ball) {
	if (ball.c <= ball.leftBound)
		return -1;
	else if (ball.c >= ball.rightBound)
		return 1;
	else
		return 0;
}

int centerTextWidth(const char *str) {
    const char *s;
    for (s = str; *s; s++);
    return (W - (s - str)*6) / 2;
}

void waitForVblank() {
    while(SCANLINECOUNTER > H);
    while(SCANLINECOUNTER < H);
}