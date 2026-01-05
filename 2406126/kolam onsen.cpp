#include <GL/freeglut.h>
#include <GL/glu.h> 
#include <cmath>
#include <iostream>

// ---------------------------------- konstanta
const float PI = 3.14159265f;
const float MOVE_SPEED = 5.0f;
const float MOUSE_SENSITIVITY = 0.1f;

// ---------------------------------- variabel kamera
float cameraPosX = 0.0f, cameraPosY = 10.0f, cameraPosZ = 45.0f;
float cameraYaw = 0.0f;
float cameraPitch = -20.0f;

int windowWidth = 1200, windowHeight = 800;
bool mouseCaptured = false;
bool keys[256] = {false};
float deltaTime = 0.0f;
float lastFrameTime = 0.0f;

// ==========================================
// ASET ONSEN (VARIABLE GLOBAL)
// ==========================================
#define MAX_ROCKS 80 
float rockX[MAX_ROCKS], rockY[MAX_ROCKS], rockZ[MAX_ROCKS];
float rockScale[MAX_ROCKS], rockRot[MAX_ROCKS];
int rockType[MAX_ROCKS]; 

float poolSizeX = 21.0f;
float poolSizeZ = 16.0f;

void initRocks() {
    srand(1234);
    for(int i = 0; i < MAX_ROCKS; i++) {
        float side = (float)i / MAX_ROCKS;
        float x, z;
        if (side < 0.25f) { x = -poolSizeX + (side * 4.0f) * (poolSizeX * 2.0f); z = poolSizeZ; }
        else if (side < 0.5f) { x = poolSizeX; z = poolSizeZ - ((side - 0.25f) * 4.0f) * (poolSizeZ * 2.0f); }
        else if (side < 0.75f) { x = poolSizeX - ((side - 0.5f) * 4.0f) * (poolSizeX * 2.0f); z = -poolSizeZ; }
        else { x = -poolSizeX; z = -poolSizeZ + ((side - 0.75f) * 4.0f) * (poolSizeZ * 2.0f); }
        rockX[i] = x; rockZ[i] = z; rockY[i] = 0.0f;
        rockScale[i] = 1.8f + (rand() % 100) / 80.0f; 
        rockRot[i] = rand() % 360;
        rockType[i] = rand() % 2;
    }
}

// ---------------------------------- inisialisasi OpenGL
void init() {
    glClearColor(0.05f, 0.05f, 0.1f, 1.0f); // Background lebih gelap agar lampu terlihat
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glShadeModel(GL_SMOOTH);

    // --- LIGHT 0: Cahaya Global (Matahari/Bulan) ---
    // Baris 60-66: Mengatur intensitas cahaya lingkungan
    GLfloat light0_pos[] = { 0.0f, 50.0f, 0.0f, 0.0f }; 
    GLfloat light0_diffuse[] = { 0.3f, 0.3f, 0.4f, 1.0f }; // Kebiruan redup
    GLfloat light0_ambient[] = { 0.1f, 0.1f, 0.15f, 1.0f };
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, light0_pos);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  light0_diffuse);
    glLightfv(GL_LIGHT0, GL_AMBIENT,  light0_ambient);

    // --- LIGHT 1: Lampu Hangat (Lampion/Spotlight Onsen) ---
    // Baris 71-84: Mengatur lampu kuning di tengah area
    GLfloat light1_pos[] = { 0.0f, 10.0f, 0.0f, 1.0f }; 
    GLfloat light1_diffuse[] = { 1.0f, 0.7f, 0.3f, 1.0f }; // Warna orange hangat
    GLfloat light1_specular[] = { 1.0f, 0.9f, 0.5f, 1.0f };
    glEnable(GL_LIGHT1);
    glLightfv(GL_LIGHT1, GL_POSITION, light1_pos);
    glLightfv(GL_LIGHT1, GL_DIFFUSE,  light1_diffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);

    // Atenuasi agar cahaya meredup alami (Baris 80-82)
    glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 1.0f);
    glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.02f);
    glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.005f);

    // --- Material Highlights ---
    GLfloat mat_specular[] = { 0.6f, 0.6f, 0.6f, 1.0f };
    GLfloat mat_shininess[] = { 50.0f };
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

    initRocks(); 
}

// ==========================================
// FUNGSI GAMBAR ASET
// ==========================================

void drawRocks() {
    glColor3f(0.4f, 0.4f, 0.43f); 
    for(int i = 0; i < MAX_ROCKS; i++) {
        glPushMatrix();
        glTranslatef(rockX[i], rockY[i], rockZ[i]);
        glRotatef(rockRot[i], 0, 1, 0);
        glScalef(rockScale[i], rockScale[i], rockScale[i]); 
        if(rockType[i] == 0) glutSolidDodecahedron();
        else { glScalef(1.1f, 1.1f, 1.1f); glutSolidIcosahedron(); }
        glPopMatrix();
    }
}

void drawOnsenPool() {
    glColor3f(0.05f, 0.55f, 0.45f);
    glBegin(GL_QUADS);
        glVertex3f(-poolSizeX, 0.1f, -poolSizeZ);
        glVertex3f( poolSizeX, 0.1f, -poolSizeZ);
        glVertex3f( poolSizeX, 0.1f,  poolSizeZ);
        glVertex3f(-poolSizeX, 0.1f,  poolSizeZ);
    glEnd();
}

void drawBambooSpout() {
    glPushMatrix();
        glColor3f(0.5f, 0.6f, 0.3f);
        glTranslatef(0.0f, 1.8f, -poolSizeZ - 1.5f);
        glRotatef(-20.0f, 1, 0, 0); 
        GLUquadric* q = gluNewQuadric();
        gluCylinder(q, 0.6, 0.6, 9.0, 16, 1);
        glPushMatrix();
            glTranslatef(0, 0, 8.5f);
            glRotatef(20.0f, 1, 0, 0); 
            glDisable(GL_LIGHTING);
            glColor4f(0.7f, 0.9f, 1.0f, 0.8f);
            glLineWidth(6.0f);
            glBegin(GL_LINES); glVertex3f(0, 0, 0); glVertex3f(0, -5.0f, 0); glEnd();
            glEnable(GL_LIGHTING);
            glTranslatef(0, -4.8f, 0);
            glColor3f(0.8f, 0.95f, 1.0f);
            glutSolidSphere(0.6, 12, 12);
        glPopMatrix();
        gluDeleteQuadric(q);
    glPopMatrix();
}

void drawFloor() {
    glColor3f(0.15f, 0.15f, 0.18f); // Lantai dibuat lebih gelap agar kontras lampu terasa
    glBegin(GL_QUADS);
    glVertex3f(-150, -0.05f, -150); glVertex3f(150, -0.05f, -150);
    glVertex3f(150, -0.05f, 150); glVertex3f(-150, -0.05f, 150);
    glEnd();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    gluPerspective(60.0, (double)windowWidth / windowHeight, 0.1, 500.0);
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
    
    float yR = cameraYaw * PI / 180.0f, pR = cameraPitch * PI / 180.0f;
    float lx = cameraPosX + sin(yR)*cos(pR);
    float ly = cameraPosY + sin(pR);
    float lz = cameraPosZ - cos(yR)*cos(pR);
    gluLookAt(cameraPosX, cameraPosY, cameraPosZ, lx, ly, lz, 0, 1, 0);

    // --- Update Posisi Lampu 1 (Lampu Hangat) ---
    // Baris 158: Memastikan posisi lampu tetap stabil di koordinat (0,10,0)
    GLfloat light1_pos[] = { 0.0f, 10.0f, 0.0f, 1.0f }; 
    glLightfv(GL_LIGHT1, GL_POSITION, light1_pos);

    drawFloor();
    drawOnsenPool();
    drawRocks();
    drawBambooSpout();

    glutSwapBuffers();
}

// ---------------------------------- Sistem Navigasi & Timer
void updateCameraMovement() {
    float yawRad = cameraYaw * PI / 180.0f;
    float pitchRad = cameraPitch * PI / 180.0f;
    float fx = sin(yawRad) * cos(pitchRad), fy = sin(pitchRad), fz = -cos(yawRad) * cos(pitchRad);
    float speed = MOVE_SPEED * deltaTime;
    if (keys['w']) { cameraPosX += fx * speed; cameraPosY += fy * speed; cameraPosZ += fz * speed; }
    if (keys['s']) { cameraPosX -= fx * speed; cameraPosZ -= fz * speed; }
    if (keys['a']) { cameraPosX -= cos(yawRad) * speed; cameraPosZ -= sin(yawRad) * speed; }
    if (keys['d']) { cameraPosX += cos(yawRad) * speed; cameraPosZ += sin(yawRad) * speed; }
    if (keys['e']) cameraPosY += speed; if (keys['q']) cameraPosY -= speed;
}

void mouseMotion(int x, int y) {
    if (!mouseCaptured) return;
    cameraYaw += (x - windowWidth/2) * MOUSE_SENSITIVITY;
    cameraPitch -= (y - windowHeight/2) * MOUSE_SENSITIVITY;
    if (cameraPitch > 89.0f) cameraPitch = 89.0f; if (cameraPitch < -89.0f) cameraPitch = -89.0f;
    glutWarpPointer(windowWidth/2, windowHeight/2);
}

void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        mouseCaptured = true; glutSetCursor(GLUT_CURSOR_NONE);
    }
}

void keyboard(unsigned char key, int x, int y) {
    keys[tolower(key)] = true; if (key == 27) exit(0);
}

void keyboardUp(unsigned char key, int x, int y) { keys[tolower(key)] = false; }

void timer(int value) {
    float currentTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
    deltaTime = currentTime - lastFrameTime; lastFrameTime = currentTime;
    updateCameraMovement(); glutPostRedisplay(); glutTimerFunc(16, timer, 0);
}

void reshape(int w, int h) { windowWidth = w; windowHeight = h; glViewport(0, 0, w, h); }

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("Onsen Night Lighting");
    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardUp);
    glutPassiveMotionFunc(mouseMotion);
    glutMouseFunc(mouse);
    glutTimerFunc(0, timer, 0);
    glutMainLoop();
    return 0;
}