/*CS 445/545 Prog 4 for Digya Acharya*/
/***********************************************************************************************
alien_lizard_advance.cpp
EXTRA_CREDIT: All four extra credit features have been implemented, i.e, placing two food pieces randomly
in the channel, determining their number and position at random for each spawned lizard (uses globals random_x
and random_y for positioning food at random places), growing lizard by 10 units in length each time it ingests a
food piece (globals food_x, food_y are used for making foods disapear after lizard reaches it). The lizard
grows 25 units in length on entering a new channel. Blaster needs two hit to destroy the lizard (uses global
hit_step for determining the number of times lizard has been hit). Score increases by an extra 50 if the lizard
is in the top channel or the next-to-top channel.
Apart from these, after the game ends, a "GAME_OVER" message has been added and the screen closes itself after
2 secs. (global flag end_game has been used for this feature)

Software Architecture Statement: This program utilizes three callback functions for achieving animation:
display_func(), the display callback handler,  timer_func(), the timer callback handler and keyboard_func(),
the glut keyboard callback handler. The display function draws all the objects and text in the scene, and
conditionally draws laser beam, reposition the blaster (using globals x_pos_laser and y_pos_laser) in response
to the keyboard events. The timer event is added as soon as the first frame is displayed, and helps in
achieving motion in lizard (uses globals move_lizard_x and move_lizard_y for lizard motion).

The blaster took 4.4 ms when moved at the rate of 5 pixel increments each 50 ms. To enable the blaster take 3.5 ms
and lizard take 4 ms to move from one end of the screen to another, I moved the blaster by 5.8 units and lizard by
5.1 units on each new frame redisplay. For measurement of time, blaster was placed in the leftmost position
of canvas and moved upto the rightmost position and the stop watch was used to measure the time in between.

Other major global variables: lizard_size helps in changing the lizard size on ingesting food and entering
new channel, animation period is the interval between each lizard movement, x_pos_human is used to locate
the position of human used for determining the end of game, channel_count to determine if it's a clockwise channel
or not, isAnimate and fire_signal are the flags for animation and if laser beam is fired respectively, y_laser_hit
helps in drawing laser beam only upto the position of lizard if it is hit by the blaster, and upto the end of the screen
otherwise, x_tip_head and x_tip_tail is used for determining end of channel and score is used for keeping track of score.
************************************************************************************************/


#include "pch.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <math.h> 
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "OpenGL445Setup.h"

using namespace std;

/*const int INSIDE = 0; // 0000
const int LEFT = 1; // 0001
const int RIGHT = 2; // 0010
const int BOTTOM = 4; // 0100
const int TOP = 8; // 1000*/

static int end_game;
static int x_min;
static int x_max;
static int y_min;
static int y_max;
static double x_move_fish1;
static double y_move_fish1;
static double x2_move_fish1;
static double y2_move_fish1;
static bool fish1_direction;
static bool fish2_direction;

static int x_cursor_food;
static int y_cursor_food;

static double x_move_fish2;
static double y_move_fish2;
static double x2_move_fish2;
static double y2_move_fish2;

static double x_tip_head_fish1;
static double y_tip_head_fish1;

static double x_tip_head_fish2;
static double y_tip_head_fish2;
static double x_tip_head;
bool done = false;
bool front = true;
bool back = false;
static bool fed;
static bool bonus_fed;
bool food_flag;
bool happy_fish = false;
bool sad_fish = false;
bool bonus_food = true;
static int random_rotation_angle;

static float bonus_x_pos;
static float bonus_y_pos;

static float matShine[] = { 25 };	//Shininess value used throughout the program

static int score;			//Flag to keep track of the score
static int timer;
char canvas_Name[] = "Fish Movement with Cohen Sutherland"; // Name at the top of canvas
char game_over[] = "GAME OVER"; // Display after game is over
char fish_happiness[] = "The fish is happy. It has been recently fed.";
char fish_sadness[] = "The fish is sad. It hasn't been recently fed.";


enum {
	INSIDE = 0, LEFT = 1, RIGHT = 2, BOTTOM = 4, TOP = 8
};


// Sets width and height of canvas to 600 by 600.
#define canvas_Width 800
#define canvas_Height 600

//Function prototypes
void timer_func(int val);
void keyboard_func(unsigned char key, int x, int y);
//void apply_cohen_sutherland_algorithm(int x1, int y1, int x2, int y2);
void calculate_random_angle();
void find_bonus_food_coordinates();

void init(void) {
	/* This function sets the background color, initializes all the global variables and calculate random
	coordinates to place the food.*/
	glClearColor(0.0, 0.0, 0.0, 1.0);
	x_min = -200;
	x_max = 200;
	y_min = -150;
	y_max = 150;
	/*x_min = -400;
	x_max = 400;
	y_min = -300;
	y_max = 300;*/
	score = 0;
	timer = 8;
	random_rotation_angle = 0;

	x_move_fish1 = 0;
	y_move_fish1 = 0;
	x2_move_fish1 = 0;
	y2_move_fish1 = 0;

	x_move_fish2 = 75;
	y_move_fish2 = 0;
	x2_move_fish2 = 0;
	y2_move_fish2 = 0;
	fish1_direction = true; //clockwise
	fish2_direction = false;
	food_flag = false;
	fed = false;

	x_cursor_food = 0;
	y_cursor_food = 0;
	x_tip_head_fish1 = 0;
	y_tip_head_fish1 = 0;

	x_tip_head_fish2 = 0;
	y_tip_head_fish2 = 0;
	bonus_x_pos = 0;
	bonus_y_pos = 0;
	bonus_food = false;
	bonus_fed = false;
	//find_bonus_food_coordinates();	
}

void find_bonus_food_coordinates() {
	bonus_food = true;
	int max = 130;
	srand((unsigned int)time(NULL));
	bonus_x_pos = (float)(rand() % max);
	cout << bonus_x_pos << endl;
	bonus_y_pos = (float)(rand() % max);
	cout << bonus_y_pos << endl;
}

void setup_light_source(void) {
	/* This function turns on OpenGL lighting and keeps light at the world origin.*/

	// Set light property vectors
	float lightAmb[] = { 1.0, 1.0, 1.0, 1.0 };
	float lightDif[] = { 1.0, 1.0, 1.0, 1.0 };
	float lightSpec[] = { 1.0, 1.0, 1.0, 1.0 };

	// Set light position at the world origin
	float lightPos[] = { 0.0, 0.0, 0.0, 1.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

	// Properties of GL_LIGHT0
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDif);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpec);

	//Enable lighting and light source GL_LIGHT0
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE); // Enable local viewpoint
}


void writeBitmapString(void *font, char *string) {
	/*
	This function writes a bitmap text, one character at a time.
	Parameters:
	----------
		font: void pointer
			The font to use to write a text.
		string: char pointer
			The text to write.
	*/
	char *c;
	for (c = string; *c != '\0'; c++) {
		glutBitmapCharacter(font, *c);
	}
}

void display_score(float x, float y) {
	/*
	This function is used for displaying score at the top of the canvas.
	----------
		x: float
			The x-position where the score should be placed.
		y: float
			The y-position where the score should be placed.
		z: float
			The z-position where the score should be placed.
	*/
	glColor3f(1.0, 1.0, 1.0);
	glPushMatrix();
	glRasterPos2f(x, y);
	char score_text[20] = "SCORE: ";
	char score_num[6];
	sprintf_s(score_num, 6, "%d", score);
	strcat_s(score_text, 20, score_num);
	writeBitmapString(GLUT_BITMAP_HELVETICA_18, score_text);
	glPopMatrix();
}


void display_timer(float x, float y) {
	/*
	This function is used for displaying score at the top of the canvas.
	----------
		x: float
			The x-position where the score should be placed.
		y: float
			The y-position where the score should be placed.
		z: float
			The z-position where the score should be placed.
	*/
	//glColor3f(1.0, 1.0, 1.0);
	if (timer > 3) {
		glColor3f(1.0, 1.0, 1.0);
	}
	else {
		glColor3f(1, 0.6, 0.1);
	}
	glPushMatrix();
	glRasterPos2f(x, y);
	char timer_text[20] = "TIMER: ";
	char timer_num[6];
	sprintf_s(timer_num, 6, "%d", timer);
	strcat_s(timer_text, 20, timer_num);
	writeBitmapString(GLUT_BITMAP_HELVETICA_18, timer_text);
	glPopMatrix();
}

void display_game_over(double x, double y, double z) {
	/*
	This function is used for displaying game over at the end of game.
	----------
		x: float
			The x-position where the game over should be placed.
		y: float
			The y-position where the game over should be placed.
		z: float
			The z-position where the game over should be placed.
	*/
	glPushMatrix();
	glRasterPos3d(x, y, z);
	writeBitmapString(GLUT_BITMAP_HELVETICA_12, game_over);
	glPopMatrix();
}

void display_happy_msg(double x, double y, double z) {
	/*
	This function is used for displaying game over at the end of game.
	----------
		x: float
			The x-position where the game over should be placed.
		y: float
			The y-position where the game over should be placed.
		z: float
			The z-position where the game over should be placed.
	*/
	glPushMatrix();
	glRasterPos3d(x, y, z);
	writeBitmapString(GLUT_BITMAP_HELVETICA_12, fish_happiness);
	glPopMatrix();
}


void display_sad_msg(double x, double y, double z) {
	/*
	This function is used for displaying game over at the end of game.
	----------
		x: float
			The x-position where the game over should be placed.
		y: float
			The y-position where the game over should be placed.
		z: float
			The z-position where the game over should be placed.
	*/
	glPushMatrix();
	glRasterPos3d(x, y, z);
	writeBitmapString(GLUT_BITMAP_HELVETICA_12, fish_sadness);
	glPopMatrix();
}



void draw_subcanvas(double x_min, double x_max, double y_min, double y_max) {
	glPushMatrix();
	glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_LINE_LOOP);
	glVertex2d(x_min, y_min);
	glVertex2d(x_max, y_min);
	glVertex2d(x_max, y_max);
	glVertex2d(x_min, y_max);
	glEnd();
	glPopMatrix();

}

void draw_line(double x1, double y1, double x2,double y2) {
	glPushMatrix();
	glBegin(GL_LINES);
	glVertex2d(x1, y1);
	glVertex2d(x2, y2);
	glEnd();
	glPopMatrix();
}

int compute_outcode(double x, double y) {
	int outcode = 0;
	if (y > y_max) {
		outcode |= TOP;
	}
	else if (y < y_min) {
		outcode |= BOTTOM;
	}
	if (x > x_max) {
		outcode |= RIGHT;
	}
	if (x < x_min) {
		outcode |= LEFT;
	}
	return (int)outcode;
}

void draw_cohen_sutherland_line_clip(double x1, double y1, double x2, double y2) {
	int outcode1 = compute_outcode(x1, y1);
	int outcode2 = compute_outcode(x2, y2);
	while (true) {
		if ((outcode1 & outcode2) != 0) {
			break;
		} else if ((outcode1 | outcode2) == 0) {
			draw_line(x1, y1, x2, y2);
			break;
		} else {
			double x, y;
			int outcode = outcode1 ? outcode1 : outcode2;
			if (outcode & LEFT) {
				y = y1 + (y2 - y1) * (x_min - x1) / (x2 - x1);
				x = x_min;
			} else if (outcode & RIGHT) {
				y = y1 + (y2 - y1) * (x_max - x1) / (x2 - x1);
				x = x_max;
			} else if (outcode & BOTTOM) {
				y = y_min;
				x = x1 + (x2 - x1)*((y_min - y1) / (y2 - y1));

			} else {
				y = y_max;
				x = x1 + (x2 - x1)*((y_max - y1) / (y2 - y1));
			}
			if (outcode == outcode1) {
				x1 = x;
				y1 = y;
				outcode1 = compute_outcode(x1, y1);
			} else {
				x2 = x;
				y2 = y;
				outcode2 = compute_outcode(x2, y2);
			}
		}
	}
}

void calculate_random_angle() {
	srand((unsigned int)time(NULL));
	const int array_num[2] = { -45, 45 };
	int rand_index = rand() % 2;
	random_rotation_angle = (int)array_num[rand_index];
	//find_bonus_food_coordinates();
	
}

void draw_fish_head_clockwise(double x1, double y1, double x2, double y2) {
	draw_cohen_sutherland_line_clip(x1, y1, x2, y2);
	draw_cohen_sutherland_line_clip(x2, y2, x2 + 9, y2 - 5);
	draw_cohen_sutherland_line_clip(x2 + 9, y2 - 5, x1, y1);
}

void draw_fish_head_anticlockwise(double x1, double y1, double x2, double y2) {
	draw_cohen_sutherland_line_clip(x1, y1, x2, y2);
	draw_cohen_sutherland_line_clip(x2, y2, x2 - 9, y2 - 5);
	draw_cohen_sutherland_line_clip(x2 - 9, y2 - 5, x1, y1);
}

void draw_fish_line(double x1, double y1, double x2, double y2, double angle) {
	glPushMatrix();
	glTranslated(x1, y1, 0);
	glRotated(angle, 0, 0, 1);
	glTranslated(-x1, -y1, 0);
	draw_cohen_sutherland_line_clip(x1, y1, x2, y2);
	glPopMatrix();
}

void draw_fish(double x1, double y1, double x2, double y2, bool clockwise, double height, double width, double angle) {
	glPushMatrix();
	glTranslated(x1, y1, 0);
	glRotated(random_rotation_angle, 0, 0, 1);
	glTranslated(-x1, -y1, 0);
	
	// without angle
	double y_height = height / 2;
	//double x_mid =  (x1+x2) / 2;
	double x_mid = (width - 10) / 2;
	draw_cohen_sutherland_line_clip(x1, y1, x1 + x_mid, y1 + y_height);
	
	draw_cohen_sutherland_line_clip(x1 + x_mid, y1 + y_height, x2, y2);
	draw_cohen_sutherland_line_clip(x1, y1, x1+x_mid, y1-y_height);
	draw_cohen_sutherland_line_clip(x1 + x_mid, y1 - y_height, x2, y2);
	
	if (clockwise) {
		draw_fish_head_clockwise(x2, y2-5, x2, y2+5);
	}
	else {
		draw_fish_head_anticlockwise(x1, y1 - 5, x1, y1 + 5);	
	}
	glPopMatrix();

}

void handle_food_ingest_fish1() {
	if ((x_tip_head_fish1 - x_cursor_food <= 10 && x_tip_head_fish1 - x_cursor_food > 0) || (x_tip_head_fish1 - x_cursor_food >= -10 && x_tip_head_fish1 - x_cursor_food < 0))
		//&&
		//(y_move_fish1 - y_cursor_food <= 10 && y_move_fish1 - y_cursor_food >= 0) || (y_move_fish1 - y_cursor_food >= -20 && y_move_fish1 - y_cursor_food <= 0)) 
	{
		fed = true;
	}
}


void handle_food_ingest_fish2() {
	if ((x_tip_head_fish2 - x_cursor_food <= 10 && x_tip_head_fish2 - x_cursor_food > 0) || (x_tip_head_fish2 - x_cursor_food >= -10 && x_tip_head_fish2 - x_cursor_food < 0))
		//&&
		//(y_move_fish1 - y_cursor_food <= 10 && y_move_fish1 - y_cursor_food >= 0) || (y_move_fish1 - y_cursor_food >= -20 && y_move_fish1 - y_cursor_food <= 0)) 
	{
		fed = true;
	}
}

void handle_bonus_food_ingest_fish1() {
	if ((x_tip_head_fish1 - bonus_x_pos<= 1 && x_tip_head_fish1 - bonus_x_pos > 0) || (x_tip_head_fish1 - bonus_x_pos >= -1 && x_tip_head_fish1 - bonus_x_pos < 0))
		//&&
		//(y_move_fish1 - y_cursor_food <= 10 && y_move_fish1 - y_cursor_food >= 0) || (y_move_fish1 - y_cursor_food >= -20 && y_move_fish1 - y_cursor_food <= 0)) 
	{
		fed = true;
		if (fed == true) {
			bonus_food = false;
			//score += 50;
			bonus_fed = true;
		}
		glutTimerFunc(10000, timer_func, 11);
	}
}


void handle_bonus_food_ingest_fish2() {
	if ((x_tip_head_fish2 - bonus_x_pos <= 1 && x_tip_head_fish2 - bonus_x_pos > 0) || (x_tip_head_fish2 - bonus_x_pos >= -1 && x_tip_head_fish2 - bonus_x_pos < 0))
		//&&
		//(y_move_fish1 - y_cursor_food <= 10 && y_move_fish1 - y_cursor_food >= 0) || (y_move_fish1 - y_cursor_food >= -20 && y_move_fish1 - y_cursor_food <= 0)) 
	{
		fed = true;
		if (fed == true) {
			bonus_food = false;
			bonus_fed = true;
			//score += 50;
		}
		//find_bonus_food_coordinates();
		glutTimerFunc(10000, timer_func, 11);
	}
}

void draw_bonus_food(double x, double y) {
	setup_light_source();
	float matAmb[] = { 0.5f, 0.35f, 0.05f, 1.0f };
	float matDif[] = { 0.5f, 0.35f, 0.05f, 1.0f };
	float matSpec[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glMaterialfv(GL_FRONT, GL_AMBIENT, matAmb);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, matDif);
	glMaterialfv(GL_FRONT, GL_SPECULAR, matSpec);
	glMaterialfv(GL_FRONT, GL_SHININESS, matShine);
	glPushMatrix();
	glTranslated(x, y, 0);
	//glutSolidTorus(4, 13, 30, 30);
	glutSolidCube(10);
	glPopMatrix();
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);
	

}

void draw_food(int x, int y) {
	glColor3d(1, 1, 0);
	
	glPushMatrix();
	glTranslated(x, y, 0);
	glutSolidCube(5);
	glPopMatrix();
	
}



void fish1_turn_back(int random_rotation_angle) {
	if (random_rotation_angle == 0) {
		if (x_tip_head_fish1 >= 350) {
			fish1_direction = false;
		}
		else if (x_tip_head_fish1 <= -350) {
			fish1_direction = true;
		}
	}
	else {
		if (x_tip_head_fish1 >= 270 || y_move_fish1 >= 250 || y_move_fish1 <= -250) {
			fish1_direction = false;
		}
		else if (x_tip_head_fish1 <= -270 || y_move_fish1 >= 250 || y_move_fish1 <= -250) {
			fish1_direction = true;
		}
	}
}

void fish2_turn_back(int random_rotation_angle) {
	if (random_rotation_angle == 0) {
		if (x_tip_head_fish2 >= 350) {
			fish2_direction = false;
		}
		else if (x_tip_head_fish2 <= -350) {
			fish2_direction = true;
		}
	}
	else {
		if (x_tip_head_fish2 >= 270 || y_move_fish2 >= 250 || y_move_fish2 <= -250) {
			fish2_direction = false;
		}
		else if (x_tip_head_fish2 <= -270 || y_move_fish2 >= 250 || y_move_fish2 <= -250) {
			fish2_direction = true;
		}
	}
}

void fish1_end_boundary_test() {
	if ((x_tip_head_fish1) >= 270) {
		x_move_fish1 = 270;
	}
	if ((x_tip_head_fish1) <= -270) {
		x_move_fish1 = -270;
	}
	if (y_move_fish1 >= 270) {
		y_move_fish1 = 270;
	}
	if (y_move_fish1 <= -270) {
		y_move_fish1 = -270;
	}
}

void fish2_end_boundary_test() {
	if ((x_tip_head_fish2) >= 270) {
		x_move_fish2 = 270;
	}
	if ((x_tip_head_fish2) <= -270) {
		x_move_fish2 = -270;
	}
	if (y_move_fish2 >= 270) {
		y_move_fish2 = 270;
	}
	if (y_move_fish2 <= -270) {
		y_move_fish2 = -270;
	}
}



void timer_func(int val) {
	/*
	This function is a glut timer callback function which displays 20 new frames in 1 second to enable lizard movement in both anticlockwise and clockwise direction,
	sets the fire_signal to 0 to disable laser beam display after 100 ms, decreases the lizard size by a factor of 2 if it has been hit once, spawns the lizard
	after 1000ms and exit after 2000ms at the end of game.
	Parameters:
	-----------
		value: Integer
			This is the timerID.
	*/
	switch (val) {
	case 1:
		if (random_rotation_angle == 0) {
			glutTimerFunc(0, timer_func, 2);
		}
		else if (random_rotation_angle == 45) {
			glutTimerFunc(0, timer_func, 3);
			}
		else {
			glutTimerFunc(0, timer_func, 4);
		}
		if (fish1_direction) {
			x_tip_head_fish1 = x_move_fish1 + 10;
		}
		else {
			x_tip_head_fish1 = x_move_fish1 - 10;
		}
		if (fish2_direction) {
			x_tip_head_fish2 = x_move_fish2 + 10;
		}
		else {
			x_tip_head_fish2 = x_move_fish2 - 10;
		}
		

		break;
	case 2:
		/*if (x_tip_head_fish1 >= 350) {
			fish1_direction = false;
		}
		else if (x_tip_head_fish1 <= -350) {
			fish1_direction = true;
		}*/
		fish1_turn_back(0);
		fish2_turn_back(0);
		handle_food_ingest_fish1();
		handle_food_ingest_fish2();
		handle_bonus_food_ingest_fish1();
		handle_bonus_food_ingest_fish2();
		if (fish1_direction) {
			x_move_fish1 += 4;
		}
		else {
			x_move_fish1 -= 4;
		}

		if (fish2_direction) {
			x_move_fish2 += 4;
		}
		else {
			x_move_fish2 -= 4;
		}
		glutPostRedisplay();
		glutTimerFunc(62, timer_func, 1);
		break;
	case 3:
		/*if (x_tip_head_fish1 >= 270 || y_move_fish1 >= 250 || y_move_fish1 <= -250) {
			fish1_direction = false;
		}
		else if (x_tip_head_fish1 <= -270 || y_move_fish1 >= 250 || y_move_fish1 <= -250) {
			fish1_direction = true;
		}*/
		fish1_turn_back(45);
		fish2_turn_back(45);
		handle_food_ingest_fish1();
		handle_food_ingest_fish2();
		handle_bonus_food_ingest_fish1();
		handle_bonus_food_ingest_fish2();
		if (fish1_direction) {
			x_move_fish1 += 4;
			y_move_fish1 += 4;
		}
		else {
			x_move_fish1 -= 4;
			y_move_fish1 -= 4;
		}

		if (fish2_direction) {
			x_move_fish2 += 4;
			y_move_fish2 += 4;
		}
		else {
			x_move_fish2 -= 4;
			y_move_fish2 -= 4;
		}
		fish1_end_boundary_test();
		fish2_end_boundary_test();
		glutPostRedisplay();
		glutTimerFunc(62, timer_func, 1);
		break;
	case 4:
		/*if (x_tip_head_fish1 >= 270 || y_move_fish1 >= 250 || y_move_fish1 <= -250) {
			fish1_direction = false;
		}
		else if (x_tip_head_fish1 <= -270 || y_move_fish1 >= 250|| y_move_fish1 <= -250) {
			fish1_direction = true;
		}*/
		fish1_turn_back(-45);
		fish2_turn_back(-45);
		handle_food_ingest_fish1();
		handle_food_ingest_fish2();
		handle_bonus_food_ingest_fish1();
		handle_bonus_food_ingest_fish2();
			if (fish1_direction) {
				x_move_fish1 += 4;
				y_move_fish1 -= 4;
			}
			else {
				x_move_fish1 -= 4;
				y_move_fish1 += 4;
			}

			if (fish2_direction) {
				x_move_fish2 += 4;
				y_move_fish2 -= 4;
				
			}
			else {
				x_move_fish2 -= 4;
				y_move_fish2 += 4;
			}
			fish1_end_boundary_test();
			fish2_end_boundary_test();
			glutPostRedisplay();
			glutTimerFunc(62, timer_func, 1);
		break;
	case 5:
		exit(0);
		break;

	case 6:
		calculate_random_angle();
		glutTimerFunc(2000, timer_func, 6);
		break;
	case 7:
		food_flag = false;
		//glutPostRedisplay();
		break;
	case 8:
		timer -= 1;
		if (timer < 0){
			timer = 0;
		}
		
		glutTimerFunc(1000, timer_func, 8);
		break;
	case 10:
		happy_fish = false;
		sad_fish = false;
		break;
	case 11:
		if (bonus_fed) {
			timer = 8;
			score += 50;
			happy_fish = true;
			bonus_fed = false;
		}
		//else {
			//sad_fish = true;
		//}
		glutTimerFunc(1000, timer_func, 10);
		if (bonus_food == false) {
			find_bonus_food_coordinates();
		}
		
		handle_bonus_food_ingest_fish1();
		handle_bonus_food_ingest_fish2();
		
		//glutTimerFunc(3000, timer_func, 11);
		break;
	default:
		break;
	}
}



void display_func(void) {
	/*
	This is a glut display callback handler which is called whenever a window needs to be displayed or redisplayed. It clears the
	canvas screen and reloads all the objects of the scene when called. This function also clears the food when lizard passes over
	them, displays the text score and the text game over at the end of the game.
	*/
	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();
	draw_subcanvas(x_min, x_max, y_min, y_max);
	glColor3d(0, 0.7, 1);
	draw_fish(x_move_fish1, y_move_fish1, 40 + x_move_fish1, y_move_fish1, fish1_direction, 25, 50, 45);
	glColor3d(0, 0.9, 0.1);
	draw_fish(x_move_fish2, y_move_fish2, 30 + x_move_fish2, y_move_fish2, fish2_direction, 30, 40, 40);
	
	display_score(250, -270);
	display_timer(300, 270);
	if (food_flag) {
		draw_food(x_cursor_food, y_cursor_food);
	}
	
	if (end_game) {
		glutKeyboardFunc(NULL);
		display_game_over(-50, 0, -150);
	}
	if (happy_fish) {
		glColor3f(1, 1, 1);
		display_happy_msg(-400,-270,0);
		glutTimerFunc(1000, timer_func, 10);
	}

	if (sad_fish) {
		glColor3f(1, 1, 1);
		display_sad_msg(-400,-270,0);
		glutTimerFunc(1000, timer_func, 10);
	}
	if (bonus_food) {
		draw_bonus_food(bonus_x_pos, bonus_y_pos);
	}
	
	//glutTimerFunc(1000, timer_func, 7);
	glutSwapBuffers();
	glFlush();

}

void passive_motion_handler(int x, int y) {
	cout << "measured x is" << x << endl;
	cout << "measuered y is " << y << endl;	
	x_cursor_food = x - 400;
	y_cursor_food = 300 - y;
	if (x_cursor_food >= x_min && x_cursor_food <= x_max && y_cursor_food >= y_min && y_cursor_food <= y_max) {
		food_flag = true;
		glutPostRedisplay();
	}
	cout << "food x is" << x_cursor_food << endl;
	cout << "food y is " << y_cursor_food << endl;
	

}

void motion_handler(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN){
		//&& x < bonus_x_pos -10 && x > bonus_x_pos + 10 && y < bonus_y_pos -10 && y> bonus_y_pos+10) {
		bonus_x_pos = (x-400);
		bonus_y_pos =  (300-y);
		bonus_food = true;
		glutPostRedisplay();
	}
	
	
}

void motion_handler2(int x, int y) {
		bonus_x_pos = (x - 400);
		bonus_y_pos = (300 - y);
		bonus_food = true;
		//draw_bonus_food(bonus_x_pos, bonus_y_pos);
		glutPostRedisplay();
	}



void mouse_handler(int botton, int state, int x, int y) {
	cout << "went here" << endl;
	//if (x > x_min && x < x_max && y > y_min && y < y_max) {
	food_flag = true;
	//x_cursor_food = x;
	//y_cursor_food = y;
	cout << "x_cursor_food is" << x_cursor_food << endl;
	cout << "y_cursor_food is" << y_cursor_food << endl;
	glutPostRedisplay();
}

void keyboard_func(unsigned char key, int x, int y) {
	/*
	This function is a glut keyboard callback handler. Whenever a key is pressed, it gets stored in a key
	variable. The switch condition matches the corresponding key condition and perform necessary
	computation.
	Parameters:
	-----------
		key: unsigned char
			The key input received
		x: Integer
			Mouse x-coordinate
		y: Integer
			Mouse y-coordinate
	*/
	switch (key)
	{
	case 'H': case'h':
		x_min = x_min - 5;
		x_max = x_max - 5;
		//glutPostRedisplay();
		break;
	case 'J': case'j':
		x_min = x_min + 5;
		x_max = x_max + 5;
		//glutPostRedisplay();
		break;
	case 'U': case 'u':
		y_min = y_min + 5;
		y_max = y_max + 5;
		//glutPostRedisplay();
		break;
	case 'N': case 'n':
		y_min = y_min - 5;
		y_max = y_max - 5;
		//glutPostRedisplay();
		break;
	case 'F': case 'f':
		//fed = false;
		x_cursor_food = x - 400;
		y_cursor_food = 300 - y;
		if (x_cursor_food >= x_min && x_cursor_food <= x_max && y_cursor_food >= y_min && y_cursor_food <= y_max) {
			food_flag = true;
			glutPostRedisplay();
		}
		if (food_flag && fed) {
			timer = 8;
			score += 50;
			happy_fish = true;
			fed = false;
		}
		else {
			sad_fish = true;
		}
		glutTimerFunc(1000, timer_func, 7);
		break;

	default:
		break;
	}
}

void measure(int button, int state, int x, int y) {
	cout << "measured x is" << x << endl;
	cout << "measuered y is " << y << endl;
}

void measure_one(int x, int y) {
	cout << "measured x is" << x << endl;
	cout << "measuered y is " << y << endl;
	food_flag = true;
	x_cursor_food = x-400;
	y_cursor_food = 300-y;
	cout << "food x is" << x_cursor_food << endl;
	cout << "food y is " << y_cursor_food << endl;
	glutPostRedisplay();

}


/************** MAIN FUNCTION **************/
int main(int argc, char ** argv) {
	// Setup preliminaries
	glutInit(&argc, argv);
	my_setup(canvas_Width, canvas_Height, canvas_Name);
	glutDisplayFunc(display_func);
	glutKeyboardFunc(keyboard_func);
	glutTimerFunc(62, timer_func, 1);
	glutTimerFunc(2000, timer_func, 6);
	glutTimerFunc(1000, timer_func, 8);
	//glutMouseFunc(motion_handler);
	glutMotionFunc(motion_handler2);
	//not needed functions
	glutTimerFunc(6000, timer_func, 11);
	//glutTimerFunc(50000, timer_func, 5);
	//glutMouseFunc(measure);
	//glutTimerFunc(62, timer_func, 10);
	//glutPassiveMotionFunc(measure_one);
	//glutPassiveMotionFunc(passive_motion_handler);
	// Set up light source at the world origin. Initialized at main to reduce overhead though small if initialized in the
	// display callback handler
	//setup_light_source();
	init();
	glutMainLoop();
	return 0;
}
