#define _USE_MATH_DEFINES

#include <GL/glut.h>
#include <glm/glm/vec3.hpp> // glm::vec3
#include <glm/glm/vec4.hpp> // glm::vec4
#include <glm/glm/mat4x4.hpp> // glm::mat4
#include <glm/glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <cmath>

int widthW = 500;
int heightW = 500;
int currentTime = 0; int previousTime = 0;

glm::vec3 srediste;

class Vrh
{
public:
	glm::vec3 v;
	Vrh(float x, float y, float z)
	{
		v.x = x; v.y = y; v.z = z;
	}
};

class Poligon
{
public:
	Vrh *v1, *v2, *v3;
	Poligon(Vrh *v11, Vrh *v21, Vrh *v31)
	{
		v1 = v11; v2 = v21; v3 = v31;
	}
};



class Putanja
{
public:
	std::vector<Vrh> vrhoviR;
	glm::mat4x4 B;
	glm::mat4x3 Bderivirano;
	glm::vec3 polozajP;
	std::vector<glm::vec3> tockePuta;

	Putanja(std::vector<Vrh> R)
	{
		B = glm::mat4x4(-1, 3, -3, 1, 3, -6, 0, 4, -3, 3, 3, 1, 1, 0, 0, 0);
		Bderivirano = glm::mat4x3(-1, 2, -1, 3, -4, 0, -3, 2, 1, 1, 0, 0);
		vrhoviR = R;
		//???
		polozajP = izracunajPolozajP(0, 1);
		tockePuta = izracunajSveTockePuta();
	}

	std::vector <glm::vec3> izracunajSveTockePuta()
	{
		std::vector<glm::vec3> vrhoviPuta;

		for (unsigned int i = 1; i < vrhoviR.size() - 2; i++)
		{
			for (float t = 0.00f; t < 1.00f; t += 0.01f)
			{
				glm::vec3 pom = izracunajPolozajP(t, i);
				vrhoviPuta.push_back(pom);
			}
		}
		return vrhoviPuta;
	}

	//i pocinje od 1 do n - 2
	// za jedan i proci po svim t-ovima od 0 do 1 po 0.01
	glm::vec3 izracunajPolozajP(float t, int i)
	{
		//neki od ovih vektora ide transponirano
		glm::vec4 vecT = glm::vec4(t*t*t, t*t, t, 1);
		glm::mat4x4 vecR = glm::mat3x4(	vrhoviR[i - 1].v.x, vrhoviR[i].v.x, vrhoviR[i + 1].v.x, vrhoviR[i + 2].v.x,
										vrhoviR[i - 1].v.y, vrhoviR[i].v.y, vrhoviR[i + 1].v.y, vrhoviR[i + 2].v.y,
										vrhoviR[i - 1].v.z, vrhoviR[i].v.z, vrhoviR[i + 1].v.z, vrhoviR[i + 2].v.z);

		glm::vec3 a = vecT * 0.1666666f * B * vecR;
		return a;
	}

	//ne pokriva slucaj kada je segment krivulje ravan
	glm::vec3 izracunajDeriviraniP(float t, int i)
	{
		glm::vec3 vecT = glm::vec3(t*t, t, 1);
		glm::mat4x4 vecR = glm::mat3x4(vrhoviR[i - 1].v.x, vrhoviR[i].v.x, vrhoviR[i + 1].v.x, vrhoviR[i + 2].v.x,
			vrhoviR[i - 1].v.y, vrhoviR[i].v.y, vrhoviR[i + 1].v.y, vrhoviR[i + 2].v.y,
			vrhoviR[i - 1].v.z, vrhoviR[i].v.z, vrhoviR[i + 1].v.z, vrhoviR[i + 2].v.z);

		return vecT * 0.5f * Bderivirano * vecR;
	}

	glm::vec3 izracunajDerivirani2P(float t, int i)
	{
		glm::vec4 vecDerv = glm::vec4(1 - t, 3 * t - 2, 1 - 3 * t, t);
		glm::mat4x4 vecR = glm::mat3x4(vrhoviR[i - 1].v.x, vrhoviR[i].v.x, vrhoviR[i + 1].v.x, vrhoviR[i + 2].v.x,
			vrhoviR[i - 1].v.y, vrhoviR[i].v.y, vrhoviR[i + 1].v.y, vrhoviR[i + 2].v.y,
			vrhoviR[i - 1].v.z, vrhoviR[i].v.z, vrhoviR[i + 1].v.z, vrhoviR[i + 2].v.z);

		return vecDerv * vecR;
	}
	
};

class Objekt
{
public:
	std::vector<Poligon> poligoni;
	glm::vec3 pocetnaRotacija;
	glm::vec3 trenutnaRotacija;
	glm::vec3 pocetniPolozaj;
	glm::vec3 trenutniPolozaj;

	Objekt(glm::vec3 pocRot, glm::vec3 pocPol, std::vector<Poligon> poly)
	{
		pocetnaRotacija = pocRot;
		pocetniPolozaj = pocPol;
		trenutnaRotacija = pocRot;
		trenutniPolozaj = pocPol;
		poligoni = poly;
	}

	glm::vec3 IzracunajTrenutnuPoziciju(Putanja put, int i, float t)
	{
		trenutniPolozaj = put.izracunajPolozajP(t, i);
		return trenutniPolozaj;
	}

	void SetPoligoni(std::vector<Poligon> noviPoligoni)
	{
		poligoni = noviPoligoni;
	}

};

Objekt* objekt;
Putanja* putanja;

Objekt* readObj(std::string filePath, Putanja put)
{
	glm::vec3 sumVector = glm::vec3(0,0,0);
	std::ifstream ifs(filePath);
	std::string tmp;
	std::vector<std::string> vrhoviStr;
	std::vector<std::string> poligoniStr;
	while(std::getline(ifs, tmp))
	{
		if(tmp[0] == 'f')
		{
			poligoniStr.push_back(tmp);
		}
		else if (tmp[0] == 'v')
		{
			vrhoviStr.push_back(tmp);
			float x, y, z;
			sscanf_s(tmp.c_str(), "v %f %f %f", &x, &y, &z);
			sumVector += glm::vec3(x, y, z);
		}
	}
	std::vector<Poligon> poligoni;
	int v1, v2, v3;
	float x, y, z;
	for(unsigned int i = 0; i < poligoniStr.size(); i++)
	{
		sscanf_s(poligoniStr[i].c_str(), "f %d %d %d", &v1, &v2, &v3);

		sscanf_s(vrhoviStr[v1-1].c_str(), "v %f %f %f", &x, &y, &z);
		Vrh* vrh1 = new Vrh(x, y, z);
		sscanf_s(vrhoviStr[v2-1].c_str(), "v %f %f %f", &x, &y, &z);
		Vrh* vrh2 = new Vrh(x, y, z);
		sscanf_s(vrhoviStr[v3-1].c_str(), "v %f %f %f", &x, &y, &z);
		Vrh* vrh3 = new Vrh(x, y, z);

		Poligon p = Poligon( vrh1, vrh2, vrh3);
		poligoni.push_back(p);
	}

	sumVector /= vrhoviStr.size();
	srediste = sumVector;
	Objekt* obj = new Objekt(glm::vec3(0,0,1), sumVector, poligoni);


	return obj;
}

std::vector<Vrh> readPutanja(std::string filePath)
{
	std::ifstream ifs(filePath);
	std::string tmp;
	float x, y, z;
	std::vector<Vrh> vrhovi;
	while (std::getline(ifs, tmp))
	{
		sscanf_s(tmp.c_str(), "%f %f %f", &x, &y, &z);
		Vrh vrh = Vrh(x, y, z);
		vrhovi.push_back(vrh);
	}
	return vrhovi;
}

void renderPath()
{
	glBegin(GL_POINTS);
	for(unsigned int i = 0; i < putanja->tockePuta.size(); i++)
	{
		glVertex3f(putanja->tockePuta[i].x / 30, putanja->tockePuta[i].y / 30, putanja->tockePuta[i].z /30);
	}
	glEnd();
	
}

void renderObject(Objekt obj, Putanja put, int j, float t)
{
	glColor3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_TRIANGLES);
	//glm::vec3 pomak = (obj.IzracunajTrenutnuPoziciju(put, j, t) - obj.IzracunajTrenutnuPoziciju(put, 1, 0.0f));
	glm::vec3 pomak = put.izracunajPolozajP(t, j) - srediste; //- put.izracunajPolozajP(0.0f,1);


	for(unsigned int i = 0; i < obj.poligoni.size(); i++)
	{
		glm::vec3 v1 = obj.poligoni[i].v1->v/ 5.0f + pomak/30.0f;
		glm::vec3 v2 = obj.poligoni[i].v2->v/ 5.0f + pomak/30.0f;
		glm::vec3 v3 = obj.poligoni[i].v3->v/ 5.0f + pomak/30.0f;

		glVertex3f(v1.x, v1.y, v1.z);
		glVertex3f(v2.x, v2.y, v2.z);
		glVertex3f(v3.x, v3.y, v3.z);
	}
	glEnd();
}

float vectorLength(glm::vec3 v)
{
	return sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
}

float findAngle(glm::vec3 v1, glm::vec3 v2)
{
	return acos(glm::dot(v1,v2) / (vectorLength(v1)* vectorLength(v2))) * 180/M_PI;
}

float t = 0.0f;
int i = 1;


void setup()
{
	std::string filename2 = R"(C:\Users\Antun Mesar\Desktop\Grafika objekti\Bspline.txt)";
	std::vector<Vrh> pToc = readPutanja(filename2);

	putanja = new Putanja(pToc);

	std::string filename = R"(C:\Users\Antun Mesar\Desktop\Grafika objekti\747.obj)";
	objekt = readObj(filename, *putanja);


	// ugasi ako se koristi display()
	std::vector<Poligon> noviPoly;
	srediste = glm::vec3(0, 0, 0);
	for(unsigned int i = 0; i < objekt->poligoni.size(); i++)
	{
		glm::vec3 v1 = objekt->poligoni[i].v1->v;// *inverse(R);
		glm::vec3 v2 = objekt->poligoni[i].v2->v;// *inverse(R);
		glm::vec3 v3 = objekt->poligoni[i].v3->v;// * inverse(R);
		
		Vrh* vrh1 = new Vrh(v1.z, v1.y, v1.x);
		Vrh* vrh2 = new Vrh(v2.z, v2.y, v2.x);
		Vrh* vrh3 = new Vrh(v3.z, v3.y, v3.x);
		
		srediste += v1 + v2 + v3;

		Poligon p = Poligon(vrh1, vrh2, vrh3);
		noviPoly.push_back(p);
	}
	srediste /= float(3 * objekt->poligoni.size());
	objekt->SetPoligoni(noviPoly);
	
	
}

int brojac = 0;
void display(void)
{
	//glPushMatrix();

	glm::vec3 pocetnaRotacija = glm::vec3(0, 1, 1);
	glm::vec3 ciljnaRotacija = putanja->izracunajDeriviraniP(t, i);
	

	float fi = findAngle(pocetnaRotacija, ciljnaRotacija);
	glm::vec3 os = glm::cross(pocetnaRotacija, ciljnaRotacija);

	//glRotatef(fi, os.x, os.y, os.z);

	

	glRotatef(fi, os.x, os.y, os.z);

	//pom
	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_POLYGON);
	glVertex3f(srediste.x - 0.01, srediste.y - 0.01, srediste.z);
	glVertex3f(srediste.x + 0.01, srediste.y - 0.01, srediste.z);
	glVertex3f(srediste.x - 0.01, srediste.y + 0.01, srediste.z);
	glVertex3f(srediste.x + 0.01, srediste.y + 0.01, srediste.z);
	glEnd();
	printf("srediste je %f, %f, %f", srediste.x, srediste.y, srediste.z);

	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(0.0, 1.0, 1.0);
	renderPath();

	//iscrtavanje tangente
	glm::vec3 tangenta = putanja->izracunajDeriviraniP(t, i);
	glColor3f(0.0, 1.0, 0.0);
	glm::vec3 start = putanja->izracunajPolozajP(t, i);
	glm::vec3 end = start + tangenta;
	glBegin(GL_LINES);
	glVertex3f(start.x / 30, start.y / 30, start.z / 30);
	glVertex3f(end.x / 30, end.y / 30, end.z / 30);
	glEnd();

	glColor3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_TRIANGLES);
	for (unsigned int j = 0; j < objekt->poligoni.size(); j++)
	{
		//glm::vec3 v1 = objekt->poligoni[i].v1->v / 5.0f + (putanja->tockePuta[brojac] - srediste) / 30.0f;
		//glm::vec3 v2 = objekt->poligoni[i].v2->v / 5.0f + (putanja->tockePuta[brojac] - srediste) / 30.0f;
		//glm::vec3 v3 = objekt->poligoni[i].v3->v / 5.0f + (putanja->tockePuta[brojac] - srediste) / 30.0f;


		glm::vec3 v1 = ((objekt->poligoni[j].v1->v - srediste)) / 5.0f + (putanja->izracunajPolozajP(t, i)) / 30.0f;
		glm::vec3 v2 = ((objekt->poligoni[j].v2->v - srediste)) / 5.0f + (putanja->izracunajPolozajP(t, i)) / 30.0f;
		glm::vec3 v3 = ((objekt->poligoni[j].v3->v - srediste)) / 5.0f + (putanja->izracunajPolozajP(t, i)) / 30.0f;

		glVertex3f(v1.x, v1.y, v1.z);
		glVertex3f(v2.x, v2.y, v2.z);
		glVertex3f(v3.x, v3.y, v3.z);
	}
	glEnd();
	brojac += 1;
	glRotatef(-fi, os.x, os.y, os.z);

	printf("%d, %f\n", i, t);
	t += 0.1;
	if (t >= 1.0f)
	{
		i++;
		t = 0.0f;
	}

	if (i >= putanja->vrhoviR.size() - 2)
	{
		i = 1;
		t = 0.0f;
		brojac = 0;
	}
	if(brojac >= putanja->tockePuta.size())
	{
		brojac = 0;
	}
	glFlush();
}

void display2(void)
{
	//glRotatef(45, 0, 1, 0);
	//srediste
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_POLYGON);
	glVertex3f(srediste.x - 0.01, srediste.y - 0.01, srediste.z);
	glVertex3f(srediste.x + 0.01, srediste.y - 0.01, srediste.z);
	glVertex3f(srediste.x - 0.01, srediste.y + 0.01, srediste.z);
	glVertex3f(srediste.x + 0.01, srediste.y + 0.01, srediste.z);
	glEnd();
	//printf("srediste je %f, %f, %f", srediste.x, srediste.y, srediste.z);

	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(0.0, 1.0, 1.0);
	renderPath();

	//iscrtavanje tangente
	glm::vec3 tangenta = normalize(putanja->izracunajDeriviraniP(t, i));
	glColor3f(0.0, 1.0, 0.0);
	glm::vec3 start = putanja->izracunajPolozajP(t, i);
	glm::vec3 end = start + 5.0f * tangenta;
	glBegin(GL_LINES);
	glVertex3f(start.x / 30, start.y / 30, start.z / 30);
	glVertex3f(end.x / 30, end.y / 30, end.z / 30);
	glEnd();

	// iscrtavanje noramle
	glm::vec3 normala = normalize(glm::cross(tangenta, putanja->izracunajDerivirani2P(t, i)));
	glColor3f(0.0, 0.0, 1.0);
	glm::vec3 startN = putanja->izracunajPolozajP(t, i);
	glm::vec3 endN = start + 5.0f * normala;
	glBegin(GL_LINES);
	glVertex3f(startN.x / 30, startN.y / 30, startN.z / 30);
	glVertex3f(endN.x / 30, endN.y / 30, endN.z / 30);
	glEnd();

	// iscrtavanje binoramle
	glm::vec3 binormala = normalize(glm::cross(tangenta, normala));
	glColor3f(1.0, 0.0, 0.0);
	glm::vec3 startB = putanja->izracunajPolozajP(t, i);
	glm::vec3 endB = start + 3.0f * binormala;
	glBegin(GL_LINES);
	glVertex3f(startB.x / 30, startB.y / 30, startB.z / 30);
	glVertex3f(endB.x / 30, endB.y / 30, endB.z / 30);
	glEnd();
	
	//rotacijska matrica R
	glm::mat3x3 R = glm::mat3x3(tangenta, normala, binormala);
	
	//iscrtavanje objekta
	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_TRIANGLES);
	for (unsigned int j = 0; j < objekt->poligoni.size(); j++)
	{


		glm::vec3 v1 = ((objekt->poligoni[j].v1->v - srediste)*inverse(R)) / 5.0f +(putanja->izracunajPolozajP(t, i)) / 30.0f;
		glm::vec3 v2 = ((objekt->poligoni[j].v2->v - srediste)*inverse(R)) / 5.0f +(putanja->izracunajPolozajP(t, i) ) / 30.0f;
		glm::vec3 v3 = ((objekt->poligoni[j].v3->v - srediste)*inverse(R)) / 5.0f +(putanja->izracunajPolozajP(t, i) ) / 30.0f;

		glVertex3f(v1.x, v1.y, v1.z);
		glVertex3f(v2.x, v2.y, v2.z);
		glVertex3f(v3.x, v3.y, v3.z);
	}
	glEnd();
	brojac += 1;

	t += 0.1;
	if (t >= 1.0f)
	{
		i++;
		t = 0.0f;
	}

	if (i >= putanja->vrhoviR.size() - 2)
	{
		i = 1;
		t = 0.0f;
		brojac = 0;
	}

	glFlush();
}


void idle() {
	currentTime = glutGet(GLUT_ELAPSED_TIME);
	int timeInterval = currentTime - previousTime;
	//printf("%d\n", timeInterval);
	if (timeInterval > 2) {
		display2();
		previousTime = currentTime;
	}
}

int main(int argc, char** argv)
{
	setup();
	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(widthW, heightW);
	glutInitWindowPosition(100, 100);
	glutCreateWindow(" Test ");
	gluLookAt(1, 0, 1, 0, 0, 0, 0, 1, 0);
	glutDisplayFunc(display2);
	glutIdleFunc(idle);
	glutMainLoop();
	return 0;
}