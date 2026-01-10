// ---------------------------------- library
#include <GL/freeglut.h>
#include <cmath>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cctype>

// ---------------------------------- konstanta
const float PI = 3.14159265f;
const float MOVE_SPEED = 5.0f;          // unit per detik
const float MOUSE_SENSITIVITY = 0.1f;   // sensitivitas mouse

// ---------------------------------- variabel kamera
float cameraPosX = 0.0f, cameraPosY = 3.0f, cameraPosZ = 8.0f;
float cameraYaw = 0.0f;      // rotasi horizontal (derajat)
float cameraPitch = -20.0f; // rotasi vertikal (derajat)

// Tambahkan di bagian global (dekat variabel kamera lainnya)
enum CameraMode { FREE_CAMERA, ORBIT_CAMERA };
CameraMode currentMode = FREE_CAMERA;  // default: bebas gerak

// Untuk mode Orbit
float orbitRadius = 10.0f;          // jarak kamera dari pusat
float orbitCenterX = 0.0f;          // pusat yang dilihat (biasanya 0,0,0)
float orbitCenterY = 1.0f;
float orbitCenterZ = 0.0f;
float orbitYaw   = 0.0f;            // sudut horizontal (mirip yaw)
float orbitPitch = 20.0f;           // sudut vertikal (dari horizontal)
float targetLookX = 0.0f;
float targetLookY = 1.0f;   // biasanya agak di atas lantai supaya lebih natural
float targetLookZ = 0.0f;

float autoRotateSpeed = 20.0f;     // derajat per detik (positif = searah jarum jam, negatif = berlawanan)
bool autoRotateEnabled = true;     // bisa di-toggle kalau mau

int windowWidth = 1200, windowHeight = 800;
bool mouseCaptured = false;

bool keys[256] = {false};

float deltaTime = 0.0f;
float lastFrameTime = 0.0f;

//------------ Bagian Bangunan, Lantai Regie Adhia Rozabbissalam ---------------

void stair(int colorType = 0) {
    glPushMatrix();
        switch (colorType)
        {
        case 1:
            glColor3ub(62, 62, 62);
            break;
        default:
            glColor3ub(65, 42, 20);
            break;
        }
        glTranslatef(0.0f, 0.5f, 0.0f);
        glScalef(1.0f, 0.5f, 1.0f);
        glutSolidCube(1.0);
        glPopMatrix();
        glPushMatrix();
        glTranslatef(0.25f, 0.0f, 0.0f);
        glScalef(0.5f, 0.5f, 1.0f);
        glutSolidCube(1.0);
    glPopMatrix();
}

void trapdoor() {
    glPushMatrix();
        glColor3ub(58, 39, 17);
        glScalef(1.0f, 0.2f, 1.0f);
        glutSolidCube(1.0);
    glPopMatrix();
}

void slab(int colorType = 0) {
    glPushMatrix();
        switch (colorType)
        {
        case 1:
            glColor3ub(62, 62, 62);
            break;
        default:
            glColor3ub(65, 42, 20);
            break;
        }
        glScalef(1.0f, 0.5f, 1.0f);
        glutSolidCube(1.0);
    glPopMatrix();
}

void cube(int colorType = 0) {
    switch (colorType)
    {
    case 1:
        glColor3ub(179, 147, 88); // Wood Plank Color
        break;
    case 2:
        glColor3ub(128, 127, 128); // Stone Color
        break;
    case 3:
        glColor3ub(121, 89, 51); // Wood Slab Color
        break;
    case 4:
        glColor3ub(234, 226, 218); // Quartz White Color
        break;
    case 5:
        glColor3ub(65, 42, 20); // Roof Color
        break;
    case 6:
        glColor3ub(134, 146, 140);
        break;
    default:
        glColor3ub(62, 48, 31); // Log Beam Color
        break;
    }
    glutSolidCube(1.0);
}

// Variabel global (taruh di atas, bersama variabel lain)
float doorAngle = 0.0f;           // sudut saat ini
float doorTargetAngle = 0.0f;     // 0 = tertutup, 90 = terbuka
float doorOpenSpeed = 180.0f;     // cepat buka/tutup (~0.5 detik)

// Fungsi update animasi (panggil setiap frame)
void updateDoor(float deltaTime) {
    if (fabs(doorAngle - doorTargetAngle) > 0.01f) {
        float direction = (doorTargetAngle > doorAngle) ? 1.0f : -1.0f;
        doorAngle += direction * doorOpenSpeed * deltaTime;

        // Agar tidak overshoot
        if (direction > 0 && doorAngle > doorTargetAngle) doorAngle = doorTargetAngle;
        if (direction < 0 && doorAngle < doorTargetAngle) doorAngle = doorTargetAngle;
    }
}

// Fungsi pintu yang kamu punya, ditambah rotasi saja
void pintu() {
    glPushMatrix();
        glColor3ub(101, 67, 33);

        glTranslatef(0.0f, 1.5f, 4.0f);           // posisi awal pintu
        
        // Engsel di sisi kiri (seperti pintu minecraft)
        glTranslatef(-0.5f, 0.0f, 0.0f);          // geser pivot ke kiri (setengah lebar)
        glRotatef(doorAngle, 0.0f, 1.0f, 0.0f);   // rotasi di sumbu Y
        glTranslatef(0.5f, 0.0f, 0.0f);           // balik ke tengah

        glScalef(1.0f, 2.0f, 0.2f);
        cube();                      // atau cube() kalau kamu pakai fungsi custom
    glPopMatrix();
}

void Bangunan() {
    // Building Base
    glPushMatrix(); //Base Kayu
        glTranslatef(0.0f, 0.0f, 0.0f);
        glScalef(13.0f, 1.0f, 7.0f);
        cube(1);
    glPopMatrix();
    glPushMatrix(); //Batu Depan Kayu
        glTranslatef(0.0f, 0.0f, 4.0f);
        glScalef(15.0f, 1.0f, 1.0f);
        cube(2);
    glPopMatrix();
    glPushMatrix(); //Batu Belakang Kayu
        glTranslatef(0.0f, 0.0f, -4.0f);
        glScalef(15.0f, 1.0f, 1.0f);
        cube(2);
    glPopMatrix();
    glPushMatrix(); //Batu Kanan Kayu
        glTranslatef(7.0f, 0.0f, 0.0f);
        glScalef(1.0f, 1.0f, 7.0f);
        cube(2);
    glPopMatrix();
    glPushMatrix(); //Batu Kiri Kayu
        glTranslatef(-7.0f, 0.0f, 0.0f);
        glScalef(1.0f, 1.0f, 7.0f);
        cube(2);
    glPopMatrix();
    glPushMatrix(); //Kayu Depan Batu
        glTranslatef(0.0f, 0.25f, 5.5f);
        glScalef(19.0f, 0.5f, 2.0f);
        cube(3);
    glPopMatrix();
    glPushMatrix(); //Kayu Belakang Batu
        glTranslatef(0.0f, 0.25f, -5.5f);
        glScalef(19.0f, 0.5f, 2.0f);
        cube(3);
    glPopMatrix();
    glPushMatrix(); //Kayu Kiri Batu
        glTranslatef(8.5f, 0.25f, 0.0f);
        glScalef(2.0f, 0.5f, 9.0f);
        cube(3);
    glPopMatrix();
    glPushMatrix(); //Kayu Kanan Batu
        glTranslatef(-8.5f, 0.25f, 0.0f);
        glScalef(2.0f, 0.5f, 9.0f);
        cube(3);
    glPopMatrix();
    // Log Beam
    glPushMatrix(); //Beam
        glTranslatef(-7.0f, 2.0f, 0.0f);
        glScalef(1.0f, 3.0f, 1.0f);
        cube();
    glPopMatrix();
    glPushMatrix(); //Beam
        glTranslatef(-7.0f, 2.5f, 4.0f);
        glScalef(1.0f, 4.0f, 1.0f);
        cube();
    glPopMatrix();
    glPushMatrix(); //Beam
        glTranslatef(-7.0f, 2.5f, -4.0f);
        glScalef(1.0f, 4.0f, 1.0f);
        cube();
    glPopMatrix();
    glPushMatrix(); //Beam
        glTranslatef(7.0f, 2.0f, 0.0f);
        glScalef(1.0f, 3.0f, 1.0f);
        cube();
    glPopMatrix();
    glPushMatrix(); //Beam
        glTranslatef(7.0f, 2.5f, 4.0f);
        glScalef(1.0f, 4.0f, 1.0f);
        cube();
    glPopMatrix();
    glPushMatrix(); //Beam
        glTranslatef(7.0f, 2.5f, -4.0f);
        glScalef(1.0f, 4.0f, 1.0f);
        cube();
    glPopMatrix();
    glPushMatrix(); //Beam
        glTranslatef(2.0f, 2.0f, 4.0f);
        glScalef(1.0f, 3.0f, 1.0f);
        cube();
    glPopMatrix();
    glPushMatrix(); //Beam
        glTranslatef(2.0f, 2.0f, -4.0f);
        glScalef(1.0f, 3.0f, 1.0f);
        cube();
    glPopMatrix();
    glPushMatrix(); //Beam
        glTranslatef(-2.0f, 2.0f, 4.0f);
        glScalef(1.0f, 3.0f, 1.0f);
        cube();
    glPopMatrix();
    glPushMatrix(); //Beam
        glTranslatef(-2.0f, 2.0f, -4.0f);
        glScalef(1.0f, 3.0f, 1.0f);
        cube();
    glPopMatrix();
    glPushMatrix(); //Beam
        glTranslatef(0.0f, 4.0f, -4.0f);
        glScalef(13.0f, 1.0f, 1.0f);
        cube();
    glPopMatrix();
    glPushMatrix(); //Beam
        glTranslatef(0.0f, 4.0f, 4.0f);
        glScalef(13.0f, 1.0f, 1.0f);
        cube();
    glPopMatrix();
    glPushMatrix(); //Beam
        glTranslatef(7.0f, 4.0f, 0.0f);
        glScalef(1.0f, 1.0f, 7.0f);
        cube();
    glPopMatrix();
    glPushMatrix(); //Beam
        glTranslatef(-7.0f, 4.0f, 0.0f);
        glScalef(1.0f, 1.0f, 7.0f);
        cube();
    glPopMatrix();
    // Dinding Samping
    glPushMatrix(); 
        glTranslatef(-7.0f, 2.0f, -2.0f);
        glScalef(1.0f, 3.0f, 3.0f);
        cube(4);
    glPopMatrix();
    glPushMatrix(); 
        glTranslatef(7.0f, 2.0f, -2.0f);
        glScalef(1.0f, 3.0f, 3.0f);
        cube(4);
    glPopMatrix();
    glPushMatrix(); 
        glTranslatef(-7.0f, 2.0f, 2.0f);
        glScalef(1.0f, 3.0f, 3.0f);
        cube(4);
    glPopMatrix();
    glPushMatrix(); 
        glTranslatef(7.0f, 2.0f, 2.0f);
        glScalef(1.0f, 3.0f, 3.0f);
        cube(4);
    glPopMatrix();
    // Dinding Depan
    glPushMatrix(); 
        glTranslatef(6.0f, 1.5f, 4.0f);
        glScalef(1.0f, 2.0f, 1.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix(); 
        glTranslatef(3.0f, 1.5f, 4.0f);
        glScalef(1.0f, 2.0f, 1.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix(); 
        glTranslatef(1.0f, 1.5f, 4.0f);
        glScalef(1.0f, 2.0f, 1.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix(); 
        glTranslatef(-1.0f, 1.5f, 4.0f);
        glScalef(1.0f, 2.0f, 1.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix(); 
        glTranslatef(-6.0f, 1.5f, 4.0f);
        glScalef(1.0f, 2.0f, 1.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix(); 
        glTranslatef(-3.0f, 1.5f, 4.0f);
        glScalef(1.0f, 2.0f, 1.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix(); 
        glTranslatef(-4.5f, 3.0f, 4.0f);
        glScalef(4.0f, 1.0f, 1.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix(); 
        glTranslatef(0.0f, 3.0f, 4.0f);
        glScalef(3.0f, 1.0f, 1.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix(); 
        glTranslatef(4.5f, 3.0f, 4.0f);
        glScalef(4.0f, 1.0f, 1.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    //Dinding Belakang
    glPushMatrix(); 
        glColor3ub(234, 226, 218);
        glTranslatef(6.0f, 1.5f, -4.0f);
        glScalef(1.0f, 2.0f, 1.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix(); 
        glColor3ub(234, 226, 218);
        glTranslatef(3.0f, 1.5f, -4.0f);
        glScalef(1.0f, 2.0f, 1.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix(); 
        glColor3ub(234, 226, 218);
        glTranslatef(-6.0f, 1.5f, -4.0f);
        glScalef(1.0f, 2.0f, 1.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix(); 
        glColor3ub(234, 226, 218);
        glTranslatef(-3.0f, 1.5f, -4.0f);
        glScalef(1.0f, 2.0f, 1.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix(); 
        glColor3ub(234, 226, 218);
        glTranslatef(-4.5f, 3.0f, -4.0f);
        glScalef(4.0f, 1.0f, 1.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix(); 
        glColor3ub(234, 226, 218);
        glTranslatef(0.0f, 3.0f, -4.0f);
        glScalef(3.0f, 1.0f, 1.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix(); 
        glColor3ub(234, 226, 218);
        glTranslatef(4.5f, 3.0f, -4.0f);
        glScalef(4.0f, 1.0f, 1.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    // Window Kiri Depan
    glPushMatrix(); 
        glColor3ub(71, 48, 22);
        glTranslatef(4.0f, 1.5f, 4.0f);
        glScalef(0.3f, 2.0f, 0.3f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix(); 
        glColor3ub(71, 48, 22);
        glTranslatef(5.0f, 1.5f, 4.0f);
        glScalef(0.3f, 2.0f, 0.3f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix(); 
        glColor3ub(71, 48, 22);
        glTranslatef(4.5f, 1.0f, 4.0f);
        glScalef(2.0f, 0.15f, 0.15f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix(); 
        glColor3ub(71, 48, 22);
        glTranslatef(4.5f, 1.3f, 4.0f);
        glScalef(2.0f, 0.15f, 0.15f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix(); 
        glColor3ub(71, 48, 22);
        glTranslatef(4.5f, 2.0f, 4.0f);
        glScalef(2.0f, 0.15f, 0.15f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix(); 
        glColor3ub(71, 48, 22);
        glTranslatef(4.5f, 2.3f, 4.0f);
        glScalef(2.0f, 0.15f, 0.15f);
        glutSolidCube(1.0);
    glPopMatrix();
    // Window Kanan Depan
    glPushMatrix(); 
        glColor3ub(71, 48, 22);
        glTranslatef(-4.0f, 1.5f, 4.0f);
        glScalef(0.3f, 2.0f, 0.3f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix(); 
        glColor3ub(71, 48, 22);
        glTranslatef(-5.0f, 1.5f, 4.0f);
        glScalef(0.3f, 2.0f, 0.3f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix(); 
        glColor3ub(71, 48, 22);
        glTranslatef(-4.5f, 1.0f, 4.0f);
        glScalef(2.0f, 0.15f, 0.15f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix(); 
        glColor3ub(71, 48, 22);
        glTranslatef(-4.5f, 1.3f, 4.0f);
        glScalef(2.0f, 0.15f, 0.15f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix(); 
        glColor3ub(71, 48, 22);
        glTranslatef(-4.5f, 2.0f, 4.0f);
        glScalef(2.0f, 0.15f, 0.15f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix(); 
        glColor3ub(71, 48, 22);
        glTranslatef(-4.5f, 2.3f, 4.0f);
        glScalef(2.0f, 0.15f, 0.15f);
        glutSolidCube(1.0);
    glPopMatrix();
    // Window Kiri Belakang
    glPushMatrix(); 
        glColor3ub(71, 48, 22);
        glTranslatef(4.0f, 1.5f, -4.0f);
        glScalef(0.3f, 2.0f, 0.3f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix(); 
        glColor3ub(71, 48, 22);
        glTranslatef(5.0f, 1.5f, -4.0f);
        glScalef(0.3f, 2.0f, 0.3f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix(); 
        glColor3ub(71, 48, 22);
        glTranslatef(4.5f, 1.0f, -4.0f);
        glScalef(2.0f, 0.15f, 0.15f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix(); 
        glColor3ub(71, 48, 22);
        glTranslatef(4.5f, 1.3f, -4.0f);
        glScalef(2.0f, 0.15f, 0.15f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix(); 
        glColor3ub(71, 48, 22);
        glTranslatef(4.5f, 2.0f, -4.0f);
        glScalef(2.0f, 0.15f, 0.15f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix(); 
        glColor3ub(71, 48, 22);
        glTranslatef(4.5f, 2.3f, -4.0f);
        glScalef(2.0f, 0.15f, 0.15f);
        glutSolidCube(1.0);
    glPopMatrix();
    // Window Kanan Belakang
    glPushMatrix(); 
        glColor3ub(71, 48, 22);
        glTranslatef(-4.0f, 1.5f, -4.0f);
        glScalef(0.3f, 2.0f, 0.3f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix(); 
        glColor3ub(71, 48, 22);
        glTranslatef(-5.0f, 1.5f, -4.0f);
        glScalef(0.3f, 2.0f, 0.3f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix(); 
        glColor3ub(71, 48, 22);
        glTranslatef(-4.5f, 1.0f, -4.0f);
        glScalef(2.0f, 0.15f, 0.15f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix(); 
        glColor3ub(71, 48, 22);
        glTranslatef(-4.5f, 1.3f, -4.0f);
        glScalef(2.0f, 0.15f, 0.15f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix(); 
        glColor3ub(71, 48, 22);
        glTranslatef(-4.5f, 2.0f, -4.0f);
        glScalef(2.0f, 0.15f, 0.15f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix(); 
        glColor3ub(71, 48, 22);
        glTranslatef(-4.5f, 2.3f, -4.0f);
        glScalef(2.0f, 0.15f, 0.15f);
        glutSolidCube(1.0);
    glPopMatrix();
    //Atap Layer 0
    glPushMatrix();
        glColor3ub(65, 42, 20);
        glTranslatef(2.0f, 4.0f, 5.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glColor3ub(65, 42, 20);
        glTranslatef(-2.0f, 4.0f, 5.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glColor3ub(65, 42, 20);
        glTranslatef(4.0f, 4.0f, 5.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glColor3ub(65, 42, 20);
        glTranslatef(-4.0f, 4.0f, 5.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glColor3ub(65, 42, 20);
        glTranslatef(6.0f, 4.0f, 5.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glColor3ub(65, 42, 20);
        glTranslatef(-6.0f, 4.0f, 5.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glColor3ub(65, 42, 20);
        glTranslatef(2.0f, 4.0f, -5.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glColor3ub(65, 42, 20);
        glTranslatef(-2.0f, 4.0f, -5.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glColor3ub(65, 42, 20);
        glTranslatef(4.0f, 4.0f, -5.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glColor3ub(65, 42, 20);
        glTranslatef(-4.0f, 4.0f, -5.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glColor3ub(65, 42, 20);
        glTranslatef(6.0f, 4.0f, -5.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glColor3ub(65, 42, 20);
        glTranslatef(-6.0f, 4.0f, -5.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glColor3ub(65, 42, 20);
        glTranslatef(-8.0f, 4.0f, 1.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glColor3ub(65, 42, 20);
        glTranslatef(-8.0f, 4.0f, -1.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glColor3ub(65, 42, 20);
        glTranslatef(-8.0f, 4.0f, 3.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glColor3ub(65, 42, 20);
        glTranslatef(-8.0f, 4.0f, -3.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glColor3ub(65, 42, 20);
        glTranslatef(8.0f, 4.0f, 1.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glColor3ub(65, 42, 20);
        glTranslatef(8.0f, 4.0f, -1.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glColor3ub(65, 42, 20);
        glTranslatef(8.0f, 4.0f, 3.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glColor3ub(65, 42, 20);
        glTranslatef(8.0f, 4.0f, -3.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    //Stair Left
    glPushMatrix();
        glTranslatef(-8.0f, 3.75f, -4.0f);
        stair();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-8.0f, 3.75f, -2.0f);
        stair();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-8.0f, 3.75f, 0.0f);
        stair();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-8.0f, 3.75f, 2.0f);
        stair();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-8.0f, 3.75f, 4.0f);
        stair();
    glPopMatrix();
    //Stair Right
    glPushMatrix();
        glTranslatef(8.0f, 3.75f, -4.0f);
        glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
        stair();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(8.0f, 3.75f, -2.0f);
        glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
        stair();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(8.0f, 3.75f, 0.0f);
        glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
        stair();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(8.0f, 3.75f, 2.0f);
        glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
        stair();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(8.0f, 3.75f, 4.0f);
        glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
        stair();
    glPopMatrix();
    //Stair Front
    glPushMatrix();
        glTranslatef(-7.0f, 3.75f, 5.0f);
        glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
        stair();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-5.0f, 3.75f, 5.0f);
        glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
        stair();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-3.0f, 3.75f, 5.0f);
        glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
        stair();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(7.0f, 3.75f, 5.0f);
        glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
        stair();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(5.0f, 3.75f, 5.0f);
        glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
        stair();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(3.0f, 3.75f, 5.0f);
        glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
        stair();
    glPopMatrix();
    //Stair Back
    glPushMatrix();
        glTranslatef(-7.0f, 3.75f, -5.0f);
        glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
        stair();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-5.0f, 3.75f, -5.0f);
        glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
        stair();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-3.0f, 3.75f, -5.0f);
        glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
        stair();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(7.0f, 3.75f, -5.0f);
        glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
        stair();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(5.0f, 3.75f, -5.0f);
        glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
        stair();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(3.0f, 3.75f, -5.0f);
        glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
        stair();
    glPopMatrix();
    //Trapdoor
    glPushMatrix();
        glTranslatef(3.0f, 4.4f, 6.0f);
        trapdoor();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(5.0f, 4.4f, 6.0f);
        trapdoor();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(7.0f, 4.4f, 6.0f);
        trapdoor();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-3.0f, 4.4f, 6.0f);
        trapdoor();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-5.0f, 4.4f, 6.0f);
        trapdoor();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-7.0f, 4.4f, 6.0f);
        trapdoor();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(3.0f, 4.4f, -6.0f);
        trapdoor();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(5.0f, 4.4f, -6.0f);
        trapdoor();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(7.0f, 4.4f, -6.0f);
        trapdoor();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-3.0f, 4.4f, -6.0f);
        trapdoor();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-5.0f, 4.4f, -6.0f);
        trapdoor();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-7.0f, 4.4f, -6.0f);
        trapdoor();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-9.0f, 4.4f, 0.0f);
        trapdoor();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-9.0f, 4.4f, 2.0f);
        trapdoor();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-9.0f, 4.4f, 4.0f);
        trapdoor();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-9.0f, 4.4f, -2.0f);
        trapdoor();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-9.0f, 4.4f, -4.0f);
        trapdoor();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(9.0f, 4.4f, 0.0f);
        trapdoor();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(9.0f, 4.4f, 2.0f);
        trapdoor();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(9.0f, 4.4f, 4.0f);
        trapdoor();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(9.0f, 4.4f, -2.0f);
        trapdoor();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(9.0f, 4.4f, -4.0f);
        trapdoor();
    glPopMatrix();
    //Slabs
    glPushMatrix();
        glTranslatef(2.0f, 4.25f, 6.0f);
        slab();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(4.0f, 4.25f, 6.0f);
        slab();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(6.0f, 4.25f, 6.0f);
        slab();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-2.0f, 4.25f, 6.0f);
        slab();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-4.0f, 4.25f, 6.0f);
        slab();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-6.0f, 4.25f, 6.0f);
        slab();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(2.0f, 4.25f, -6.0f);
        slab();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(4.0f, 4.25f, -6.0f);
        slab();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(6.0f, 4.25f, -6.0f);
        slab();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-2.0f, 4.25f, -6.0f);
        slab();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-4.0f, 4.25f, -6.0f);
        slab();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-6.0f, 4.25f, -6.0f);
        slab();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-6.0f, 4.25f, -6.0f);
        slab();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-9.0f, 4.25f, -3.0f);
        slab();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-9.0f, 4.25f, -1.0f);
        slab();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-9.0f, 4.25f, 1.0f);
        slab();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-9.0f, 4.25f, 3.0f);
        slab();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(9.0f, 4.25f, -3.0f);
        slab();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(9.0f, 4.25f, -1.0f);
        slab();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(9.0f, 4.25f, 1.0f);
        slab();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(9.0f, 4.25f, 3.0f);
        slab();
    glPopMatrix();
    //Atap Layer 1
    glPushMatrix();
        glColor3ub(65, 65, 65);
        glTranslatef(4.0f, 5.0f, 4.5f);
        glScalef(5.0f, 1.0f, 2.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glColor3ub(65, 65, 65);
        glTranslatef(4.0f, 5.0f, -4.5f);
        glScalef(5.0f, 1.0f, 2.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glColor3ub(65, 65, 65);
        glTranslatef(-4.0f, 5.0f, 4.5f);
        glScalef(5.0f, 1.0f, 2.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glColor3ub(65, 65, 65);
        glTranslatef(-4.0f, 5.0f, -4.5f);
        glScalef(5.0f, 1.0f, 2.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glColor3ub(65, 65, 65);
        glTranslatef(-7.5f, 5.0f, 0.0f);
        glScalef(2.0f, 1.0f, 11.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glColor3ub(65, 65, 65);
        glTranslatef(7.5f, 5.0f, 0.0f);
        glScalef(2.0f, 1.0f, 11.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(0.0f, 5.25f, 5.0f);
        slab();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(0.0f, 5.25f, -5.0f);
        slab();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(1.0f, 4.75f, -5.0f);
        stair();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(1.0f, 4.75f, 5.0f);
        stair();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-1.0f, 4.75f, -5.0f);
        glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
        stair();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-1.0f, 4.75f, 5.0f);
        glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
        stair();
    glPopMatrix();
    glPushMatrix();
        glColor3ub(234, 226, 218);
        glTranslatef(0.0f, 5.0f, 4.0f);
        glScalef(3.0f, 1.0f, 1.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glColor3ub(234, 226, 218);
        glTranslatef(0.0f, 5.0f, -4.0f);
        glScalef(3.0f, 1.0f, 1.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-9.0f, 4.75f, 0.0f);
        glScalef(1.0f, 1.0f, 7.0f);
        slab(1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(9.0f, 4.75f, 0.0f);
        glScalef(1.0f, 1.0f, 7.0f);
        slab(1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(4.0f, 4.75f, 6.0f);
        glScalef(3.0f, 1.0f, 1.0f);
        slab(1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-4.0f, 4.75f, 6.0f);
        glScalef(3.0f, 1.0f, 1.0f);
        slab(1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(4.0f, 4.75f, -6.0f);
        glScalef(3.0f, 1.0f, 1.0f);
        slab(1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-4.0f, 4.75f, -6.0f);
        glScalef(3.0f, 1.0f, 1.0f);
        slab(1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-9.0f, 5.25f, -6.0f);
        glScalef(1.0f, 1.0f, 1.0f);
        slab(1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(9.0f, 5.25f, -6.0f);
        glScalef(1.0f, 1.0f, 1.0f);
        slab(1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-9.0f, 5.25f, 6.0f);
        glScalef(1.0f, 1.0f, 1.0f);
        slab(1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(9.0f, 5.25f, 6.0f);
        glScalef(1.0f, 1.0f, 1.0f);
        slab(1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(7.0f, 5.0f, 6.0f);
        glScalef(3.0f, 1.0f, 1.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(7.0f, 5.0f, -6.0f);
        glScalef(3.0f, 1.0f, 1.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-7.0f, 5.0f, 6.0f);
        glScalef(3.0f, 1.0f, 1.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-7.0f, 5.0f, -6.0f);
        glScalef(3.0f, 1.0f, 1.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(9.0f, 5.0f, -4.5f);
        glScalef(1.0f, 1.0f, 2.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(9.0f, 5.0f, 4.5f);
        glScalef(1.0f, 1.0f, 2.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-9.0f, 5.0f, -4.5f);
        glScalef(1.0f, 1.0f, 2.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-9.0f, 5.0f, 4.5f);
        glScalef(1.0f, 1.0f, 2.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(2.0f, 5.0f, 6.0f);
        glScalef(1.0f, 1.0f, 1.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-2.0f, 5.0f, 6.0f);
        glScalef(1.0f, 1.0f, 1.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(2.0f, 5.0f, -6.0f);
        glScalef(1.0f, 1.0f, 1.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-2.0f, 5.0f, -6.0f);
        glScalef(1.0f, 1.0f, 1.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(1.0f, 5.25f, 6.0f);
        glScalef(1.0f, 1.0f, 1.0f);
        slab(1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-1.0f, 5.25f, 6.0f);
        glScalef(1.0f, 1.0f, 1.0f);
        slab(1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(1.0f, 5.25f, -6.0f);
        glScalef(1.0f, 1.0f, 1.0f);
        slab(1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-1.0f, 5.25f, -6.0f);
        glScalef(1.0f, 1.0f, 1.0f);
        slab(1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-9.0f, 5.75f, -6.0f);
        glScalef(1.0f, 1.0f, 1.0f);
        slab(1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(9.0f, 5.75f, -6.0f);
        glScalef(1.0f, 1.0f, 1.0f);
        slab(1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-9.0f, 5.75f, 6.0f);
        glScalef(1.0f, 1.0f, 1.0f);
        slab(1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(9.0f, 5.75f, 6.0f);
        glScalef(1.0f, 1.0f, 1.0f);
        slab(1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-1.0f, 5.75f, 5.0f);
        glScalef(1.0f, 1.0f, 3.0f);
        slab(1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(1.0f, 5.75f, 5.0f);
        glScalef(1.0f, 1.0f, 3.0f);
        slab(1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-1.0f, 5.75f, -5.0f);
        glScalef(1.0f, 1.0f, 3.0f);
        slab(1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(1.0f, 5.75f, -5.0f);
        glScalef(1.0f, 1.0f, 3.0f);
        slab(1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(6.0f, 5.75f, -5.0f);
        glScalef(1.0f, 1.0f, 1.0f);
        slab(1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(6.0f, 5.75f, 5.0f);
        glScalef(1.0f, 1.0f, 1.0f);
        slab(1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-6.0f, 5.75f, -5.0f);
        glScalef(1.0f, 1.0f, 1.0f);
        slab(1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-6.0f, 5.75f, 5.0f);
        glScalef(1.0f, 1.0f, 1.0f);
        slab(1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(0.0f, 6.0f, 5.0f);
        glScalef(1.0f, 1.0f, 3.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(0.0f, 6.0f, -5.0f);
        glScalef(1.0f, 1.0f, 3.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(1.0f, 6.0f, 3.0f);
        glScalef(1.0f, 1.0f, 1.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(3.0f, 6.0f, 3.0f);
        glScalef(1.0f, 1.0f, 1.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(5.0f, 6.0f, 3.0f);
        glScalef(1.0f, 1.0f, 1.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(7.0f, 6.0f, 3.0f);
        glScalef(1.0f, 1.0f, 1.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-1.0f, 6.0f, 3.0f);
        glScalef(1.0f, 1.0f, 1.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-3.0f, 6.0f, 3.0f);
        glScalef(1.0f, 1.0f, 1.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-5.0f, 6.0f, 3.0f);
        glScalef(1.0f, 1.0f, 1.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-7.0f, 6.0f, 3.0f);
        glScalef(1.0f, 1.0f, 1.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(1.0f, 6.0f, -3.0f);
        glScalef(1.0f, 1.0f, 1.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(3.0f, 6.0f, -3.0f);
        glScalef(1.0f, 1.0f, 1.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(5.0f, 6.0f, -3.0f);
        glScalef(1.0f, 1.0f, 1.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(7.0f, 6.0f, -3.0f);
        glScalef(1.0f, 1.0f, 1.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-1.0f, 6.0f, -3.0f);
        glScalef(1.0f, 1.0f, 1.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-3.0f, 6.0f, -3.0f);
        glScalef(1.0f, 1.0f, 1.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-5.0f, 6.0f, -3.0f);
        glScalef(1.0f, 1.0f, 1.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-7.0f, 6.0f, -3.0f);
        glScalef(1.0f, 1.0f, 1.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-6.0f, 6.0f, -4.0f);
        glScalef(1.0f, 1.0f, 1.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(6.0f, 6.0f, -4.0f);
        glScalef(1.0f, 1.0f, 1.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-6.0f, 6.0f, 4.0f);
        glScalef(1.0f, 1.0f, 1.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(6.0f, 6.0f, 4.0f);
        glScalef(1.0f, 1.0f, 1.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(7.0f, 5.75f, 0.0f);
        glScalef(1.0f, 1.0f, 5.0f);
        slab(1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-7.0f, 5.75f, 0.0f);
        glScalef(1.0f, 1.0f, 5.0f);
        slab(1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-6.0f, 6.25f, 3.0f);
        glScalef(1.0f, 1.0f, 1.0f);
        slab(1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-6.0f, 6.25f, -3.0f);
        glScalef(1.0f, 1.0f, 1.0f);
        slab(1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(6.0f, 6.25f, 3.0f);
        glScalef(1.0f, 1.0f, 1.0f);
        slab(1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(6.0f, 6.25f, -3.0f);
        glScalef(1.0f, 1.0f, 1.0f);
        slab(1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(2.0f, 6.25f, 3.0f);
        glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
        glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
        stair(1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(4.0f, 6.25f, 3.0f);
        glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
        glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
        stair(1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-2.0f, 6.25f, 3.0f);
        glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
        glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
        stair(1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-4.0f, 6.25f, 3.0f);
        glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
        glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
        stair(1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(2.0f, 6.25f, -3.0f);
        glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
        glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
        stair(1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(4.0f, 6.25f, -3.0f);
        glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
        glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
        stair(1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-2.0f, 6.25f, -3.0f);
        glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
        glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
        stair(1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-4.0f, 6.25f, -3.0f);
        glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
        glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
        stair(1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-7.0f, 5.75f, 4.0f);
        glScalef(1.0f, 1.0f, 1.0f);
        slab(1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(7.0f, 5.75f, 4.0f);
        glScalef(1.0f, 1.0f, 1.0f);
        slab(1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-7.0f, 5.75f, -4.0f);
        glScalef(1.0f, 1.0f, 1.0f);
        slab(1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(7.0f, 5.75f, -4.0f);
        glScalef(1.0f, 1.0f, 1.0f);
        slab(1);
    glPopMatrix();
    //Atap Layer 2
    glPushMatrix();
        glTranslatef(-6.0f, 6.75f, 2.0f);
        glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
        stair(1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(0.0f, 6.75f, 2.0f);
        glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
        stair(1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(6.0f, 6.75f, 2.0f);
        glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
        stair(1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-6.0f, 6.75f, -2.0f);
        glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
        stair(1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(0.0f, 6.75f, -2.0f);
        glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
        stair(1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(6.0f, 6.75f, -2.0f);
        glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
        stair(1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-6.0f, 7.25f, -3.0f);
        glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
        glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
        stair(1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(6.0f, 7.25f, -3.0f);
        glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
        glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
        stair(1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-6.0f, 7.25f, 3.0f);
        glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
        glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
        stair(1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(6.0f, 7.25f, 3.0f);
        glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
        glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
        stair(1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(2.0f, 7.25f, 2.0f);
        glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
        glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
        stair(1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(4.0f, 7.25f, 2.0f);
        glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
        glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
        stair(1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-2.0f, 7.25f, 2.0f);
        glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
        glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
        stair(1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-4.0f, 7.25f, 2.0f);
        glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
        glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
        stair(1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(2.0f, 7.25f, -2.0f);
        glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
        glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
        stair(1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(4.0f, 7.25f, -2.0f);
        glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
        glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
        stair(1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-2.0f, 7.25f, -2.0f);
        glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
        glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
        stair(1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-4.0f, 7.25f, -2.0f);
        glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
        glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
        stair(1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(0.0f, 7.0f, -3.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(0.0f, 7.0f, 3.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(1.0f, 7.0f, -2.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(3.0f, 7.0f, -2.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(5.0f, 7.0f, -2.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(7.0f, 7.0f, -2.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-1.0f, 7.0f, -2.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-3.0f, 7.0f, -2.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-5.0f, 7.0f, -2.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-7.0f, 7.0f, -2.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(1.0f, 7.0f, 2.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(3.0f, 7.0f, 2.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(5.0f, 7.0f, 2.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(7.0f, 7.0f, 2.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-1.0f, 7.0f, 2.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-3.0f, 7.0f, 2.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-5.0f, 7.0f, 2.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-7.0f, 7.0f, 2.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(0.0f, 6.75f, -4.0f);
        glScalef(1.0f, 1.0f, 1.0f);
        slab(1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(0.0f, 6.75f, 4.0f);
        glScalef(1.0f, 1.0f, 1.0f);
        slab(1);
    glPopMatrix();
    //Atap Layer 3
    glPushMatrix();
        glTranslatef(-6.0f, 7.75f, 1.0f);
        glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
        stair(1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(0.0f, 7.75f, 1.0f);
        glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
        stair(1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(6.0f, 7.75f, 1.0f);
        glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
        stair(1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-6.0f, 7.75f, -1.0f);
        glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
        stair(1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(0.0f, 7.75f, -1.0f);
        glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
        stair(1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(6.0f, 7.75f, -1.0f);
        glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
        stair(1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-6.0f, 8.25f, -2.0f);
        glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
        glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
        stair(1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(6.0f, 8.25f, -2.0f);
        glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
        glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
        stair(1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-6.0f, 8.25f, 2.0f);
        glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
        glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
        stair(1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(6.0f, 8.25f, 2.0f);
        glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
        glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
        stair(1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(2.0f, 8.25f, 1.0f);
        glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
        glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
        stair(1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(4.0f, 8.25f, 1.0f);
        glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
        glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
        stair(1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-2.0f, 8.25f, 1.0f);
        glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
        glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
        stair(1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-4.0f, 8.25f, 1.0f);
        glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
        glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
        stair(1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(2.0f, 8.25f, -1.0f);
        glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
        glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
        stair(1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(4.0f, 8.25f, -1.0f);
        glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
        glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
        stair(1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-2.0f, 8.25f, -1.0f);
        glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
        glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
        stair(1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-4.0f, 8.25f, -1.0f);
        glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
        glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
        stair(1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(0.0f, 8.0f, -2.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(0.0f, 8.0f, 2.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(1.0f, 8.0f, -1.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(3.0f, 8.0f, -1.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(5.0f, 8.0f, -1.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(7.0f, 8.0f, -1.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-1.0f, 8.0f, -1.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-3.0f, 8.0f, -1.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-5.0f, 8.0f, -1.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-7.0f, 8.0f, -1.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(1.0f, 8.0f, 1.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(3.0f, 8.0f, 1.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(5.0f, 8.0f, 1.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(7.0f, 8.0f, 1.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-1.0f, 8.0f, 1.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-3.0f, 8.0f, 1.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-5.0f, 8.0f, 1.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-7.0f, 8.0f, 1.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    //Atap Layer 4
    glPushMatrix();
        glTranslatef(0.0f, 9.0f, 0.0f);
        glScalef(11.0f, 1.0f, 1.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(6.0f, 9.0f, 0.0f);
        glScalef(1.0f, 1.0f, 3.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-6.0f, 9.0f, 0.0f);
        glScalef(1.0f, 1.0f, 3.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(0.0f, 9.25f, -1.0f);
        glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
        glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
        stair(1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(0.0f, 9.25f, 1.0f);
        glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
        glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
        stair(1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(7.0f, 8.75f, 0.0f);
        glScalef(1.0f, 1.0f, 1.0f);
        slab(1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-7.0f, 8.75f, 0.0f);
        glScalef(1.0f, 1.0f, 1.0f);
        slab(1);
    glPopMatrix();
    //Atap Layer 5
    glPushMatrix();
        glTranslatef(0.0f, 10.0f, 0.0f);
        glScalef(15.0f, 1.0f, 1.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(7.0f, 10.25f, 1.0f);
        slab(1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(7.0f, 10.25f, -1.0f);
        slab(1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-7.0f, 10.25f, 1.0f);
        slab(1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-7.0f, 10.25f, -1.0f);
        slab(1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(7.0f, 10.75f, 0.0f);
        slab(1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-7.0f, 10.75f, 0.0f);
        slab(1);
    glPopMatrix();
    //Benteng
    glPushMatrix();
        glColor3ub(62, 48, 31);
        glTranslatef(11.25f, 3.0f, 0.0f);
        glScalef(7.5f, 1.0f, 1.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glColor3ub(234, 226, 218);
        glTranslatef(10.75f, 1.0f, 0.0f);
        glScalef(6.5f, 3.0f, 1.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glColor3ub(62, 48, 31);
        glTranslatef(-11.25f, 3.0f, 0.0f);
        glScalef(7.5f, 1.0f, 1.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
        glColor3ub(234, 226, 218);
        glTranslatef(-10.75f, 1.0f, 0.0f);
        glScalef(6.5f, 3.0f, 1.0f);
        glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix(); 
        glTranslatef(14.5f, 1.0f, 0.0f);
        glScalef(1.0f, 3.0f, 1.0f);
        cube();
    glPopMatrix();
    glPushMatrix(); 
        glTranslatef(-14.5f, 1.0f, 0.0f);
        glScalef(1.0f, 3.0f, 1.0f);
        cube();
    glPopMatrix();
    pintu();
}

void drawFloor() {
    glPushMatrix();
        glTranslatef(10.0f, -0.25f, 2.5);
        glScalef(8.0f, 0.5f, 31.0f);
        cube(6);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-10.0f, -0.25f, 2.5);
        glScalef(8.0f, 0.5f, 31.0f);
        cube(6);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(0.0f, -0.25f, 9.0);
        glScalef(12.0f, 0.5f, 18.0f);
        cube(6);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(0.0f, -0.25f, -12.5);
        glScalef(12.0f, 0.5f, 1.5f);
        cube(6);
    glPopMatrix();
}

// ---------------------------------- Bagian Akhir Regie Adhia Rozabbissalam ----------------------------------

// ------------------------ Kolam, Batu, Partikel, Lampu Toro, Lampu Garden M Asad Badriana ------------------------

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

void Kolam() {
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

    //bambu 
     glPushMatrix(); 
        glColor3ub(85, 107, 47);
        glTranslatef(0.0f, 0.4f, -10.0f); //Translasi
        glScalef( 0.05f, 0.05f, 1.05f);
        glutSolidCylinder(2.0, 2.0, 20.0, 20.0);
    glPopMatrix();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);
    glDisable(GL_CULL_FACE);
    //kolam
     glPushMatrix(); 
        glColor4ub(102.0, 238.0, 187.0, 180);
        glTranslatef(0.0f, -1.0f, -4.0f); //Translasi
        glScalef( 11.0f, 2.5f, 11.0f);
        glutSolidCube(1.0);
    glPopMatrix();
        
    //air dalam bambu
    glPushMatrix(); 
        glColor4ub(102.0, 238.0, 187.0, 128);
        glTranslatef(0.0f, 0.4f, -10.0f); //Translasi
        glScalef( 0.04f, 0.04f, 1.09f);
        glutSolidCylinder(2.0, 2.0, 20.0, 20.0);
    glPopMatrix();

    //air yang keluar dari bambu
    glPushMatrix(); 
    glColor4ub(102.0, 238.0, 187.0, 128);
    glTranslatef(0.0f, 0.4f, -7.85f); 
    glRotatef(45.0f, 1.0f, 0.0f, 0.0f);
    glScalef(0.04f, 0.04f, 0.19f);
    glutSolidCylinder(2.0, 2.0, 20.0, 20.0);
    glPopMatrix();

    glDepthMask(GL_TRUE);

}

// ---------------------------------- PARTIKEL ASAP ONSEN (sekarang berbentuk sphere)
struct Particle {
    float x, y, z;
    float vy;       // kecepatan vertikal
    float life;     // 1.0 = baru lahir, 0.0 = mati
    float size;     // ukuran sphere
};

std::vector<Particle> particles;
float lastBurstTime = 0.0f;
float nextBurstInterval = 7.0f;
bool randSeeded = false;

// Posisi sumber asap (di tengah scene, sedikit di atas lantai)
const float smokeSourceX = 3.0f;
const float smokeSourceY = -1.5f;   // mulai sedikit di atas lantai
const float smokeSourceZ = -3.0f;

void updateParticles() {
    float currentTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;

    // Burst baru setiap 5-10 detik
    if (currentTime - lastBurstTime >= nextBurstInterval) {
        lastBurstTime = currentTime;
        nextBurstInterval = 5.0f + static_cast<float>(rand()) / RAND_MAX * 5.0f;

        int numParticles = 70 + rand() % 25;  // 30-54 partikel per burst
        for (int i = 0; i < numParticles; ++i) {
            Particle p;
            p.x = smokeSourceX + (static_cast<float>(rand()) / RAND_MAX - 1.0f) * 8.0f;
            p.y = smokeSourceY;
            p.z = smokeSourceZ + (static_cast<float>(rand()) / RAND_MAX - 1.0f) * 8.0f;
            p.vy = 0.25f + static_cast<float>(rand()) / RAND_MAX * 0.35f;  // naik pelan
            p.life = 1.0f;
            p.size = 0.0008f + static_cast<float>(rand()) / RAND_MAX * 0.12f;  // ukuran awal kecil
            particles.push_back(p);
        }
    }

    // Update partikel
    for (size_t i = 0; i < particles.size(); ) {
        Particle& p = particles[i];

        p.y += p.vy * deltaTime * 1.5f;
        p.vy += 0.06f * deltaTime;          // sedikit mengambang ke atas
        p.life -= deltaTime * 0.1f;        // hidup sekitar 4-5 detik
        p.size += deltaTime * 0.007f;         // melebar seiring waktu

        if (p.life <= 0.0f) {
            particles.erase(particles.begin() + i);
        } else {
            ++i;
        }
    }
}

void renderSmoke() {
    if (particles.empty()) return;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glDepthMask(GL_FALSE);
    glDisable(GL_LIGHTING);

    for (const Particle& p : particles) {
        float alpha = p.life * 0.13f;
        glColor4f(0.94f, 0.97f, 1.0f, alpha);

        glPushMatrix();
        glTranslatef(p.x, p.y, p.z);
        glScalef(1.0f, 1.35f, 1.0f);
        glutSolidSphere(p.size * (0.35f + p.life * 0.65f), 10, 8);
        glPopMatrix();
    }

    glDepthMask(GL_TRUE);
    glEnable(GL_LIGHTING);
    glDisable(GL_BLEND);
}

// Fungsi untuk menggambar Lampu Taman Modern
void drawGardenLamp(float x, float y, float z, float scale) {
    glPushMatrix();
        glTranslatef(x, y, z);
        glScalef(scale, scale, scale);

        // --- 1. TIANG BESI (Batangan Bawah) ---
        glColor3ub(30, 30, 35); // Abu-abu gelap (besi tua)

        glPushMatrix();
            glRotatef(-90.0f, 1.0f, 0.0f, 0.0f); // Tegakkan silinder
            glutSolidCylinder(0.10, 3.0, 16, 8);   // Tiang utama
            // Dudukan bawah (base)
            glTranslatef(0.0f, 0.0f, -0.05f);
            glutSolidCylinder(0.30, 0.10, 16, 4);
        glPopMatrix();

        // --- 2. BOLA LAMPU (Bulb) - hanya visual, tanpa menyala ---
        glPushMatrix();
            glTranslatef(0.0f, 3.0f, 0.0f); // Posisi di ujung tiang
            // Bola lampu warna kuning-putih hangat (mati/tidak menyala)
            glColor3ub(240, 235, 200); // Warna kuning lembut agak pudar
            glutSolidSphere(0.50, 20, 20);
        glPopMatrix();

        // --- 3. TOPI/PENUTUP ATAS (opsional) ---
        glPushMatrix();
            glColor3ub(30, 30, 35); // Warna besi sama seperti tiang
            glTranslatef(0.0f, 2.5f, 0.0f);
            glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
            glutSolidCone(0.18, 0.25, 12, 3); // Topi agak lebih lebar
        glPopMatrix();

    glPopMatrix();
}

void lampu() {
    drawGardenLamp(12.0f, 0.0f, 15.0f, 1.0f); 
    drawGardenLamp(-12.0f, 0.0f, 15.0f, 1.0f); 
    drawGardenLamp(12.0f, 0.0f, 8.0f, 1.0f); 
    drawGardenLamp(-12.0f, 0.0f, 8.0f, 1.0f); 
    drawGardenLamp(12.0f, 0.0f, -10.0f, 1.0f); 
    drawGardenLamp(-12.0f, 0.0f, -10.0f, 1.0f); 
}

// ---------------------------------- Bagian Akhir M Asad Badriana ----------------------------------

// ---------------------------------- Dinding dan Cahaya Muhammad Rafli Ramdhani ----------------------------------
void Dinding() {
    //Dinding Belakang
    glPushMatrix(); 
        glTranslatef(14.5f, 1.5f, -13.5f);
        glScalef(1.5f, 5.0f, 1.5f);
        cube();
    glPopMatrix();
    glPushMatrix(); 
        glTranslatef(9.6667f, 1.5f, -13.5f);
        glScalef(1.5f, 5.0f, 1.5f);
        cube();
    glPopMatrix();
    glPushMatrix(); 
        glTranslatef(4.8333f, 1.5f, -13.5f);
        glScalef(1.5f, 5.0f, 1.5f);
        cube();
    glPopMatrix();
    glPushMatrix(); 
        glTranslatef(0.0f, 1.5f, -13.5f);
        glScalef(1.5f, 5.0f, 1.5f);
        cube();
    glPopMatrix();
    glPushMatrix(); 
        glTranslatef(-9.6667f, 1.5f, -13.5f);
        glScalef(1.5f, 5.0f, 1.5f);
        cube();
    glPopMatrix();
    glPushMatrix(); 
        glTranslatef(-4.8333f, 1.5f, -13.5f);
        glScalef(1.5f, 5.0f, 1.5f);
        cube();
    glPopMatrix();
    glPushMatrix(); 
        glTranslatef(-14.5f, 1.5f, -13.5f);
        glScalef(1.5f, 5.0f, 1.5f);
        cube();
    glPopMatrix();
    glPushMatrix(); 
        glTranslatef(14.5f, 1.5f, -7.625f);
        glScalef(1.5f, 5.0f, 1.5f);
        cube();
    glPopMatrix();
    glPushMatrix(); 
        glTranslatef(-14.5f, 1.5f, -7.625f);
        glScalef(1.5f, 5.0f, 1.5f);
        cube();
    glPopMatrix();
    glPushMatrix(); 
        glTranslatef(14.5f, 1.5f, -1.75f);
        glScalef(1.5f, 5.0f, 1.5f);
        cube();
    glPopMatrix();
    glPushMatrix(); 
        glTranslatef(-14.5f, 1.5f, -1.75f);
        glScalef(1.5f, 5.0f, 1.5f);
        cube();
    glPopMatrix();
    glPushMatrix(); 
        glTranslatef(14.5f, 1.5f, 4.125f);
        glScalef(1.5f, 5.0f, 1.5f);
        cube();
    glPopMatrix();
    glPushMatrix(); 
        glTranslatef(-14.5f, 1.5f, 4.125f);
        glScalef(1.5f, 5.0f, 1.5f);
        cube();
    glPopMatrix();
    glPushMatrix(); 
        glTranslatef(14.5f, 1.5f, 10.0f);
        glScalef(1.5f, 5.0f, 1.5f);
        cube();
    glPopMatrix();
    glPushMatrix(); 
        glTranslatef(-14.5f, 1.5f, 10.0f);
        glScalef(1.5f, 5.0f, 1.5f);
        cube();
    glPopMatrix();
    glPushMatrix(); 
        glTranslatef(0.0f, 3.0f, -13.5f);
        glScalef(30.0f, 1.0f, 1.0f);
        cube();
    glPopMatrix();
    glPushMatrix(); 
        glTranslatef(0.0f, 1.0f, -13.5f);
        glScalef(28.0f, 3.0f, 1.0f);
        cube(4);
    glPopMatrix();
    //Dinding Kiri
    glPushMatrix(); 
        glTranslatef(-14.5f, 3.0f, -1.75f);
        glScalef(1.0f, 1.0f, 22.5f);
        cube();
    glPopMatrix();
    glPushMatrix(); 
        glTranslatef(-14.5f, 1.0f, -1.75f);
        glScalef(1.0f, 3.0f, 22.5f);
        cube(4);
    glPopMatrix();
    //Dinding Kanan
    glPushMatrix(); 
        glTranslatef(14.5f, 3.0f, -1.75f);
        glScalef(1.0f, 1.0f, 22.5f);
        cube();
    glPopMatrix();
    glPushMatrix(); 
        glTranslatef(14.5f, 1.0f, -1.75f);
        glScalef(1.0f, 3.0f, 22.5f);
        cube(4);
    glPopMatrix();
}

// 1. Tambahkan di bagian global (dekat konstanta lain)
#define MAX_LIGHTS 6

float lightX[MAX_LIGHTS] = { 12.0f,  -12.0f, 12.0f,  -12.0f,  12.0f,  -12.0f };
float lightY[MAX_LIGHTS] = { 3.0f,  3.0f,  3.0f,  3.0f,  3.0f,  3.0f };
float lightZ[MAX_LIGHTS] = { 15.0f,  15.0f,  8.0f,  8.0f, -10.0f, -10.0f };

bool lightIsSpot[MAX_LIGHTS] = { false, false, false, false, false, false }; // true = spotlight

float spotCutoff = 35.0f;    // bisa diubah-ubah nanti
float spotExponent = 5.0f;

int totalLights = 6;


// 2. Fungsi updateLighting (copy utuh)
void updateLighting() {
    glEnable(GL_LIGHTING);

    // Matikan semua light dulu (aman)
    for (int i = 0; i < 8; i++) {
        glDisable(GL_LIGHT0 + i);
    }

    for (int i = 0; i < totalLights; i++) {
        GLenum light = GL_LIGHT0 + i;
        glEnable(light);

        GLfloat lightColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
        GLfloat ambient[]    = {0.05f, 0.05f, 0.08f, 1.0f};

        glLightfv(light, GL_DIFFUSE,  lightColor);
        glLightfv(light, GL_SPECULAR, lightColor);
        glLightfv(light, GL_AMBIENT,  ambient);

        GLfloat pos[] = {lightX[i], lightY[i], lightZ[i], 1.0f};
        glLightfv(light, GL_POSITION, pos);

        if (lightIsSpot[i]) {
            GLfloat dir[] = {0.0f, -1.0f, 0.0f};
            glLightfv(light, GL_SPOT_DIRECTION, dir);
            glLightf(light, GL_SPOT_CUTOFF, spotCutoff);
            glLightf(light, GL_SPOT_EXPONENT, spotExponent);
        } else {
            glLightf(light, GL_SPOT_CUTOFF, 45.0f);
        }

        // Attenuation supaya realistis
        glLightf(light, GL_CONSTANT_ATTENUATION, 1.0f);
        glLightf(light, GL_LINEAR_ATTENUATION, 0.17f);
        glLightf(light, GL_QUADRATIC_ATTENUATION, 0.035f);
    }
}


// 3. (Opsional) Visualisasi sumber cahaya — bagus untuk debug
void drawLightSource(float x, float y, float z, bool isSpot) {
    glDisable(GL_LIGHTING);
    glPushMatrix();
    glTranslatef(x, y, z);
    if (isSpot) {
        glColor3f(1.0f, 1.0f, 0.3f);
        glRotatef(180, 1, 0, 0);
        glRotatef(90, 1, 0, 0);
        glutWireCone(0.4, 1.2, 12, 8);
    } else {
        glColor3f(0.9f, 0.9f, 1.0f);
        glutSolidSphere(0.55, 20, 20);
    }
    glPopMatrix();
    glEnable(GL_LIGHTING);
}

// ---------------------------------- Bagian Akhir Muhammad Rafli Ramdhani ----------------------------------

// ---------------------------------- pergerakan kamera
void updateCameraMovement() {
    float speed = MOVE_SPEED * deltaTime;

    if (currentMode == FREE_CAMERA) {
        // Mode Free Camera (FPS style) - kode asli kamu
        float yawRad   = cameraYaw   * PI / 180.0f;
        float pitchRad = cameraPitch * PI / 180.0f;

        float frontX = sin(yawRad) * cos(pitchRad);
        float frontY = sin(pitchRad);
        float frontZ = -cos(yawRad) * cos(pitchRad);

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
    else {  // ORBIT_CAMERA
        // Kontrol manual tetap ada (opsional)
        if (keys['w'] || keys['W']) orbitPitch += 40.0f * deltaTime;
        if (keys['s'] || keys['S']) orbitPitch -= 40.0f * deltaTime;
        if (keys['a'] || keys['A']) orbitYaw   -= 80.0f * deltaTime;
        if (keys['d'] || keys['D']) orbitYaw   += 80.0f * deltaTime;

        // Zoom in/out
        if (keys['q'] || keys['Q']) orbitRadius += 6.0f * deltaTime;
        if (keys['e'] || keys['E']) orbitRadius -= 6.0f * deltaTime;

        // ── ROTASI OTOMATIS ───────────────────────────────
        if (autoRotateEnabled) {
            orbitYaw += autoRotateSpeed * deltaTime;
            // Opsional: batasi agar tidak terlalu besar (hindari overflow lama kelamaan)
            // orbitYaw = fmod(orbitYaw, 360.0f);
        }
        // ──────────────────────────────────────────────────

        // Clamp nilai supaya tetap nyaman
        if (orbitPitch > 85.0f)  orbitPitch = 85.0f;
        if (orbitPitch < -85.0f) orbitPitch = -85.0f;
        if (orbitRadius < 2.0f)  orbitRadius = 2.0f;
        if (orbitRadius > 30.0f) orbitRadius = 30.0f;

        // Hitung posisi kamera dari sudut orbit
        float yawRad   = orbitYaw   * PI / 180.0f;
        float pitchRad = orbitPitch * PI / 180.0f;

        cameraPosX = orbitCenterX + orbitRadius * sin(yawRad) * cos(pitchRad);
        cameraPosY = orbitCenterY + orbitRadius * sin(pitchRad);
        cameraPosZ = orbitCenterZ + orbitRadius * cos(yawRad) * cos(pitchRad);
    }
}

// ---------------------------------- GLUT callbacks
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Update pencahayaan DULUAN sebelum view matrix
    updateLighting();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (double)windowWidth / windowHeight, 0.1, 100.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    if (currentMode == FREE_CAMERA) {
        // perhitungan lookAt seperti sebelumnya (arah depan)
        float yawRad   = cameraYaw * PI / 180.0f;
        float pitchRad = cameraPitch * PI / 180.0f;
        gluLookAt(cameraPosX, cameraPosY, cameraPosZ,
                  cameraPosX + sin(yawRad)*cos(pitchRad),
                  cameraPosY + sin(pitchRad),
                  cameraPosZ - cos(yawRad)*cos(pitchRad),
                  0,1,0);
    } else {
        // Orbit: selalu menghadap tepat ke pusat
        gluLookAt(cameraPosX, cameraPosY, cameraPosZ,
                  orbitCenterX, orbitCenterY, orbitCenterZ,
                  0,1,0);
    }

    drawFloor();

    // Opsional: tampilkan visualisasi lampu (untuk debug)
    for (int i = 0; i < totalLights; i++) {
        drawLightSource(lightX[i], lightY[i], lightZ[i], lightIsSpot[i]);
    }
    glPushMatrix();
        glTranslatef(0.0f, 0.0f, 10.0f);
        Bangunan();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(0.0f, 0.0f, -10.0f);
        glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
        Kolam();
    glPopMatrix();
    lampu();
    glPushMatrix();
        Dinding();
    glPopMatrix();
    renderSmoke();   // asap tetap di akhir supaya transparan benar

    glutSwapBuffers();
}

// ---------------------------------- inisialisasi OpenGL
void init() {
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);           // tetap nyalakan lighting secara global
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glShadeModel(GL_SMOOTH);

    // Material default yang bagus (akan dipakai semua objek kecuali di-override)
    GLfloat default_specular[]  = {0.7f, 0.7f, 0.7f, 1.0f};
    GLfloat default_shininess[] = {35.0f};   // agak lebih lembut dari 50
    glMaterialfv(GL_FRONT, GL_SPECULAR, default_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, default_shininess);

    // Inisialisasi random (tetap dipertahankan)
    if (!randSeeded) {
        srand(static_cast<unsigned>(time(NULL)));
        randSeeded = true;
    }
}

void mouseMotion(int x, int y) {
    if (!mouseCaptured) return;

    int centerX = windowWidth / 2;
    int centerY = windowHeight / 2;
    float deltaX = (x - centerX) * MOUSE_SENSITIVITY;
    float deltaY = (y - centerY) * MOUSE_SENSITIVITY;

    if (currentMode == FREE_CAMERA) {
        cameraYaw   += deltaX;
        cameraPitch -= deltaY;
        if (cameraPitch > 89.0f)  cameraPitch = 89.0f;
        if (cameraPitch < -89.0f) cameraPitch = -89.0f;
    }
    // else {  // ORBIT_CAMERA
    //     orbitYaw   += deltaX * 1.5f;   // lebih responsif di orbit
    //     orbitPitch -= deltaY * 1.2f;
    //     if (orbitPitch > 85.0f)  orbitPitch = 85.0f;
    //     if (orbitPitch < -85.0f) orbitPitch = -85.0f;
    // }

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

    if (key == 'm') {  // Tekan M untuk switch mode
        if (currentMode == FREE_CAMERA) {
            currentMode = ORBIT_CAMERA;
            printf("Switched to ORBIT CAMERA\n");
            // Opsional: reset orbit ke posisi awal
            orbitYaw = 0;
            orbitPitch = 20;
            orbitRadius = 10;
        } else {
            currentMode = FREE_CAMERA;
            printf("Switched to FREE CAMERA\n");
        }
    }
    if (key == 'f') {
        // Toggle buka / tutup
        doorTargetAngle = (doorTargetAngle == 0.0f) ? 90.0f : 0.0f;
    }

    if (key == 27) {  // ESC
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
    updateParticles();
    updateDoor(deltaTime);

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
    glutCreateWindow("TB Kelompok 4 - Onsen Jepang dengan Asap");

    glutFullScreen(); // uncomment jika ingin fullscreen

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