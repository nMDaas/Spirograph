#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "spdlog/spdlog.h"
#include "View.h"

// Implementation of View of Program.

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

View::View() {
    int sections = 200;
    curveColor = glm::vec4(0.431,0.780,0.408,1);
    showCurve = true; // initially show curve
}

View::~View(){
}

// sets up the View for the program
int View::init(Model* m)
{
    // save the model and controller
    this->model = m;

    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    
    window_dimensions = glm::vec2(800,800);

    window = glfwCreateWindow(window_dimensions.x,window_dimensions.y, "Spirograph", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetWindowUserPointer(window, this);
    
    //using C++ functions as callbacks to a C-style library
    glfwSetKeyCallback(window, 
    [](GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        static_cast<View*>(glfwGetWindowUserPointer(window))->onkey(window,key,scancode,action,mods);
    });

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress); // loading Glad
    glfwSwapInterval(1);

    // create the shader program
    program.createProgram(string("shaders/default.vert"),
                          string("shaders/default.frag"));
    // assuming it got created, get all the shader variables that it uses
    // so we can initialize them at some point
    // enable the shader program
    program.enable();
    shaderLocations = program.getAllShaderVariables();

    shaderVarsToVertexAttribs["vPosition"] = "position";
    
    innerCircAngle = 0.0;
    innerCircRotAngle = 0.0;
    savedInnerCircAngle = 0.0;
    savedInnerCircRotAngle = 0.0;
    vector<util::PolygonMesh<VertexAttrib>> meshes = model->getCircleMesh();
    makeObject(meshes);
    meshes = model->getCurveMesh();
    makeObject(meshes);

    //prepare the projection matrix for orthographic projection
	projection = glm::ortho(-800.0, 800.0, -800.0, 800.0);

    frames = 0;
    time = glfwGetTime();

    //default motion is speed 1 in direction (1,1)
    speed = 1;

    return 0;
}

//now we create an object that will be used to render this mesh in opengl
    /*
     * now we create an ObjectInstance for it.
     * The ObjectInstance encapsulates a lot of the OpenGL-specific code
     * to draw this object
     */

    /* so in the mesh, we have some attributes for each vertex. In the shader
     * we have variables for each vertex attribute. We have to provide a mapping
     * between attribute name in the mesh and corresponding shader variable
     name.
     *
     * This will allow us to use PolygonMesh with any shader program, without
     * assuming that the attribute names in the mesh and the names of
     * shader variables will be the same.
       We create such a shader variable -> vertex attribute mapping now
     */
void View::makeObject(vector<util::PolygonMesh<VertexAttrib> > meshes) {
    for (int i=0;i<meshes.size();i++) {
        util::ObjectInstance *obj = new util::ObjectInstance("meshes");
        obj->initPolygonMesh<VertexAttrib>(
            program,                    // the shader program
            shaderLocations,            // the shader locations
            shaderVarsToVertexAttribs,  // the shader variable -> attrib map
            meshes[i]);                 // the actual mesh object

        objects.push_back(obj);
    }
}

void processInput(GLFWwindow *window)
{
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}


// draws the lines based on vertex coordinates, defines circles
void View::display() {
    
    program.enable();
    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT);

    // draw objects
    drawDrawingAndInnerCircle();
    drawOuterCircle();
    drawCurve();
    
    glFlush();
    program.disable();
    glfwSwapBuffers(window);

    glfwPollEvents();

    frames++;
    double currenttime = glfwGetTime();

    if ((currenttime-time)>1.0) {
        printf("Framerate: %2.0f\r",frames/(currenttime-time));
        frames = 0;
        time = currenttime;
    }
}

// draws drawing and inner circle
void View::drawDrawingAndInnerCircle() {
    color = glm::vec4(0.431,0.780,0.408,1);
    float radius = (float) model->getSmallCircRadius();
    float seedRadius = (float) 5.0;
    if (showCurve) {
    float bigRadius = (float) model->getBigCircRadius();
    

    // radius = 400, circumfrence = 2800, step of 1 = 0.1°
    float step = 10; 

    // if angle is theta, then length of arc is r
    // if the arc is step, then angle will be step/r
    float deltaTheta1 = 1.0f *  step / bigRadius;
    float deltaTheta2 = - 1.0f * step / radius;

    glm::vec4 cp;
    glm::mat4 cpMatrix;
    glm::vec4 sp;
    glm::mat4 spMatrix;

    // if radius changed, refresh needs to be done and this loop executes until angle is reached again
    // if radius has not been changed, innerCircAngle = savedInnerCircAngle
    while (innerCircAngle != savedInnerCircAngle) {
        cp = glm::vec4(0,0,0,1);
        cpMatrix = glm::mat4(1.0);
        sp = glm::vec4(0.0,0,0,1);
        spMatrix = glm::mat4(1.0);

        // scale seed/drawing circle
        glm::mat4 s_sp = glm::scale(glm::mat4(1.0f), glm::vec3(seedRadius, seedRadius, seedRadius));
        sp = s_sp * sp;
        spMatrix = s_sp * spMatrix;

        // Initial center of inner circle
        float x = bigRadius - radius;
        float x_sp = x + (radius/2);
        glm::mat4 t = glm::translate(glm::mat4(1.0f), glm::vec3(x, 0, 0));
        glm::mat4 t_sp = glm::translate(glm::mat4(1.0f), glm::vec3(x_sp, 0, 0));
        cp = t * cp;
        cpMatrix = t * cpMatrix;
        sp = t_sp * sp;
        spMatrix = t_sp * spMatrix;

        // first rotation matrix
        glm::mat4 r1 = glm::rotate(glm::mat4(1.0f), innerCircAngle, glm::vec3(0.0, 0.0, 1.0));
        cp = r1 * cp;
        cpMatrix = r1 * cpMatrix;
        sp = r1 * sp;
        spMatrix = r1 * spMatrix;
    
        // translate to origin
        float dx = cp.x; // inner circle current posX
        float dy = cp.y; // inner circle current posY
        glm::mat4 t2 = glm::translate(glm::mat4(1.0f), glm::vec3(-dx, -dy, 0));
        cp = t2 * cp;
        cpMatrix = t2 * cpMatrix;
        sp = t2 * sp;
        spMatrix = t2 * spMatrix;
        
        // second rotation matrix
        glm::mat4 r2 = glm::rotate(glm::mat4(1.0f), innerCircRotAngle, glm::vec3(0.0, 0.0, 1.0));
        cp = r2 * cp;
        cpMatrix = r2 * cpMatrix;
        sp = r2 * sp;
        spMatrix = r2 * spMatrix;

        // scale
        glm::mat4 s = glm::scale(glm::mat4(1.0f), glm::vec3(radius, radius, radius));
        cp = s * cp;
        cpMatrix = s * cpMatrix;

        // translate back to position
        glm::mat4 t3 = glm::translate(glm::mat4(1.0f), glm::vec3(dx, dy, 0));
        cp = t3 * cp;
        cpMatrix = t3 * cpMatrix;
        sp = t3 * sp;
        spMatrix = t3 * spMatrix;

        innerCircAngle = innerCircAngle + deltaTheta1;
        innerCircRotAngle = innerCircRotAngle + deltaTheta2;
    }

    savedInnerCircAngle = savedInnerCircAngle + deltaTheta1;
    savedInnerCircRotAngle = savedInnerCircRotAngle + deltaTheta2;

    innerCircCentre = glm::vec2(cp.x, cp.y);
    seedCentre = glm::vec2(sp.x, sp.y);
    
    // draw seed/drawing circle
    modelview = glm::mat4(1.0) * spMatrix;
    getShaderLocations();
    objects[0]->draw();

    // draw inner Circle
    color = glm::vec4(0.949,0.549,0.156,1);
    modelview = glm::mat4(1.0) * cpMatrix;
    getShaderLocations();
    objects[0]->draw();
    }
    else {
        // draw seed/drawing circle
        modelview = glm::mat4(1.0);
        modelview = modelview * glm::translate(glm::mat4(1.0f), glm::vec3(seedCentre.x, seedCentre.y, 0))
            * glm::scale(glm::mat4(1.0),glm::vec3(seedRadius,seedRadius,seedRadius));
            
         getShaderLocations();
        objects[0]->draw();

        // draw inner Circle
        color = glm::vec4(0.949,0.549,0.156,1);
        modelview = glm::mat4(1.0);
        modelview = modelview * glm::translate(glm::mat4(1.0f), glm::vec3(innerCircCentre.x, innerCircCentre.y, 0))
            * glm::scale(glm::mat4(1.0),glm::vec3(radius,radius,radius));//send modelview matrix to GPU  
         getShaderLocations();
        objects[0]->draw();
    }
}

// draws outer circle
void View::drawOuterCircle() {
    color = glm::vec4(1,0,0,1);
    float radius3 = 400.0f;
    modelview = glm::mat4(1.0);
    modelview = modelview *
            glm::scale(glm::mat4(1.0),glm::vec3(radius3,radius3,radius3));//send modelview matrix to GPU  
    getShaderLocations();
    objects[0]->draw();
}

// draws curves
void View::drawCurve() {
    color = curveColor;
    modelview = glm::mat4(1.0);
    getShaderLocations();
    objects[1]->draw();
}

// gets shader locations
void View::getShaderLocations() {
     glUniformMatrix4fv(shaderLocations.getLocation("modelview"), 1, GL_FALSE, glm::value_ptr(modelview));
    //send projection matrix to GPU    
    glUniformMatrix4fv(shaderLocations.getLocation("projection"), 1, GL_FALSE, glm::value_ptr(projection));
    //send color to GPU
    glUniform4fv(shaderLocations.getLocation("vColor"),1,glm::value_ptr(color));
}

void View::onkey(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    // check if "I" is pressed
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
        int currentRadius = model->getSmallCircRadius();
        // check if "SHIFT" is pressed
        if (mods == GLFW_MOD_SHIFT) {
            if (currentRadius + 5 <= 400) {
                model->changeInnerCircRadius(5);
            }
        }
        else {
            if (currentRadius - 5 >= 5) {
                model->changeInnerCircRadius(-5);
            }
        }

        for (int i=0;i<objects.size();i++) {
        objects[i]->cleanup();
        delete objects[i];
        }
        objects.clear();

        // reset model and save angles
        savedInnerCircAngle = innerCircAngle;
        savedInnerCircRotAngle = innerCircRotAngle;
        innerCircAngle = 0.0;
        innerCircRotAngle = 0.0;

        // regenerate models
        vector<util::PolygonMesh<VertexAttrib>> meshes = model->getCircleMesh();
        makeObject(meshes);
        meshes = model->getCurveMesh();
        makeObject(meshes);

    }

    // check if "C" is pressed
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
        if (showCurve) {
            //curveColor = glm::vec4(0,0,0,1);
            showCurve = false;
            printf("showCurve turned off\n");
        }
        else {
            //curveColor = glm::vec4(0.431,0.780,0.408,1);
            showCurve = true;
            printf("showCurve turned on\n");
        }
    }
}

// called from Controller.cpp
bool View::shouldWindowClose() {
    return glfwWindowShouldClose(window);
}

// called from Controller.cpp
void View::closeWindow(){
    
    glfwDestroyWindow(window);
    glfwTerminate();
};