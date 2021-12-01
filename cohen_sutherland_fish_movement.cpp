/*CS 445/545 Program 5 for Digya Acharya*/

/***********************************************************************************************

cohen_sutherland_fish_movement.cpp

EXTRA CREDIT: All four extra credit features have been implemented, i.e, changing timer's color to orange when it's

at 3 or below, adding a smile to a fish when it has been fed and adding a frown on a fish that has not been fed in more

than 8 seconds (uses global flags fish1_fed,  fish2_fed, happy_fish1, happy_fish2, sad_fish, unhappy_fish1,

unhappy_fish2, unhappy_face_fish1, unhappy_face_fish2 for this), displaying messages on the screen bottom about fish

happiness and adding random bonus food which user can drag to the fish with the mouse (global variables bonus fed, bonus

food, and function find_bonus_food_coordinates has been used for this), bonus food has a different color than regular

food and lit according to Phong illumination with suitable specular effects. Sometimes, feeding bonus food may require

a click when the fishes are moving.

Apart from this, fish1 score and fish2 score are displayed on the top left part of the screen which increases the score of

a particular fish after it gets fed (uses global variable fish1_score and fish2_score for this).



Software Architecture Statement: This program illustrates the implementation of cohen sutherland line clipping algorithm in 

the line-drawn moving fishes using four callback functions: display_func(), the display callback handler 

which displays tank, fishes, conditionally displays food, and  text on the screen when the flags are turned on; timer_func(),

the timer callback handler which moves fishes and randomly rotates its movement; keyboard_func(), the glut keyboard handler which handles

display of food, movement of box and motion_handler(),the glut motion callback handler which handles dragging of food to the fish.

x_move_fish1, y_move_fish1, x_move_fish2 and y_move_fish2 are the major global variables which handles fish1 and fish2 movement respectively.



Other major global variables: x_min, x_max, y_min, y_max are used to determine the size of subcanvas; fish1_direction and

fish2_direction are used for determining the direction of fish; fish_name is used for determining the particular fish move

direction and whether particular fish has been fed; x_cursor_food and y_cursor_food is displaying food at the position of

cursor; x_tip_head_fish1 and x_tip_head_fish2 for keeping track of fish head tip; food_flag, bonus_food, bonus_fed, fed, fish1_fed,

fish2_fed for determining the fed status of the food, random_rotation_angle for randomly rotating angle of fish movement; score and

timer for keeping track of score and time of the game.

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



static int x_min;					// determine size of subcanvas 

static int x_max;

static int y_min;

static int y_max;



static double x_move_fish1;			// x and y coordinate fo fish1 and fish2

static double y_move_fish1;			// Set to double instead of typecasting as they will be used in cohen sutherland 								

static double x_move_fish2;			// algorithm

static double y_move_fish2;



static bool fish1_direction;		// Keeps track of direction of fish (clockwise or anticlockwise)

static bool fish2_direction;



static string fish_name;			// Keeps track of the fish name



static float x_cursor_food;			// x and y coordinate of where food should be drawn on pressing 'F' key	

static float y_cursor_food;



static float bonus_x_pos;				// x and y coordinate of where random bonus food should be placed

static float bonus_y_pos;



static double x_tip_head_fish1;		// tip of head of fish 1 and fish 2

static double x_tip_head_fish2;



static bool fed;					// fed status of normal food and bonus food

static bool bonus_fed;



static bool food_flag;				// flags to whether draw normal food and bonus food

static bool bonus_food;



static bool happy_fish;				// flags to keep track of happiness of fish		

static bool sad_fish;

static bool happy_fish1;

static bool happy_fish2;

static bool unhappy_fish1;			// flag to keep track whether unhappy fish 1 should be drawn

static bool unhappy_fish2;			// flag to keep track whether unhappy fish 2 should be drawn



static bool happy_face_fish1;		// flags to keep track of whether the face of fishes should be happy

static bool happy_face_fish2;



static bool unhappy_face_fish1;		// flags to keep track of whether the face of fishes should be unhappy

static bool unhappy_face_fish2;



static bool fish1_fed;				// flags to keep track of keeping track of fish fed status and their score

static bool fish2_fed;

static int fish1_score;

static int fish2_score;



static int random_rotation_angle;	// rotation angle to rotate the fish

static bool end_game;



static int score;					// keep track of the score

static int timer;					// keep track of time

static int fish1_timer;				// keep track of whether fish1 has been fed in 8s							

static int fish2_timer;				// keep track of whether fish2 has been fed in 8s



static bool isAnimate;				// Used for pausing and unpausing fish and the timer



static float matShine[] = { 1 };	//Shininess value for the food





char canvas_Name[] = "Fish Movement with Cohen Sutherland"; // Name at the top of canvas





enum {

	INSIDE = 0, LEFT = 1, RIGHT = 2, BOTTOM = 4, TOP = 8	//determine if the line is inside, left, right, bottom or top of the boundary

};															// in cohen-sutherland algorithm



															// Sets width and height of canvas to 800 by 600.

#define canvas_Width 800

#define canvas_Height 600



															//Function prototypes

void timer_func(int val);

void display_func(void);





void init(void) {

	/* This function sets the background color, initializes all the global variables.*/

	glClearColor(0.0, 0.0, 0.0, 1.0);

	x_min = -200;

	x_max = 200;

	y_min = -150;

	y_max = 150;



	score = 0;

	timer = 8;

	fish1_timer = 8;

	fish2_timer = 8;



	isAnimate = 1;          //unpause fish at the beginning 

	random_rotation_angle = 0;



	x_move_fish1 = 0.0;

	y_move_fish1 = 0.0;



	x_move_fish2 = 75.0;

	y_move_fish2 = 0.0;

	fish1_direction = true; //clockwise

	fish2_direction = false; //anticlockwise

	food_flag = false;

	fed = false;



	x_cursor_food = 0.0f;

	y_cursor_food = 0.0f;

	x_tip_head_fish1 = 0.0;

	x_tip_head_fish2 = 0.0;

	bonus_x_pos = 0.0f;

	bonus_y_pos = 0.0f;



	fish1_score = 0;

	fish2_score = 0;



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



	unhappy_face_fish1 = false;

	unhappy_face_fish2 = false;



	happy_face_fish1 = false;

	happy_face_fish2 = false;

}



void draw_subcanvas(int x_min, int x_max, int y_min, int y_max) {

	/*

	This function draws the subcanvas with x-coordinate from x_min to x_max and y-coordinate from y_min to y_max.

	Parameters:

	-----------

	x_min: Integer

	x-coodinate of left part of subcanvas

	x_max: Integer

	x-coordinate of right part of subcanvas

	y_min: Integer

	y-coordinate of bottom part of subcanvas

	y_max: Integer

	y-coordinate of top part of subcanvas

	*/



	glPushMatrix();

	glColor3f(1.0f, 1.0f, 1.0f);

	glBegin(GL_LINE_LOOP);

	glVertex2i(x_min, y_min);

	glVertex2i(x_max, y_min);

	glVertex2i(x_max, y_max);

	glVertex2i(x_min, y_max);

	glEnd();

	glPopMatrix();



}



void draw_line(double x1, double y1, double x2, double y2) {

	/*

	This function draws line required for Cohen-Sutherland algorithm.

	Parameters:

	-----------

	x1: Double

	x-coordinate of start of line

	y1: Double

	y-coordinate of start of line

	x2: Double

	x-coordinate of end of line

	y2: Double

	y-coordinate of end of line

	*/

	glPushMatrix();

	glBegin(GL_LINES);

	glVertex2d(x1, y1);

	glVertex2d(x2, y2);

	glEnd();

	glPopMatrix();

}



int compute_outcode(double x, double y) {

	/*

	This function computes outcode of given x and y coordinate and return its value.

	Parameters:

	----------

	x : Double

	x coordinate whose outcode needs to be computed.

	y : Double

	y coordinate whose outcode needs to be computed.

	*/

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

	/*

	This function applies cohen sutherland algorithm to the line drawn from x1, y1 to x2, y2.

	Parameters:

	-----------

	x1: Double

	x-coordinate of the start of line

	y1: Double

	y-coordinate of the start of line

	x2: Double

	x-coordinate of the end of line

	y2: Double

	y-coordinate of the end of line

	*/

	int outcode1 = compute_outcode(x1, y1);

	int outcode2 = compute_outcode(x2, y2);

	while (true) {

		if ((outcode1 & outcode2) != 0) {

			// reject condition

			break;

		}

		else if ((outcode1 | outcode2) == 0) {

			// accept condition

			draw_line(x1, y1, x2, y2);

			break;

		}

		else {

			// determining part of line which can be accepted

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



void draw_fish_head_clockwise(double x, double y, double head_l1) {

	/*

	This function draws the fish head to the right side if it's heading in clockwise direction.

	Parameters:

	----------

	x: Double

	x-coordinate where head of the fish should be drawn.

	y: Double

	y-coordinate where head of the fish should be drawn.

	head_l1: Double

	Width of the head.

	*/

	draw_cohen_sutherland_line_clip(x, y + 10, x, y - 10);

	draw_cohen_sutherland_line_clip(x, y + 10, x + head_l1, y);

	draw_cohen_sutherland_line_clip(x, y - 10, x + head_l1, y);

}



void draw_fish_head_anticlockwise(double x, double y, double head_l1) {

	/*

	This function draws the fish head to the left side if it's heading in anticlockwise direction.

	Parameters:

	----------

	x: Double

	x-coordinate where head of the fish should be drawn.

	y: Double

	y-coordinate where head of the fish should be drawn.

	head_l1: Double

	Width of the head.

	*/

	draw_cohen_sutherland_line_clip(x, y + 10, x, y - 10);

	draw_cohen_sutherland_line_clip(x, y + 10, x - head_l1, y);

	draw_cohen_sutherland_line_clip(x, y - 10, x - head_l1, y);

}



void handle_fish1_move(double x, double y, bool clockwise, double diamond_l1, double head_l1) {

	/*

	This function handles the movement of fish1.

	Parameters:

	-----------

	x: Double

	x-coordinate where fish should be drawn

	y: Double

	y-coordinate where fish should be drawn

	clockwise: Boolean

	Direction of the fish

	diamond_l1: Double

	Diamond body length

	head_l1: Double

	Fish head length

	*/

	if (clockwise) {

		x_tip_head_fish1 = x + diamond_l1 + head_l1;

		// Keep smile on fish's face

		if (happy_face_fish1) {

			draw_cohen_sutherland_line_clip(x + diamond_l1 + 3, y, x + diamond_l1 + 4, y - 4);

			draw_cohen_sutherland_line_clip(x + diamond_l1 + 4, y - 4, x + diamond_l1 + 7, y);

		}

		// Keep frown on fish's face

		else if (unhappy_face_fish1) {

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

		else if (unhappy_face_fish1) {

			draw_cohen_sutherland_line_clip(x - diamond_l1 - 4, y, x - diamond_l1 - 2, y - 4);

			draw_cohen_sutherland_line_clip(x - diamond_l1 - 4, y, x - diamond_l1 - 7, y - 4);

		}

	}

}



void handle_fish2_move(double x, double y, bool clockwise, double diamond_l1, double head_l1) {

	/*

	This function handles the movement of fish2.

	Parameters:

	-----------

	x: Double

	x-coordinate where fish should be drawn

	y: Double

	y-coordinate where fish should be drawn

	clockwise: Boolean

	Direction of the fish

	diamond_l1: Double

	Diamond body length

	head_l1: Double

	Fish head length

	*/

	if (clockwise) {

		x_tip_head_fish2 = x + diamond_l1 + head_l1;

		if (happy_face_fish2) {

			draw_cohen_sutherland_line_clip(x + diamond_l1 + 3, y, x + diamond_l1 + 4, y - 4);

			draw_cohen_sutherland_line_clip(x + diamond_l1 + 4, y - 4, x + diamond_l1 + 7, y);

		}

		else if (unhappy_face_fish2) {

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

		else if (unhappy_face_fish2) {

			draw_cohen_sutherland_line_clip(x - diamond_l1 - 4, y, x - diamond_l1 - 2, y - 4);

			draw_cohen_sutherland_line_clip(x - diamond_l1 - 4, y, x - diamond_l1 - 7, y - 4);

		}

	}

}



void draw_fish(double x, double y, double length, double height, bool clockwise, string fishname) {

	/*

	This function draws a fish in x and y coordinate with a name given in fishname, and with length specified in length parameter.

	Parameters:

	-----------

	x: Double

	x-coordinate where fish should be drawn

	y: Double

	y-coordinate where fish should be drawn

	height: Double

	height of the fish

	clockwise: Boolean

	Direction of the fish

	fishname: String

	Name of the fish

	*/

	glPushMatrix();

	double head_l1 = 10.0;

	double diamond_l1 = (length - head_l1) / 2;

	double h1 = height / 2;

	double hypotenuse = sqrt(diamond_l1 * diamond_l1 + h1 * h1);

	draw_cohen_sutherland_line_clip(x + diamond_l1, y, x, y + hypotenuse);

	draw_cohen_sutherland_line_clip(x, y + hypotenuse, x - diamond_l1, y);

	draw_cohen_sutherland_line_clip(x - diamond_l1, y, x, y - hypotenuse);

	draw_cohen_sutherland_line_clip(x, y - hypotenuse, x + diamond_l1, y);

	if (fishname == "fish1") {

		handle_fish1_move(x, y, clockwise, diamond_l1, head_l1);

	}

	else if (fishname == "fish2") {

		handle_fish2_move(x, y, clockwise, diamond_l1, head_l1);

	}



	if (clockwise) {

		draw_fish_head_clockwise(x + diamond_l1, y, head_l1);

	}

	else {

		draw_fish_head_anticlockwise(x - diamond_l1, y, head_l1);

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



void display_score(int x, int y) {

	/*

	This function is used for displaying score at the bottom right of the canvas.

	Parameters:

	----------

	x: Integer

	The x-position where the score should be placed.

	y: Integer

	The y-position where the score should be placed.

	*/

	glColor3f(1.0f, 1.0f, 1.0f);

	glPushMatrix();

	glRasterPos2i(x, y);

	char score_text[20] = "SCORE: ";

	char score_num[6];

	sprintf_s(score_num, 6, "%d", score);

	strcat_s(score_text, 20, score_num);

	writeBitmapString(GLUT_BITMAP_HELVETICA_18, score_text);

	glPopMatrix();

}



void display_fish1_score(int x, int y) {

	/*

	This function is used for displaying fish1's score at the top left of the canvas.

	Parameters:

	----------

	x: Integer

	The x-position where the score should be placed.

	y: Integer

	The y-position where the score should be placed.

	*/

	glColor3f(1.0f, 1.0f, 1.0f);

	glPushMatrix();

	glRasterPos2i(x, y);

	char score_text[20] = "FISH1 SCORE: ";

	char score_num[6];

	sprintf_s(score_num, 6, "%d", fish1_score);

	strcat_s(score_text, 20, score_num);

	writeBitmapString(GLUT_BITMAP_HELVETICA_10, score_text);

	glPopMatrix();

}



void display_fish2_score(int x, int y) {

	/*

	This function is used for displaying fish2's score at the top left of the canvas.

	Parameters:

	----------

	x: Integer

	The x-position where the score should be placed.

	y: Integer

	The y-position where the score should be placed.

	*/

	glColor3f(1.0f, 1.0f, 1.0f);

	glPushMatrix();

	glRasterPos2i(x, y);

	char score_text[20] = "FISH2 SCORE: ";

	char score_num[6];

	sprintf_s(score_num, 6, "%d", fish2_score);

	strcat_s(score_text, 20, score_num);

	writeBitmapString(GLUT_BITMAP_HELVETICA_10, score_text);

	glPopMatrix();

}



void display_timer(int x, int y) {

	/*

	This function is used for displaying timer at the top right of the canvas.

	Parameters:

	----------

	x: Integer

	The x-position where the timer should be placed.

	y: Integer

	The y-position where the timer should be placed.

	*/

	if (timer > 3) {

		glColor3f(1.0f, 1.0f, 1.0f);

	}

	else {

		glColor3f(1.0f, 0.6f, 0.1f);

	}

	glPushMatrix();

	glRasterPos2i(x, y);

	char timer_text[20] = "TIMER: ";

	char timer_num[6];

	sprintf_s(timer_num, 6, "%d", timer);

	strcat_s(timer_text, 20, timer_num);

	writeBitmapString(GLUT_BITMAP_HELVETICA_18, timer_text);

	glPopMatrix();

}



void display_game_over(int x, int y) {

	/*

	This function is used for displaying game over at the end of game.

	Parameters:

	----------

	x: Integer

	The x-position where the game over should be placed.

	y: Integer

	The y-position where the game over should be placed.

	*/

	char game_over[] = "GAME OVER"; // Display after game is over

	glColor3f(1.0f, 0.0f, 0.0f);

	glPushMatrix();

	glRasterPos2i(x, y);

	writeBitmapString(GLUT_BITMAP_HELVETICA_18, game_over);

	glPopMatrix();

}



void display_fish1_happy_msg(int x, int y) {

	/*

	This function is used for displaying a happy message if fish1 has been fed.

	Parameters:

	----------

	x: Integer

	The x-position where the happy msg should be placed.

	y: Integer

	The y-position where the happy msg should be placed.

	*/

	char fish1_happiness[] = "Fish1 is happy. It has just been fed.";

	glPushMatrix();

	glRasterPos2i(x, y);

	writeBitmapString(GLUT_BITMAP_HELVETICA_12, fish1_happiness);

	glPopMatrix();

}



void display_fish2_happy_msg(int x, int y) {

	/*

	This function is used for displaying a happy message if fish2 has been fed.

	Parameters:

	----------

	x: Integer

	The x-position where the happy msg should be placed.

	y: Integer

	The y-position where the happy msg should be placed.

	*/

	char fish2_happiness[] = "Fish2 is happy. It has just been fed.";

	glPushMatrix();

	glRasterPos2i(x, y);

	writeBitmapString(GLUT_BITMAP_HELVETICA_12, fish2_happiness);

	glPopMatrix();

}



void display_fish1_unhappy_msg(int x, int y) {

	/*

	This function is used for displaying a unhappy message if fish1 hasn't been fed for 8 seconds.

	Parameters:

	----------

	x: Integer

	The x-position where the unhappy msg should be placed.

	y: Integer

	The y-position where the unhappy msg should be placed.

	*/

	char fish1_happiness[] = "Fish1 is unhappy. It hasn't been fed for 8s.";

	glPushMatrix();

	glRasterPos2i(x, y);

	writeBitmapString(GLUT_BITMAP_HELVETICA_12, fish1_happiness);

	glPopMatrix();

}



void display_fish2_unhappy_msg(int x, int y) {

	/*

	This function is used for displaying a unhappy message if fish2 hasn't been fed for 8 seconds.

	Parameters:

	----------

	x: Integer

	The x-position where the unhappy msg should be placed.

	y: Integer

	The y-position where the unhappy msg should be placed.

	*/

	char fish2_happiness[] = "Fish2 is unhappy. It hasn't been fed for 8s.";

	glPushMatrix();

	glRasterPos2i(x, y);

	writeBitmapString(GLUT_BITMAP_HELVETICA_12, fish2_happiness);

	glPopMatrix();

}





void display_sad_msg(int x, int y) {

	/*

	This function is used for displaying a sad message if both fishes hasn't been fed for 8 seconds.

	Parameters:

	----------

	x: Integer

	The x-position where the sad msg should be placed.

	y: Integer

	The y-position where the sad msg should be placed.

	*/

	char fish_sadness[] = "Both fishes are sad. They haven't been fed for 8s.";

	glPushMatrix();

	glRasterPos2i(x, y);

	writeBitmapString(GLUT_BITMAP_HELVETICA_12, fish_sadness);

	glPopMatrix();

}



void draw_food(float x, float y) {

	/*

	This function draws a food at the specified x and y position.

	Parameters:

	-----------

	x: Float

	x-coordinate where food should be placed

	y: Float

	y-coordinate where food should be placed

	*/

	glColor3f(1.0f, 1.0f, 0.0f);

	glPushMatrix();

	glTranslatef(x, y, 0.0f);

	glutSolidCube(5.0);

	glPopMatrix();



}



void fish1_turn_back(int random_rotation_angle) {

	/*

	This function turns fish1 back if it hits the edge based on the angle it's rotating.

	Parameters:

	-----------

	random_rotation_angle: Integer

	Rotation angle the fish has while calling this function.



	*/

	if (random_rotation_angle == 0) {

		if (x_tip_head_fish1 >= 385.0) {

			fish1_direction = false;

		}

		else if (x_tip_head_fish1 <= -385.0) {

			fish1_direction = true;

		}

	}

	else {

		if (x_tip_head_fish1 >= 385.0 || y_move_fish1 >= 270.0 || y_move_fish1 <= -270.0) {

			fish1_direction = false;

		}

		else if (x_tip_head_fish1 <= -385.0 || y_move_fish1 >= 270.0 || y_move_fish1 <= -270.0) {

			fish1_direction = true;

		}

	}

}



void fish2_turn_back(int random_rotation_angle) {

	/*

	This function turns fish2 back if it hits the edge based on the angle it's rotating.

	Parameters:

	-----------

	random_rotation_angle: Integer

	Rotation angle the fish has while calling this function.

	*/

	if (random_rotation_angle == 0) {

		if (x_tip_head_fish2 >= 385.0) {

			fish2_direction = false;

		}

		else if (x_tip_head_fish2 <= -385.0) {

			fish2_direction = true;

		}

	}

	else {

		if (x_tip_head_fish2 >= 385.0 || y_move_fish2 >= 270.0 || y_move_fish2 <= -270.0) {

			fish2_direction = false;

		}

		else if (x_tip_head_fish2 <= -385.0 || y_move_fish2 >= 270.0 || y_move_fish2 <= -270.0) {

			fish2_direction = true;

		}

	}

}



void fish1_end_boundary_test(void) {

	/*

	Checks if the fish has hit the boundary and turns it back if it has hit boundary.

	*/

	if ((x_tip_head_fish1) >= 385.0) {

		x_move_fish1 = 350.0;

	}

	if ((x_tip_head_fish1) <= -385.0) {

		x_move_fish1 = -350.0;

	}

	if (y_move_fish1 >= 280.0) {

		y_move_fish1 = 210.0;

		random_rotation_angle = -random_rotation_angle;

	}

	if (y_move_fish1 <= -280.0) {

		y_move_fish1 = -210.0;

		random_rotation_angle = -random_rotation_angle;

	}

	fish1_turn_back(random_rotation_angle);

}



void fish2_end_boundary_test(void) {

	/*

	Checks if the fish has hit the boundary and turns it back if it has hit boundary.

	*/

	if ((x_tip_head_fish2) >= 385.0) {

		x_move_fish2 = 350;

	}

	if ((x_tip_head_fish2) <= -385.0) {

		x_move_fish2 = -350.0;

	}

	if (y_move_fish2 >= 280.0) {

		y_move_fish2 = 210.0;

		random_rotation_angle = -random_rotation_angle;

	}

	if (y_move_fish2 <= -280.0) {

		y_move_fish2 = -210.0;

		random_rotation_angle = -random_rotation_angle;

	}

	fish2_turn_back(random_rotation_angle);

}



void handle_ingest_fish1(void) {

	/*

	Checks if fish1 is within 10 pixels of the food.

	*/

	if (fish1_direction) {

		// if fish1 is in clockwise direction

		if ((x_cursor_food >= x_move_fish1 - 30.0 && x_cursor_food <= x_move_fish1 + 40.0) && (y_cursor_food >= y_move_fish1 - 22.5 && y_cursor_food <= y_move_fish1 + 22.5)) {

			fed = true;

			fish1_fed = true;

		}

	}

	else if (!fish1_direction) {

		// if fish1 is in anticlockwise direction

		if ((x_cursor_food >= x_move_fish1 - 40.0 && x_cursor_food <= x_move_fish1 + 30.0) && (y_cursor_food >= y_move_fish1 - 22.5 && y_cursor_food <= y_move_fish1 + 22.5)) {

			fed = true;

			fish1_fed = true;

		}

	}

}



void handle_ingest_fish2(void) {

	/*

	Checks if fish2 is within 10 pixels of the food.

	*/

	if (fish2_direction) {

		// if fish2 is in clockwise direction

		if ((x_cursor_food >= x_move_fish2 - 25.0 && x_cursor_food <= x_move_fish2 + 35.0) && (y_cursor_food >= y_move_fish2 - 25.0 && y_cursor_food <= y_move_fish2 + 25.0)) {

			fed = true;

			fish2_fed = true;

		}

	}

	else if (!fish2_direction) {

		// if fish2 is in anticlockwise direction

		if ((x_cursor_food >= x_move_fish2 - 35.0 && x_cursor_food <= x_move_fish2 + 25.0) && (y_cursor_food >= y_move_fish2 - 25.0 && y_cursor_food <= y_move_fish2 + 25.0)) {

			fed = true;

			fish2_fed = true;

		}

	}

}



void handle_bonus_ingest_fish1(void) {

	/*

	Checks if fish1 is within 10 pixels of the bonus food in both clockwise and anticlockwise direction.

	*/

	if (fish1_direction) {

		if ((bonus_x_pos >= x_move_fish1 - 40.0 && bonus_x_pos <= x_move_fish1 + 50.0) && (bonus_y_pos >= y_move_fish1 - 32.5 && bonus_y_pos <= y_move_fish1 + 32.5)) {

			bonus_fed = true;

			bonus_food = false;

			fish1_fed = true;

		}

	}

	else if (!fish1_direction) {

		if ((bonus_x_pos >= x_move_fish1 - 50.0 && bonus_x_pos <= x_move_fish1 + 40.0) && (bonus_y_pos >= y_move_fish1 - 32.5 && bonus_y_pos <= y_move_fish1 + 32.5)) {

			bonus_fed = true;

			bonus_food = false;

			fish1_fed = true;

		}

	}



}



void handle_bonus_ingest_fish2(void) {

	/*

	Checks if fish2 is within 10 pixels of the bonus food in both clockwise and anticlockwise directionf.

	*/

	if (fish2_direction) {

		if ((bonus_x_pos >= x_move_fish2 - 35.0 && bonus_x_pos <= x_move_fish2 + 45.0) && (bonus_y_pos >= y_move_fish2 - 35.0 && bonus_y_pos <= y_move_fish2 + 35.0)) {

			bonus_fed = true;

			bonus_food = false;

			fish2_fed = true;

		}

	}

	else if (!fish2_direction) {

		if ((bonus_x_pos >= x_move_fish2 - 45.0 && bonus_x_pos <= x_move_fish2 + 35.0) && (bonus_y_pos >= y_move_fish2 - 35.0 && bonus_y_pos <= y_move_fish2 + 35.0)) {

			bonus_fed = true;

			bonus_food = false;

			fish2_fed = true;

		}

	}

}



void setup_light_source(void) {

	/* This function turns on OpenGL lighting and keeps light at the world origin.*/



	// Set light property vectors

	float lightAmb[] = { 0.0f, 0.0f, 0.0f, 1.0f };

	float lightDif[] = { 0.7f, 0.7f, 0.7f, 1.0f };

	float lightSpec[] = { 1.0f, 1.0f, 1.0f, 1.0f };



	// Set light position at the world origin

	float lightPos[] = { 0.0f, 0.0f, 0.0f, 1.0f };

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



void draw_bonus_food(float x, float y) {

	/*

	This function draws the bonus food made of glutSolidTorus and x and y position.

	Parameters:

	----------

	x: Float

	x-position where bonus food should be placed

	y: Float

	y-position where bonus food should be placed.

	*/

	setup_light_source();

	float matAmb[] = { 0.2f, 0.0f, 0.0f, 1.0f };

	float matDif[] = { 0.8f, 0.0f, 0.0f, 1.0f };

	float matSpec[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	glMaterialfv(GL_FRONT, GL_AMBIENT, matAmb);

	glMaterialfv(GL_FRONT, GL_DIFFUSE, matDif);

	glMaterialfv(GL_FRONT, GL_SPECULAR, matSpec);

	glMaterialfv(GL_FRONT, GL_SHININESS, matShine);

	glPushMatrix();

	glTranslatef(x, y, 0.0f);

	glutSolidTorus(4.0, 8.0, 30, 30);

	glPopMatrix();

	glDisable(GL_LIGHTING);

	glDisable(GL_LIGHT0);



}



void calculate_random_angle(void) {

	/*

	Select random angle between -45 and 45 for the fish to move.

	*/

	srand((unsigned int)time(NULL));

	const int array_num[2] = { -45, 45 };

	int rand_index = rand() % 2;

	random_rotation_angle = (int)array_num[rand_index];



}



void motion_handler(int x, int y) {

	/*

	Stores the position where bonus food should be drawn.

	Parameters:

	-----------

	x: Integer

	Get the x-coordinate

	y: Integer

	Get the y-coordinate

	*/

	bonus_x_pos = (float)(x - 400);

	bonus_y_pos = (float)(300 - y);

	bonus_food = true;

	glutPostRedisplay();

}



void find_bonus_food_coordinates(void) {

	/*

	Get bonus food position.

	*/

	bonus_food = true;

	int max = 100;

	srand((unsigned int)time(0));

	float rand_x = (float)(rand() % max);

	float rand_y = (float)(rand() % max);

	const float array_x[2] = { rand_x, -rand_x };

	const float array_y[2] = { rand_y, -rand_y };

	int x = rand() % 2;

	int y = rand() % 2;

	bonus_x_pos = array_x[x];

	bonus_y_pos = array_y[y];

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

		// moves subcanvas to the left

		x_min = x_min - 5;

		x_max = x_max - 5;

		glutPostRedisplay();

		break;

	case 'J': case'j':

		// moves subcanvas to the right

		x_min = x_min + 5;

		x_max = x_max + 5;

		glutPostRedisplay();

		break;

	case 'U': case 'u':

		// moves subcanvas upward

		y_min = y_min + 5;

		y_max = y_max + 5;

		glutPostRedisplay();

		break;

	case 'N': case 'n':

		// moves subcanvas downward

		y_min = y_min - 5;

		y_max = y_max - 5;

		glutPostRedisplay();

		break;

	case 'F': case 'f':

		// places food in the position of mouse cursor

		x_cursor_food = float(x - 400);

		y_cursor_food = float(300 - y);

		if (x_cursor_food >= x_min && x_cursor_food <= x_max && y_cursor_food >= y_min && y_cursor_food <= y_max) {

			food_flag = true;

			glutPostRedisplay();

		}



		glutTimerFunc(1000, timer_func, 7);

		break;

	case 'P': case 'p':

		// unpauses fish and timer

		if (isAnimate) {

			isAnimate = 0;

		}

		else {

			// pauses fish and timer

			isAnimate = 1;

			glutTimerFunc(63, timer_func, 1);

			glutTimerFunc(1000, timer_func, 11);

		}

		glutPostRedisplay();

		break;

	default:

		break;

	}

}



void handle_rotation_angle_zero_move(void) {

	/*

	This function handles the move when rotation angle is zero.

	*/

	fish1_turn_back(0);

	fish2_turn_back(0);

	if (fish1_direction) {

		x_move_fish1 += 4.0;

	}

	else {

		x_move_fish1 -= 4.0;

	}



	if (fish2_direction) {

		x_move_fish2 += 4.0;

	}

	else {

		x_move_fish2 -= 4.0;

	}

}



void handle_rotation_angle_45_move(void) {

	/*

	This function handles the move when rotation angle is 45 degree.

	*/

	fish1_turn_back(45);

	fish2_turn_back(45);

	if (fish1_direction) {

		x_move_fish1 += 2.83;

		y_move_fish1 += 2.83;

	}

	else {

		x_move_fish1 -= 2.83;

		y_move_fish1 -= 2.83;

	}



	if (fish2_direction) {

		x_move_fish2 += 2.83;

		y_move_fish2 += 2.83;

	}

	else {

		x_move_fish2 -= 2.83;

		y_move_fish2 -= 2.83;

	}

	fish1_end_boundary_test();

	fish2_end_boundary_test();

}



void handle_rotation_angle_minus_45_move(void) {

	/*

	This function handles the move when rotation angle is -45 degree.

	*/

	fish1_turn_back(-45);

	fish2_turn_back(-45);

	if (fish1_direction) {

		x_move_fish1 += 2.83;

		y_move_fish1 -= 2.83;

	}

	else {

		x_move_fish1 -= 2.83;

		y_move_fish1 += 2.83;

	}



	if (fish2_direction) {

		x_move_fish2 += 2.83;

		y_move_fish2 -= 2.83;



	}

	else {

		x_move_fish2 -= 2.83;

		y_move_fish2 += 2.83;

	}

	fish1_end_boundary_test();

	fish2_end_boundary_test();



}



void handle_bonus_food_ingest(void) {

	/*

	This function handles bonus food ingest of both fish1 and fish2, and sets associated flag. Also,

	starts the timer from 8 and adds score.

	*/

	handle_bonus_ingest_fish1();

	handle_bonus_ingest_fish2();

	if (bonus_fed and fish1_fed) {

		fish1_timer = 8;

		timer = 8;

		score += 50;

		fish1_score += 50;

		fish1_fed = false;

		happy_fish1 = true;

		happy_face_fish1 = true;



		bonus_fed = false;

		bonus_food = false;

		unhappy_fish1 = false;



	}

	else if (bonus_fed and fish2_fed) {

		fish2_timer = 8;

		timer = 8;

		score += 50;

		fish2_score += 50;

		fish2_fed = false;

		happy_fish2 = true;

		happy_face_fish2 = true;



		bonus_fed = false;

		bonus_food = false;

		unhappy_fish2 = false;

	}

	glutPostRedisplay();

}



void handle_food_ingest() {

	/*

	This function handle food ingest of both fish1 and fish2, and sets associated flags. Also,

	starts the timer and each individual fish timers from 8 and add score.

	*/

	handle_ingest_fish1();

	handle_ingest_fish2();

	if (fed and fish1_fed) {

		fish1_timer = 8;

		timer = 8;

		score += 50;

		fish1_score += 50;

		fish1_fed = false;

		fed = false;

		happy_fish1 = true;

		happy_face_fish1 = true;

		unhappy_fish1 = false;

	}

	else if (fed and fish2_fed) {

		fish2_timer = 8;

		timer = 8;

		score += 50;

		fish2_score += 50;

		fish2_fed = false;

		fed = false;

		happy_fish2 = true;

		happy_face_fish2 = true;

		unhappy_fish2 = false;

	}

	food_flag = false;

	glutPostRedisplay();

}



void set_fish_faces(void) {

	/*This function sets the fish faces at the end of game.*/

	if (unhappy_fish1 and unhappy_fish2) {

		sad_fish = true;

		unhappy_face_fish1 = true;

		unhappy_face_fish2 = true;

		glutPostRedisplay();

	}

	else if (!happy_fish1 and !sad_fish) {

		unhappy_face_fish1 = true;

		sad_fish = false;

		glutPostRedisplay();

	}

	else if (!happy_fish2 and !sad_fish) {

		unhappy_face_fish2 = true;

		sad_fish = false;

		glutPostRedisplay();

	}

}



void timer_func(int val) {

	/*

	This function is a glut timer callback function which displays 16 new frames in 1 second to enable fish movement in both anticlockwise and clockwise direction,

	sets the timer to 8 if fish has been fed, rotate the fish direction every 2s. Also, sets the flag for showing game status.

	Parameters:

	-----------

	value: Integer

	This is the timerID.

	*/

	switch (val) {

	case 1:

		// different timer id for different rotation position

		if (isAnimate) {

			if (random_rotation_angle == 0) {

				glutTimerFunc(0, timer_func, 2);

			}

			else if (random_rotation_angle == 45) {

				glutTimerFunc(0, timer_func, 3);

			}

			else {

				glutTimerFunc(0, timer_func, 4);

			}

		}

		break;

	case 2:

		// handles when angle is zero

		handle_rotation_angle_zero_move();

		glutPostRedisplay();

		glutTimerFunc(63, timer_func, 1);

		break;

	case 3:

		// handles when angle is 45

		handle_rotation_angle_45_move();

		glutPostRedisplay();

		glutTimerFunc(63, timer_func, 1);

		break;

	case 4:

		// handles when angle is -45

		handle_rotation_angle_minus_45_move();

		glutPostRedisplay();

		glutTimerFunc(63, timer_func, 1);

		break;

	case 5:

		exit(0);

		break;

	case 6:

		// rotates fish every 2s

		calculate_random_angle();

		glutTimerFunc(2000, timer_func, 6);

		break;

	case 7:

		handle_food_ingest();

		break;

	case 8:

		// reduce timer and set appropriate flag. Also, quit the game after 2s if timer is set to 0.

		if (isAnimate) {

			timer -= 1;

			fish1_timer -= 1;

			fish2_timer -= 1;

		}

		if (fish1_timer <= 0 and fish2_timer <= 0) {

			unhappy_fish1 = true;

			unhappy_face_fish1 = true;

			unhappy_fish2 = true;

			unhappy_face_fish2 = true;

		}

		if (fish1_timer <= 0) {

			unhappy_fish1 = true;

			unhappy_face_fish1 = true;

		}

		if (fish2_timer <= 0) {

			unhappy_fish2 = true;

			unhappy_face_fish2 = true;

		}

		if (timer <= 0) {

			timer = 0;

			set_fish_faces();

			glutKeyboardFunc(NULL);

			glutMouseFunc(NULL);

			end_game = true;

			glutTimerFunc(2000, timer_func, 5);

		}

		glutTimerFunc(1000, timer_func, 8);

		break;

	case 10:

		// Disable the flags

		happy_fish = false;

		happy_fish1 = false;

		happy_fish2 = false;

		break;

	case 11:

		// display bonus food only if no previous bonus food exists.

		if (bonus_food == false) {

			find_bonus_food_coordinates();

			glutPostRedisplay();

		}

		glutTimerFunc(0, timer_func, 12);

		break;



	case 12:

		handle_bonus_food_ingest();

		glutTimerFunc(500, timer_func, 10);

		glutTimerFunc(3000, timer_func, 11);

		break;

	default:

		break;

	}

}





void display_messages(void) {

	/*

	This function is called by the display_func. It shows different messages, facial expression on fish based on the flag.

	*/

	if (food_flag) {

		draw_food(x_cursor_food, y_cursor_food);

	}

	if (sad_fish) {

		glColor3f(1.0f, 1.0f, 1.0f);

		display_sad_msg(-400, -270);

		glutTimerFunc(4000, timer_func, 10);

	}

	else if (happy_fish1) {

		glColor3f(1.0f, 1.0f, 1.0f);

		display_fish1_happy_msg(-400, -270);

		glutTimerFunc(4000, timer_func, 10);

	}

	else if (happy_fish2) {

		glColor3f(1.0f, 1.0f, 1.0f);

		display_fish2_happy_msg(-400, -270);

		glutTimerFunc(4000, timer_func, 10);

	}

	else if (!sad_fish and unhappy_fish1) {

		glColor3f(1.0f, 1.0f, 1.0f);

		display_fish1_unhappy_msg(-400, -270);

		glutTimerFunc(4000, timer_func, 10);

	}

	else if (!sad_fish and unhappy_fish2) {

		glColor3f(1.0f, 1.0f, 1.0f);

		display_fish2_unhappy_msg(-400, -270);

		glutTimerFunc(4000, timer_func, 10);

	}

	if (bonus_food) {

		draw_bonus_food(bonus_x_pos, bonus_y_pos);

	}

	if (end_game) {

		display_game_over(0, 0);

	}

}





void display_func(void) {

	/*

	This is a glut display callback handler which is called whenever a window needs to be displayed or redisplayed. It clears the

	canvas screen and reloads all the objects of the scene when called. This function also displays timer, score and the text game over

	at the end of the game.

	*/

	glClear(GL_COLOR_BUFFER_BIT);

	glLoadIdentity();

	draw_subcanvas(x_min, x_max, y_min, y_max);

	glColor3f(0.0f, 0.7f, 1.0f);

	// draw fish1

	draw_fish(x_move_fish1, y_move_fish1, 50.0, 25.0, fish1_direction, "fish1");

	glColor3f(0.0f, 0.9f, 0.1f);

	// draw fish2

	draw_fish(x_move_fish2, y_move_fish2, 40.0, 30.0, fish2_direction, "fish2");



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

	glutTimerFunc(2000, timer_func, 11);

	glutMotionFunc(motion_handler);

	glutKeyboardFunc(keyboard_func);

	init();

	glutMainLoop();

	return 0;

}