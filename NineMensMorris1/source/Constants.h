#pragma once

const unsigned int WINDOW_WIDTH = 1000;
const unsigned int WINDOW_HEIGHT = 850;

const unsigned int FPS = 60;
const unsigned int FRAME_TARGET_TIME = 1000 / FPS;



/* Top left to bottom right
*
*	60,60	410,60	760,60
*	175,175	410,175	642,175
*	292,292	410,292	525,292
*	60,410	175,410	292,410	525,410	642,410	760,10
*	292,525	410,525	525,525
*	175,642 410,642	642,642
*	60,760	410,760	760,760
*/