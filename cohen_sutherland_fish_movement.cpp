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
static double x1_move_fish1;
static double y1_move_fish1;
static double x2_move_fish1;
static double y2_move_fish1;
static bool fish1_direction;
static bool fish2_direction;

static double x1_move_fish2;
static double y1_move_fish2;
static double x2_move_fish2;
static double y2_move_fish2;
bool done = false;
bool front = true;
bool back = false;
static int random_rotation_angle;

static float matShine[] = { 25 };	//Shininess value used throughout the program

static int score;			//Flag to keep track of the score
static int timer;
char canvas_Name[] = "Fish Movement with Cohen Sutherland"; // Name at the top of canvas
char game_over[] = "GAME OVER"; // Display after game is over


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

void init(void) {
	/* This function sets the background color, initializes all the global variables and calculate random
	coordinates to place the food.*/
	glClearColor(0.0, 0.0, 0.0, 1.0);
	x_min = -200;
	x_max = 200;
	y_min = -150;
	y_max = 150;
	score = 0;
	timer = 8;
	random_rotation_angle = 0;

	x1_move_fish1 = 0;
	y1_move_fish1 = 0;
	x2_move_fish1 = 0;
	y2_move_fish1 = 0;

	x1_move_fish2 = 75;
	y1_move_fish2 = 0;
	x2_move_fish2 = 0;
	y2_move_fish2 = 0;
	fish1_direction = true; //clockwise
	fish2_direction = false;
	
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
	glColor3f(0.0, 0.0, 0.0);
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

/*void apply_cohen_sutherland_line_clip(double x1, double y1, double x2, double y2)
	{
		// Compute region codes for P1, P2
		int code1 = compute_outcode(x1, y1);
		int code2 = compute_outcode(x2, y2);

		// Initialize line as outside the rectangular window
		bool accept = false;

		while (true) {
			if ((code1 == 0) && (code2 == 0)) {
				// If both endpoints lie within rectangle
				accept = true;
				break;
			}
			else if (code1 & code2) {
				// If both endpoints are outside rectangle,
				// in same region
				break;
			}
			else {
				// Some segment of line lies within the
				// rectangle
				int code_out;
				double x, y;

				// At least one endpoint is outside the
				// rectangle, pick it.
				if (code1 != 0)
					code_out = code1;
				else
					code_out = code2;

				// Find intersection point;
				// using formulas y = y1 + slope * (x - x1),
				// x = x1 + (1 / slope) * (y - y1)
				if (code_out & TOP) {
					// point is above the clip rectangle
					x = x1 + (x2 - x1) * (y_max - y1) / (y2 - y1);
					y = y_max;
				}
				else if (code_out & BOTTOM) {
					// point is below the rectangle
					x = x1 + (x2 - x1) * (y_min - y1) / (y2 - y1);
					y = y_min;
				}
				else if (code_out & RIGHT) {
					// point is to the right of rectangle
					y = y1 + (y2 - y1) * (x_max - x1) / (x2 - x1);
					x = x_max;
				}
				else if (code_out & LEFT) {
					// point is to the left of rectangle
					y = y1 + (y2 - y1) * (x_min - x1) / (x2 - x1);
					x = x_min;
				}

				// Now intersection point x, y is found
				// We replace point outside rectangle
				// by intersection point
				if (code_out == code1) {
					x1 = x;
					y1 = y;
					code1 = compute_outcode(x1, y1);
				}
				else {
					x2 = x;
					y2 = y;
					code2 = compute_outcode(x2, y2);
				}
			}
		}
		if (accept) {
			cout << "Line accepted from " << x1 << ", "
				<< y1 << " to " << x2 << ", " << y2 << endl;
			// Here the user can add code to display the rectangle
			// along with the accepted (portion of) lines
			draw_line(x1, y1, x2, y2);
		}
		else
			cout << "Line rejected" << endl;
}*/

void calculate_random_angle() {
	srand((unsigned int)time(NULL));
	const int array_num[2] = { -45, 45 };
	int rand_index = rand() % 2;
	random_rotation_angle = (int)array_num[rand_index];
	
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
	draw_cohen_sutherland_line_clip(x1, y1, x2, y2);
	//when using angle
	/*
	draw_fish_line(x1, y1, x1, y1 + height + 5, -angle);
	draw_fish_line(x2, y2, x2, y2 + height + 5, angle);
	draw_fish_line(x1, y1, x1, y1 - height - 5, angle);
	draw_fish_line(x2, y2, x2, y2 - height - 5, -angle);
	*/
	
	// without angle
	double y_height = height / 2;
	//double x_mid =  (x1+x2) / 2;
	double x_mid = (width - 10) / 2;
	draw_cohen_sutherland_line_clip(x1, y1, x1 + x_mid, y1 + y_height);
	
	draw_cohen_sutherland_line_clip(x1 + x_mid, y1 + y_height, x2, y2);
	/*glPopMatrix();
	glPushMatrix();
	glTranslated(x1, y1, 0);
	glRotated(random_rotation_angle, 0, 0, 1);
	glTranslated(-x1, -y1, 0);*/
	//draw_cohen_sutherland_line_clip(x1, y1, x1 + x_mid, -(y1 + y_height));
	//draw_cohen_sutherland_line_clip(x1 + x_mid, -y1 - y_height, x2, y2);
	draw_cohen_sutherland_line_clip(x1, y1, x1+x_mid, y1-y_height);
	draw_cohen_sutherland_line_clip(x1 + x_mid, y1 - y_height, x2, y2);
	//glPopMatrix();
	

	/*glPushMatrix();
	glTranslated(x1, y1, 0);
	glRotated(random_rotation_angle, 0, 0, 1);
	glTranslated(-x1, -y1, 0);*/
	
	if (clockwise) {
		draw_fish_head_clockwise(x2+1, y2-5, x2+1, y2+5);
	}
	else {
		draw_fish_head_anticlockwise(x1-1, y1 - 5, x1-1, y1 + 5);	
	}
	glPopMatrix();
	


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
	draw_fish(0 + x1_move_fish1, 0 + y1_move_fish1, 40 + x1_move_fish1, 0 + y1_move_fish1, fish1_direction, 25, 50, 45);
	cout << "x movement is from here " << x1_move_fish1  << "to here  " << 40 + x1_move_fish1 << endl;
	cout << "y movement is from here" << y1_move_fish1 << endl;
	glColor3d(0, 0.9, 0.1);
	draw_fish(x1_move_fish2, 0 + y1_move_fish2, 30 + x1_move_fish2, 0 + y1_move_fish2, fish2_direction, 30, 40, 40);
	//draw_fish_head(0, 0, 0, 10);
	//apply_cohen_sutherland_algorithm(-200, -200, 100, 100);
	display_score(250, -270);
	display_timer(300, 270);
	if (end_game) {
		glutKeyboardFunc(NULL);
		display_game_over(-50, 0, -150);
	}
	glutSwapBuffers();
	glFlush();

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
		glutTimerFunc(5000, timer_func, 5);

		break;
	case 2:
		if (fish1_direction) {
			x1_move_fish1 += 4;
			x2_move_fish1 += 4;
		}
		else {
			x1_move_fish1 -= 4;
			x2_move_fish1 -= 4;
		}

		if (fish2_direction) {
			x1_move_fish2 += 4;
			x2_move_fish2 += 4;
		}
		else {
			x1_move_fish2 -= 4;
			x2_move_fish2 -= 4;	
		}
		glutPostRedisplay();
		glutTimerFunc(62, timer_func, 1);
		break;
	case 3:
		if (fish1_direction) {
			x1_move_fish1 += 4;
			x2_move_fish1 += 4;
			y1_move_fish1 += 4;
			y2_move_fish1 += 4;
		}
		else {
			x1_move_fish1 -= 4;
			x2_move_fish1 -= 4;
			y1_move_fish1 -= 4;
			y2_move_fish1 -= 4;
		}

		if (fish2_direction) {
			x1_move_fish2 += 4;
			x2_move_fish2 += 4;
			y1_move_fish2 += 4;
			y2_move_fish2 += 4;
		}
		else {
			x1_move_fish2 -= 4;
			x2_move_fish2 -= 4;
			y1_move_fish2 -= 4;
			y2_move_fish2 -= 4;

		}
		glutPostRedisplay();
		glutTimerFunc(62, timer_func, 1);
		break;
	case 4:
			if (fish1_direction) {
				x1_move_fish1 += 4;
				x2_move_fish1 += 4;
				y1_move_fish1 -= 4;
				y2_move_fish1 -= 4;
			}
			else {
				x1_move_fish1 -= 4;
				x2_move_fish1 -= 4;
				y1_move_fish1 += 4;
				y2_move_fish1 += 4;	
			}

			if (fish2_direction) {
				x1_move_fish2 += 4;
				x2_move_fish2 += 4;
				y1_move_fish2 -= 4;
				y2_move_fish2 -= 4;
				
			}
			else {
				x1_move_fish2 -= 4;
				x2_move_fish2 -= 4;
				y1_move_fish2 += 4;
				y2_move_fish2 += 4;

			}
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
	default:
		break;
	}
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
		glutPostRedisplay();
		break;
	case 'J': case'j':
		x_min = x_min + 5;
		x_max = x_max + 5;
		glutPostRedisplay();
		break;
	case 'U': case 'u':
		y_min = y_min + 5;
		y_max = y_max + 5;
		glutPostRedisplay();
		break;
	case 'N': case 'n':
		y_min = y_min - 5;
		y_max = y_max - 5;
		glutPostRedisplay();
		break;
	case 'F': case 'f':
		break;

	default:
		break;
	}
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
	// Set up light source at the world origin. Initialized at main to reduce overhead though small if initialized in the
	// display callback handler
	//setup_light_source();
	init();
	glutMainLoop();
	return 0;
}
