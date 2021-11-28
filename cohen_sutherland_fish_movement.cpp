#include "pch.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <math.h> 
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "OpenGL445Setup.h"

using namespace std;

static bool end_game;
static int x_min;
static int x_max;
static int y_min;
static int y_max;

static double move = 4;

static double x_move_fish1;
static double y_move_fish1;
static bool fish1_direction;
static bool fish2_direction;

static string fish_name;

static double x_cursor_food;
static double y_cursor_food;

static double bonus_x_pos;
static double bonus_y_pos;

static double x_move_fish2;
static double y_move_fish2;
static double x_tip_head_fish1;
static double x_tip_head_fish2;

static bool fed;
static bool bonus_fed;

bool food_flag;
bool happy_fish;
bool sad_fish;
bool happy_fish1;
bool happy_fish2;
bool unhappy_fish1;
bool unhappy_fish2;
bool bonus_food;
bool food;

bool happy_face_fish1;
bool happy_face_fish2;

bool fish1_fed;
bool fish2_fed;
int fish1_score;
int fish2_score;

static int random_rotation_angle;

static int score;			//Flag to keep track of the score
static int timer;

static float matShine[] = { 100 };	//Shininess value used throughout the program


char canvas_Name[] = "Fish Movement with Cohen Sutherland"; // Name at the top of canvas


enum {
	INSIDE = 0, LEFT = 1, RIGHT = 2, BOTTOM = 4, TOP = 8
};


// Sets width and height of canvas to 600 by 600.
#define canvas_Width 800
#define canvas_Height 600

//Function prototypes
void timer_func(int val);
void display_func(void);


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

	x_move_fish2 = 75;
	y_move_fish2 = 0;
	fish1_direction = true; //clockwise
	fish2_direction = false;
	food_flag = false;
	fed = false;

	x_cursor_food = 0;
	y_cursor_food = 0;
	x_tip_head_fish1 = 0;
	x_tip_head_fish2 = 0;
	bonus_x_pos = 0;
	bonus_y_pos = 0;

	fish1_score = 0;
	fish2_score = 0;

	food = false;
	bonus_food = false;
	bonus_fed = false;
	fish1_fed = false;
	fish2_fed = false;
	
	happy_fish = false;
	sad_fish = false;
	happy_fish1 = false;
	happy_fish2 = false;
	unhappy_fish1 = false;
	unhappy_fish2 = false;

	happy_face_fish1 = false;
	happy_face_fish2 = false;
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

void draw_line(double x1, double y1, double x2, double y2) {
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
		}
		else if ((outcode1 | outcode2) == 0) {
			draw_line(x1, y1, x2, y2);
			break;
		}
		else {
			double x, y;
			int outcode = outcode1 ? outcode1 : outcode2;
			if (outcode & LEFT) {
				y = y1 + (y2 - y1) * (x_min - x1) / (x2 - x1);
				x = x_min;
			}
			else if (outcode & RIGHT) {
				y = y1 + (y2 - y1) * (x_max - x1) / (x2 - x1);
				x = x_max;
			}
			else if (outcode & BOTTOM) {
				y = y_min;
				x = x1 + (x2 - x1)*((y_min - y1) / (y2 - y1));

			}
			else {
				y = y_max;
				x = x1 + (x2 - x1)*((y_max - y1) / (y2 - y1));
			}
			if (outcode == outcode1) {
				x1 = x;
				y1 = y;
				outcode1 = compute_outcode(x1, y1);
			}
			else {
				x2 = x;
				y2 = y;
				outcode2 = compute_outcode(x2, y2);
			}
		}
	}
}

void draw_fish_head_clockwise_new(double x, double y, double height_l1) {
	draw_cohen_sutherland_line_clip(x, y + 10, x, y - 10);
	draw_cohen_sutherland_line_clip(x, y + 10, x + height_l1, y);
	draw_cohen_sutherland_line_clip(x, y - 10, x + height_l1, y);
}

void draw_fish_head_anticlockwise_new(double x, double y, double height_l1) {
	draw_cohen_sutherland_line_clip(x, y + 10, x, y - 10);
	draw_cohen_sutherland_line_clip(x, y + 10, x - height_l1, y);
	draw_cohen_sutherland_line_clip(x, y - 10, x - height_l1, y);
}

void handle_fish1_move(double x, double y, bool clockwise, string fishname, double diamond_l1, double head_l1) {
	if (clockwise) {
		x_tip_head_fish1 = x + diamond_l1 + head_l1;
		if (happy_face_fish1) {
			draw_cohen_sutherland_line_clip(x + diamond_l1 + 3, y, x + diamond_l1 + 4, y - 4);
			draw_cohen_sutherland_line_clip(x + diamond_l1 + 4, y - 4, x + diamond_l1 + 7, y);
		}
		else if (unhappy_fish1) {
			draw_cohen_sutherland_line_clip(x + diamond_l1 + 4, y, x + diamond_l1 + 2, y - 4);
			draw_cohen_sutherland_line_clip(x + diamond_l1 + 4, y, x + diamond_l1 + 7, y - 4);
		}
	}
	else if (!clockwise) {
		x_tip_head_fish1 = x - diamond_l1 - head_l1;
		if (happy_face_fish1) {
			draw_cohen_sutherland_line_clip(x - diamond_l1 - 3, y, x - diamond_l1 - 4, y - 4);
			draw_cohen_sutherland_line_clip(x - diamond_l1 - 4, y - 4, x - diamond_l1 - 7, y);
		}
		else if (unhappy_fish1) {
			draw_cohen_sutherland_line_clip(x - diamond_l1 - 4, y, x - diamond_l1 - 2, y - 4);
			draw_cohen_sutherland_line_clip(x - diamond_l1 - 4, y, x - diamond_l1 - 7, y - 4);
		}
	}
}

void handle_fish2_move(double x, double y, bool clockwise, string fishname, double diamond_l1, double head_l1) {
	if (clockwise) {
	x_tip_head_fish2 = x + diamond_l1 + head_l1;
	if (happy_face_fish2) {
		draw_cohen_sutherland_line_clip(x + diamond_l1 + 3, y, x + diamond_l1 + 4, y - 4);
		draw_cohen_sutherland_line_clip(x + diamond_l1 + 4, y - 4, x + diamond_l1 + 7, y);
	}
	else if (unhappy_fish2) {
		draw_cohen_sutherland_line_clip(x + diamond_l1 + 4, y, x + diamond_l1 + 2, y - 4);
		draw_cohen_sutherland_line_clip(x + diamond_l1 + 4, y, x + diamond_l1 + 7, y - 4);
	}
	}
	else if (!clockwise) {
	x_tip_head_fish2 = x - diamond_l1 - head_l1;
	if (happy_face_fish2) {
		draw_cohen_sutherland_line_clip(x - diamond_l1 - 3, y, x - diamond_l1 - 4, y - 4);
		draw_cohen_sutherland_line_clip(x - diamond_l1 - 4, y - 4, x - diamond_l1 - 7, y);
	}
	else if (unhappy_fish2) {
		draw_cohen_sutherland_line_clip(x - diamond_l1 - 4, y, x - diamond_l1 - 2, y - 4);
		draw_cohen_sutherland_line_clip(x - diamond_l1 - 4, y, x - diamond_l1 - 7, y - 4);
	}
	}
}

void draw_fish_new(double x, double y, int length, int height, bool clockwise, string fishname) {
	// x and y are the fish centers
	glPushMatrix();
	double head_l1 = 10;
	double diamond_l1 = (length - head_l1) / 2;
	double h1 = height / 2;
	double hypotenuse = sqrt(diamond_l1 * diamond_l1 + h1 * h1);
	draw_cohen_sutherland_line_clip(x + diamond_l1, y, x, y + hypotenuse);
	draw_cohen_sutherland_line_clip(x, y + hypotenuse, x - diamond_l1, y);
	draw_cohen_sutherland_line_clip(x - diamond_l1, y, x, y - hypotenuse);
	draw_cohen_sutherland_line_clip(x, y - hypotenuse, x + diamond_l1, y);
	if (fishname == "fish1") {
		handle_fish1_move(x, y, clockwise, fishname, diamond_l1, head_l1);
	}
	else if (fishname == "fish2") {
		handle_fish2_move(x, y, clockwise, fishname, diamond_l1, head_l1);
	}
	
	if (clockwise) {
		draw_fish_head_clockwise_new(x + diamond_l1, y, head_l1);
	}
	else {
		draw_fish_head_anticlockwise_new(x - diamond_l1, y, head_l1);
	}
	glPopMatrix();
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

void display_fish1_score(float x, float y) {
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
	char score_text[20] = "FISH1 SCORE: ";
	char score_num[6];
	sprintf_s(score_num, 6, "%d", fish1_score);
	strcat_s(score_text, 20, score_num);
	writeBitmapString(GLUT_BITMAP_HELVETICA_10, score_text);
	glPopMatrix();
}

void display_fish2_score(float x, float y) {
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
	char score_text[20] = "FISH2 SCORE: ";
	char score_num[6];
	sprintf_s(score_num, 6, "%d", fish2_score);
	strcat_s(score_text, 20, score_num);
	writeBitmapString(GLUT_BITMAP_HELVETICA_10, score_text);
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
	char game_over[] = "GAME OVER"; // Display after game is over
	glColor3f(1.0, 0.0, 0.0);
	glPushMatrix();
	glRasterPos3d(x, y, z);
	writeBitmapString(GLUT_BITMAP_HELVETICA_18, game_over);
	glPopMatrix();
}

void display_fish1_happy_msg(double x, double y, double z) {
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
	char fish1_happiness[] = "Fish1 is happy. It has just been fed.";
	glPushMatrix();
	glRasterPos3d(x, y, z);
	writeBitmapString(GLUT_BITMAP_HELVETICA_12, fish1_happiness);
	glPopMatrix();
}

void display_fish2_happy_msg(double x, double y, double z) {
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
	char fish2_happiness[] = "Fish2 is happy. It has just been fed.";
	glPushMatrix();
	glRasterPos3d(x, y, z);
	writeBitmapString(GLUT_BITMAP_HELVETICA_12, fish2_happiness);
	glPopMatrix();
}

void display_fish1_unhappy_msg(double x, double y, double z) {
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
	char fish1_happiness[] = "Fish1 is unhappy. It hasn't been fed for 8s.";
	glPushMatrix();
	glRasterPos3d(x, y, z);
	writeBitmapString(GLUT_BITMAP_HELVETICA_12, fish1_happiness);
	glPopMatrix();
}

void display_fish2_unhappy_msg(double x, double y, double z) {
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
	char fish2_happiness[] = "Fish2 is unhappy. It hasn't been fed for 8s.";
	glPushMatrix();
	glRasterPos3d(x, y, z);
	writeBitmapString(GLUT_BITMAP_HELVETICA_12, fish2_happiness);
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
	char fish_happiness[] = "The fish is happy. It has been recently fed.";
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
	char fish_sadness[] = "Both fishes are sad. They haven't been recently fed.";
	glPushMatrix();
	glRasterPos3d(x, y, z);
	writeBitmapString(GLUT_BITMAP_HELVETICA_12, fish_sadness);
	glPopMatrix();
}

void draw_food(int x, int y) {
	glColor3d(1, 1, 0);
	glPushMatrix();
	glTranslatef(x, y, 0);
	glutSolidCube(5);
	glPopMatrix();

}

void fish1_turn_back(int random_rotation_angle) {
	if (random_rotation_angle == 0) {
		if (x_tip_head_fish1 >= 385) {
			fish1_direction = false;
		}
		else if (x_tip_head_fish1 <= -385) {
			fish1_direction = true;
		}
	}
	else {
		if (x_tip_head_fish1 >= 385 || y_move_fish1 >= 270 || y_move_fish1 <= -270) {
			fish1_direction = false;
		}
		else if (x_tip_head_fish1 <= -385 || y_move_fish1 >= 270 || y_move_fish1 <= -270) {
			fish1_direction = true;
		}
	}
}

void fish2_turn_back(int random_rotation_angle) {
	if (random_rotation_angle == 0) {
		if (x_tip_head_fish2 >= 385) {
			fish2_direction = false;
		}
		else if (x_tip_head_fish2 <= -385) {
			fish2_direction = true;
		}
	}
	else {
		if (x_tip_head_fish2 >= 385 || y_move_fish2 >= 270 || y_move_fish2 <= -270) {
			fish2_direction = false;
		}
		else if (x_tip_head_fish2 <= -385 || y_move_fish2 >= 270 || y_move_fish2 <= -270) {
			fish2_direction = true;
		}
	}
}

void fish1_end_boundary_test() {
	if ((x_tip_head_fish1) >= 385) {
		x_move_fish1 = 350;
	}
	if ((x_tip_head_fish1) <= -385) {
		x_move_fish1 = -350;
	}
	if (y_move_fish1 >= 280) {
		y_move_fish1 = 210;
		random_rotation_angle = -random_rotation_angle;
	}
	if (y_move_fish1 <= -280) {
		y_move_fish1 = -210;
		random_rotation_angle = -random_rotation_angle;
	}
	fish1_turn_back(random_rotation_angle);
}

void fish2_end_boundary_test() {
	if ((x_tip_head_fish2) >= 385) {
		x_move_fish2 = 350;
	}
	if ((x_tip_head_fish2) <= -385) {
		x_move_fish2 = -350;
	}
	if (y_move_fish2 >= 280) {
		y_move_fish2 = 210;
		random_rotation_angle = -random_rotation_angle;
	}
	if (y_move_fish2 <= -280) {
		y_move_fish2 = -210;
		random_rotation_angle = -random_rotation_angle;
	}
	fish2_turn_back(random_rotation_angle);
}

void handle_food_ingest_fish1() {
	if ((x_tip_head_fish1 - x_cursor_food <= 10 && x_tip_head_fish1 - x_cursor_food > 0) || (x_tip_head_fish1 - x_cursor_food >= -10 && x_tip_head_fish1 - x_cursor_food < 0)
		&&
		(y_move_fish1 + 12.5 - y_cursor_food <= 10 && y_move_fish1 + 12.5 - y_cursor_food > 0) || (y_move_fish1 + 12.5 - y_cursor_food >= -10 && y_move_fish1 + 12.5 - y_cursor_food < 0)
		&&
		((y_move_fish1 - 12.5 - y_cursor_food <= 10 && y_move_fish1 - 12.5 - y_cursor_food > 0) || (y_move_fish1 - 12.5 - y_cursor_food >= -10 && y_move_fish1 - 12.5 - y_cursor_food < 0)))
	{
		fed = true;
		fish1_fed = true;
	}
}


void handle_food_ingest_fish2() {
	if ((x_tip_head_fish2 - x_cursor_food <= 10 && x_tip_head_fish2 - x_cursor_food > 0) || (x_tip_head_fish2 - x_cursor_food >= -10 && x_tip_head_fish2 - x_cursor_food < 0)
		&&
		(y_move_fish2 - y_cursor_food <= 10 && y_move_fish2 - y_cursor_food > 0) || (y_move_fish2 - y_cursor_food >= -10 && y_move_fish2 - y_cursor_food < 0)) 
	{
		fed = true;
		fish2_fed = true;
	}

}

void handle_ingest_fish1() {
	if (fish1_direction) {
		if ((x_cursor_food >= x_move_fish1 - 30 && x_cursor_food <= x_move_fish1 + 40) && (y_cursor_food >= y_move_fish1 - 22.5 && y_cursor_food <= y_move_fish1 + 22.5)) {
			fed = true;
			fish1_fed = true;
		}
	}
	else if(!fish1_direction) {
		if ((x_cursor_food >= x_move_fish1 - 40 && x_cursor_food <= x_move_fish1 + 30) && (y_cursor_food >= y_move_fish1 - 22.5 && y_cursor_food <= y_move_fish1 + 22.5)) {
			fed = true;
			fish1_fed = true;
		}
	}
	//if(x_cursor_food > x_tip)

}

void handle_ingest_fish2() {
	if (fish2_direction) {
		if ((x_cursor_food >= x_move_fish2 - 25 && x_cursor_food <= x_move_fish2 + 35) && (y_cursor_food >= y_move_fish2 - 25 && y_cursor_food <= y_move_fish2 + 25)) {
			fed = true;
			fish2_fed = true;
		}
	}
	else if (!fish2_direction) {
		if ((x_cursor_food >= x_move_fish2 - 35 && x_cursor_food <= x_move_fish2 + 25) && (y_cursor_food >= y_move_fish2 - 25 && y_cursor_food <= y_move_fish2 + 25)) {
			fed = true;
			fish2_fed = true;
		}
	}
	//if(x_cursor_food > x_tip)

}

void handle_bonus_ingest_fish1() {
	if (fish1_direction) {
		if ((bonus_x_pos >= x_move_fish1 - 30 && bonus_x_pos <= x_move_fish1 + 40) && (bonus_y_pos >= y_move_fish1 - 22.5 && bonus_y_pos <= y_move_fish1 + 22.5)) {
			bonus_fed = true;
			bonus_food = false;
			fish1_fed = true;
		}
	}
	else if (!fish1_direction) {
		if ((bonus_x_pos >= x_move_fish1 - 40 && bonus_x_pos <= x_move_fish1 + 30) && (bonus_y_pos >= y_move_fish1 - 22.5 && bonus_y_pos <= y_move_fish1 + 22.5)) {
			bonus_fed = true;
			bonus_food = false;
			fish1_fed = true;
		}
	}
	//if(x_cursor_food > x_tip)

}

void handle_bonus_ingest_fish2() {
	if (fish2_direction) {
		if ((bonus_x_pos >= x_move_fish2 - 25 && bonus_x_pos <= x_move_fish2 + 35) && (bonus_y_pos >= y_move_fish2 - 25 && bonus_y_pos <= y_move_fish2 + 25)) {
			bonus_fed = true;
			bonus_food = false;
			fish2_fed = true;
		}
	}
	else if (!fish2_direction) {
		if ((bonus_x_pos >= x_move_fish2 - 35 && bonus_x_pos <= x_move_fish2 + 25) && (bonus_y_pos >= y_move_fish2 - 25 && bonus_y_pos <= y_move_fish2 + 25)) {
			bonus_fed = true;
			bonus_food = false;
			fish2_fed = true;
		}
	}
	//if(x_cursor_food > x_tip)

}


void setup_light_source(void) {
	/* This function turns on OpenGL lighting and keeps light at the world origin.*/

	// Set light property vectors
	float lightAmb[] = { 1, 1, 1, 1 };
	float lightDif[] = { 1, 1, 1, 1 };
	float lightSpec[] = { 1, 1, 1, 1 };

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
	glutSolidTorus(6, 13, 30, 30);
	glPopMatrix();
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);

}



void calculate_random_angle() {
	srand((unsigned int)time(NULL));
	const int array_num[2] = { -45, 45 };
	int rand_index = rand() % 2;
	random_rotation_angle = (int)array_num[rand_index];

}

void motion_handler2(int x, int y) {
	bonus_x_pos = (x - 400);
	bonus_y_pos = (300 - y);
	bonus_food = true;
	//draw_bonus_food(bonus_x_pos, bonus_y_pos);
	//glutPostRedisplay();
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
		
		glutTimerFunc(1000, timer_func, 7);
		break;

	default:
		break;
	}
}

void handle_rotation_angle_zero_move() {
	fish1_turn_back(0);
	fish2_turn_back(0);
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
}

void handle_rotation_angle_45_move() {
	fish1_turn_back(45);
	fish2_turn_back(45);
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
}

void handle_rotation_angle_minus_45_move() {
	fish1_turn_back(-45);
	fish2_turn_back(-45);
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

}

void handle_bonus_food_ingest() {
	handle_bonus_ingest_fish1();
	handle_bonus_ingest_fish2();
	if (bonus_fed and fish1_fed) {
		timer = 8;
		score += 50;
		fish1_score += 50;
		fish1_fed = false;
		happy_fish1 = true;
		happy_face_fish1 = true;

		bonus_fed = false;
		bonus_food = false;

	}
	else if (bonus_fed and fish2_fed) {
		timer = 8;
		score += 50;
		fish2_score += 50;
		fish2_fed = false;
		happy_fish2 = true;
		happy_face_fish2 = true;

		bonus_fed = false;
		bonus_food = false;
	}
}


void handle_food_ingest() {
	handle_ingest_fish1();
	handle_ingest_fish2();
	if (fed and fish1_fed) {
		timer = 8;
		score += 50;
		fish1_score += 50;
		fish1_fed = false;
		fed = false;
		happy_fish1 = true;
		happy_face_fish1 = true;
	}
	else if (fed and fish2_fed) {
		timer = 8;
		score += 50;
		fish2_score += 50;
		fish2_fed = false;
		fed = false;
		happy_fish2 = true;
		happy_face_fish2 = true;
	}
	//else {
		//sad_fish = true;

	//}
	food_flag = false;
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
		break;

	case 2:
		handle_rotation_angle_zero_move();
		glutPostRedisplay();
		glutTimerFunc(63, timer_func, 1);
		break;
	case 3:
		handle_rotation_angle_45_move();
		glutPostRedisplay();
		glutTimerFunc(63, timer_func, 1);
		break;
	case 4:
		handle_rotation_angle_minus_45_move();
		glutPostRedisplay();
		glutTimerFunc(63, timer_func, 1);
		break;
	case 5:
		exit(0);
		break;
	case 6:
		calculate_random_angle();
		glutTimerFunc(2000, timer_func, 6);
		break;
	case 7:
		handle_food_ingest();
		break;
	case 8:
		timer -= 1;
		if (timer <= 0) {
			timer = 0;
			glutKeyboardFunc(NULL);
			glutMouseFunc(NULL);

			if (!happy_fish1 and !happy_fish2) {
				sad_fish = true;
				unhappy_fish1 = true;
				unhappy_fish2 = true;

			} else if (!happy_fish1 and !sad_fish) {
				unhappy_fish1 = true;
				sad_fish = false;
				glutPostRedisplay();
			}
			else if (!happy_fish2 and !sad_fish) {
				unhappy_fish2 = true;
				sad_fish = false;
				glutPostRedisplay();
			}
			end_game = true;
			glutTimerFunc(2000, timer_func, 5);
		}

		glutTimerFunc(1000, timer_func, 8);
		break;
	case 10:
		happy_fish = false;
		happy_fish1 = false;
		happy_fish2 = false;
		sad_fish = false;
		break;
	case 11:
		if (bonus_food == false) {
			find_bonus_food_coordinates();
			glutPostRedisplay();
		}
		glutTimerFunc(100, timer_func, 12);
		break;
		
	case 12:
		handle_bonus_food_ingest();
			
		glutTimerFunc(1000, timer_func, 10);
		glutTimerFunc(5000, timer_func, 11);

		break;
	default:
		break;
	}
}


void display_messages(void) {
	if (food_flag) {
		draw_food(x_cursor_food, y_cursor_food);
	}
	if (sad_fish) {
		glColor3f(1, 1, 1);
		display_sad_msg(-400, -270, 0);
		glutTimerFunc(4000, timer_func, 10);
	}
	else if (happy_fish1) {
		glColor3f(1, 1, 1);
		display_fish1_happy_msg(-400, -270, 0);
		glutTimerFunc(4000, timer_func, 10);
	}
	else if (happy_fish2) {
		glColor3f(1, 1, 1);
		display_fish2_happy_msg(-400, -270, 0);
		glutTimerFunc(4000, timer_func, 10);
	}
	else if (!sad_fish and unhappy_fish1) {
		glColor3f(1, 1, 1);
		display_fish1_unhappy_msg(-400, -270, 0);
		glutTimerFunc(4000, timer_func, 10);
	}
	else if (!sad_fish and unhappy_fish2) {
		glColor3f(1, 1, 1);
		display_fish2_unhappy_msg(-400, -270, 0);
		glutTimerFunc(4000, timer_func, 10);
	}
	if (bonus_food) {
		draw_bonus_food(bonus_x_pos, bonus_y_pos);
	}
	if (end_game) {
		display_game_over(0, 0, 0);
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
	draw_fish_new(x_move_fish1, y_move_fish1, 50, 25, fish1_direction, "fish1");
	glColor3d(0, 0.9, 0.1);
	draw_fish_new(x_move_fish2, y_move_fish2, 40, 30, fish2_direction, "fish2");

	display_score(250, -270);
	display_fish1_score(-370, 270);
	display_fish2_score(-370, 250);
	display_timer(300, 270);
	display_messages();

	glutSwapBuffers();
	glFlush();

}

/************** MAIN FUNCTION **************/
int main(int argc, char ** argv) {
	// Setup preliminaries
	glutInit(&argc, argv);
	my_setup(canvas_Width, canvas_Height, canvas_Name);
	glutDisplayFunc(display_func);
	glutTimerFunc(63, timer_func, 1);
	glutTimerFunc(2000, timer_func, 6);
	glutTimerFunc(1000, timer_func, 8);
	//glutTimerFunc(1000, timer_func, 11);
	glutTimerFunc(60000, timer_func, 5);
	glutMotionFunc(motion_handler2);
	glutKeyboardFunc(keyboard_func);
	init();
	glutMainLoop();
	return 0;
}