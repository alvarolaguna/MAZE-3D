#include <stdlib.h>
#include <glew.h>
#include <freeglut.h>
#include <iostream>
#include <cmath>

#include "Maze.h"
#include "Player.h"


using namespace std;

// Global variables for 3D camera
enum materials { jade, chrome, ruby, turquoise };
int material = 0;
int mouseButton = 0;
bool mouseRotate = false;
bool mouseZoom = false;
bool mousePan = false;
int xClick = 0;
int yClick = 0;
int xAux = 0;
int yAux = 0;
double rotX = 0.;
double rotY = 0.;
double panX = 0.;
double panY = 0.;
double zoomZ = -10.;
double moveZ = -10.;
int mainWindow, subWindow1, subWindow2, subWindow3;
int width = 1200, height = 600;



Maze myMaze(0);
Player myPlayer;

//float angle = 0.0f;
float angle = 1.57079633f*2.0;
float lx = sin(angle);
float lz = -cos(angle);
//float lx = 0.0f, lz = -1.0f;
float x = -36.0f, z = -20.0f;

double radiusVision = 100.;

/*
*	A structure to represent the mouse information
*/
struct Mouse
{
	int x;		/*	the x coordinate of the mouse cursor	*/
	int y;		/*	the y coordinate of the mouse cursor	*/
	int lmb;	/*	is the left button pressed?		*/
	int mmb;	/*	is the middle button pressed?	*/
	int rmb;	/*	is the right button pressed?	*/

				/*
				*	These two variables are a bit odd. Basically I have added these to help replicate
				*	the way that most user interface systems work. When a button press occurs, if no
				*	other button is held down then the co-ordinates of where that click occured are stored.
				*	If other buttons are pressed when another button is pressed it will not update these
				*	values.
				*
				*	This allows us to "Set the Focus" to a specific portion of the screen. For example,
				*	in maya, clicking the Alt+LMB in a view allows you to move the mouse about and alter
				*	just that view. Essentually that viewport takes control of the mouse, therefore it is
				*	useful to know where the first click occured....
				*/
	int xpress; /*	stores the x-coord of when the first button press occurred	*/
	int ypress; /*	stores the y-coord of when the first button press occurred	*/
};

/*
*	rename the structure from "struct Mouse" to just "Mouse"
*/
typedef struct Mouse Mouse;

/*
*	Create a global mouse structure to hold the mouse information.
*/
Mouse TheMouse = { 0,0,0,0,0 };

/*----------------------------------------------------------------------------------------
*	Button Stuff
*/

/*
*	We will define a function pointer type. ButtonCallback is a pointer to a function that
*	looks a bit like this :
*
*	void func() {
*	}
*/
typedef void(*ButtonCallback)();

/*
*	This is a simple structure that holds a button.
*/
struct Button
{
	int	  id;							//
	int   x;							/* top left x coord of the button */
	int   y;							/* top left y coord of the button */
	int   w;							/* the width of the button */
	int   h;							/* the height of the button */
	int	  state;						/* the state, 1 if pressed, 0 otherwise */
	int	  highlighted;					/* is the mouse cursor over the control? */
	char* label;						/* the text label of the button */
	ButtonCallback callbackFunction;	/* A pointer to a function to call if the button is pressed */
};
typedef struct Button Button;



/*----------------------------------------------------------------------------------------
*	This is an example callback function. Notice that it's type is the same
*	an the ButtonCallback type. We can assign a pointer to this function which
*	we can store and later call.
*/
void setLevelEasy()
{
	myMaze.setLevel(0);
	//printf("Set Easy Maze!");
}

void setLevelMedium()
{
	myMaze.setLevel(1);
	//printf("Set Medium Maze!");
}

void setLevelHard()
{
	myMaze.setLevel(2);
	//printf("Set Hard Maze!");
}

/*----------------------------------------------------------------------------------------
*	This is the button visible in the viewport. This is a shorthand way of
*	initialising the structure's data members. Notice that the last data
*	member is a pointer to the above function.
*/
Button MyButton = { 0, 40, 95, 120,50, 0,0, "Easy", NULL }; //setLevelEasy
Button MySecButton = {1, 40, 155, 120,50, 0,0, "Medium", NULL }; //setLevelMedium
Button MyThirdButton = {2, 40, 215, 120,50, 0,0, "Hard", NULL };

Button ButtonJade = { 3, 40, 340, 120,50, 0,0, "Jade", NULL };
Button ButtonChrome = { 3, 40, 400, 120,50, 0,0, "Chrome", NULL };
Button ButtonRuby = { 3, 40, 460, 120,50, 0,0, "Ruby", NULL };
Button ButtonTurquoise = { 3, 40, 520, 120,50, 0,0, "Turquoise", NULL };


/*----------------------------------------------------------------------------------------
*	\brief	This function draws a text string to the screen using glut bitmap fonts.
*	\param	font	-	the font to use. it can be one of the following :
*
*					GLUT_BITMAP_9_BY_15
*					GLUT_BITMAP_8_BY_13
*					GLUT_BITMAP_TIMES_ROMAN_10
*					GLUT_BITMAP_TIMES_ROMAN_24
*					GLUT_BITMAP_HELVETICA_10
*					GLUT_BITMAP_HELVETICA_12
*					GLUT_BITMAP_HELVETICA_18
*
*	\param	text	-	the text string to output
*	\param	x		-	the x co-ordinate
*	\param	y		-	the y co-ordinate
*/
void Font(void *font, char *text, int x, int y)
{
	glRasterPos2i(x, y);

	while (*text != '\0')
	{
		glutBitmapCharacter(font, *text);
		++text;
	}
}

int ButtonClickTest(Button* b, int x, int y)
{
	if (b)
	{
		/*
		*	If clicked within button area, then return true
		*/
		if (x > b->x      &&
			x < b->x + b->w &&
			y > b->y      &&
			y < b->y + b->h) {
			return 1;
		}
	}

	/*
	*	otherwise false.
	*/
	return 0;
}

void ButtonRelease(Button *b, int x, int y)
{
	if (b)
	{
		/*
		*	If the mouse button was pressed within the button area
		*	as well as being released on the button.....
		*/
		if (ButtonClickTest(b, TheMouse.xpress, TheMouse.ypress) &&
			ButtonClickTest(b, x, y))
		{
			/*
			*	Then if a callback function has been set, call it.
			*/

			if (b->callbackFunction) {
				b->callbackFunction();
			}
		}

		/*
		*	Set state back to zero.
		*/
		b->state = 0;
	}
}

void ButtonPress(Button *b, int x, int y)
{
	if (b)
	{
		/*
		*	if the mouse click was within the buttons client area,
		*	set the state to true.
		*/
		if (ButtonClickTest(b, x, y))
		{
			b->state = 1;
		}
	}
}

void ButtonPassive(Button *b, int x, int y)
{
	if (b)
	{
		/*
		*	if the mouse moved over the control
		*/
		if (ButtonClickTest(b, x, y))
		{
			/*
			*	If the cursor has just arrived over the control, set the highlighted flag
			*	and force a redraw. The screen will not be redrawn again until the mouse
			*	is no longer over this control
			*/
			if (b->highlighted == 0) {
				b->highlighted = 1;
				glutPostRedisplay();
			}
		}
		else

			/*
			*	If the cursor is no longer over the control, then if the control
			*	is highlighted (ie, the mouse has JUST moved off the control) then
			*	we set the highlighting back to false, and force a redraw.
			*/
			if (b->highlighted == 1)
			{
				b->highlighted = 0;
				glutPostRedisplay();
			}
	}
}

void checkFunction(int x, int y)
{
	if (ButtonClickTest(&MyButton, x, y) == 1)
	{
		setLevelEasy();
	}
	else if (ButtonClickTest(&MySecButton, x, y) == 1)
	{
		setLevelMedium();
	}
	else if (ButtonClickTest(&MyThirdButton, x, y) == 1)
	{
		setLevelHard();
	}
	else if (ButtonClickTest(&ButtonJade, x, y) == 1)
	{
		material = jade;
	}
	else if (ButtonClickTest(&ButtonChrome, x, y) == 1)
	{
		material = chrome;
	}
	else if (ButtonClickTest(&ButtonRuby, x, y) == 1)
	{
		material = ruby;
	}
	else if (ButtonClickTest(&ButtonTurquoise, x, y) == 1)
	{
		material = turquoise;
	}

}

void ButtonDraw(Button *b)
{
	int fontx;
	int fonty;

	if (b)
	{
		/*
		*	We will indicate that the mouse cursor is over the button by changing its
		*	colour.
		*/
		if (b->highlighted)
			glColor3f(0.7f, 0.7f, 0.8f);
		else
			glColor3f(0.47f, 0.72f, 0.8f);

		/*
		*	draw background for the button.
		*/
		glBegin(GL_QUADS);
		glVertex2i(b->x, b->y);
		glVertex2i(b->x, b->y + b->h);
		glVertex2i(b->x + b->w, b->y + b->h);
		glVertex2i(b->x + b->w, b->y);
		glEnd();

		/*
		*	Draw an outline around the button with width 3
		*/
		glLineWidth(3);

		/*
		*	The colours for the outline are reversed when the button.
		*/
		if (b->state)
			glColor3f(0.4f, 0.4f, 0.4f);
		else
			glColor3f(0.8f, 0.8f, 0.8f);

		glBegin(GL_LINE_STRIP);
		glVertex2i(b->x + b->w, b->y);
		glVertex2i(b->x, b->y);
		glVertex2i(b->x, b->y + b->h);
		glEnd();

		if (b->state)
			glColor3f(0.8f, 0.8f, 0.8f);
		else
			glColor3f(0.4f, 0.4f, 0.4f);

		glBegin(GL_LINE_STRIP);
		glVertex2i(b->x, b->y + b->h);
		glVertex2i(b->x + b->w, b->y + b->h);
		glVertex2i(b->x + b->w, b->y);
		glEnd();

		glLineWidth(1);


		/*
		*	Calculate the x and y coords for the text string in order to center it.
		*/
		fontx = b->x + (b->w - glutBitmapLength(GLUT_BITMAP_HELVETICA_18, reinterpret_cast<unsigned const char*>(b->label))) / 2;
		fonty = b->y + (b->h + 10) / 2;

		/*
		*	if the button is pressed, make it look as though the string has been pushed
		*	down. It's just a visual thing to help with the overall look....
		*/
		if (b->state) {
			fontx += 2;
			fonty += 2;
		}

		/*
		*	If the cursor is currently over the button we offset the text string and draw a shadow
		*/
		if (b->highlighted)
		{
			glColor3f(0, 0, 0);
			Font(GLUT_BITMAP_HELVETICA_18, b->label, fontx, fonty);
			fontx--;
			fonty--;
		}

		glColor3f(1, 1, 1);
		Font(GLUT_BITMAP_HELVETICA_18, b->label, fontx, fonty);
	}
}


// The init OpenGL Context: The OpenGL state machine initialization
void init()
{
	/*
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glColor3f(1.0, 1.0, 1.0);
	*/
	
	glutSetWindow(subWindow1);
	glClearColor(.2, .2, .2, .0);
	glColor3f(1.0, 1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//glTranslatef(0., 0., 0.); // Camera/World initial position

	glutSetWindow(subWindow2);
	glClearColor(.7, .7, .7, .0);
	glColor3f(1.0, 1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	myMaze.setLight(jade);

	glutSetWindow(subWindow3);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glColor3f(1.0, 1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	myMaze.setLight(jade);
	
}


void displayFloor()
{
	float z = -radiusVision / 2.;
	for (int i = 0; i <= radiusVision; i++)
	{
		glBegin(GL_LINES);
		glVertex3f(-radiusVision / 2., 0., z);
		glVertex3f(radiusVision / 2., 0., z);
		glEnd();
		z = z + 1.;
	}
	float x = -radiusVision / 2.;
	for (int i = 0; i <= radiusVision; i++)
	{
		glBegin(GL_LINES);
		glVertex3f(x, 0., -radiusVision / 2.);
		glVertex3f(x, 0., radiusVision / 2.);
		glEnd();
		x = x + 1.;
	}
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glutSwapBuffers();	// Swap buffers
}

void display1()
{
	glClear(GL_COLOR_BUFFER_BIT |
		GL_DEPTH_BUFFER_BIT);



	/*
	*	Set the orthographic viewing transformation
	*/
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, width/6, height, 0, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glColor3f(1, 1, 1);
	Font(GLUT_BITMAP_HELVETICA_18, "MAZE 3D!", 50, 30);
	Font(GLUT_BITMAP_HELVETICA_18, "Select difficulty", 40, 80);
	Font(GLUT_BITMAP_HELVETICA_18, "Select material", 40, 320);
	ButtonDraw(&MyButton);
	ButtonDraw(&MySecButton);
	ButtonDraw(&MyThirdButton);
	ButtonDraw(&ButtonJade);
	ButtonDraw(&ButtonChrome);
	ButtonDraw(&ButtonRuby);
	ButtonDraw(&ButtonTurquoise);


	/*
	*	Bring the back buffer to the front and vice-versa.
	*/
	glutSwapBuffers();// Swap buffers
}

// The scene display function
void display2()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	myMaze.setLight(material);
	// Camera
	glPushMatrix();
	glLoadIdentity();
	//gluLookAt(x, 1.0f, z, x + lx, 1.0f, z + lz, 0.0f, 1.0f, 0.0f);
	gluLookAt(0., 300., 0., 0., 0., 0., 0., 0., 1.);
	
	
	// Scene Model
	displayFloor();
	glPushMatrix();
	glTranslatef(x, 0.0, z);
	glColor3f(1.,0.,0.);
	//glTranslatef(32., 0., 4.);
	glutSolidCube(3.);
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(-40., 0., -20.);
	glScalef(4., 8., 4.);
	for (int i = 0; i < myMaze.getGridHeight(); i++)
	{
		for (int j = 0; j < myMaze.getGridWidth(); j++)
		{
			if (myMaze.getCoordValue(i, j) == 1) {
				glPushMatrix();
				glTranslatef(1.*j, 0., 1.*i);
				glutSolidCube(1.);
				glPopMatrix();
			}
		}
	}
	glPopMatrix();
	
	//End Scene Model

	glPopMatrix();

	glutSwapBuffers();	// Swap buffers
}

// The scene display function
void display3()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	myMaze.setLight(material);

	// Camera
	glPushMatrix();
	glLoadIdentity();
	gluLookAt(x, 1.0f, z, x + lx, 1.0f, z + lz, 0.0f, 1.0f, 0.0f);
	


	// Scene Model
	displayFloor();
	glPushMatrix();
	//glTranslatef(-40., 0., -8.);
	//glutSolidCube(3.5);
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(-40., 0., -20.);
	glScalef(4., 8., 4.);
	for (int i = 0; i < myMaze.getGridHeight(); i++)
	{
		for (int j = 0; j < myMaze.getGridWidth(); j++)
		{
			if (myMaze.getCoordValue(i, j) == 1) {
				glPushMatrix();
				glTranslatef(1.*j, 0., 1.*i);
				glutSolidCube(1.);
				glPopMatrix();
			}
		}
	}
	glPopMatrix();
	

	//End Scene Model

	glPopMatrix();

	glutSwapBuffers();	// Swap buffers
}

// The proyection function

void reshape(int w, int h)
{
	if (h == 0)
	{
		h = 1;
	}
	width = w;
	height = h;

	glutSetWindow(subWindow1);
	glutPositionWindow(0, 0);
	glutReshapeWindow(200, height);

	glutSetWindow(subWindow2);
	glutPositionWindow(200, 0);
	glutReshapeWindow(500, height);

	glutSetWindow(subWindow3);
	glutPositionWindow(700, 0);
	glutReshapeWindow(500, height);

}

void reshape1(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (h == 0)
	{
		h = 1;
	}
	gluPerspective(25.0, (double)w / h, 0.1, 1000.0);	//proyección en perspectiva
	glMatrixMode(GL_MODELVIEW);
}

void reshape2(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (h == 0)
	{
		h = 1;
	}
	gluPerspective(25.0, (double)w / h, 0.1, 1000.0);	//proyección en perspectiva
	glMatrixMode(GL_MODELVIEW);
}

void reshape3(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (h == 0)
	{
		h = 1;
	}
	gluPerspective(25.0, (double)w / h, 0.1, 1000.0);	//proyección en perspectiva
	glMatrixMode(GL_MODELVIEW);
}

void idle()
{
	glutPostRedisplay();
	glutSetWindow(subWindow2);
	glutPostRedisplay();
	glutSetWindow(subWindow3);
	glutPostRedisplay();
}

/*
void mouse(int button, int state, int x, int y)
{
	mouseButton = button;
	mouseRotate = false;
	mouseZoom = false;
	mousePan = false;
	xClick = x;
	yClick = y;
	if (button == GLUT_LEFT_BUTTON)
	{
		mouseRotate = true;
		xAux = rotX;
		yAux = rotY;
	}
	else if (button == GLUT_RIGHT_BUTTON)
	{
		mouseZoom = true;
		xAux = zoomZ;
	}
	else if (button == GLUT_MIDDLE_BUTTON)
	{
		mousePan = true;
		xAux = panX;
		yAux = panY;
	}
}

void mouseMotion(int x, int y)
{
	if (mouseRotate == true)
	{
		if (mouseButton == GLUT_LEFT_BUTTON)
		{
			if ((x - xClick + xAux) > 360.0)
			{
				xAux = xAux - 360.0;
			}
			if ((x - xClick + xAux) < 0.0)
			{
				xAux = xAux + 360.0;
			}
			if ((y - yClick + yAux) > 360.0)
			{
				yAux = yAux - 360.0;
			}
			if ((y - yClick + yAux) < 0.0)
			{
				yAux = yAux + 360.0;
			}
			rotX = x - xClick + xAux;
			rotY = y - yClick + yAux;
		}
	}
	else if (mouseZoom == true)
	{
		if (mouseButton == GLUT_RIGHT_BUTTON)
		{
			zoomZ = ((x - xClick) / 10.0) + xAux;
		}
	}
	else if (mousePan == true)
	{
		if (mouseButton == GLUT_MIDDLE_BUTTON)
		{
			panX = ((x - xClick) / 63.0) + xAux;
			panY = ((y - yClick) / (-63.0)) + yAux;
		}
	}
}
*/

void MouseButton(int button, int state, int x, int y)
{
	/*
	*	update the mouse position
	*/
	TheMouse.x = x;
	TheMouse.y = y;

	/*
	*	has the button been pressed or released?
	*/
	if (state == GLUT_DOWN)
	{
		/*
		*	This holds the location of the first mouse click
		*/
		if (!(TheMouse.lmb || TheMouse.mmb || TheMouse.rmb)) {
			TheMouse.xpress = x;
			TheMouse.ypress = y;
		}

		/*
		*	Which button was pressed?
		*/
		switch (button)
		{
		case GLUT_LEFT_BUTTON:
			TheMouse.lmb = 1;
			checkFunction(x,y);
			/*
			ButtonPress(&MyButton, x, y);
			ButtonPress(&MySecButton, x, y);
			ButtonPress(&MyThirdButton, x, y);
			*/
		case GLUT_MIDDLE_BUTTON:
			TheMouse.mmb = 1;
			break;
		case GLUT_RIGHT_BUTTON:
			TheMouse.rmb = 1;
			break;
		}
	}
	else
	{
		/*
		*	Which button was released?
		*/
		switch (button)
		{
		case GLUT_LEFT_BUTTON:
			TheMouse.lmb = 0;
			ButtonRelease(&MyButton, x, y);
			ButtonRelease(&MySecButton, x, y);
			ButtonRelease(&MyThirdButton, x, y);
			ButtonRelease(&ButtonJade, x, y);
			ButtonRelease(&ButtonChrome, x, y);
			ButtonRelease(&ButtonRuby, x, y);
			ButtonRelease(&ButtonTurquoise, x, y);
			break;
		case GLUT_MIDDLE_BUTTON:
			TheMouse.mmb = 0;
			break;
		case GLUT_RIGHT_BUTTON:
			TheMouse.rmb = 0;
			break;
		}
	}

	/*
	*	Force a redraw of the screen. If we later want interactions with the mouse
	*	and the 3D scene, we will need to redraw the changes.
	*/
	glutPostRedisplay();
}

/*----------------------------------------------------------------------------------------
*	\brief	This function is called whenever the mouse cursor is moved AND A BUTTON IS HELD.
*	\param	x	-	the new x-coord of the mouse cursor.
*	\param	y	-	the new y-coord of the mouse cursor.
*/
void MouseMotion(int x, int y)
{
	/*
	*	Calculate how much the mouse actually moved
	*/
	int dx = x - TheMouse.x;
	int dy = y - TheMouse.y;

	/*
	*	update the mouse position
	*/
	TheMouse.x = x;
	TheMouse.y = y;


	/*
	*	Check MyButton to see if we should highlight it cos the mouse is over it
	*/
	ButtonPassive(&MyButton, x, y);
	ButtonPassive(&MySecButton, x, y);
	ButtonPassive(&MyThirdButton, x, y);
	ButtonPassive(&ButtonJade, x, y);
	ButtonPassive(&ButtonChrome, x, y);
	ButtonPassive(&ButtonRuby, x, y);
	ButtonPassive(&ButtonTurquoise, x, y);
	/*
	*	Force a redraw of the screen
	*/
	glutPostRedisplay();
}

/*----------------------------------------------------------------------------------------
*	\brief	This function is called whenever the mouse cursor is moved AND NO BUTTONS ARE HELD.
*	\param	x	-	the new x-coord of the mouse cursor.
*	\param	y	-	the new y-coord of the mouse cursor.
*/
void MousePassiveMotion(int x, int y)
{
	/*
	*	Calculate how much the mouse actually moved
	*/
	int dx = x - TheMouse.x;
	int dy = y - TheMouse.y;

	/*
	*	update the mouse position
	*/
	TheMouse.x = x;
	TheMouse.y = y;

	/*
	*	Check MyButton to see if we should highlight it cos the mouse is over it
	*/
	ButtonPassive(&MyButton, x, y);
	ButtonPassive(&MySecButton, x, y);
	ButtonPassive(&MyThirdButton, x, y);
	ButtonPassive(&ButtonJade, x, y);
	ButtonPassive(&ButtonChrome, x, y);
	ButtonPassive(&ButtonRuby, x, y);
	ButtonPassive(&ButtonTurquoise, x, y);
	/*
	*	Note that I'm not using a glutPostRedisplay() call here. The passive motion function
	*	is called at a very high frequency. We really don't want much processing to occur here.
	*	Redrawing the screen every time the mouse moves is a bit excessive. Later on we
	*	will look at a way to solve this problem and force a redraw only when needed.
	*/
}

void specialKeyboard(int key, int xx, int yy)
{
	float fraction = 4.f;

	switch (key) {
	case GLUT_KEY_LEFT:
		angle -= 1.57079633f;
		lx = sin(angle);
		lz = -cos(angle);
		myPlayer.turnLeft();
		break;
	case GLUT_KEY_RIGHT:
		angle += 1.57079633f;
		lx = sin(angle);
		lz = -cos(angle);
		myPlayer.turnRight();
		break;
	case GLUT_KEY_UP:
		if (myMaze.checkMove(myPlayer.nextMove()[0], myPlayer.nextMove()[1]))
		{
			x += lx * fraction;
			z += lz * fraction;
			myPlayer.move();
		}
		break;
		/*
	case GLUT_KEY_DOWN:
		x -= lx * fraction;
		z -= lz * fraction;
		break;
		*/
	}

	glutPostRedisplay();

}

// The main program
int main(int argc, char** argv)
{
	// Init Window (GLUT)
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(width, height);
	mainWindow = glutCreateWindow("Maze 3D");
	subWindow1 = glutCreateSubWindow(mainWindow, 0, 0, 200, height);
	subWindow2 = glutCreateSubWindow(mainWindow, 200, 0, 500, height);
	subWindow3 = glutCreateSubWindow(mainWindow, 700, 0, 500, height);
	

	// Callback functions registration (GLUT)
	glutSetWindow(mainWindow);
	glutDisplayFunc(display);		// display scene function
	glutReshapeFunc(reshape);		// reshape window function

	glutSetWindow(subWindow1);
	glutDisplayFunc(display1);		// display scene function
	glutReshapeFunc(reshape1);		// reshape window function
	glutMouseFunc(MouseButton);
	glutMotionFunc(MouseMotion);
	glutPassiveMotionFunc(MousePassiveMotion);

	glutSetWindow(subWindow2);
	glutDisplayFunc(display2);		// display scene function
	glutReshapeFunc(reshape2);		// reshape window function
	glutIdleFunc(idle);
	
	glutSetWindow(subWindow3);
	glutDisplayFunc(display3);		// display scene function
	glutReshapeFunc(reshape3);		// reshape window function
	glutSpecialFunc(specialKeyboard);
	glutIdleFunc(idle);

	// Callback functions registration (GLUT)
	/*
	glutIdleFunc(idle);				// idle events function
	glutMouseFunc(mouse);			// processing mouse events function
	glutMotionFunc(mouseMotion);	// processing mouse motion function
	glutSpecialFunc(specialKeyboard);
	*/

	// a valid OpenGL context?
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		fprintf(stderr, "GLEW error");
		return 1;
	}
	init();							// init OpenGL context
	glutMainLoop();					// Main loop waiting for events!
	return 0;
}





