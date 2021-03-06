#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
 
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string>
#include <vector>

using namespace std;

class Izvor {
public:
	float x, y, z;
	int q;
	float cR, cG, cB;
	double size;
	float v;
};

class Vrh {
public:
	float x, y, z;
	Vrh(float x1, float y1, float z1) {
		x = x1; y = y1; z = z1;
	}
};

class Cestica {
public:
	float x, y, z;
	float r, g, b;
	float v;
	int t;
	float sX, sY, sZ;
	float osX, osY, osZ;
	double kut; double size;
};

vector<Cestica> cestice;
Izvor i;
Izvor i2;

//*********************************************************************************
//	Pokazivac na glavni prozor i pocetna velicina.
//*********************************************************************************

GLuint window;
GLuint sub_width = 1024, sub_height = 512;

//*********************************************************************************
//	Function Prototypes.
//*********************************************************************************

void myDisplay();
void myIdle();
void myReshape(int width, int height);
void myKeyboard(unsigned char theKey, int mouseX, int mouseY);
void myMouse(int button, int state, int x, int y);

void nacrtajIzvor();
void nacrtajCestice();
void nacrtajCesticu(Cestica c);
GLuint LoadTexture(const char * filename,int w, int h);

double maxH = 42.0;
double pi = 3.14159;
int tempQ = -1;
GLuint tex;
Vrh ociste(0.0, 0.0, 75.0);

int main(int argc, char ** argv)
{
	i.x = 0.0; i.y = 0; i.z = 0.0;
	i.q = 10;
	i.cB = 1.0; i.cG = 1.0; i.cR = 1.0;
	i.size = 0.6;
	i.v = 1;
	srand(time(NULL));
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(sub_width, sub_height);
	glutInitWindowPosition(100, 100);
	glutInit(&argc, argv);

	window = glutCreateWindow("2. labos");
	glutReshapeFunc(myReshape);
	glutDisplayFunc(myDisplay);
	glutKeyboardFunc(myKeyboard);
	glutMouseFunc(myMouse);
	glutIdleFunc(myIdle);
	tex = LoadTexture("C:/Users/Antun Mesar/Desktop/Grafika objekti/Teksture/snow.bmp",256,256);

	//Prozirnost Tekstura
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glEnable(GL_BLEND);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, tex);

	glutMainLoop();
	return 0;
}

void myReshape(int width, int height)
{
	sub_width = width;                      	//promjena sirine prozora
	sub_height = height;						//promjena visine prozora

	glViewport(0, 0, sub_width, sub_height);	//  otvor u prozoru

	//-------------------------
	glMatrixMode(GL_PROJECTION);                // Select The Projection Matrix
	glLoadIdentity();                           // Reset The Projection Matrix

	// Calculate The Aspect Ratio Of The Window
	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 150.0f);

	glMatrixMode(GL_MODELVIEW);                 // Select The Modelview Matrix
	glLoadIdentity();                           // Reset The Modelview Matrix
}

int t = 0;

void myDisplay()
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glTranslatef(ociste.x, ociste.y, -ociste.z);

	nacrtajCestice();

	glutSwapBuffers();
}

int currentTime = 0; int previousTime = 0;

void myIdle() {
	currentTime = glutGet(GLUT_ELAPSED_TIME);
	int timeInterval = currentTime - previousTime;
	if (timeInterval > 100) {

		// Stvori nove cestice
		if (i.q > 0) {
			int n = rand() % i.q + 1;
			for (int j = 0; j < n; j++) {
				double x, y, z;
				x = (rand() % 2000 - 1000) / (double)1000;
				y = (rand() % 2000 - 1000) / (double)1000;
				z = (rand() % 2000 - 1000) / (double)1000;
				Cestica c;
				c.x = i.x; c.z = i.z; c.y = i.y;
				c.r = (rand() % 100 / (double)100);
				c.g = (rand() % 100 / (double)100);
				c.b = (rand() % 100 / (double)100);

				c.v = i.v;
				c.sX = x; c.sY = y; c.sZ = z;
				c.t = 80 + (rand() % 21 - 10);
				c.size = i.size;
				cestice.push_back(c);
			}
		}
		// Pomakni cestice
		for (int j = cestice.size() - 1; j >= 0; j--) {
			
			//Rotacija
			Vrh s(0.0, 0.0, 1.0);
			Vrh os(0.0, 0.0, 0.0);
			Vrh e(0.0, 0.0, 0.0);

			e.x = cestice.at(j).x - ociste.x;
			e.y = cestice.at(j).y - ociste.y;
			e.z = cestice.at(j).z - ociste.z;

			os.x = s.y*e.z - e.y*s.z;
			os.y = e.x*s.z - s.x*e.z;
			os.z = s.x*e.y - s.y*e.x;

			double apsS = pow(pow((double)s.x, 2.0) + pow((double)s.y, 2.0) + pow((double)s.z, 2.0), 0.5);
			double apsE = pow(pow((double)e.x, 2.0) + pow((double)e.y, 2.0) + pow((double)e.z, 2.0), 0.5);
			double se = s.x*e.x + s.y*e.y + s.z*e.z;
			double kut = acos(se / (apsS*apsE));
			cestice.at(j).kut = kut / (2 * pi) * 360;
			cestice.at(j).osX = os.x; cestice.at(j).osY = os.y; cestice.at(j).osZ = os.z;
			
			// Translacija
			cestice.at(j).sY += 0.09;
			cestice.at(j).y += cestice.at(j).v * cestice.at(j).sY;
			

			cestice.at(j).z += cestice.at(j).v * cestice.at(j).sZ;
			cestice.at(j).x += cestice.at(j).v * cestice.at(j).sX;
			//Smanji zivot
			cestice.at(j).t--;

			if (cestice.at(j).t <= 0) {
				cestice.erase(cestice.begin() + j);
			}
		}

		myDisplay();
		previousTime = currentTime;
	}
}

void nacrtajCestice() {
	for (int j = 0; j < cestice.size(); j++) {
		nacrtajCesticu(cestice.at(j));
	}
}

void nacrtajCesticu(Cestica c) {

	glColor3f(c.r, c.g, c.b);
	glTranslatef(c.x, c.y, c.z);
	glRotatef(c.kut, c.osX, c.osY, c.osZ);
	glBegin(GL_QUADS);

	glTexCoord2d(0.0, 0.0); glVertex3f(-c.size, -c.size, 0.0);
	glTexCoord2d(1.0, 0.0); glVertex3f(-c.size, +c.size, 0.0);
	glTexCoord2d(1.0, 1.0); glVertex3f(+c.size, +c.size, 0.0);
	glTexCoord2d(0.0, 1.0); glVertex3f(+c.size, -c.size, 0.0);

	glEnd();
	glRotatef(-c.kut, c.osX, c.osY, c.osZ);
	glTranslatef(-c.x, -c.y, -c.z);
}

void myKeyboard(unsigned char theKey, int mouseX, int mouseY) {

	//pozicija izvora
	if (theKey == 'a') i.x -= 0.5;
	if (theKey == 'd') i.x += 0.5;
	if (theKey == 's') i.y -= 0.5;
	if (theKey == 'w') i.y += 0.5;

	// kolicina cestica
	if (theKey == 'q' && i.q > 0) i.q--;
	if (theKey == 'e' && i.q < 49) i.q++;

	// velicina cestica
	if (theKey == '+' && i.size < 4.0) i.size += 0.01;
	if (theKey == '-' && i.size > 0.02) i.size -= 0.01;

	// brzina cestica
	if (theKey == 'y') i.v+=0.1;
	if (theKey == 'x') i.v-=0.1;
}

void myMouse(int button, int state, int x, int y)
{
	// Wheel reports as button 3(scroll up) and button 4(scroll down)
	if ((button == 3) || (button == 4)) // It's a wheel event
	{
		// Each wheel event reports like a button click, GLUT_DOWN then GLUT_UP
		if (state == GLUT_UP) return; // Disregard redundant GLUT_UP events
		//udaljenost izvora
		if(button == 3)
			i.z += 0.5;
		else
			i.z -= 0.5;

	}
}

GLuint LoadTexture(const char * filename, int w, int h)
{
	GLuint texture;

	unsigned char* data;

	FILE* file;

	file = fopen(filename, "rb");

	if (file == NULL) return 0;
	int width = w;
	int height = h;
	data = (unsigned char *)malloc(width * height * 3);
	//int size = fseek(file,);
	fread(data, width * height * 3, 1, file);
	fclose(file);

	for (int i = 0; i < width * height; ++i)
	{
		int index = i * 3;
		unsigned char B, R;
		B = data[index];
		R = data[index + 2];

		data[index] = R;
		data[index + 2] = B;
	}

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);


	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);
	free(data);

	return texture;
}