// ---------------------------------- library
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/FreeImage.h>
#include <cmath>
#include <iostream>

using namespace std;


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

// ID Texture
GLuint texDinding, texAtap;

// --- FUNGSI CUSTOM: Kubus dengan Koordinat Tekstur  ---
void drawTexturedCube(float size) {
    float s = size / 2.0f;
    glBegin(GL_QUADS);

    // --- Sisi Depan (Menghadap Z Positif) ---
    glNormal3f(0.0f, 0.0f, 1.0f); // <--- Normal arah depan
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-s, -s,  s);
    glTexCoord2f(1.0f, 0.0f); glVertex3f( s, -s,  s);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( s,  s,  s);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-s,  s,  s);

    // --- Sisi Belakang (Menghadap Z Negatif) ---
    glNormal3f(0.0f, 0.0f, -1.0f); // <--- Normal arah belakang
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-s, -s, -s);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-s,  s, -s);
    glTexCoord2f(0.0f, 1.0f); glVertex3f( s,  s, -s);
    glTexCoord2f(0.0f, 0.0f); glVertex3f( s, -s, -s);

    // --- Sisi Atas (Menghadap Y Positif) ---
    glNormal3f(0.0f, 1.0f, 0.0f); // <--- Normal arah atas
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-s,  s, -s);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-s,  s,  s);
    glTexCoord2f(1.0f, 0.0f); glVertex3f( s,  s,  s);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( s,  s, -s);

    // --- Sisi Bawah (Menghadap Y Negatif) ---
    glNormal3f(0.0f, -1.0f, 0.0f); // <--- Normal arah bawah
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-s, -s, -s);
    glTexCoord2f(0.0f, 1.0f); glVertex3f( s, -s, -s);
    glTexCoord2f(0.0f, 0.0f); glVertex3f( s, -s,  s);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-s, -s,  s);

    // --- Sisi Kanan (Menghadap X Positif) ---
    glNormal3f(1.0f, 0.0f, 0.0f); // <--- Normal arah kanan
    glTexCoord2f(1.0f, 0.0f); glVertex3f( s, -s, -s);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( s,  s, -s);
    glTexCoord2f(0.0f, 1.0f); glVertex3f( s,  s,  s);
    glTexCoord2f(0.0f, 0.0f); glVertex3f( s, -s,  s);

    // --- Sisi Kiri (Menghadap X Negatif) ---
    glNormal3f(-1.0f, 0.0f, 0.0f); // <--- Normal arah kiri
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-s, -s, -s);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-s, -s,  s);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-s,  s,  s);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-s,  s, -s);

    glEnd();
}

// --- FUNGSI LOAD TEXTURE ---
int loadTexture(const char* path) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    
    FREE_IMAGE_FORMAT format = FreeImage_GetFIFFromFilename(path);
    if (format == FIF_UNKNOWN) return -1;

    FIBITMAP* bitmap = FreeImage_Load(format, path, 0);
    if (!bitmap) return -1;

    FIBITMAP* bitmap2 = FreeImage_ConvertTo24Bits(bitmap);
    FreeImage_Unload(bitmap);

    int imgWidth = FreeImage_GetWidth(bitmap2);
    int imgHeight = FreeImage_GetHeight(bitmap2);
    void* imgData = FreeImage_GetBits(bitmap2);

    if (imgData) {
        // Deteksi format warna BGR vs RGB
        int formatColor = (FI_RGBA_RED == 0) ? GL_RGB : GL_BGR;
        
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imgWidth, imgHeight, 0, formatColor, GL_UNSIGNED_BYTE, imgData);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        FreeImage_Unload(bitmap2);
        return textureID;
    }
    return -1;
}

// ---------------------------------- inisialisasi OpenGL
void init() {
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_TEXTURE_2D);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glShadeModel(GL_SMOOTH);

    // Directional light tetap dari atas tengah (seperti matahari)
    GLfloat light_direction[] = {0.0f, -10.5f, 0.0f, 0.0f}; // w=0 → directional
    GLfloat light_diffuse[]   = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat light_ambient[]   = {0.3f, 0.3f, 0.3f, 1.0f};
    GLfloat light_specular[]  = {1.0f, 1.0f, 1.0f, 1.0f};

    glLightfv(GL_LIGHT0, GL_POSITION, light_direction);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);
    glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

    // Material agar ada sedikit highlight
    GLfloat mat_specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat mat_shininess[] = {50.0f};
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

    // Load textures dengan path lengkap
    texDinding = loadTexture("src/tugas/Texture/Quartz.jpg");
    texAtap    = loadTexture("src/tugas/Texture/Atap.jpg");
}

void drawRoof() { 
    float LB = 2.5f; // Lebar Bawah
    float LA = 2.0f; // Lebar Atas 
    float TB = 0.7f; // Tebal Bawah
    float TA = 0.21f;// Tebal Atas
    float T  = 0.5f; // Tinggi Atap

    glBindTexture(GL_TEXTURE_2D, texAtap);
    glBegin(GL_QUADS);
    glColor3f(1.0f, 1.0f, 1.0f);
    // Sisi Depan
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-LA, T, TA);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-LB, 0.0f, TB);
    glTexCoord2f(1.0f, 0.0f); glVertex3f( LB, 0.0f, TB);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( LA, T, TA);

    // Sisi Belakang 
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-LA, T, -TA);
    glTexCoord2f(0.0f, 1.0f); glVertex3f( LA, T, -TA);
    glTexCoord2f(0.0f, 0.0f); glVertex3f( LB, 0.0f, -TB);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-LB, 0.0f, -TB);

    glDisable(GL_TEXTURE_2D);
    glColor3f(0.1f, 0.1f, 0.1f); 

    glBegin(GL_QUADS);
    // Sisi Atas
    glVertex3f(-LA, T,  TA);
    glVertex3f( LA, T,  TA);
    glVertex3f( LA, T, -TA);
    glVertex3f(-LA, T, -TA);

    // Sisi Bawah 
    glVertex3f(-LB, 0.0f,  TB);
    glVertex3f(-LB, 0.0f, -TB);
    glVertex3f( LB, 0.0f, -TB);
    glVertex3f( LB, 0.0f,  TB);

    glColor3f(1.0f, 1.0f, 1.0f);
    glEnable(GL_TEXTURE_2D);

    // Sisi Kiri 
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-LA, T, -TA);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-LA, T, TA);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-LB, 0.0f, TB);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-LB, 0.0f, -TB);

    // Sisi Kanan
    glTexCoord2f(0.0f, 1.0f); glVertex3f( LA, T, TA);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( LA, T, -TA);
    glTexCoord2f(1.0f, 0.0f); glVertex3f( LB, 0.0f, -TB);
    glTexCoord2f(0.0f, 0.0f); glVertex3f( LB, 0.0f, TB);

    glEnd();
}

void drawObject() {
    glPushMatrix(); 
        glColor3ub(179, 147, 88); //Warna 
        glTranslatef(0.0f, 0.0f, 0.0f); //Translasi
        glRotatef(0.0f, 1.0f, 0.0f, 0.0f); //Rotasi X
        glRotatef(0.0f, 0.0f, 1.0f, 0.0f); //Rotasi Y
        glRotatef(0.0f, 0.0f, 0.0f, 1.0f); //Rotasi Z
        glScalef(50.0f, 1.0f, 50.0f); //Scale
        glutSolidCube(1.0);
    glPopMatrix();
    // Tambahkan bagian-bagian lain dari objek di sini
    // DINDING 1
    glBindTexture(GL_TEXTURE_2D, texDinding);
    glPushMatrix();
    glTranslatef(0.0f, 1.0f, 0.0f);
    glScalef(4.0f, 2.0f, 0.5f);
    glColor3f(1.0f, 1.0f, 1.0f);
    drawTexturedCube(1.0f);
    glPopMatrix();

    // ATAP 1
    glPushMatrix();
    glTranslatef(0.0f, 2.0f, 0.0f);
    drawRoof();
    glPopMatrix();

    // --- BALOK PENYAMBUNG DINDING 1 & 3 ---
    glPushMatrix();
    glTranslatef(1.9f, 1.0f, 0.0f); 
    glScalef(0.5f, 2.0f, 1.0f); // Buat agak besar dan tinggi
    glColor3f(1.0f, 1.0f, 1.0f);
    glBindTexture(GL_TEXTURE_2D, texDinding);
    drawTexturedCube(1.0f);
    glPopMatrix();

    // DINDING 2
    glBindTexture(GL_TEXTURE_2D, texDinding);
    glPushMatrix();
    glTranslatef(6.0f, 1.0f, 1.8f);
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    glScalef(4.0f, 2.0f, 0.5f);
    glColor3f(1.0f, 1.0f, 1.0f);
    drawTexturedCube(1.0f);
    glPopMatrix();

    //ATAP 2
    glPushMatrix();
    glTranslatef(6.0f, 2.0f, 1.8f);
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    drawRoof();
    glPopMatrix();

    // DINDING 3
    glBindTexture(GL_TEXTURE_2D, texDinding);
    glPushMatrix();
    glTranslatef(4.2f, 1.0f, 0.0f);
    glScalef(4.0f, 2.0f, 0.5f);
    glColor3f(1.0f, 1.0f, 1.0f);
    drawTexturedCube(1.0f);
    glPopMatrix();

    // ATAP 3
    glPushMatrix();
    glTranslatef(4.0f, 2.0f, 0.0f);
    drawRoof();
    glPopMatrix();

    // DINDING 4
    glBindTexture(GL_TEXTURE_2D, texDinding);
    glPushMatrix();
    glTranslatef(6.0f, 1.0f, 5.9f);
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    glScalef(4.0f, 2.0f, 0.5f);
    glColor3f(1.0f, 1.0f, 1.0f);
    drawTexturedCube(1.0f);
    glPopMatrix();

    //ATAP 4
    glPushMatrix();
    glTranslatef(6.0f, 2.0f, 5.8f);
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    drawRoof();
    glPopMatrix();

    // --- BALOK PENYAMBUNG DINDING 2 & 4 ---
    glPushMatrix();
    glTranslatef(6.0f, 1.0f, 4.0f); 
    glScalef(1.0f, 2.0f, 0.5f); // Buat agak besar dan tinggi
    glColor3f(1.0f, 1.0f, 1.0f);
    glBindTexture(GL_TEXTURE_2D, texDinding);
    drawTexturedCube(1.0f);
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
    glutCreateWindow("Dinding Bertekstur");

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