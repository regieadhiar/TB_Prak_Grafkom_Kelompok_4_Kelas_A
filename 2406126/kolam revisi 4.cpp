// ---------------------------------- library
#include <GL/freeglut.h>
#include <cmath>
#include <iostream>

// ---------------------------------- konstanta
const float PI = 3.14159265f;
const float MOVE_SPEED = 5.0f;          // unit per detik
const float MOUSE_SENSITIVITY = 0.1f;   // sensitivitas mouse

// ---------------------------------- variabel kamera
float cameraPosX = 0.0f, cameraPosY = 3.0f, cameraPosZ = 8.0f;
float cameraYaw = 0.0f;      // rotasi horizontal (derajat)
float cameraPitch = -20.0f; // rotasi vertikal (derajat)

int windowWidth = 1200, windowHeight = 800;
bool mouseCaptured = false;

bool keys[256] = {false};

float deltaTime = 0.0f;
float lastFrameTime = 0.0f;

// ---------------------------------- inisialisasi OpenGL
void init() {
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    
    // Expert Note: Color Material memungkinkan glColor mempengaruhi interaksi cahaya
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glShadeModel(GL_SMOOTH);

    // Directional light tetap dari atas tengah (seperti matahari)
    GLfloat light_direction[] = {0.0f, -10.5f, 0.0f, 0.0f}; // w=0 -> directional
    GLfloat light_diffuse[]   = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat light_ambient[]   = {0.3f, 0.3f, 0.3f, 1.0f};
    GLfloat light_specular[]  = {1.0f, 1.0f, 1.0f, 1.0f};

    glLightfv(GL_LIGHT0, GL_POSITION, light_direction);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);
    glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
}

// lampu toro
void drawToro(float x, float y, float z, float scale) {
    glPushMatrix();
        glTranslatef(x, y, z); 
        glScalef(scale, scale, scale);

        // Warna batu nya
        glColor3ub(105, 105, 105); 

        // KISO (Kaki)
        glPushMatrix();
            glScalef(1.0f, 0.2f, 1.0f); 
            glutSolidCube(1.5);
        glPopMatrix();

        // SAO (Tiang)
        glPushMatrix();
            glTranslatef(0.0f, 0.15f, 0.0f); 
            glRotatef(-90, 1.0f, 0.0f, 0.0f); 
            glutSolidCylinder(0.4, 1.5, 20, 20); 
        glPopMatrix();

        // CHUDAI (Alas Tengah)
        glPushMatrix();
            glTranslatef(0.0f, 1.6f, 0.0f); 
            glScalef(1.2f, 0.3f, 1.2f);
            glutSolidCube(1.2);
        glPopMatrix();

        // HIBUKURO (Bilik Api)
        glPushMatrix();
            glTranslatef(0.0f, 2.0f, 0.0f); 
            
            // Trik Expert: Menyalakan Emission
            GLfloat light_emission[] = { 1.0f, 0.6f, 0.2f, 1.0f }; // Oranye menyala
            glMaterialfv(GL_FRONT, GL_EMISSION, light_emission);
            
            glColor3ub(255, 165, 0); // Warna cadangan jika lighting off
            glScalef(0.6f, 0.6f, 0.6f);
            glutSolidCube(1.0); 

            // PENTING: Matikan emission agar objek lain tidak ikut menyala
            GLfloat no_emission[] = { 0.0f, 0.0f, 0.0f, 1.0f };
            glMaterialfv(GL_FRONT, GL_EMISSION, no_emission);
        glPopMatrix();

        // warna batu
        glColor3ub(105, 105, 105); 

        // KASA (Atap)
        glPushMatrix();
            glTranslatef(0.0f, 2.3f, 0.0f);
            glRotatef(-90, 1.0f, 0.0f, 0.0f); 
            glutSolidCone(1.3, 0.8, 6, 4); 
        glPopMatrix();

        // HOJU (bundelan di atas)
        glPushMatrix();
            glTranslatef(0.0f, 3.1f, 0.0f);
            glutSolidSphere(0.2, 10, 10);
        glPopMatrix();

    glPopMatrix();
}

void batu(int rockType = 0) {
    switch (rockType)
    {
    case 1:
        glColor3ub(112.0, 128.0, 144.0);
        glScalef(0.5f, 0.5f, 0.5f); //Scale
        glutSolidIcosahedron();
        break;
    default:
        glColor3ub(112.0, 128.0, 144.0);
        glScalef(0.25f, 0.25f, 0.25f); //Scale
        glutSolidDodecahedron();
        break;
    }
}

void drawObject() {
    // toro satu kiri
    drawToro(8.0f, 0.2f, -8.0f, 0.8f); 
    
    // Toro dua kanan
    drawToro(-8.0f, 0.2f, -8.0f, 0.8f); 

    //depan
    glPushMatrix(); 
        glTranslatef(0.5f, 0.0f, 1.5f); //Tengah
        glScalef(1.5f, 1.5f, 1.5f);
        batu();
    glPopMatrix();
    glPushMatrix(); 
        glTranslatef(1.5f, 0.0f, 1.5f); //Translasi
        glScalef(1.5f, 1.5f, 1.5f);
        batu();
    glPopMatrix();
    glPushMatrix(); 
        glTranslatef(2.5f, 0.0f, 1.5f); //Translasi
        glScalef(1.5f, 1.5f, 1.5f);
        batu();
    glPopMatrix();
    glPushMatrix(); 
        glTranslatef(3.5f, 0.0f, 1.5f); //Translasi
        glScalef(1.5f, 1.5f, 1.5f);
        batu(1);
    glPopMatrix();
    glPushMatrix(); 
        glTranslatef(4.5f, 0.0f, 1.5f); //Translasi
        glScalef(1.5f, 1.5f, 1.5f);
        batu(1);
    glPopMatrix();
    glPushMatrix(); 
        glTranslatef(5.5f, 0.0f, 1.5f); //ujung depan kanan
        glScalef(1.7f, 1.7f, 1.7f);
        batu(1);
    glPopMatrix(); 
    glPushMatrix(); 
        glTranslatef(-0.5f, 0.0f, 1.5f); //Translasi
        glScalef(1.5f, 1.5f, 1.5f);
        batu(1);
    glPopMatrix();
    glPushMatrix(); 
        glTranslatef(-1.5f, 0.0f, 1.5f); //Translasi
        glScalef(1.5f, 1.5f, 1.5f);
        batu();
    glPopMatrix();
    glPushMatrix(); 
        glTranslatef(-2.5f, 0.0f, 1.5f); //Translasi
        glScalef(1.5f, 1.5f, 1.5f);
        batu();
    glPopMatrix();
    glPushMatrix(); 
        glTranslatef(-3.5f, 0.0f, 1.5f); //Translasi
        glScalef(1.5f, 1.5f, 1.5f);
        batu(1);
    glPopMatrix();
    glPushMatrix(); 
        glTranslatef(-4.5f, 0.0f, 1.5f); //Translasi
        glScalef(1.5f, 1.5f, 1.5f);
        batu(1);
    glPopMatrix();
    glPushMatrix(); 
        glTranslatef(-5.5f, 0.0f, 1.5f); //ujung depan kiri
        glScalef(1.8f, 1.8f, 1.8f);
        batu(1);
    glPopMatrix();
    //belakang
    glPushMatrix(); 
        glTranslatef(0.5f, 0.0f, -9.5f); //Translasi
        glScalef(2.0f, 2.0f, 2.0f);
        batu();
    glPopMatrix();
    glPushMatrix(); 
        glTranslatef(1.5f, 0.0f, -9.5f); //Translasi
        glScalef(1.9f, 1.9f, 1.9f);
        batu();
    glPopMatrix();
    glPushMatrix(); 
        glTranslatef(2.5f, 0.0f, -9.5f); //Translasi
        glScalef(1.9f, 1.9f, 1.9f);
        batu();
    glPopMatrix();
    glPushMatrix(); 
        glTranslatef(3.5f, 0.0f, -9.5f); //Translasi
        glScalef(1.8f, 1.8f, 1.8f);
        batu(1);
    glPopMatrix();
    glPushMatrix(); 
        glTranslatef(4.5f, 0.0f, -9.5f); //Translasi
        glScalef(1.7f, 1.7f, 1.7f);
        batu(1);
    glPopMatrix();
    glPushMatrix(); 
        glTranslatef(5.5f, 0.0f, -9.5f); //Ujung belakang kanan
        glScalef(1.8f, 1.8f, 1.8f);
        batu(1);
    glPopMatrix(); 
    glPushMatrix(); 
        glTranslatef(-0.5f, 0.0f, -9.5f); //Translasi
        glScalef(2.0f, 2.0f, 2.0f);
        batu(1);
    glPopMatrix();
    glPushMatrix(); 
        glTranslatef(-1.5f, 0.0f, -9.5f); //Translasi
        glScalef(1.9f, 1.9f, 1.9f);
        batu();
    glPopMatrix();
    glPushMatrix(); 
        glTranslatef(-2.5f, 0.0f, -9.5f); //Translasi
        glScalef(1.9f, 1.9f, 1.9f);
        batu();
    glPopMatrix();
    glPushMatrix(); 
        glTranslatef(-3.5f, 0.0f, -9.5f); //Translasi
        glScalef(1.8f, 1.8f, 1.8f);
        batu(1);
    glPopMatrix();
    glPushMatrix(); 
        glTranslatef(-4.5f, 0.0f, -9.5f); //Translasi
        glScalef(1.7f, 1.7f, 1.7f);
        batu(1);
    glPopMatrix();
    glPushMatrix(); 
        glTranslatef(-5.5f, 0.0f, -9.5f); //ujung belakang kiri
        glScalef(1.8f, 1.8f, 1.8f);
        batu(1);
    glPopMatrix();
    //kiri
    glPushMatrix(); 
        glTranslatef(-5.5f, 0.0f, 0.5f); //Translasi
        glScalef(1.5f, 1.5f, 1.5f);
        batu(1);
    glPopMatrix();
    glPushMatrix(); 
        glTranslatef(-5.5f, 0.0f, -0.5f); //Translasi
        glScalef(1.5f, 1.5f, 1.5f);
        batu(1);
    glPopMatrix();
    glPushMatrix(); 
        glTranslatef(-5.5f, 0.0f, -1.5f); //Translasi
        glScalef(1.5f, 1.5f, 1.5f);
        batu(0);
    glPopMatrix();
    glPushMatrix(); 
        glTranslatef(-5.5f, 0.0f, -2.5f); //Translasi
        glScalef(1.5f, 1.5f, 1.5f);
        batu(1);
    glPopMatrix();
    glPushMatrix(); 
        glTranslatef(-5.5f, 0.0f, -3.5f); //Translasi
        glScalef(1.5f, 1.5f, 1.5f);
        batu(1);
    glPopMatrix();
    glPushMatrix(); 
        glTranslatef(-5.5f, 0.0f, -4.5f); //Translasi
        glScalef(1.5f, 1.5f, 1.5f);
        batu(0);
    glPopMatrix();
    glPushMatrix(); 
        glTranslatef(-5.5f, 0.0f, -5.5f); //Translasi
        glScalef(1.5f, 1.5f, 1.5f);
        batu(0);
    glPopMatrix();
    glPushMatrix(); 
        glTranslatef(-5.5f, 0.0f, -6.5f); //Translasi
        glScalef(1.5f, 1.5f, 1.5f);
        batu(0);
    glPopMatrix();
    glPushMatrix(); 
        glTranslatef(-5.5f, 0.0f, -7.5f); //Translasi
        glScalef(1.5f, 1.5f, 1.5f);
        batu(1);
    glPopMatrix();
    glPushMatrix(); 
        glTranslatef(-5.5f, 0.0f, -8.5f); //Translasi
        glScalef(1.5f, 1.5f, 1.5f);
        batu(1);
    glPopMatrix();
    //kanan
    glPushMatrix(); 
        glTranslatef(5.5f, 0.0f, 0.5f); //Translasi
        glScalef(1.5f, 1.5f, 1.5f);
        batu(1);
    glPopMatrix();
    glPushMatrix(); 
        glTranslatef(5.5f, 0.0f, -0.5f); //Translasi
        glScalef(1.5f, 1.5f, 1.5f);
        batu(1);
    glPopMatrix();
    glPushMatrix(); 
        glTranslatef(5.5f, 0.0f, -1.5f); //Translasi
        glScalef(1.5f, 1.5f, 1.5f);
        batu(0);
    glPopMatrix();
    glPushMatrix(); 
        glTranslatef(5.5f, 0.0f, -2.5f); //Translasi
        glScalef(1.5f, 1.5f, 1.5f);
        batu(1);
    glPopMatrix();
    glPushMatrix(); 
        glTranslatef(5.5f, 0.0f, -3.5f); //Translasi
        glScalef(1.5f, 1.5f, 1.5f);
        batu(1);
    glPopMatrix();
    glPushMatrix(); 
        glTranslatef(5.5f, 0.0f, -4.5f); //Translasi
        glScalef(1.5f, 1.5f, 1.5f);
        batu(0);
    glPopMatrix();
    glPushMatrix(); 
        glTranslatef(5.5f, 0.0f, -5.5f); //Translasi
        glScalef(1.5f, 1.5f, 1.5f);
        batu(1);
    glPopMatrix();
    glPushMatrix(); 
        glTranslatef(5.5f, 0.0f, -6.5f); //Translasi
        glScalef(1.5f, 1.5f, 1.5f);
        batu(0);
    glPopMatrix();
    glPushMatrix(); 
        glTranslatef(5.5f, 0.0f, -7.5f); //Translasi
        glScalef(1.5f, 1.5f, 1.5f);
        batu(1);
    glPopMatrix();
    glPushMatrix(); 
        glTranslatef(5.5f, 0.0f, -8.5f); //Translasi
        glScalef(1.5f, 1.5f, 1.5f);
        batu(1);
    glPopMatrix();
    
    //kolam
     glPushMatrix(); 
        glColor3ub(102.0, 238.0, 187.0);
        glTranslatef(0.0f, 0.0f, -4.0f); //Translasi
        glScalef( 11.0f, 0.5f, 11.0f);
        glutSolidCube(1.0);
    glPopMatrix();

    //bambu 
     glPushMatrix(); 
        glColor3ub(85, 107, 47);
        glTranslatef(0.0f, 0.4f, -10.0f); //Translasi
        glScalef( 0.05f, 0.05f, 1.05f);
        glutSolidCylinder(2.0, 2.0, 20.0, 20.0);
    glPopMatrix();
    
    //air dalam bambu
    glPushMatrix(); 
        glColor3ub(102.0, 238.0, 187.0);
        glTranslatef(0.0f, 0.4f, -10.0f); //Translasi
        glScalef( 0.04f, 0.04f, 1.09f);
        glutSolidCylinder(2.0, 2.0, 20.0, 20.0);
    glPopMatrix();

    //air yang keluar dari bambu
    glPushMatrix(); 
    glColor3ub(102.0, 238.0, 187.0);
    glTranslatef(0.0f, 0.4f, -7.85f); 
    glRotatef(45.0f, 1.0f, 0.0f, 0.0f);
    glScalef(0.04f, 0.04f, 1.09f);
    glutSolidCylinder(2.0, 2.0, 20.0, 20.0);
    glPopMatrix();
}

void drawFloor() {
    // Lantai abu-abu gelap
    glBegin(GL_QUADS);
    glColor3f(0.2f, 0.2f, 0.25f);
    glVertex3f(-10.0f, 0.0f, -10.0f);
    glVertex3f(-10.0f, 0.0f,  10.0f);
    glVertex3f( 10.0f, 0.0f,  10.0f);
    glVertex3f( 10.0f, 0.0f, -10.0f);
    glEnd();

    // Garis grid
    glDisable(GL_LIGHTING);
    glColor3f(0.3f, 0.3f, 0.35f);
    glBegin(GL_LINES);
    for (float i = -10.0f; i <= 10.0f; i += 1.0f) {
        glVertex3f(i, 0.01f, -10.0f); glVertex3f(i, 0.01f, 10.0f);
        glVertex3f(-10.0f, 0.01f, i); glVertex3f(10.0f, 0.01f, i);
    }
    glEnd();
    glEnable(GL_LIGHTING);
}

// ---------------------------------- pergerakan kamera
void updateCameraMovement() {
    float yawRad = cameraYaw * PI / 180.0f;
    float pitchRad = cameraPitch * PI / 180.0f;

    float frontX = sin(yawRad) * cos(pitchRad);
    float frontY = sin(pitchRad);
    float frontZ = -cos(yawRad) * cos(pitchRad);

    float speed = MOVE_SPEED * deltaTime;

    if (keys['w'] || keys['W']) {
        cameraPosX += frontX * speed;
        cameraPosY += frontY * speed;
        cameraPosZ += frontZ * speed;
    }
    if (keys['s'] || keys['S']) {
        cameraPosX -= frontX * speed;
        cameraPosY -= frontY * speed;
        cameraPosZ -= frontZ * speed;
    }
    if (keys['a'] || keys['A']) {
        float rightX = cos(yawRad);
        float rightZ = sin(yawRad);
        cameraPosX -= rightX * speed;
        cameraPosZ -= rightZ * speed;
    }
    if (keys['d'] || keys['D']) {
        float rightX = cos(yawRad);
        float rightZ = sin(yawRad);
        cameraPosX += rightX * speed;
        cameraPosZ += rightZ * speed;
    }
    if (keys['e'] || keys['E']) cameraPosY += speed;  // Naik
    if (keys['q'] || keys['Q']) cameraPosY -= speed;  // Turun
}

// ---------------------------------- GLUT callbacks
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (double)windowWidth / windowHeight, 0.1, 100.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    float yawRad = cameraYaw * PI / 180.0f;
    float pitchRad = cameraPitch * PI / 180.0f;

    float lookX = cameraPosX + sin(yawRad) * cos(pitchRad);
    float lookY = cameraPosY + sin(pitchRad);
    float lookZ = cameraPosZ - cos(yawRad) * cos(pitchRad);

    gluLookAt(cameraPosX, cameraPosY, cameraPosZ,
              lookX, lookY, lookZ,
              0.0f, 1.0f, 0.0f);

    drawFloor();
    drawObject();

    glutSwapBuffers();
}

void mouseMotion(int x, int y) {
    if (!mouseCaptured) return;

    int centerX = windowWidth / 2;
    int centerY = windowHeight / 2;

    float deltaX = (x - centerX) * MOUSE_SENSITIVITY;
    float deltaY = (y - centerY) * MOUSE_SENSITIVITY;

    cameraYaw   += deltaX;
    cameraPitch -= deltaY; // dibalik agar sesuai intuisi

    if (cameraPitch > 89.0f)  cameraPitch = 89.0f;
    if (cameraPitch < -89.0f) cameraPitch = -89.0f;

    glutWarpPointer(centerX, centerY);
}

void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        mouseCaptured = true;
        glutSetCursor(GLUT_CURSOR_NONE);
        glutWarpPointer(windowWidth / 2, windowHeight / 2);
    }
}

void keyboard(unsigned char key, int x, int y) {
    key = tolower(key);
    keys[key] = true;

    if (key == 27) { // ESC
        if (mouseCaptured) {
            mouseCaptured = false;
            glutSetCursor(GLUT_CURSOR_INHERIT);
        } else {
            exit(0);
        }
    }
}

void keyboardUp(unsigned char key, int x, int y) {
    key = tolower(key);
    keys[key] = false;
}

void timer(int value) {
    float currentTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
    deltaTime = currentTime - lastFrameTime;
    lastFrameTime = currentTime;

    updateCameraMovement();
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0); // ~60 FPS
}

void reshape(int w, int h) {
    windowWidth = w;
    windowHeight = h;
    glViewport(0, 0, w, h);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("Taman Jepang Expert");

    // glutFullScreen(); // uncomment jika ingin fullscreen

    init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardUp);
    glutPassiveMotionFunc(mouseMotion);
    glutMotionFunc(mouseMotion);
    glutMouseFunc(mouse);
    glutTimerFunc(0, timer, 0);
    glutMainLoop();
    return 0;
}