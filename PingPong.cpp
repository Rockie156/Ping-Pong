//
//  main.m
//  Take2
//
//

#include <stdlib.h>
#include <time.h>
#include <math.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

bool CalcBool = true;
bool on;
int direction;
double pS = .03625;
double max[2];
double speed = 10;
bool noPlayerInput = TRUE;
bool initialised = FALSE;
double t = 0;
bool help = false;
bool cheat = false;
bool pausebool = TRUE;
double RGB[3] = {0.8, 0.8, 1.0};

struct ball {
	double pa[3];
	double pb[3];
	double gC[3];
	double r;
	double d;
	double iX;
	double iY;
} b;

struct Paddle {
	double yC[2];
	bool b;
	double ps;
	double pa[2];
	double pb[2];
	double pd;
	double i;
	double leng;
	double stupidity;
} P1, P2;

void init () 
{	double tmpdirection = pi / 180 * 30;
	double tmppointa[3]= {0, 0, 0};
	double tmppointb[3] = {cos(tmpdirection), sin(tmpdirection), 0};
	double tmpgCoord[3] = {tmppointa[0], tmppointa[1], tmppointa[2]};
	
	b.d = tmpdirection;
	b.pa[0] = tmppointa[0];
	b.pb[0] = tmppointb[0];
	b.gC[0] = tmpgCoord[0];
	b.pa[1] = tmppointa[1];
	b.pb[1] = tmppointb[1];
	b.gC[1] = tmpgCoord[1];
	b.pa[2] = tmppointa[2];
	b.pb[2] = tmppointb[2];
	b.gC[2] = tmpgCoord[2];
	b.r = 0.1;
	
	P1.leng = 1;
	P1.b = false;
	P1.yC[0] = -.5;
	P1.yC[1] = .5;
	
	P2.stupidity = 5;
	P2.b = false;
	P2.yC[0] = -.5;
	P2.yC[1] = .5;
	
	GLfloat ambient[] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat position[] = { 0.0, 3.0, 2.0, 0.0 };
	GLfloat lmodel_ambient[] = { 0.4, 0.4, 0.4, 1.0 };
	GLfloat local_view[] = { 0.0 };
	
	glClearColor(RGB[0], RGB[1], RGB[2], 0);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
	
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, position);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
	glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER, local_view);
	
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);	
	
	printf("Direction is: %f\n", b.d);
	
}

void intersectCalc()
{	
	double sign = 1;
	b.iX = (max[0] - b.pa[0]) / (b.pb[0] - b.pa[0]);
	b.iY = (max[1] - b.pa[1]) / (b.pb[1] - b.pa[1]);  //time when ball intersects top
	
	if (cos(b.d) < 0 && b.gC[0] > -5) {
		P2.i = ((-5 - b.pa[0]) / (b.pb[0] - b.pa[0]))  * (b.pb[1] - b.pa[1]) + b.pa[1];//position when x = -5
		
		if ((P2.i < -fabs(max[1])) || P2.i > fabs(max[1])) {
			if (P2.i < -fabs(max[1])) {
				sign = -1;
			}
			P2.pa[0] = b.pa[0] + b.iY * (b.pb[0] - b.pa[0]);  // plug into the parametric equation. = position where ball intersects X
			P2.pa[1] = sign * fabs(max[1]); //initial point is the point where the ball intersects the top/bottom
			P2.pd = -b.d;
			P2.pb[0] = P2.pa[0] + cos(P2.pd); 
			P2.pb[1] = P2.pa[1] + sin(P2.pd);
			P2.i = P2.pa[1] + ((-5 - P2.pa[0]) / (P2.pb[0] - P2.pa[0]) * (P2.pb[1] - P2.pa[1]));
		}
	}
	
		if (cos(b.d) > 0 && b.gC[0] < 5) {
			P1.i = b.pa[1] + (5 - b.pa[0]) / (b.pb[0] - b.pa[0])*(b.pb[1] - b.pa[1]); 
			if ((P1.i > fabs(max[1]) || P1.i < -fabs(max[1])) && cos(b.d)>0) {
				if (P1.i < -fabs(max[1])) {
					sign = -1;
				}
				P1.pa[0] = b.pa[0] + b.iY * (b.pb[0] - b.pa[0]);  // plug into the parametric equation. = position where ball intersects X
				P1.pa[1] = sign * fabs(max[1]); //initial point is the point where the ball intersects the top/bottom
				P1.pd = -b.d;
				P1.pb[0] = P1.pa[0] + cos(P1.pd); 
				P1.pb[1] = P1.pa[1] + sin(P1.pd);
				
				P1.i = P1.pa[1] + ((5 - P1.pa[0]) / (P1.pb[0] - P1.pa[0]) * (P1.pb[1] - P1.pa[1]));
		}
	}
	CalcBool = false;	
}

void PaddleMove() {
	if (cos(b.d) < 0 && b.gC[0] < P2.stupidity) {
		double difference1 =  (P2.i - (P1.leng / 2)) - P2.yC[0];
		if (fabs(difference1) > .1) {
			P2.yC[0] += pS * (difference1) / fabs(difference1);
			P2.yC[1] = P2.yC[0] + P1.leng;
		}
	} else if (P2.yC[1] > (P1.leng / 2) + .1 || P2.yC[0] < - (P1.leng / 2) - .1) {
		P2.yC[0] += pS * -(P2.yC[0] + (P1.leng / 2))/fabs(P2.yC[0] + .5);
		P2.yC[1] = P2.yC[0] + P1.leng;
	}			
	if (noPlayerInput) {
		if (cos(b.d) > 0) {
			double difference2 = (P1.i - (P1.leng / 2)) - P1.yC[0];
			if (fabs(difference2) > .1) { 
				P1.yC[0] += pS * (difference2) / fabs(difference2);
				P1.yC[1] = P1.yC[0] + P1.leng;
			}
			
		} else if (P1.yC[0] < - (P1.leng / 2) - .1 || P1.yC[1] > P1.leng + .1) {
			P1.yC[0] += pS * -(P1.yC[0]+ (P1.leng/2))/fabs(P1.yC[0]+(P1.leng/2));
			P1.yC[1] = P1.yC[0] + P1.leng;
		}			
	} else 	if (on) {
		if (cheat) {
			P1.yC[0] = -fabs(max[1]) - b.r;
			P1.yC[1] = fabs(max[1]) + b.r;
		} else {
			P1.yC[0] += pS * direction;
			P1.yC[1] = P1.yC[0] + P1.leng;
		}
	}
}

void animate()
{	if (pausebool) {
	//calculate t at which ball intersects a paddle/side
	if (CalcBool) {
		//oddly enough if the following statement is used, the ball direction is inverted!
		//fmod(t,1) > 0) {
		intersectCalc();
	}
	
	//move one unit along the ray
	b.gC[0] = b.pa[0] + t*(b.pb[0] - b.pa[0]);
	b.gC[1] = b.pa[1] + t*(b.pb[1] - b.pa[1]); 
	glutPostRedisplay();
	
	if (b.iX <= t) {
		t = 0;
		max[0] = -max[0];
		b.d = pi - b.d;
		b.pa[0] = b.gC[0];
		b.pa[1] = b.gC[1];
		b.pb[0] = b.pa[0] + cos(b.d);
		b.pb[1] = b.pa[1] + sin(b.d);
		pausebool = false;
		CalcBool = true;
		intersectCalc();
	}
	if (b.iY <= t) {
		CalcBool = true;
		t=0;
		max[1] = -max[1];
		b.d = -b.d;
		b.pa[0] = b.gC[0];
		b.pa[1] = b.gC[1];
		b.pb[0] = b.pa[0] + cos(b.d);
		b.pb[1] = b.pa[1] + sin(b.d);
		
	}
	if (P1.yC[0] <= b.gC[1] && P1.yC[1] >= b.gC[1] && 5 - b.r <= b.gC[0] && 5.25 + b.r >= b.gC[0] && cos(b.d) > 0) {
		CalcBool = true;
		max[0] = -max[0];
		b.d = pi - b.d;
		b.pa[0] = b.gC[0];
		b.pa[1] = b.gC[1];
		b.pb[0] = b.pa[0] + cos(b.d);
		b.pb[1] = b.pa[1] + sin(b.d);
		t = 0;
	}
	if (P2.yC[0] <= b.gC[1] && P2.yC[1] >= b.gC[1] && -5 + b.r >= b.gC[0] && -5.25 + b.r <= b.gC[0] && cos(b.d) < 0) {
		CalcBool = true;
		max[0] = -max[0];
		b.d = pi - b.d;
		b.pa[0] = b.gC[0];
		b.pa[1] = b.gC[1];
		b.pb[0] = b.pa[0] + cos(b.d);
		b.pb[1] = b.pa[1] + sin(b.d);
		t = 0;
	}
	//PaddleMove
	PaddleMove();
	
	//callback function
	t+= .05;
	glutTimerFunc(speed, animate, 1);
}
}


void reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (w <= (h * 2)) {
		max[0] = 6;
		max[1] = 3.0*((GLfloat)h*2)/(GLfloat)w;
		glOrtho(-max[0], max[0], -max[1], max[1],-10.0, 10.0);
	}
	else {
		max[0] = 6.0*(GLfloat)w/((GLfloat)h*2);
		max[1] = 3;	
		glOrtho(-max[0], max[0], -max[1], max[1], -10.0, 10.0);
	}
	max[0] -= b.r;
	max[1] -= b.r;
	printf("maxX: %f\nmaxY: %f\n", max[0], max[1]);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void display(void)
{
	if (!initialised) {
		animate();
		initialised = true;
	}
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(RGB[0], RGB[1], RGB[2], 0);
	glPushMatrix();
	glTranslatef (b.gC[0], b.gC[1], b.gC[2]);
	glutSolidSphere(b.r, 256, 256);	
	glPopMatrix();
	
	glColor3f( 1,  0,  0);
	glPushMatrix();
	glTranslatef(5, 0, 0);
	glRectf(0, P1.yC[0], .15, P1.yC[1]);
	if (help) {
		glTranslatef(0, P1.i, 0);
		glutSolidSphere(.05, 8, 8);
		}
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(-5, 0, 0);
	glRectf(0, P2.yC[0], -.15, P2.yC[1]);
	glPopMatrix();
	
	glutSwapBuffers();
}



void keyboard(unsigned char key, int x, int y)
{
	switch(key) {
		case 'a':
			P2.stupidity--;
			break;
		case 'A':
			P2.stupidity++;
			break;
		case '1':
			RGB[0] = .8;
			RGB[1] = .8;
			RGB[2] = 1.0;
			break;
		case '2':
			RGB[0] = .9;
			RGB[1] = 1.0;
			RGB[2] = .8;
			break;
		case '3':
			RGB[0] = .8;
			RGB[1] = .9;
			RGB[2] = 1.0;
			break;
		case '4':
			RGB[0] = .9;
			RGB[1] = .8;
			RGB[2] = 1.0;
			break;
		case 'r':
			if (RGB[0] != 1.0) {
			RGB[0]+= .1;
			}
			break;
		case 'R':
			if (RGB[0] != 0) {
			RGB[0]-= .1;
			}
			break;
		case 'g':
			if (RGB[1] != 1.0) {
			RGB[1]+= .1;
			}
			break;
		case 'G':
			if (RGB[1] != 0) {
			RGB[1]-= .1;
			}
			break;
		case 'b':
			if (RGB[2] != 1.0) {
			RGB[2]+= .1;
			}
			break;
		case 'B':
			if (RGB[2] != 0) {
			RGB[2]-= .1;
			}
			break;
		case 27:
			exit(0);
			break;
		case 'n':
		case 'N':
			pausebool = false;
			init();
			CalcBool = true;
			t = 0;
			break;
		case 'h':
		case 'H':
			help = !help;
			break;
		case 'c':
		case 'C':
			cheat = !cheat;
			break;
		case 'd':	
			printf("Debug Data...\n");
			printf("RGB= {%f, %f, %f, alpha isn't used.}\n", RGB[0], RGB[1], RGB[2]);
			printf("P1.i = %f, P2.i = %f\n", P1.i, P2.i);
			printf("P1.yC[0-1] = (%f, %f)... P2.yC[0-1] = (%f, %f)\n", P1.yC[0], P1.yC[1], P2.yC[0], P2.yC[1]);
			printf("t= %f, pausebool = %s\n", t, (pausebool)?"true":"false");
			break;
		case ' ':
			noPlayerInput = true;
			pausebool = !pausebool;
			CalcBool = true;
			animate();
			break;
		case 'w':
			noPlayerInput = false;
			direction = 1;
			on = true;
			break;
		case 's':
			noPlayerInput = false;
			direction = -1;
			on = true;
			break;
		case '+':
			speed--;
			break;
		case '-':
			speed++;
			break;
	}
}

void keyboardup(unsigned char key, int x, int y)
{
	switch (key) {
		case 'w':
			on = false;
			break;
		case 's':
			on = false;
			break;
	}
}

void special(int button, int x, int y)
{
	switch (button) {
		case GLUT_KEY_UP:
			noPlayerInput = false;
			direction = 1;
			on = true;
			break;
		case GLUT_KEY_DOWN:
			noPlayerInput = false;
			direction = -1;
			on = true;
			break;
	}	
}

void specialup(int button, int x, int y)
{
	switch (button) {
		case GLUT_KEY_UP:
			on = false;
			break;
		case GLUT_KEY_DOWN:
			on = false;
			break;
	}
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize (1588, 1000);
	glutCreateWindow("Pong");
	init();
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutKeyboardUpFunc(keyboardup);
	glutSpecialFunc(special);
	glutSpecialUpFunc(specialup);
	glutDisplayFunc(display);
	glutMainLoop();
	return 0; 
}
