#include <iostream>
#include <stdlib.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <math.h>
#include "CarregarArquivo2.cpp"
#define Pi 3.1415

using namespace std;

CarregarArquivo car;

GLubyte matrixTextureFlo[256][256][3], matrixTextureCar[256][256][3];
GLuint texturaIdFloor, texturaIdCar;

float XRotation = 0, YRotation = 0, ZRotation = 0;
float traX = 0.0, traZ = 0.0;
int camera = 0;
bool meteor = true;

float numberOfMeteors = 500;

float meteorPositionX[500];
float meteorPositionY[500];
float meteorPositionZ[500];

int slices[500], stacks[500];

void DefineIluminacao(void)
{
    GLfloat luzAmbiente[4] = {0.1, 0.1, 0.1, 1.0};
    GLfloat luzDifusa[4] = {0.7, 0.7, 0.7, 1.0};    // "cor"
    GLfloat luzEspecular[4] = {1.0, 1.0, 1.0, 1.0}; // "brilho"
    GLfloat posicaoLuz[4] = {0.0, 0.0, 13.0, 1.0};
    GLfloat especularidade[4] = {1.0, 1.0, 1.0, 1.0};
    GLint especMaterial = 60;

    glMaterialfv(GL_FRONT, GL_SPECULAR, especularidade);
    glMateriali(GL_FRONT, GL_SHININESS, especMaterial);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, luzAmbiente);
    glLightfv(GL_LIGHT0, GL_AMBIENT, luzAmbiente);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, luzDifusa);
    glLightfv(GL_LIGHT0, GL_SPECULAR, luzEspecular);
    glLightfv(GL_LIGHT0, GL_POSITION, posicaoLuz);
}

void SpecialKeys(int tecla, int x, int y)
{
    switch (tecla)
    {
    case GLUT_KEY_LEFT:
        YRotation = YRotation - 5;
        break;
    case GLUT_KEY_RIGHT:
        YRotation = YRotation + 5;
        break;
    case GLUT_KEY_UP:
        XRotation = XRotation + 5;
        break;
    case GLUT_KEY_DOWN:
        XRotation = XRotation - 5;
        break;
    case GLUT_KEY_HOME:
        ZRotation = ZRotation + 5;
        break;
    case GLUT_KEY_END:
        ZRotation = ZRotation - 5;
        break;
    }
}

void keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 'a':
        YRotation += 5;
        break;
    case 'd':
        YRotation -= 5;
        break;
    case 'w':
        traX += sin(YRotation * Pi / 180);
        traZ += cos(YRotation * Pi / 180);
        break;
    case 's':
        traX -= sin(YRotation * Pi / 180);
        traZ -= cos(YRotation * Pi / 180);
        break;
    case 'c':
        if (camera < 2)
            camera = camera + 1;
        else
            camera = 0;
        break;
    case 'm':
        meteor = !meteor;
        break;
    default:
        break;
    }
}

void Initialize(void)
{
    glClearColor(1.0, 1.0, 1.0, 0.0);

    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(65, 1, 0.5, 500);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    car.Carregar("Koenigsegg.obj");

    try
    {
        ifstream arq("asfalto.bmp", ios::binary);
        char c;
        if (!arq)
            cout << "Erro ao abrir";

        for (int i = 0; i < 54; i++)
            c = arq.get();
        for (int i = 0; i < 256; i++)
            for (int j = 0; j < 256; j++)
            {
                c = arq.get();
                matrixTextureFlo[i][j][2] = c;
                c = arq.get();
                matrixTextureFlo[i][j][1] = c;
                c = arq.get();
                matrixTextureFlo[i][j][0] = c;
            }

        arq.close();
        arq.clear();
    }
    catch (...)
    {
        cout << "Erro ao ler imagem" << endl;
    }

    glGenTextures(1, &texturaIdFloor);

    glBindTexture(GL_TEXTURE_2D, texturaIdFloor);

    glTexImage2D(GL_TEXTURE_2D, 0, 3, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, matrixTextureFlo);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    try
    {
        ifstream arq("quadriculado.bmp", ios::binary);
        char c;
        if (!arq)
            cout << "Erro ao abrir";

        for (int i = 0; i < 54; i++)
            c = arq.get();
        for (int i = 0; i < 256; i++)
            for (int j = 0; j < 256; j++)
            {
                c = arq.get();
                matrixTextureCar[i][j][2] = c;
                c = arq.get();
                matrixTextureCar[i][j][1] = c;
                c = arq.get();
                matrixTextureCar[i][j][0] = c;
            }

        arq.close();
        arq.clear();
    }
    catch (...)
    {
        cout << "Erro ao ler imagem" << endl;
    }

    glGenTextures(2, &texturaIdCar);
    printf("%d", texturaIdFloor);

    glBindTexture(GL_TEXTURE_2D, texturaIdCar);

    glTexImage2D(GL_TEXTURE_2D, 0, 3, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, matrixTextureCar);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    //Initializendo a posição dos meteors
    for (int i = 0; i < numberOfMeteors; i++)
    {
        meteorPositionX[i] = -5;
        meteorPositionY[i] = -1;
        meteorPositionZ[i] = -5;
        slices[i] = rand() % 20;
        stacks[i] = rand() % 20;
    }
}

void Floor(float escala, float altura)
{
    glColor3f(0.0, 1.0, 0.0);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texturaIdFloor);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glPushMatrix();
    glTranslatef(0.0, altura, 0.0);
    glScalef(escala, escala, escala);
    glBegin(GL_POLYGON);
    glNormal3f(0, -1, 0);
    glTexCoord2f(0, 0);
    glVertex3f(100.0, 0.0, 100.0);
    glTexCoord2f(10, 0);
    glVertex3f(-100.0, 0.0, 100.0);
    glTexCoord2f(10, 10);
    glVertex3f(-100.0, 0.0, -100.0);
    glTexCoord2f(0, 10);
    glVertex3f(100.0, 0.0, -100.0);
    glEnd();
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

void carSolid(void)
{
    glPushMatrix();

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texturaIdCar);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    for (unsigned int j = 0; j < (car.faces).size(); ++j)
    {
        glBegin(GL_POLYGON);
        for (unsigned int i = 0; i < (car.faces[j]).size(); ++i)
        {
            GLfloat vert[3] = {(car.vertices[car.faces[j][i][0]][0]), (car.vertices[car.faces[j][i][0]][1]), (car.vertices[car.faces[j][i][0]][2])};
            GLfloat nor[3] = {(car.normais[car.faces[j][i][2]][0]), (car.normais[car.faces[j][i][2]][1]), (car.normais[car.faces[j][i][2]][2])};
            GLfloat tex[2] = {car.texturas[car.faces[j][i][1]][0], car.texturas[car.faces[j][i][1]][1]};
            glNormal3fv(nor);
            glTexCoord2fv(tex);
            glVertex3fv(vert);
        }
        glEnd();
    }

    glPopMatrix();
}

void Timer(int value)
{
    glutTimerFunc(50, Timer, value);
    glutPostRedisplay();
}

void fallMeteor()
{

    for (int i = 0; i < numberOfMeteors; i++)
    {
        if (meteor)
        {
            if (meteorPositionY[i] > -5)
                meteorPositionY[i] = meteorPositionY[i] - 0.5;
            else
            {
                meteorPositionX[i] = (rand() % 100) + 1 - (rand() % 100);
                meteorPositionZ[i] = (rand() % 100) + 1 - (rand() % 100);
                meteorPositionY[i] = (rand() % 50) + 50;
                slices[i] = rand() % 20;
                stacks[i] = rand() % 20;
            }
        }
        else
        {
            meteorPositionX[i] = 0;
            meteorPositionZ[i] = 0;
            meteorPositionY[i] = -5;
        }
    }
}

void drawMeteor(float x, float y, float z, int positionMeteor)
{
    glPushMatrix();
    glColor3f(1.0, 0.0, 0.0);
    glTranslatef(x, y, z);
    glutSolidSphere(1, slices[positionMeteor], stacks[positionMeteor]);
    glPopMatrix();
}

void Draw(void)
{
    fallMeteor();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    if (camera == 0)
    {
        gluLookAt(0, 10, 13, 0, 0, 0, 0, 1, 0);
    }
    else if (camera == 1)
    {
        gluLookAt(traX - 10 * sin(YRotation * Pi / 180), 10, traZ - 10 * cos(YRotation * Pi / 180), traX, 0, traZ, 0, 1, 0);
    }
    else if (camera == 2)
    {
        gluLookAt(traX - 30 * sin(YRotation * Pi / 180), 10, traZ - 30 * cos(YRotation * Pi / 180), traX, 0, traZ, 0, 1, 0);
    }

    DefineIluminacao();
    Floor(1.0, -3.0);

    for (int i = 0; i < numberOfMeteors; i++)
    {
        drawMeteor(meteorPositionX[i], meteorPositionY[i], meteorPositionZ[i], i);
    }

    glPushMatrix();
    glTranslatef(traX, 0, traZ);
    glRotatef(XRotation, 1, 0, 0);
    glRotatef(YRotation, 0, 1, 0);
    glColor3f(0.0, 1.0, 0.0);
    glScalef(2.0, 2.0, 2.0);
    carSolid();
    glPopMatrix();

    glutSwapBuffers();
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);

    glutInitWindowSize(800, 800);

    glutInitWindowPosition(0, 0);

    glutCreateWindow("Computa��o Gr�fica: car");

    Initialize();
    Timer(0);
    glutDisplayFunc(Draw);

    glutKeyboardFunc(keyboard);

    glutSpecialFunc(SpecialKeys);

    glutMainLoop();

    return 0;
}
