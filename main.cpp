#include <GL/glew.h>
#include "GL/freeglut.h"
#include <GL/gl.h>
#include <GL/glut.h>

#include <vector>
#include <stdio.h>
#include <iostream>
#include <math.h>   // fabs
#include "Fan.h"

#include "CImg.h"

#include <chrono>
using namespace std::chrono;
using namespace cimg_library;
using namespace std;

Fan fan;

static double zoom = .01;
static int width = 0;
static int height = 0;
static float tx=0, ty=0;
static float thetax=0, thetay=0, thetaz=0;

enum DrawType
{
    kImmediate,
    kVertexArray,
    kVBO,
    kShaderVBO,
    kShaderVAO
};
DrawType drawType = kImmediate;

GLuint framebufferTexture;
GLuint renderbuffer;
GLuint framebuffer;
GLuint originalFramebuffer;

GLuint* vboIds = NULL;
GLuint* vaoIds = NULL;
GLuint phongProgram = 0;
GLuint texProgram = 0;
GLuint VERTEX_ATTR_COORDS = 1;
GLuint VERTEX_ATTR_COLOR = 2;

const int winWidth = 500;
const int winHeight = 500;

GLfloat outBuffer[128*512*3];
GLfloat initOutBuffer[500*500*3];

const int nCoordsComponents = 3;
const int nColorComponents = 3;
const int nLines = 3;
const int nVerticesPerLine = 2;
const int nFaces = 6;
const int nVerticesPerFace = 3;

// =========== Axis Data ======================================================

//      Y
//      |           Z
//      |         /
//      |       /
//      |     /
//      |   /
//      | /
//      /--------------
//      O              X


float av[] = { 0.0, 0.0, 0.0,    // origin
               2.0, 0.0, 0.0,    // x-axis
               0.0, 2.0, 0.0,    // y-axis
               0.0, 0.0, 2.0 };  // z-axis

GLubyte avi[] = { 0, 1,
                  0, 2,
                  0, 3 };

float ac[] = { 1.0, 0.0, 0.0,    // red   x-axis
               0.0, 1.0, 0.0,    // green y-axis
               0.0, 0.0, 1.0 };  // blue  z-axis

GLubyte aci[] = { 0, 0,
                  1, 1,
                  2, 2 };

float ave[nLines*nVerticesPerLine*nCoordsComponents];
void expandAxesVertices()
{
    for (int i=0; i<6; i++)
    {
        ave[i*3+0] = av[avi[i]*3+0];
        ave[i*3+1] = av[avi[i]*3+1];
        ave[i*3+2] = av[avi[i]*3+2];
    }
}

float ace[nLines*nVerticesPerLine*nColorComponents];
void expandAxesColors()
{
    for (int i=0; i<6; i++)
    {
        ace[i*3+0] = ac[aci[i]*3+0];
        ace[i*3+1] = ac[aci[i]*3+1];
        ace[i*3+2] = ac[aci[i]*3+2];
    }
}

// =========== Pyramid Data =================================================

//  (3,4,5)          (6,7,8)
//     1----------------2
//     | \            / |
//     |   \        /   |
//     |     \    /     |
//     |        4       | (12,13,14)
//     |     /    \     |
//     |   /        \   |
//     | /            \ |
//     0 ---------------3
//  (0,1,2)          (9,10,11)

float pv[] = { -0.5, -0.5, 0.5,    // 0
               -0.5, 0.5, 0.5,    // 1
               0.5, 0.5, 0.5,    // 2
               0.5, -0.5, 0.5,    // 3
               0.0, 0.0, 1.5 };  // 4

GLubyte pvi[] = {0, 1, 2,
                 2, 3, 0,
                 0, 3, 4,
                 3, 2, 4,
                 2, 1, 4,
                 1, 0, 4};

float pve[nFaces*nVerticesPerFace*nCoordsComponents];
void expandPyramidVertices()
{
    for (int i=0; i<nFaces; i++)
    {
        for (int j=0; j<nVerticesPerFace; j++)
        {
            for (int k=0; k<nCoordsComponents; k++)
            {
                pve[(i*3+j)*3+k] = pv[pvi[i*3+j]*3+k];
            }
        }
    }
}

float pc[] = { 0.3f, 0.30f, 0.3f,
               1.0f, 0.70f, 0.0f,
               1.0f, 0.62f, 0.0f,
               1.0f, 0.40f, 0.0f,
               1.0f, 0.48f, 0.0f};

GLubyte pci[] = { 0, 0, 0,
                  0, 0, 0,
                  1, 1, 1,
                  2, 2, 2,
                  3, 3, 3,
                  4, 4, 4 };

float pce[nFaces*nVerticesPerFace*nColorComponents];
void expandPyramidColors()
{
    for (int i=0; i<nFaces; i++)
    {
        for (int j=0; j<nVerticesPerFace; j++)
        {
            for (int k=0; k<nColorComponents; k++)
            {
                pce[(i*3+j)*3+k] = pc[pci[i*3+j]*3+k];
            }
        }
    }
}

float n[nFaces*nVerticesPerFace*nCoordsComponents];

//============================================================================

float planeVertices[] = { -1, 1,
               1, 1,
               -1, -1,
               -1, -1,
               1, -1,
               1, 1};


// ===========================================================================

bool double_equal(double a, double b)
{
    if (fabs(a-b) < 1e-3)
        return true;
    return false;
}

// ---------------------------------------------------------------------------
static void displayCommands()
{
    printf("----- Key commands -----\n");
    printf("\n");
    printf("1           : Immediate\n");
    printf("2           : Vertex array\n");
    printf("3           : Vertex Buffer Object (VBO)\n");
    printf("4           : Shader with Vertex Buffer Object (VBO)\n");
    printf("5           : Shader with Vertex Array Object (VBO and VAO)\n");
    printf("\n");
    printf("+/=         : Zoom in\n");
    printf("-/_         : Zoom out\n");
    printf("\n");
    printf("Left arrow  : Pan left\n");
    printf("Right arrow : Pan left\n");
    printf("Up arrow    : Pan left\n");
    printf("Down arrow  : Pan left\n");
    printf("\n");
    printf("X           : Rotate +ve X axis\n");
    printf("x           : Rotate -ve X axis\n");
    printf("Y           : Rotate +ve Y axis\n");
    printf("X           : Rotate +ve Y axis\n");
    printf("Z           : Rotate +ve Z axis\n");
    printf("z           : Rotate -ve Z axis\n");
    printf("\n");
    printf("SPACE       : Reset view\n");
    printf("Q/q/ESC     : Quit\n");
}

// ---------------------------------------------------------------------------
static void onKeyPressed(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 27 :
    case 'Q':
    case 'q': 
        glutLeaveMainLoop () ; break;

    case '=':
    case '+':
        zoom=zoom-0.001; 
        if (double_equal(zoom, 0))
           zoom = 0.001;
        break;
    case '-':
    case '_': 
        zoom = zoom+0.001;
        if (double_equal(zoom, 0))
            zoom = 0.001;
        break;
    case ' ':
        zoom = 0.01;
        tx=0; ty=0;
        thetax=0; thetay=0; thetaz=0;
        break;

    case 'x':
        thetax -= 10; break;
    case 'X':
        thetax += 10; break; 
    case 'y':
        thetay -= 10; break;
    case 'Y':
        thetay += 10; break;
    case 'z':
        thetaz -= 10; break;
    case 'Z':
        thetaz += 10; break;

    case '1':
        drawType = kImmediate; break;
    case '2':
        drawType = kVertexArray; break;
    case '3':
        drawType = kVBO; break;
    case '4':
        drawType = kShaderVBO; break;
    case '5':
        drawType = kShaderVAO; break;
    default:
        break;
    }

    glutPostRedisplay();
}

// ---------------------------------------------------------------------------
static void onSpecialKeyPressed(int key, int x, int y)
{
    switch (key)
    {
    case GLUT_KEY_LEFT:     // left arrow
        tx -=0.25; break;
    case GLUT_KEY_RIGHT:    // right arrow
        tx +=0.25; break;
    case GLUT_KEY_UP:       // up arrow
        ty +=0.25; break;
    case GLUT_KEY_DOWN:     // down arrow
        ty -=0.25; break;
    }

    glutPostRedisplay();
}

// ---------------------------------------------------------------------------
static void onResize(int w, int h)
{
    width = w;
    height = h;
    glViewport(0, 0, width, height);
}

// ---------------------------------------------------------------------------
const char* vertex_shader =
    "attribute vec3 aCoords;"
    "attribute vec3 aColor;"
    "uniform mat4 umvMat;"
    "uniform mat4 upMat;"
    "varying vec3 vColor;"
    "void main () {"
        "gl_Position = upMat * umvMat * vec4(aCoords, 1.0);"
        "vColor = aColor;"
    "}";

const char* fragment_shader =
    "varying vec3 vColor;"
    "void main () {"
        "gl_FragColor = vec4 (vColor, 1.0);"
    "}";
    
    
    
const char* tex_vertex_shader =
    "attribute vec2 aCoords;"
    "varying vec2 texCoords;"
    "void main () {"
    "gl_Position = vec4(aCoords, 0.0, 1.0);"
	"texCoords = aCoords / vec2(2) + 0.5;"
    "}";

const char* tex_fragment_shader =
    "varying vec2 texCoords;"
    "float r;"
    "float theta;"
    "float x;"
    "float y;"
    "uniform sampler2D ourTexture;"
    "void main () {"
        "r = (gl_FragCoord.x-64.0)/128.0;"
        "theta = gl_FragCoord.y * (2.0*3.1415926535897932384626433832795/float(512));"
        "x = r * cos(theta);"
        "y = r * sin(theta);"
        "gl_FragColor = vec4(texture2D(ourTexture, vec2(x+0.5, y+0.5)).rgb, 1.0);"
    "}";

// ---------------------------------------------------------------------------
static void initShaders()
{
    GLuint vs = glCreateShader (GL_VERTEX_SHADER);
    glShaderSource (vs, 1, &vertex_shader, NULL);
    glCompileShader (vs);

    GLuint fs = glCreateShader (GL_FRAGMENT_SHADER);
    glShaderSource (fs, 1, &fragment_shader, NULL);
    glCompileShader (fs);

    phongProgram = glCreateProgram();
    glAttachShader (phongProgram, fs);
    glAttachShader (phongProgram, vs);

    glBindAttribLocation(phongProgram, VERTEX_ATTR_COORDS, "aCoords");
    glBindAttribLocation(phongProgram, VERTEX_ATTR_COLOR, "aColor");

    glLinkProgram (phongProgram);
    
    
    
    GLuint texVs = glCreateShader (GL_VERTEX_SHADER);
    glShaderSource (texVs, 1, &tex_vertex_shader, NULL);
    glCompileShader (texVs);

    GLuint texFs = glCreateShader (GL_FRAGMENT_SHADER);
    glShaderSource (texFs, 1, &tex_fragment_shader, NULL);
    glCompileShader (texFs);

    GLint compileStatus;
    GLint infoLogLength;

    glGetShaderiv(texVs, GL_COMPILE_STATUS, &compileStatus);

    if (compileStatus == GL_FALSE) {
        std::cerr << "Unable to compile shader vertex shader:\n";

        glGetShaderiv(texVs, GL_INFO_LOG_LENGTH, &infoLogLength);

        std::vector<char> infoLog(infoLogLength + 1);

        glGetShaderInfoLog(texVs, infoLogLength, nullptr, infoLog.data());
        infoLog[infoLogLength] = '\0';

        std::cerr << infoLog.data() << "\n";
    }

    glGetShaderiv(texFs, GL_COMPILE_STATUS, &compileStatus);

    if (compileStatus == GL_FALSE) {
        std::cerr << "Unable to compile shader fragment shader:\n";

        glGetShaderiv(texFs, GL_INFO_LOG_LENGTH, &infoLogLength);

        std::vector<char> infoLog(infoLogLength + 1);

        glGetShaderInfoLog(texFs, infoLogLength, nullptr, infoLog.data());
        infoLog[infoLogLength] = '\0';

        std::cerr << infoLog.data() << "\n";
    }


    texProgram = glCreateProgram();
    glAttachShader (texProgram, texFs);
    glAttachShader (texProgram, texVs);

    glBindAttribLocation(texProgram, VERTEX_ATTR_COORDS, "aCoords");

    glLinkProgram (texProgram);

    GLint linkStatus;

    glGetProgramiv(texProgram, GL_LINK_STATUS, &linkStatus);

    if (linkStatus == GL_FALSE) {
        std::cerr << "Unable to link program:\n";

        glGetProgramiv(texProgram, GL_INFO_LOG_LENGTH, &infoLogLength);

        std::vector<char> infoLog(infoLogLength + 1);

        glGetProgramInfoLog(texProgram, infoLogLength, nullptr, infoLog.data());
        infoLog[infoLogLength] = '\0';

        std::cerr << infoLog.data() << "\n";
    }

    glUseProgram (phongProgram);
}

// ---------------------------------------------------------------------------
static void defineVAO()
{
    vaoIds = new GLuint[2];
    glGenVertexArrays(2, vaoIds);

    vboIds = new GLuint[4];
    glGenBuffers(4, vboIds);

    GLint vertexAttribCoords = glGetAttribLocation(phongProgram, "aCoords");
    GLint vertexAttribColor = glGetAttribLocation(phongProgram, "aColor");

    // set current (bind) VAO to define axes data
    glBindVertexArray(vaoIds[0]);

    glBindBuffer(GL_ARRAY_BUFFER, vboIds[0]);  // coordinates
    glBufferData(GL_ARRAY_BUFFER, sizeof(ave), ave, GL_STATIC_DRAW);
    glVertexAttribPointer(vertexAttribCoords, nCoordsComponents, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vertexAttribCoords);

    glBindBuffer(GL_ARRAY_BUFFER, vboIds[1]);  // color
    glBufferData(GL_ARRAY_BUFFER, sizeof(ace), ace, GL_STATIC_DRAW);
    glVertexAttribPointer(vertexAttribColor, nColorComponents, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vertexAttribColor);

    // Set current (bind) VAO to define pyramid data
    glBindVertexArray(vaoIds[1]);
    
    glBindBuffer(GL_ARRAY_BUFFER, vboIds[2]);  // coordinates
    glBufferData(GL_ARRAY_BUFFER, sizeof(pve), pve, GL_STATIC_DRAW);
    glVertexAttribPointer(vertexAttribCoords, nCoordsComponents, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vertexAttribCoords);

    glBindBuffer(GL_ARRAY_BUFFER, vboIds[3]);  // color
    glBufferData(GL_ARRAY_BUFFER, sizeof(pce), pce, GL_STATIC_DRAW);
    glVertexAttribPointer(vertexAttribColor, nColorComponents, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vertexAttribColor);

    // Disable VAO
    glBindVertexArray(0);
}

// ---------------------------------------------------------------------------
static void drawShaderWithVertexArrayObject()
{
    initShaders();
    defineVAO();

    // Get the variables from the shader to which data will be passed
    GLint mvloc = glGetUniformLocation(phongProgram, "umvMat");
    GLint ploc = glGetUniformLocation(phongProgram, "upMat");

    // Pass the model-view matrix to the shader
    GLfloat mvMat[16]; 
    glGetFloatv(GL_MODELVIEW_MATRIX, mvMat); 
    glUniformMatrix4fv(mvloc, 1, false, mvMat);

    // Pass the projection matrix to the shader
    GLfloat pMat[16]; 
    glGetFloatv(GL_PROJECTION_MATRIX, pMat); 
    glUniformMatrix4fv(ploc, 1, false, pMat);

    // Enable VAO to set axes data
    glBindVertexArray(vaoIds[0]);
    
    // Draw axes
    glDrawArrays(GL_LINES, 0, nLines*nVerticesPerLine);

    // Enable VAO to set pyramid data
    glBindVertexArray(vaoIds[1]);

    // Draw pyramid
    glDrawArrays(GL_TRIANGLES, 0, nFaces*nVerticesPerFace);

    // Disable VAO
    glBindVertexArray(0);
}

// ---------------------------------------------------------------------------
static void drawShaderWithVertexBufferObject()
{
    initShaders();

    // Get the variables from the shader to which data will be passed
    GLint mvloc = glGetUniformLocation(phongProgram, "umvMat");
    GLint ploc = glGetUniformLocation(phongProgram, "upMat");
    GLint vertexAttribCoords = glGetAttribLocation(phongProgram, "aCoords");
    GLint vertexAttribColor = glGetAttribLocation(phongProgram, "aColor");

    // Pass the model-view matrix to the shader
    GLfloat mvMat[16]; 
    glGetFloatv(GL_MODELVIEW_MATRIX, mvMat);
    glUniformMatrix4fv(mvloc, 1, false, mvMat);

    // Pass the projection matrix to the shader
    GLfloat pMat[16]; 
    glGetFloatv(GL_PROJECTION_MATRIX, pMat);
    //for(i = 0; i < 16; i++){
        //std::cout<<pMat[i]<<" ";
        //if((i+1)%4==0){
            //std::cout<<"\n";
        //}
    //}
    //std::cout<<"\n"; 
    glUniformMatrix4fv(ploc, 1, false, pMat);


    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glViewport(0,0,500,500);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    vboIds = new GLuint[4];
    glGenBuffers(4, vboIds);

    //// Set axes data
    //glBindBuffer(GL_ARRAY_BUFFER, vboIds[0]);  // coordinates
    //glBufferData(GL_ARRAY_BUFFER, sizeof(ave), ave, GL_STATIC_DRAW);
    //glVertexAttribPointer(vertexAttribCoords, nCoordsComponents, GL_FLOAT, GL_FALSE, 0, 0);
    //glEnableVertexAttribArray(vertexAttribCoords);

    //glBindBuffer(GL_ARRAY_BUFFER, vboIds[1]);  // color
    //glBufferData(GL_ARRAY_BUFFER, sizeof(ace), ace, GL_STATIC_DRAW);
    //glVertexAttribPointer(vertexAttribColor, nColorComponents, GL_FLOAT, GL_FALSE, 0, 0);
    //glEnableVertexAttribArray(vertexAttribColor);

    //// Draw axes
    //glDrawArrays(GL_LINES, 0, nLines*nVerticesPerLine);

    // Set pyramid data
    glBindBuffer(GL_ARRAY_BUFFER, vboIds[2]);  // coordinates
    glBufferData(GL_ARRAY_BUFFER, sizeof(pve), pve, GL_STATIC_DRAW);
    glVertexAttribPointer(vertexAttribCoords, nCoordsComponents, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vertexAttribCoords);

    glBindBuffer(GL_ARRAY_BUFFER, vboIds[3]);  // color
    glBufferData(GL_ARRAY_BUFFER, sizeof(pce), pce, GL_STATIC_DRAW);
    glVertexAttribPointer(vertexAttribColor, nColorComponents, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vertexAttribColor);

    // Draw pyramid
    glDrawArrays(GL_TRIANGLES, 0, nFaces*nVerticesPerFace);

    // Disable the VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    //unsigned char   *pixels = (unsigned char*)malloc(500*500*3);

    ///// READ THE CONTENT FROM THE FBO
    //glReadBuffer(GL_COLOR_ATTACHMENT0);
    //glReadPixels(0, 0, 500, 500, GL_RGB, GL_UNSIGNED_BYTE, pixels);
    
    //cimg_library::CImg<uint> img(500,500,1,3); 
    //cimg_forXYC(img,x,y,c) {
        //img(x,y,c) = pixels[x*500*3 + y*3+c];
    //}
    //img.save_pfm("yoink.pfm");
    
    //cimg_foroff(img,off) { pixels[off]=initOutBuffer[off]; }
    //img.save_pfm("yeet.pfm");
    
    //glDisableVertexAttribArray(vertexAttribCoords);
    //glDisableVertexAttribArray(vertexAttribColor);

    glUseProgram(texProgram);

    glLoadIdentity();

    //glActiveTexture(GL_TEXTURE0);
    //glBindTexture(GL_TEXTURE_2D, framebufferTexture);

    GLint ourTexture = glGetUniformLocation(texProgram, "ourTexture");
    glUniform1i(ourTexture, 0);
    
    glBindFramebuffer(GL_FRAMEBUFFER, originalFramebuffer);
    //glBindFramebuffer(GL_READ_FRAMEBUFFER, originalFramebuffer);
    glViewport(0,0,128,512);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    GLuint vboId;
    glGenBuffers(1, &vboId);

    GLint texVertexAttribCoords = glGetAttribLocation(texProgram, "aCoords");

    glBindBuffer(GL_ARRAY_BUFFER, vboId);  // coordinates
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(texVertexAttribCoords);
    glVertexAttribPointer(texVertexAttribCoords, 2, GL_FLOAT, GL_FALSE, 0, 0);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    unsigned char   *pixels = (unsigned char*)malloc(128*512*3);

    /// READ THE CONTENT FROM THE FBO
    glReadBuffer(GL_BACK);
    glReadPixels(0, 0, 128, 512, GL_RGB, GL_UNSIGNED_BYTE, pixels);
    
    //cimg_library::CImg<uint> img(128,512,1,3); 
    //cimg_forXYC(img,x,y,c) {
        //img(x,y,c) = pixels[y*128*3 + x*3+c];
    //}
    //img.save_pfm("yoink.pfm");
    
    //cimg_foroff(img,off) { pixels[off]=pixels[off]; }
    //img.save_pfm("yeet.pfm");
    
    //FILE *output_image;
    //int output_width = 128;
    //int output_height = 512;
    //int h, j, k;
    //output_image = fopen("output.ppm", "wt");
    //fprintf(output_image,"P3\n");
    //fprintf(output_image,"# Created by Ricao\n");
    //fprintf(output_image,"%d %d\n",output_width,output_height);
    //fprintf(output_image,"255\n");

    //k = 0;
    //for(h=0; h<output_width; h++)
    //{
        //for(j=0; j<output_height; j++)
        //{
            //fprintf(output_image,"%u %u %u ",(unsigned int)pixels[k],(unsigned int)pixels[k+1],
                                             //(unsigned int)pixels[k+2]);
            //k = k+3;
        //}
        //fprintf(output_image,"\n");
    //}
    //free(pixels);
    
    fan.writeFrame(pixels);
    //fan.printBuffer(fan.bufferOne);
    
    //int i;
    //for(i = 0; i < 128*3*512; i++){
        //std::cout<<(int)pixels[i]<<"|";
        //if((i+1)%(3)==0){
            //std::cout<<"||";
        //}
        //if((i+1)%(128*3)==0){
            //std::cout<<"\n\n";
        //}
    //}
    //std::cout<<"\n\n";
    
    glDisableVertexAttribArray(texVertexAttribCoords);
}

// ---------------------------------------------------------------------------
static void drawVertexBufferObject()
{

    vboIds = new GLuint[3];
    glGenBuffers(3, vboIds);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    // Set axes data
    glBindBuffer(GL_ARRAY_BUFFER, vboIds[0]);  // coordinates
    glBufferData(GL_ARRAY_BUFFER, sizeof(ave), ave, GL_STATIC_DRAW);
    glVertexPointer(nCoordsComponents, GL_FLOAT, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, vboIds[1]);  // color
    glBufferData(GL_ARRAY_BUFFER, sizeof(ace), ace, GL_STATIC_DRAW);
    glColorPointer(nColorComponents, GL_FLOAT, 0, 0);

    // Draw axes
    glDrawArrays(GL_LINES, 0, nLines*nVerticesPerLine);

    // Set pyramid data
    glBindBuffer(GL_ARRAY_BUFFER, vboIds[2]);  // coordinates
    glBufferData(GL_ARRAY_BUFFER, sizeof(pve), pve, GL_STATIC_DRAW);
    glVertexPointer(nCoordsComponents, GL_FLOAT, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, vboIds[3]);  // color
    glBufferData(GL_ARRAY_BUFFER, sizeof(pce), pce, GL_STATIC_DRAW);
    glColorPointer(nColorComponents, GL_FLOAT, 0, 0);

    // Draw pyramid
    glDrawArrays(GL_TRIANGLES, 0, nFaces*nVerticesPerFace);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);

    // Disable the VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// ---------------------------------------------------------------------------
static void drawVertexArray()
{
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    // Set axes data
    glVertexPointer(nCoordsComponents, GL_FLOAT, 0, ave);
    glColorPointer(nColorComponents, GL_FLOAT, 0, ace);

    // Draw axes
    glDrawArrays(GL_LINES, 0, nLines*nVerticesPerLine);

    // Set pyramid data
    glVertexPointer(nCoordsComponents, GL_FLOAT, 0, pve);
    glColorPointer(nColorComponents, GL_FLOAT, 0, pce);

    // Draw pyramid
    glDrawArrays(GL_TRIANGLES, 0, nFaces*nVerticesPerFace);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
}

// ---------------------------------------------------------------------------
static void drawImmediate()
{
    // Draw x-axis in red
    glColor3d(ac[0], ac[1], ac[2]);
    glBegin(GL_LINES);
        glVertex3f(av[0], av[1], av[2]);
        glVertex3f(av[3], av[4], av[5]);
    glEnd();

    // Draw y-axis in green
    glColor3d(ac[3], ac[4], ac[5]);
    glBegin(GL_LINES);
        glVertex3f(av[0], av[1], av[2]);
        glVertex3f(av[6], av[7], av[8]);
    glEnd();

    // Draw z-axis in blue
    glColor3d(ac[6], ac[7], ac[8]);
    glBegin(GL_LINES);
        glVertex3f(av[0], av[1], av[2]);
        glVertex3f(av[9], av[10], av[11]);
    glEnd();

    // Draw pyramid
    glBegin(GL_TRIANGLES);
        glColor3d(pc[0], pc[1], pc[2]);

        glVertex3f(pv[0], pv[1], pv[2]);       // 0
        glVertex3f(pv[3], pv[4], pv[5]);       // 1
        glVertex3f(pv[6], pv[7], pv[8]);       // 2

        glVertex3f(pv[6], pv[7],  pv[8]);      // 2
        glVertex3f(pv[9], pv[10], pv[11]);     // 3
        glVertex3f(pv[0], pv[1],  pv[2]);      // 0

        glColor3f(pc[3], pc[4], pc[5]);

        glVertex3f(pv[0],  pv[1],  pv[2]);     // 0
        glVertex3f(pv[9],  pv[10], pv[11]);    // 3
        glVertex3f(pv[12], pv[13], pv[14]);    // 4

        glColor3f(pc[6], pc[7], pc[8]);

        glVertex3f(pv[9],  pv[10], pv[11]);    // 3
        glVertex3f(pv[6],  pv[7],  pv[8]);     // 2
        glVertex3f(pv[12], pv[13], pv[14]);    // 4

        glColor3f(pc[9], pc[10], pc[11]);

        glVertex3f(pv[6],  pv[7],  pv[8]);     // 2
        glVertex3f(pv[3],  pv[4],  pv[5]);     // 1
        glVertex3f(pv[12], pv[13], pv[14]);    // 4

        glColor3f(pc[12],  pc[13], pc[14]);

        glVertex3f(pv[3],  pv[4],  pv[5]);     // 1
        glVertex3f(pv[0],  pv[1],  pv[2]);     // 0
        glVertex3f(pv[12], pv[13], pv[14]);    // 4
    glEnd();
}

// ---------------------------------------------------------------------------
static void onDraw(void)
{
    steady_clock::time_point begin = steady_clock::now();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear Screen And Depth Buffer

    gluLookAt(0, 0, 1,  0, 0, 0,  0, 1, 0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glRotatef(thetax, 1, 0, 0);
    glRotatef(thetay, 0, 1, 0);
    glRotatef(thetaz, 0, 0, 1);

    glTranslatef(tx, ty, -4);

    drawShaderWithVertexBufferObject();

    //std::cout<<glGetError();
    //std::cout<<"\n";

    glutSwapBuffers();
    steady_clock::time_point end = steady_clock::now();
    std::cout << "Time difference = " <<duration_cast<nanoseconds> (end - begin).count() << "[ns]" << std::endl;
    std::cout << "fps = " <<1000000000/(duration_cast<nanoseconds> (end - begin).count()) << "[fps]" << std::endl;
}

void init (void) {
    
	std::cout<<sizeof(planeVertices)<<std::endl;
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
      /* Problem: glewInit failed, something is seriously wrong. */
      fprintf(stderr, "Oh fug das bad: %s\n", glewGetErrorString(err));
    }
    fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
   
    //////////////////////
    glGenTextures(1, &framebufferTexture);
    glBindTexture(GL_TEXTURE_2D, framebufferTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, winWidth, winHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    
    //////////////////////
    glGenRenderbuffers(1, &renderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);
    
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, winWidth, winHeight);

    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, (GLint*)&originalFramebuffer);

    /////////////////////
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderbuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebufferTexture, 0);
    
    GLenum buffersToDraw[] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, buffersToDraw);
    
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
        std::cout<<"ERROR: Framebuffer not configured correctly.\n";
    }

    //glReadBuffer(GL_FRONT);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //glOrtho((-width/2)*zoom, (width/2)*zoom, (-height/2)*zoom, (height/2)*zoom, -10, 10);
    gluPerspective(65, 1, 0.1, 10);

    //glEnable (GL_DEPTH_TEST);
    //glEnable (GL_LIGHTING);
    //glEnable (GL_LIGHT0);
}

static void onIdle(){
    //glutPostRedisplay();
}

// ---------------------------------------------------------------------------
int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitWindowSize(128,512);
    glutInitWindowPosition(0,0);

    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);

    glutCreateWindow("Basic OpenGL using FreeGLUT");
    
    init();

    glutReshapeFunc(onResize);
    glutDisplayFunc(onDraw);
    glutIdleFunc(onIdle);
    glutKeyboardFunc(onKeyPressed);
    glutSpecialFunc(onSpecialKeyPressed);

    glClearColor(0,0,0,1);

    glEnable(GL_DEPTH_TEST); // z-buffer test
    //glDepthFunc(GL_LESS);
    glShadeModel(GL_SMOOTH);

    expandAxesVertices();
    expandAxesColors();
    expandPyramidVertices();
    expandPyramidColors();

    displayCommands();

    glutMainLoop();

    return 0;
}
