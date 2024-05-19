#ifndef __VIEW_H__
#define __VIEW_H__

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <ShaderProgram.h>
#include "Model.h"
#include <ObjectInstance.h>

// Header for View of Spirograph program.

class View
{
public:
    View();
    ~View();
    int init(Model* m); // sets up the View for the program
    void display(); // draws the lines based on vertex coordinates, defines circles
    bool shouldWindowClose(); // called from Controller.cpp
    void closeWindow(); // called from Controller.cpp
private:   
    Model *model;
    GLFWwindow *window;
    util::ShaderProgram program;
    util::ShaderLocationsVault shaderLocations;
    vector<util::ObjectInstance *> objects;
    glm::mat4 modelview,projection;
    int frames;
    double time;
   
    glm::vec4 motion;
    glm::vec4 color;
    glm::vec4 curveColor; // color for the curves
    bool showCurve; // to toggle curve drawing
    glm::vec2 window_dimensions;
    double speed;
    glm::vec2 innerCircCentre;
    glm::vec2 seedCentre;
    float innerCircAngle; // angle for translation of inner circle
    float innerCircRotAngle; // angle of rotation of inner circle
    float savedInnerCircAngle; // saved InnerCircAngle
    float savedInnerCircRotAngle; // saved CircRotAngle
    map<string, string> shaderVarsToVertexAttribs;
    // creates objects to render mesh:
    void makeObject(vector<util::PolygonMesh<VertexAttrib> > meshes); 
    void drawInnerCircle(); // draws inner circle
    void drawDrawingAndInnerCircle(); // draws drawing and inner circle
    void drawOuterCircle(); // draws outer circle
    void drawCurve(); // draws curves
    void getShaderLocations(); // gets shader locations
    void onkey(GLFWwindow* window, int key, int scancode, int action, int mods);
};

#endif