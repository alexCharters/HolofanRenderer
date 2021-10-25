#include <GL/gl.h>
#include <GL/glut.h>
#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <string>
#include <cstring>
#include <iostream>
#include <iomanip>

GLfloat objAngle = 0.0;
GLint lines = 1024;
GLfloat resolution = 360.0/lines;
bool isShown = false;

float distance = 10;
float elevation = 3;

glm::vec3 modelPosition = glm::vec3(0.0);
glm::vec3 cameraPosition = glm::vec3(0.0,3.0,10.0);

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
    //glEnable (GL_DEPTH_TEST);
    glEnable (GL_LIGHTING);
    glEnable (GL_LIGHT0);
}

void light (void) {
    //glLightfv(GL_LIGHT0, GL_SPECULAR, whiteSpecularLight);
    //glLightfv(GL_LIGHT0, GL_AMBIENT, blackAmbientLight);
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
    //glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClear (GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    light();
    //glTranslatef(0,0,0);
    
    //glRotatef(10,1,0,0);

    //glRotatef(objAngle,0,1,0);
    cameraPosition = glm::rotateY(cameraPosition, glm::radians(1.0f));	
    objAngle ++;
    gluLookAt(cameraPosition.x,cameraPosition.y,cameraPosition.z,modelPosition.x,modelPosition.y,modelPosition.z,0,1,0);
    
    glm::vec3 rotation_axis = modelPosition-cameraPosition;

    for(int camAngle = 0; camAngle < lines; camAngle++){
        glClear (GL_COLOR_BUFFER_BIT );
        
        glRotatef(resolution, rotation_axis.x, rotation_axis.y, rotation_axis.z);
        //std::cout << camAngle << " ";
        
        glutSolidCube(2);
        if(isShown){
            glutSwapBuffers();
        }
        
        GLubyte *data = (GLubyte*)malloc(3 * 128);
        glReadBuffer(GL_BACK);
        glReadPixels(0,0,128,1, GL_RGB, GL_UNSIGNED_BYTE, data);
        
        //print_bytes(std::cout, "frame", data, 3 * 128);
    }
    std::cout << "frame" << std::flush;
}

void reshape (int w, int h) {
    if(!isShown){
        //glViewport (0, 0, (GLsizei)128, (GLsizei)1);
        glMatrixMode (GL_PROJECTION);
        glLoadIdentity ();
        gluPerspective (1, (GLfloat)128 / (GLfloat)1, 1.0, 100.0);
        glMatrixMode (GL_MODELVIEW);
    }
    else{
        glViewport (0, 0, (GLsizei)w, (GLsizei)h);
        glMatrixMode (GL_PROJECTION);
        glLoadIdentity ();
        gluPerspective (60, (GLfloat)w / (GLfloat)h, 1.0, 100.0);
        glMatrixMode (GL_MODELVIEW);
    }
}

//void keyboard (unsigned char key, int x, int y) {
    //if (key=='s')
    //{
        //if (specular==false)
        //{
            //specular = true;
            //glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, 
//whiteSpecularMaterial);
            //glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mShininess);
        //}
        //else if (specular==true)
        //{
            //specular = false;
            //glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, blankMaterial);
            //glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, 
//blankMaterial);
        //}
    //}
    
    //if (key=='d')
    //{
        //if (diffuse==false)
        //{
            //diffuse = true;
            
        //}
        //else if (diffuse==true)
        //{
            //diffuse = false;
            //glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, blankMaterial);
        //}
    //}
    
    //if (key=='e')
    //{
        //if (emissive==false)
        //{
            //emissive = true;
            //glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, 
//greenEmissiveMaterial);
        //}
        //else if (emissive==true)
        //{
            //emissive = false;
            //glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, blankMaterial);
        //}
    //}
//}

int main (int argc, char **argv) {
    if(argc != 2){
        glutInit (&argc, argv);
        glutInitDisplayMode (GLUT_DOUBLE | GLUT_DEPTH);
        glutInitWindowSize (128, 1);
        glutCreateWindow ("A basic OpenGL Window");
        init ();
        glutDisplayFunc (display);
        glutIdleFunc (display);
        //glutKeyboardFunc (keyboard);
        glutReshapeFunc (reshape);
        glClearColor (0.0,0.0,0.0,1.0);
        
        
        //glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, whiteSpecularMaterial);
        //glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mShininess);
        //glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, redDiffuseMaterial);
        
        glutMainLoop ();
        return 0;
    }
    else if(std::strcmp(argv[1], "show") == 0){
        isShown = true;
        glutInit (&argc, argv);
        glutInitDisplayMode (GLUT_DOUBLE | GLUT_DEPTH);
        glutInitWindowSize (500, 500);
        glutCreateWindow ("A basic OpenGL Window");
        init ();
        glutDisplayFunc (display);
        glutIdleFunc (display);
        //glutKeyboardFunc (keyboard);
        glutReshapeFunc (reshape);
        glClearColor (0.0,0.0,0.0,1.0);
        
        
        //glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, whiteSpecularMaterial);
        //glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mShininess);
        //glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, redDiffuseMaterial);
        
        glutMainLoop ();
        return 0;

    }
}
