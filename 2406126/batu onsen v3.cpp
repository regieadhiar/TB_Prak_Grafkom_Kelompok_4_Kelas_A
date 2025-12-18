// ---------------------------------- library
#include <GL/freeglut.h>
#include <GL/glu.h>
#include <cmath>
#include <iostream>

// ---------------------------------- konstanta
const float PI = 3.14159265f;
const float MOVE_SPEED = 5.0f;
const float MOUSE_SENSITIVITY = 0.1f;

// ---------------------------------- variabel kamera
float cameraPosX = 0.0f, cameraPosY = 7.0f, cameraPosZ = 28.0f;  // Mundur jauh agar kolam besar terlihat utuh
float cameraYaw = 0.0f;
float cameraPitch = -20.0f;

int windowWidth = 1200, windowHeight = 800;
bool mouseCaptured = false;

bool keys[256] = {false};

float deltaTime = 0.0f;
float lastFrameTime = 0.0f;

// ---------- BATU-BATU ONSEN ----------
#define MAX_ROCKS 120  // Diperbanyak lagi agar rapat di kolam besar
float rockX[MAX_ROCKS], rockY[MAX_ROCKS], rockZ[MAX_ROCKS];
float rockScale[MAX_ROCKS], rockRot[MAX_ROCKS];
int totalRocks = 120;

// ---------- BAMBU ONSEN ----------
#define NUM_BAMBOO 2
float bambooAngles[NUM_BAMBOO] = {45.0f, 225.0f};

void initRocks() {
    srand(1234);
    float a = 45.0f;   // Kolam SUPER BESAR
    float b = 32.0f;
    float edgeRadius = 47.0f;  // Tepi batu lebih keluar

    for(int i = 0; i < totalRocks; i++) {
        float t = i * 2*PI / totalRocks + (rand()%100)/2000.0f;
        float r_extra = 1.0f + (rand()%100)/100.0f * 3.0f;  // Variasi besar agar overlap sempurna
        float x = (edgeRadius + r_extra) * cosf(t);
        float z = (edgeRadius + r_extra) * (b/a) * sinf(t);

        rockX[i] = x;
        rockZ[i] = z;
        rockY[i] = 0.05f + (rand()%60)/1000.0f;
        rockScale[i] = 1.2f + (rand()%100)/100.0f * 2.0f;  // Batu besar
        rockRot[i] = rand() % 360;
    }
}

void drawRocks() {
    glColor3f(0.35f, 0.35f, 0.38f);
    for(int i = 0; i < totalRocks; i++) {
        glPushMatrix();
        glTranslatef(rockX[i], rockY[i], rockZ[i]);
        glRotatef(rockRot[i], 1, 1, 0.3f);
        glRotatef(35, 0, 1, 0);
        glScalef(rockScale[i], rockScale[i]*0.75f, rockScale[i]);
        glutSolidDodecahedron();
        glPopMatrix();
    }
}

void drawOnsenPool() {
    float a = 45.0f;           // KOLAM BESAR
    float b = 32.0f;
    float waterScale = 0.99f;
    float edgeHeight = 0.5f;
    int seg = 200;             // Segmen banyak

    srand(5678);

    // Air Hijau
    glColor3f(0.05f, 0.45f, 0.35f);
    glBegin(GL_POLYGON);
    for(int i = 0; i < seg; i++) {
        float t = i * 2*PI / seg;
        float noise = (rand() % 100) / 100.0f * 0.8f;
        float r_a = a * (waterScale + noise * 0.04f);
        float r_b = b * (waterScale + noise * 0.04f);
        glVertex3f(r_a * cosf(t), 0.06f, r_b * sinf(t));
    }
    glEnd();

    // Riak air
    glDisable(GL_LIGHTING);
    glColor3f(0.15f, 0.65f, 0.50f);
    glBegin(GL_LINES);
    for(int i = 0; i < 150; i++) {
        float t = i * 2*PI / 150;
        float offset = (rand() % 100) / 100.0f * 1.0f;
        float x1 = (a * waterScale * 0.9f) * cosf(t) + sinf(t * 5) * 0.6f + offset;
        float z1 = (b * waterScale * 0.9f) * sinf(t) + cosf(t * 5) * 0.6f + offset;
        float x2 = x1 + cosf(t) * 2.5f + offset;
        float z2 = z1 + sinf(t) * 2.5f + offset;
        glVertex3f(x1, 0.07f, z1);
        glVertex3f(x2, 0.07f, z2);
    }
    glEnd();
    glEnable(GL_LIGHTING);

    // Tepi luar batu
    glColor3f(0.28f, 0.24f, 0.18f);
    glBegin(GL_QUAD_STRIP);
    for(int i = 0; i <= seg; i++) {
        float t = i * 2*PI / seg;
        float noise = (rand() % 100) / 100.0f * 0.8f;
        float r_a = a * (1.0f + noise * 0.05f);
        float r_b = b * (1.0f + noise * 0.05f);
        float x = r_a * cosf(t);
        float z = r_b * sinf(t);
        glVertex3f(x, 0.0f, z);
        glVertex3f(x, edgeHeight, z);
    }
    glEnd();

    // Dinding dalam
    glColor3f(0.18f, 0.15f, 0.12f);
    glBegin(GL_QUAD_STRIP);
    for(int i = 0; i <= seg; i++) {
        float t = i * 2*PI / seg;
        float noise = (rand() % 100) / 100.0f * 0.8f;
        float r_a = a * (waterScale + noise * 0.04f);
        float r_b = b * (waterScale + noise * 0.04f);
        float x = r_a * cosf(t);
        float z = r_b * sinf(t);
        glVertex3f(x, 0.06f, z);
        glVertex3f(x, 0.06f - 0.48f, z);
    }
    glEnd();
}

void drawGround() {
    glColor3f(0.28f, 0.24f, 0.18f);
    glBegin(GL_QUADS);
    glVertex3f(-150, -0.01f, -150);  // Ground super luas
    glVertex3f( 150, -0.01f, -150);
    glVertex3f( 150, -0.01f,  150);
    glVertex3f(-150, -0.01f,  150);
    glEnd();
}

// ---------- BAMBU ----------
void drawBambooSpouts() {
    float a = 45.0f;
    float b = 32.0f;
    float outerRadius = 50.0f;     
    float bambooLength = 12.0f;    // Panjang bambu 
    float bambooRadius = 0.7f;     // Tebal 
    float tiltAngle = 30.0f;
    float spoutLength = 3.0f;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for(int i = 0; i < NUM_BAMBOO; i++) {
        float angleRad = bambooAngles[i] * PI / 180.0f;
        float baseX = outerRadius * cosf(angleRad);
        float baseZ = outerRadius * (b/a) * sinf(angleRad);
        float baseY = 1.2f;

        glColor3f(0.65f, 0.75f, 0.45f);

        glPushMatrix();
        glTranslatef(baseX, baseY, baseZ);
        glRotatef(bambooAngles[i] + 180.0f, 0, 1, 0);
        glRotatef(tiltAngle, 0, 0, 1);

        GLUquadric* quad = gluNewQuadric();
        gluCylinder(quad, bambooRadius, bambooRadius, bambooLength, 24, 1);
        gluDeleteQuadric(quad);

        glTranslatef(0, 0, bambooLength);
        quad = gluNewQuadric();
        gluCylinder(quad, bambooRadius, 0.0f, spoutLength, 24, 1);
        gluDeleteQuadric(quad);

        // Aliran air
        glDisable(GL_LIGHTING);
        glColor4f(0.1f, 0.6f, 0.5f, 0.7f);
        glBegin(GL_LINES);
        for(int j = 0; j < 24; j++) {
            float offsetX = (rand()%100 / 1000.0f - 0.05f) * 1.2f;
            float offsetZ = (rand()%100 / 1000.0f - 0.05f) * 1.2f;
            glVertex3f(offsetX, 0.0f, offsetZ);
            glVertex3f(offsetX - 0.4f, -8.0f, offsetZ + 1.5f);
        }
        glEnd();
        glEnable(GL_LIGHTING);

        glPopMatrix();
    }

    glDisable(GL_BLEND);
}

void init() {
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glShadeModel(GL_SMOOTH);

    GLfloat light_direction[] = {0.0f, -10.5f, 0.0f, 0.0f};
    GLfloat light_diffuse[]   = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat light_ambient[]   = {0.4f, 0.4f, 0.4f, 1.0f};
    GLfloat light_specular[]  = {1.0f, 1.0f, 1.0f, 1.0f};

    glLightfv(GL_LIGHT0, GL_POSITION, light_direction);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);
    glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

    GLfloat mat_specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat mat_shininess[] = {50.0f};
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

    initRocks();
}

void drawScene() {
    drawGround();
    drawOnsenPool();
    drawRocks();
    drawBambooSpouts();
}

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
    if (keys['e'] || keys['E']) cameraPosY += speed;
    if (keys['q'] || keys['Q']) cameraPosY -= speed;
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (double)windowWidth / windowHeight, 0.1, 600.0);  // Far clip

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

    drawScene();

    glutSwapBuffers();
}

void mouseMotion(int x, int y) {
    if (!mouseCaptured) return;
    int centerX = windowWidth / 2;
    int centerY = windowHeight / 2;
    float deltaX = (x - centerX) * MOUSE_SENSITIVITY;
    float deltaY = (y - centerY) * MOUSE_SENSITIVITY;
    cameraYaw += deltaX;
    cameraPitch -= deltaY;
    if (cameraPitch > 89.0f) cameraPitch = 89.0f;
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
    if (key == 27) {
        if (mouseCaptured) {
            mouseCaptured = false;
            glutSetCursor(GLUT_CURSOR_INHERIT);
        } else exit(0);
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
    glutTimerFunc(16, timer, 0);
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
    glutCreateWindow("Batu onsen V3");
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
