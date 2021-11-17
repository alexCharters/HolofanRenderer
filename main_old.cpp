#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <string>
#include <cstring>
#include <iostream>
#include <iomanip>

#include <chrono>
using namespace std::chrono;

GLfloat objAngle = 0.0;
GLint lines = 300;
GLfloat resolution = 360.0/lines;
bool isShown = false;

float distance = 10;
float elevation = 3;

glm::vec3 modelPosition = glm::vec3(0.0);
glm::vec3 cameraPosition = glm::vec3(0.0,3.0,10.0);

GLubyte *frame = (GLubyte*)malloc(3 * 128 * lines);
GLubyte *data = (GLubyte*)malloc(3 * 128);

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

GLfloat vertices[]  = {
     .5f, .5f, .5f,  -.5f, .5f, .5f,  -.5f,-.5f, .5f,  .5f,-.5f, .5f, // v0,v1,v2,v3 (front)
     .5f, .5f, .5f,   .5f,-.5f, .5f,   .5f,-.5f,-.5f,  .5f, .5f,-.5f, // v0,v3,v4,v5 (right)
     .5f, .5f, .5f,   .5f, .5f,-.5f,  -.5f, .5f,-.5f, -.5f, .5f, .5f, // v0,v5,v6,v1 (top)
    -.5f, .5f, .5f,  -.5f, .5f,-.5f,  -.5f,-.5f,-.5f, -.5f,-.5f, .5f, // v1,v6,v7,v2 (left)
    -.5f,-.5f,-.5f,   .5f,-.5f,-.5f,   .5f,-.5f, .5f, -.5f,-.5f, .5f, // v7,v4,v3,v2 (bottom)
     .5f,-.5f,-.5f,  -.5f,-.5f,-.5f,  -.5f, .5f,-.5f,  .5f, .5f,-.5f  // v4,v7,v6,v5 (back)
};

// normal array
GLfloat normals[] = {
     0, 0, 1,   0, 0, 1,   0, 0, 1,   0, 0, 1,  // v0,v1,v2,v3 (front)
     1, 0, 0,   1, 0, 0,   1, 0, 0,   1, 0, 0,  // v0,v3,v4,v5 (right)
     0, 1, 0,   0, 1, 0,   0, 1, 0,   0, 1, 0,  // v0,v5,v6,v1 (top)
    -1, 0, 0,  -1, 0, 0,  -1, 0, 0,  -1, 0, 0,  // v1,v6,v7,v2 (left)
     0,-1, 0,   0,-1, 0,   0,-1, 0,   0,-1, 0,  // v7,v4,v3,v2 (bottom)
     0, 0,-1,   0, 0,-1,   0, 0,-1,   0, 0,-1   // v4,v7,v6,v5 (back)
};

// colour array
GLfloat colors[] = {
     1, 1, 1,   1, 1, 0,   1, 0, 0,   1, 0, 1,  // v0,v1,v2,v3 (front)
     1, 1, 1,   1, 0, 1,   0, 0, 1,   0, 1, 1,  // v0,v3,v4,v5 (right)
     1, 1, 1,   0, 1, 1,   0, 1, 0,   1, 1, 0,  // v0,v5,v6,v1 (top)
     1, 1, 0,   0, 1, 0,   0, 0, 0,   1, 0, 0,  // v1,v6,v7,v2 (left)
     0, 0, 0,   0, 0, 1,   1, 0, 1,   1, 0, 0,  // v7,v4,v3,v2 (bottom)
     0, 0, 1,   0, 0, 0,   0, 1, 0,   0, 1, 1   // v4,v7,v6,v5 (back)
};

// texture coord array
GLfloat texCoords[] = {
    1, 0,   0, 0,   0, 1,   1, 1,               // v0,v1,v2,v3 (front)
    0, 0,   0, 1,   1, 1,   1, 0,               // v0,v3,v4,v5 (right)
    1, 1,   1, 0,   0, 0,   0, 1,               // v0,v5,v6,v1 (top)
    1, 0,   0, 0,   0, 1,   1, 1,               // v1,v6,v7,v2 (left)
    0, 1,   1, 1,   1, 0,   0, 0,               // v7,v4,v3,v2 (bottom)
    0, 1,   1, 1,   1, 0,   0, 0                // v4,v7,v6,v5 (back)
};

// index array for glDrawElements()
// A cube requires 36 indices = 6 sides * 2 tris * 3 verts
GLuint indices[] = {
     0, 1, 2,   2, 3, 0,    // v0-v1-v2, v2-v3-v0 (front)
     4, 5, 6,   6, 7, 4,    // v0-v3-v4, v4-v5-v0 (right)
     8, 9,10,  10,11, 8,    // v0-v5-v6, v6-v1-v0 (top)
    12,13,14,  14,15,12,    // v1-v6-v7, v7-v2-v1 (left)
    16,17,18,  18,19,16,    // v7-v4-v3, v3-v2-v7 (bottom)
    20,21,22,  22,23,20     // v4-v7-v6, v6-v5-v4 (back)
};


void init (void) {
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
      /* Problem: glewInit failed, something is seriously wrong. */
      fprintf(stderr, "Oh fug das bad: %s\n", glewGetErrorString(err));
    }
    fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
    
    glEnable (GL_DEPTH_TEST);
    glEnable (GL_LIGHTING);
    glEnable (GL_LIGHT0);
}

static unsigned int CompileShader(unsigned int type, const std::string& source){
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);
    
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (!result){
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;
    }
    
    return id;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader){
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);
    
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);
    
    glDeleteShader(fs);
    
    return program;
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
    GLuint vboId;
    GLuint iboId;
    
    // create VBOs
    glGenBuffers(1, &vboId);    // for vertex buffer
    glGenBuffers(1, &iboId);    // for index buffer
    
    size_t vSize = sizeof vertices;
    size_t nSize = sizeof normals;
    size_t cSize = sizeof colors;
    size_t tSize = sizeof texCoords;
    
    glBindBuffer(GL_ARRAY_BUFFER, vboId);
    glBufferData(GL_ARRAY_BUFFER, vSize+nSize+cSize+tSize, 0, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vSize, vertices);                  // copy verts at offset 0
    glBufferSubData(GL_ARRAY_BUFFER, vSize, nSize, normals);               // copy norms after verts
    glBufferSubData(GL_ARRAY_BUFFER, vSize+nSize, cSize, colors);          // copy cols after norms
    glBufferSubData(GL_ARRAY_BUFFER, vSize+nSize+cSize, tSize, texCoords); // copy texs after cols
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    // bind VBOs before drawing
    glBindBuffer(GL_ARRAY_BUFFER, vboId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboId);
    
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    
    size_t nOffset = sizeof vertices;
    size_t cOffset = nOffset + sizeof normals;
    size_t tOffset = cOffset + sizeof colors;
    
    // specify vertex arrays with their offsets
    glVertexPointer(3, GL_FLOAT, 0, (void*)0);
    glNormalPointer(GL_FLOAT, 0, (void*)nOffset);
    glColorPointer(3, GL_FLOAT, 0, (void*)cOffset);
    glTexCoordPointer(2, GL_FLOAT, 0, (void*)tOffset);
    
        // finally draw a cube with glDrawElements()
    glDrawElements(GL_TRIANGLES,            // primitive type
                   36,                      // # of indices
                   GL_UNSIGNED_INT,         // data type
                   (void*)0);               // offset to indices

    // disable vertex arrays
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    // unbind VBOs
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    
    //auto start = high_resolution_clock::now();

    //glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //glLoadIdentity();
    //light();

    //cameraPosition = glm::rotateY(cameraPosition, glm::radians(1.0f));	
    //objAngle ++;
    //gluLookAt(cameraPosition.x,cameraPosition.y,cameraPosition.z,modelPosition.x,modelPosition.y,modelPosition.z,0,1,0);
    
    //glm::vec3 rotation_axis = modelPosition-cameraPosition;

    //glutSolidCube(2);
    //glutSwapBuffers();

    ////print_bytes(std::cout, "frame", frame, 3 * 128 * lines);
    
    //auto stop = high_resolution_clock::now();
    //auto diff = stop - start;
    
    //std::cout << "frame " << 1000000000.0/((float)(diff.count())) << "\n" << std::flush;
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
    if(argc != 2){
        glutInit (&argc, argv);
        glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
        glutInitWindowSize (128, 1);
        glutCreateWindow ("A basic OpenGL Window");
        init ();
        glutDisplayFunc (display);
        glutIdleFunc (display);
        //glutKeyboardFunc (keyboard);
        glutReshapeFunc (reshape);
        
        
        //glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, whiteSpecularMaterial);
        //glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mShininess);
        //glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, redDiffuseMaterial);
        
        glutMainLoop ();
        return 0;
    }
    else if(std::strcmp(argv[1], "show") == 0){
        isShown = true;
        glutInit (&argc, argv);
        glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
        glutInitWindowSize (500, 500);
        glutCreateWindow ("A basic OpenGL Window");
        init ();
        glutDisplayFunc (display);
        glutIdleFunc (display);
        //glutKeyboardFunc (keyboard);
        glutReshapeFunc (reshape);
        
        //glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, whiteSpecularMaterial);
        //glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mShininess);
        //glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, redDiffuseMaterial);
        
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
        
        std::string vertexShader = 
        "#version 120 core\n"
        "layout(location = 0) out vec4 color;\n"
        "\n"
        "void main()\n"
        "{\n"
        "   color = vec4(1.0, 0.0, 0.0, 1.0);\n"
        "}\n";

        std::string fragmentShader = 
        "#version 120 core\n"
        "layout(location = 0) in vec4 position;\n"
        "\n"
        "void main()\n"
        "{\n"
        "   gl_position = position;\n"
        "}\n";
        
        unsigned int shader = CreateShader(vertexShader, fragmentShader);
        glUseProgram(shader);
        
        std::cout << glGetString(GL_VERSION) << std::endl;
        
        glutMainLoop ();
        return 0;

    }
}
