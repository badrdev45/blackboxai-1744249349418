#include <GL/glut.h>
#include <cmath>

// Camera position and orientation
float cameraX = 0.0f;
float cameraY = 1.7f;  // Eye level
float cameraZ = 5.0f;
float cameraAngleX = 0.0f;
float cameraAngleY = 0.0f;

// Movement speed and mouse sensitivity
const float moveSpeed = 0.1f;
const float mouseSensitivity = 0.2f;

// Mouse state
int lastMouseX = 0;
int lastMouseY = 0;
bool firstMouse = true;

// Keyboard state
bool keys[256] = {false};

void init() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
}

void drawCube(float x, float y, float z) {
    glPushMatrix();
    glTranslatef(x, y, z);
    glBegin(GL_QUADS);
    
    // Front face (red)
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(-0.5f, -0.5f, 0.5f);
    glVertex3f(0.5f, -0.5f, 0.5f);
    glVertex3f(0.5f, 0.5f, 0.5f);
    glVertex3f(-0.5f, 0.5f, 0.5f);

    // Back face (green)
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(-0.5f, 0.5f, -0.5f);
    glVertex3f(0.5f, 0.5f, -0.5f);
    glVertex3f(0.5f, -0.5f, -0.5f);

    // Top face (blue)
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-0.5f, 0.5f, -0.5f);
    glVertex3f(-0.5f, 0.5f, 0.5f);
    glVertex3f(0.5f, 0.5f, 0.5f);
    glVertex3f(0.5f, 0.5f, -0.5f);

    // Bottom face (yellow)
    glColor3f(1.0f, 1.0f, 0.0f);
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(0.5f, -0.5f, -0.5f);
    glVertex3f(0.5f, -0.5f, 0.5f);
    glVertex3f(-0.5f, -0.5f, 0.5f);

    // Right face (magenta)
    glColor3f(1.0f, 0.0f, 1.0f);
    glVertex3f(0.5f, -0.5f, -0.5f);
    glVertex3f(0.5f, 0.5f, -0.5f);
    glVertex3f(0.5f, 0.5f, 0.5f);
    glVertex3f(0.5f, -0.5f, 0.5f);

    // Left face (cyan)
    glColor3f(0.0f, 1.0f, 1.0f);
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(-0.5f, -0.5f, 0.5f);
    glVertex3f(-0.5f, 0.5f, 0.5f);
    glVertex3f(-0.5f, 0.5f, -0.5f);

    glEnd();
    glPopMatrix();
}

void drawScene() {
    // Draw floor
    glBegin(GL_QUADS);
    glColor3f(0.5f, 0.5f, 0.5f);
    for(int x = -10; x < 10; x++) {
        for(int z = -10; z < 10; z++) {
            glVertex3f(x, 0, z);
            glVertex3f(x + 1, 0, z);
            glVertex3f(x + 1, 0, z + 1);
            glVertex3f(x, 0, z + 1);
        }
    }
    glEnd();

    // Draw some cubes
    drawCube(0.0f, 0.5f, -2.0f);
    drawCube(-2.0f, 0.5f, -2.0f);
    drawCube(2.0f, 0.5f, -2.0f);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Apply camera transformation
    glRotatef(cameraAngleX, 1.0f, 0.0f, 0.0f);
    glRotatef(cameraAngleY, 0.0f, 1.0f, 0.0f);
    glTranslatef(-cameraX, -cameraY, -cameraZ);

    drawScene();

    // Draw crosshair
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_LINES);
    glVertex2f(-0.02f, 0.0f);
    glVertex2f(0.02f, 0.0f);
    glVertex2f(0.0f, -0.02f);
    glVertex2f(0.0f, 0.02f);
    glEnd();
    
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glutSwapBuffers();
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (float)w / h, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int x, int y) {
    keys[key] = true;
    
    if (key == 27) { // ESC key
        exit(0);
    }
}

void keyboardUp(unsigned char key, int x, int y) {
    keys[key] = false;
}

void mouseMotion(int x, int y) {
    if (firstMouse) {
        lastMouseX = x;
        lastMouseY = y;
        firstMouse = false;
        return;
    }

    float deltaX = x - lastMouseX;
    float deltaY = y - lastMouseY;
    lastMouseX = x;
    lastMouseY = y;

    cameraAngleY += deltaX * mouseSensitivity;
    cameraAngleX += deltaY * mouseSensitivity;

    // Limit vertical rotation
    if (cameraAngleX > 90.0f) cameraAngleX = 90.0f;
    if (cameraAngleX < -90.0f) cameraAngleX = -90.0f;

    glutPostRedisplay();
}

void update(int value) {
    float angleRad = cameraAngleY * 3.14159f / 180.0f;
    float forwardX = sin(angleRad);
    float forwardZ = -cos(angleRad);

    // Handle WASD movement
    if (keys['w']) {
        cameraX += forwardX * moveSpeed;
        cameraZ += forwardZ * moveSpeed;
    }
    if (keys['s']) {
        cameraX -= forwardX * moveSpeed;
        cameraZ -= forwardZ * moveSpeed;
    }
    if (keys['a']) {
        cameraX -= cos(angleRad) * moveSpeed;
        cameraZ -= sin(angleRad) * moveSpeed;
    }
    if (keys['d']) {
        cameraX += cos(angleRad) * moveSpeed;
        cameraZ += sin(angleRad) * moveSpeed;
    }
    if (keys[' ']) { // Space for jump
        cameraY += moveSpeed;
    }
    if (keys['c']) { // C for crouch
        cameraY -= moveSpeed;
    }

    glutPostRedisplay();
    glutTimerFunc(16, update, 0); // 60 FPS
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("FPS Game");

    init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardUp);
    glutPassiveMotionFunc(mouseMotion);
    glutSetCursor(GLUT_CURSOR_NONE); // Hide cursor
    glutWarpPointer(400, 300); // Center cursor

    glutTimerFunc(0, update, 0);

    glutMainLoop();
    return 0;
}
