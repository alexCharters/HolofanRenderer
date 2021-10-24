#include <GL/gl.h>
#include <GL/glut.h>
#include <string>
#include <iostream>
#include <iomanip>

GLfloat objAngle = 0.0;
GLint camAngle = 0;
GLfloat resolution = 360.0/1024.0;

GLfloat redDiffuseMaterial[] = {1.0, 0.0, 0.0}; //set the material to red
GLfloat whiteSpecularMaterial[] = {1.0, 1.0, 1.0}; //set the material to white
GLfloat greenEmissiveMaterial[] = {0.0, 1.0, 0.0}; //set the material to green
GLfloat whiteSpecularLight[] = {1.0, 1.0, 1.0}; //set the light specular to white
GLfloat blackAmbientLight[] = {0.0, 0.0, 0.0}; //set the light ambient to black
GLfloat whiteDiffuseLight[] = {1.0, 1.0, 1.0}; //set the diffuse light to white
GLfloat blankMaterial[] = {0.0, 0.0, 0.0}; //set the diffuse light to white
GLfloat mShininess[] = {128}; //set the shininess of the material

bool diffuse = false;
bool emissive = false;
bool specular = false;

void init (void) {
    glEnable (GL_DEPTH_TEST);
    glEnable (GL_LIGHTING);
    glEnable (GL_LIGHT0);
}

void light (void) {
    glLightfv(GL_LIGHT0, GL_SPECULAR, whiteSpecularLight);
    glLightfv(GL_LIGHT0, GL_AMBIENT, blackAmbientLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, whiteDiffuseLight);
}

void print_bytes(std::ostream& out, const char *title, const unsigned char *data, size_t dataLen, bool format = true) {
    out << title << std::endl;
    out << std::setfill('0');
    for(size_t i = 0; i < dataLen; ++i) {
        out << std::hex << std::setw(2) << (int)data[i];
        if (format) {
            out << (((i + 1) % 16 == 0) ? "\n" : " ");
        }
    }
    out << std::endl;
}

void display (void) {
    glClearColor (0.0,0.0,0.0,1.0);
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    light();
    glTranslatef(0,0,-5);
    
    //glRotatef(10,1,0,0);
    if(camAngle == 1024){
        glRotatef(objAngle,0,1,0);
        objAngle ++;
        camAngle = 0;
        std::cout << "frame";
    }
    glRotatef(camAngle*resolution,0,0,1);
    //std::cout << camAngle << " ";
    camAngle ++;
    
    glutSolidTeapot(2);
    glutSwapBuffers();
    
    GLubyte *data = (GLubyte*)malloc(3 * 128);
    glReadBuffer(GL_BACK);
    glReadPixels(0,0,128,1, GL_RGB, GL_UNSIGNED_BYTE, data);
    
    //print_bytes(std::cout, "frame", data, 3 * 128);
    
    
}

void reshape (int w, int h) {
    glViewport (0, 0, (GLsizei)128, (GLsizei)1);
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    gluPerspective (1, (GLfloat)128 / (GLfloat)1, 1.0, 100.0);
    glMatrixMode (GL_MODELVIEW);
    
    //glViewport (0, 0, (GLsizei)w, (GLsizei)h);
    //glMatrixMode (GL_PROJECTION);
    //glLoadIdentity ();
    //gluPerspective (60, (GLfloat)w / (GLfloat)h, 1.0, 100.0);
    //glMatrixMode (GL_MODELVIEW);
}

void keyboard (unsigned char key, int x, int y) {
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

int main (int argc, char **argv) {
    glutInit (&argc, argv);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize (128, 1);
    glutInitWindowPosition (100, 100);
    glutCreateWindow ("A basic OpenGL Window");
    init ();
    glutDisplayFunc (display);
    glutIdleFunc (display);
    glutKeyboardFunc (keyboard);
    glutReshapeFunc (reshape);
    
    glViewport (0, 0, (GLsizei)128, (GLsizei)1);
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    gluPerspective (1, (GLfloat)128 / (GLfloat)1, 1.0, 100.0);
    glMatrixMode (GL_MODELVIEW);
    
    glutMainLoop ();
    return 0;
}
