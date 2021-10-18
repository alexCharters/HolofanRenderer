#include <GL/glew.h> // Include the GLEW header file  
#include <GL/glut.h> // Include the GLUT header file  

bool* keyStates = new bool[256]; // Create an array of boolean values of length 256 (0-255)

bool movingUp = false; // Whether or not we are moving up or down
float yLocation = 0.0f; // Keep track of our position on the y axis.

float yRotationAngle = 0.0f; // The angle of rotation for our object

GLfloat redDiffuseMaterial[] = {1.0, 0.0, 0.0};
GLfloat whiteSpecularMaterial[] = {1.0, 1.0, 1.0};
GLfloat greenEmissiveMaterial[] = {0.0, 1.0, 0.0};

GLfloat whiteSpecularLight[] = {1.0, 1.0, 1.0};
GLfloat blackAmbientLight[] = {0.0, 0.0, 0.0};
GLfloat whiteDiffuseLight[] = {1.0, 1.0, 1.0};

GLfloat blankMaterial[] = {0.0, 0.0, 0.0}; //set the diffuse light to white
GLfloat mShininess[] = {128}; //set the shininess of the material

bool diffuse = false;
bool emissive = false;
bool specular = false;

void init (void) {
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_BLEND); // Enable the OpenGL Blending functionality
    glEnable (GL_DEPTH_TEST);
    glEnable (GL_LIGHTING);
    glEnable (GL_LIGHT0);
}

void keyOperations (void) {
    if (keyStates['a']) { // If the 'a' key has been pressed

    }
}

void renderPrimitive (void) {
    glBegin(GL_QUADS); // Start drawing a quad primitive

    glColor3f(1.0f, 1.0f, 1.0f);
    glVertex3f(-1.0f, -1.0f, 0.0f); // The bottom left corner
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(-1.0f, 1.0f, 0.0f); // The top left corner
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(1.0f, 1.0f, 0.0f); // The top right corner
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(1.0f, -1.0f, 0.0f); // The bottom right corner

    glEnd();
}

void display (void) {
    keyOperations();


    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Set the blend mode to blend our current RGBA with what is already in the buffer

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Clear the colour buffer (more buffers later on)
    glLoadIdentity(); // Load the Identity Matrix to reset our drawing locations

    glLightfv(GL_LIGHT0, GL_SPECULAR, whiteSpecularLight);
    glLightfv(GL_LIGHT0, GL_AMBIENT, blackAmbientLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, whiteDiffuseLight);

    glTranslatef(0.0f, 0.0f, -10.0f);

    renderPrimitive();

    glTranslatef(0.0f, 0.0f, 5.0f);
    glTranslatef(0.0f, yLocation, 0.0f); // Translate our object along the y axis
    glRotatef(yRotationAngle, 0.0f, 1.0f, 0.0f); // Rotate our object around the y axis
    glColor3f(1.0f, 0.0f, 0.0f);

    glutSolidTeapot(1.0);

    glutSwapBuffers(); // Flush the OpenGL buffers to the window

    if (movingUp) // If we are moving up
        yLocation -= 0.005f; // Move up along our yLocation
    else  // Otherwise
        yLocation += 0.005f; // Move down along our yLocation

    if (yLocation < -1.0f) // If we have gone up too far
        movingUp = false; // Reverse our direction so we are moving down
    else if (yLocation > 1.0f) // Else if we have gone down too far
        movingUp = true; // Reverse our direction so we are moving up

    yRotationAngle += 0.5f; // Increment our rotation value
    if (yRotationAngle > 360.0f) // If we have rotated beyond 360 degrees (a full rotation)
        yRotationAngle -= 360.0f; // Subtract 360 degrees off of our rotation
}

void reshape (int width, int height) {
    glViewport(0, 0, (GLsizei)width, (GLsizei)height); // Set our viewport to the size of our window
    glMatrixMode(GL_PROJECTION); // Switch to the projection matrix so that we can manipulate how our scene is viewed
    glLoadIdentity(); // Reset the projection matrix to the identity matrix so that we don't get any artifacts (cleaning up)
    gluPerspective(60, (GLfloat)width / (GLfloat)height, 1.0, 100.0); // Set the Field of view angle (in degrees), the aspect ratio of our window, and the new and far planes
    glMatrixMode(GL_MODELVIEW); // Switch back to the model view matrix, so that we can start drawing shapes correctly
}

void keyPressed (unsigned char key, int x, int y) {
    keyStates[key] = true; // Set the state of the current key to pressed

    if (key=='s')
    {
        if (specular==false)
        {
            specular = true;
            glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,
                         whiteSpecularMaterial);
            glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mShininess);
        }
        else if (specular==true)
        {
            specular = false;
            glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, blankMaterial);
            glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS,
                         blankMaterial);
        }
    }

    if (key=='d')
    {
        if (diffuse==false)
        {
            diffuse = true;
            glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,
                         redDiffuseMaterial);
        }
        else if (diffuse==true)
        {
            diffuse = false;
            glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, blankMaterial);
        }
    }

    if (key=='e')
    {
        if (emissive==false)
        {
            emissive = true;
            glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION,
                         greenEmissiveMaterial);
        }
        else if (emissive==true)
        {
            emissive = false;
            glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, blankMaterial);
        }
    }
}

void keyUp (unsigned char key, int x, int y) {
    keyStates[key] = false; // Set the state of the current key to not pressed
}

int main (int argc, char **argv) {
    glutInit(&argc, argv); // Initialize GLUT
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGBA); // Set up a basic display buffer (now double buffered)
    glutInitWindowSize (500, 500); // Set the width and height of the window
    glutInitWindowPosition (100, 100); // Set the position of the window
    glutCreateWindow ("Your first OpenGL Window"); // Set the title for the window

    init();

    glutDisplayFunc(display); // Tell GLUT to use the method "display" for rendering
    glutIdleFunc(display);
    glutReshapeFunc(reshape); // Tell GLUT to use the method "reshape" for reshaping

    glutKeyboardFunc(keyPressed); // Tell GLUT to use the method "keyPressed" for key presses
    glutKeyboardUpFunc(keyUp); // Tell GLUT to use the method "keyUp" for key up events

    glutMainLoop(); // Enter GLUT's main loop
    return 0;
}
