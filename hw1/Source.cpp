#include <stdio.h>;
#include <stdio.h>
#include <math.h>
#include <GL/freeglut.h> //Please change it before executing!!!!!!!!!!!!!!!!!!!!!!!!!!!
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
using namespace std;


#define XOFF          0
#define YOFF          0
#define WINDOW_WIDTH  600
#define WINDOW_HEIGHT 600


void display();
void idle();
void myinit(void);
void draw_circle(int x, int y, int r);
void draw_four_points(int x, int y, int xx, int yy);
void reshape(int w, int h);
int x, y, r;  // declaring variables
int stored_para[20][3];
int size_port = 0;
int w = size_port;
int h = size_port;
int mode;
string file_name;
int animation = 0;
int animation_index = 1;
int K = 29;

/* Function to handle file input; modification may be needed */
void file_in(void);

/*-----------------
The main function
------------------*/
int main(int argc, char **argv)
{
	 // accept user inputs


	glutInit(&argc, argv);

	/* Use both double buffering and Z buffer */
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowPosition(XOFF, YOFF);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("CS6533/CS4533 Assignment 1");
	printf("Do you want to draw your own circle or input a file?(1 or 2) ");
	scanf_s("%d", &mode);
	if (mode == 1) {
		printf("Enter the x, y and radius: ");
		scanf_s("%d %d %d", &x, &y, &r);
		stored_para[0][0] = x;
		stored_para[0][1] = y;
		stored_para[0][2] = r;
		int temp_size_x = std::max(abs(x + r), abs(x - r));
		int temp_size_y = std::max(abs(y + r), abs(y - r));
		int temp_size = std::max(abs(temp_size_x), abs(temp_size_y));
		size_port = std::max(size_port, temp_size);
		glutDisplayFunc(display);
		glutReshapeFunc(reshape);
	}
	if (mode == 2) {
		printf("Please enter the name of input file(end with txt): ");
		char temp[30];
		scanf_s("%s", temp, sizeof(temp));
		file_name = temp;
		animation = 1;
		file_in();
		glutDisplayFunc(display);
		glutReshapeFunc(reshape);

		glutIdleFunc(idle);
	}



	/* Function call to handle file input here */


	myinit();
	glutMainLoop();

	return 0;
}

/*----------
file_in(): file input function. Modify here.
------------*/
void file_in()
{
	
	string line;
	ifstream myfile(file_name);
	if (myfile.is_open()) {
		std::string line;
		int line_num = 0;
		while (getline(myfile, line)) {
			// using printf() in all tests for consistency
			printf("%s", line);
			int num1, num2, num3;
			int index[2] = { 0, 0 };
			int space_num = 0;
			int i = 0;
			for (char& c : line) {
				if (c == ' ') {
					index[space_num] = i;
					space_num += 1;
				}
				i += 1;
			}

			if ((index[0] != 0) && (index[1] != 0)) {
				string temp = line.substr(0, index[0]);
				x = stoi(temp, nullptr, 10);
				temp = line.substr(index[0] + 1, index[1]);
				y = stoi(temp, nullptr, 10);
				temp = line.substr(index[1] + 1, line.length());
				r = stoi(temp, nullptr, 10);
				int temp_size_x = std::max(abs(x + r), abs(x - r));
				int temp_size_y = std::max(abs(y + r), abs(y - r));
				int temp_size = std::max(abs(temp_size_x), abs(temp_size_y));
				size_port = std::max(size_port, temp_size);
				stored_para[line_num][0] = x;
				stored_para[line_num][1] = y;
				stored_para[line_num][2] = r;
			}
			line_num += 1;
		}
	}
	
	myfile.close();
	if (myfile.is_open()) {
		std::string line;
		int line_num = 0;
		while (getline(myfile, line)) {
			// using printf() in all tests for consistency
			printf("%s", line);
			int num1, num2, num3;
			int index[2] = { 0, 0 };
			int space_num = 0;
			int i = 0;
			for (char& c : line) {
				if (c == ' ') {
					index[space_num] = i;
					space_num += 1;
				}
				i += 1;
			}


			
		}
	}

	myfile.close();
}

void reshape(int w, int h) {
	glViewport(0, 0, w, h);
}


/*---------------------------------------------------------------------
display(): This function is called once for _every_ frame.
---------------------------------------------------------------------*/
void display()
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glColor3f(1.0, 0.84, 0);              /* draw in golden yellow */
	glPointSize(1.0);                     /* size of each point */

	
	//glVertex2i(300, 300);               /* draw a vertex here */


	int i = 0;
	if (animation == 0) {
		glBegin(GL_POINTS);
		while (i < 20) {
			if (stored_para[i][0] != NULL) {
				int aa = stored_para[i][0];
				draw_circle(stored_para[i][0], stored_para[i][1], stored_para[i][2]);
			}
			i += 1;
		}
	}
	else {
			glBegin(GL_POINTS);
			while (i < 20) {
				float curR = stored_para[i][2] * animation_index / K;
				if (stored_para[i][0] != NULL) {
					int aa = stored_para[i][0];
					draw_circle(stored_para[i][0], stored_para[i][1], curR);
				}
				i += 1;
			}
			glEnd();

			
	}
	glEnd();

	glFlush();                            /* render graphics */

	glutSwapBuffers();                    /* swap buffers */
}

/*---------------------------------------------------------------------
myinit(): Set up attributes and viewing
---------------------------------------------------------------------*/
void myinit()
{
	glClearColor(0.0, 0.0, 0.92, 0.0);    /* blue background*/

	/* set up viewing */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(-size_port, size_port, -size_port, size_port);
	glMatrixMode(GL_MODELVIEW);
}

void draw_circle(int x, int y, int r) {
	r = abs(r);
	int a = r;
	int b = 0;
	draw_four_points(x, y, a, b);
	bool inner = true;
	int calc = pow(a, 2) * 2 + pow(b, 2) * 2 - a * 2 + b * 4 + 3 - pow(r, 2) * 2;
	if (calc < 0) {
		b += 1;
		inner = false;
	}
	else {
		a -= 1;
		b += 1;
	}
	draw_four_points(x, y, a, b);


	while (b < a) {
		if (inner == true) {
			calc = pow(a, 2) * 2 + pow(b, 2) * 2 - 5 * a + 8 * b + 13 - pow(r, 2) * 2;
			if (calc < 0) {
				b += 1;
				inner = false;
			}
			else {
				a -= 1;
				b += 1;
				inner = true;
			}
		}
		else {
			calc = pow(a, 2) * 2 + pow(b, 2) * 2 - a * 2 + 8 * b + 9 - pow(r, 2) * 2;
			if (calc < 0) {
				b += 1;
				inner = false;
			}
			else {
				a -= 1;
				b += 1;
				inner = true;
			}
		}
		draw_four_points(x, y, a, b);
	}
}

void draw_four_points(int x, int y, int xx, int yy)
{
	glVertex2i(x + xx, y + yy);
	glVertex2i(x - xx, y + yy);
	glVertex2i(x + xx, y - yy);
	glVertex2i(x - xx, y - yy);
	glVertex2i(x + yy, y + xx);
	glVertex2i(x - yy, y + xx);
	glVertex2i(x + yy, y - xx);
	glVertex2i(x - yy, y - xx);
}

void idle() {
	animation_index += 1;
	if (animation_index > K) animation_index = 1;
	glutPostRedisplay();
}