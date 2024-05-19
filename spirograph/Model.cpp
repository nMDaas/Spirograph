#include "Model.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <math.h>
#include "spdlog/spdlog.h"

// Implementation of Model of Program.

// constructor of model sets up inner and outer circles.
Model::Model()
{
    bigCircRadius = 400;
    smallCircRadius = 200;
    smallCircPosX = bigCircRadius - smallCircRadius;
    smallCircPosY = 0;
    twicePi = 2 * M_PI;
    makeCircleMesh();
    makeDrawingCurveMesh();
    //genSmallCircVertexData();
    //genBigCircVertexData();
}

Model::~Model()
{
}

// creates mesh for a unit circle
void Model::makeCircleMesh() {
    vector<glm::vec4> positions;

    // start with (1,0,0)
    glm::vec4 vec = glm::vec4(1.0, 0.0, 0.0, 1.0);
    int sections = 200;
    float deltaTheta = M_PI/sections;
    
    glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), deltaTheta, glm::vec3(0.0, 0.0, 1.0));

    // drawing a line from r/2 to r to keep track of seedpoint
    // for debugging purposes
    /*
    glm::vec4 seed_tracer(0.5f, 0.0f, 0.0f, 1.0f);
    positions.push_back(seed_tracer);
    */

    // generate points for inner circle
    for(int i=0; i<=2*sections;i++){
        positions.push_back(vec);
        vec = rotate * vec;
    }

    util::PolygonMesh<VertexAttrib> mesh = createMeshFromPositions(positions);
    circleMesh.push_back(mesh);
}

// creates mesh for the curve
void Model::makeDrawingCurveMesh() {
    vector<glm::vec4> positions;

    // radius = 400, circumfrence = 2800, step of 1 = 0.1°
    float step = 10; 

    // if angle is theta, then length of arc is r
    // if the arc is step, then angle will be step/r
    float deltaTheta1 = 1.0f *  step / bigCircRadius;
    float deltaTheta2 = - 1.0f * step / smallCircRadius;

    // Initial center of inner circle
    float x = bigCircRadius - smallCircRadius;
    glm::vec4 cp(x,0.0f,0.0f,1.0f);

    // Initial location of seed point in inner circle
    float s = bigCircRadius - smallCircRadius/2;
    glm::vec4 sp(s,0.0f,0.0f,1.0f);

    // first rotation matrix
    glm::mat4 r1 = glm::rotate(glm::mat4(1.0f), deltaTheta1, glm::vec3(0.0, 0.0, 1.0));
    
    int points = 0;
    while(points < 20000) {
        positions.push_back(sp);
        sp = doubleRotate(cp,sp,deltaTheta1,deltaTheta2);
        cp = r1 * cp;
        points = points + 1;
    }

    util::PolygonMesh<VertexAttrib> mesh = createMeshFromPositions(positions);
    curveMesh.push_back(mesh);
}

// finds seed position based on seed and inner circle position and angles provided
// cp = inner circle centre
// sp = seed point position 
glm::vec4 Model::doubleRotate(glm::vec4 cp, glm::vec4 sp, float deltaTheta1, float deltaTheta2) {
    // first rotation
    glm::mat4 r1 = glm::rotate(glm::mat4(1.0f),deltaTheta1, glm::vec3(0.0, 0.0, 1.0));
    cp = r1*cp;
    sp = r1*sp;

    // Translate cp and sp that cp is at origin
    float dx = cp.x; // inner circle current posX
    float dy = cp.y; // inner circle current posY
    glm::mat4 t1 = glm::translate(glm::mat4(1.0),glm::vec3(-dx, -dy, 0)); // move to center
    cp = t1*cp; // this should be at 0,0
    sp = t1*sp; // will move relative to inner circle

    // second rotation
    glm::mat4 r2 = glm::rotate(glm::mat4(1.0f),deltaTheta2, glm::vec3(0.0, 0.0, 1.0));
    cp = r2*cp; // cp is already at 0,0 so nothing should happen here
    sp = r2*sp; // seed point will move

    // translate inner circle back up to position
    glm::mat4 t2 = glm::translate(glm::mat4(1.0),glm::vec3(dx, dy, 0));
    cp = t2*cp; 
    sp = t2*sp; 

    return sp;
}

// returns mesh for a circle
vector<util::PolygonMesh<VertexAttrib> > Model::getCircleMesh() {
    circleMesh.pop_back();
    makeCircleMesh();
    return circleMesh;
}

// returns mesh for a circle
vector<util::PolygonMesh<VertexAttrib> > Model::getCurveMesh() {
    curveMesh.pop_back();
    makeDrawingCurveMesh();
    return curveMesh;
}

// returns the radius of smaller inner circle
int Model::getSmallCircRadius() {
    return smallCircRadius;
} 

// returns the radius of bigger outer circle
int Model::getBigCircRadius() {
    return bigCircRadius;
}


// changes the inner circle radius value by amt
// amt can be a positive or negative value
void Model::changeInnerCircRadius(int amt) {
    smallCircRadius = smallCircRadius + amt;
}

// calculates distance between first and given points
float Model::calcDistance(float x1, float y1, float x2, float y2) {
    float temp = pow(x1 - x2, 2) + pow(y1 - y2, 2);
    return sqrt(temp);
}

// creates vertex data from positions vector given
vector<VertexAttrib> Model::createVertexData(vector<glm::vec4> positions){
    vector<VertexAttrib> vertexData;
    for (unsigned int i=0;i<positions.size();i++) {
        VertexAttrib v;
        vector<float> data;

        data.push_back(positions[i].x);
        data.push_back(positions[i].y);
        data.push_back(positions[i].z);
        data.push_back(positions[i].w);
        v.setData("position",data);
        vertexData.push_back(v);
    }
    return vertexData;
}

// creates mesh from positions vector given
util::PolygonMesh<VertexAttrib> Model::createMeshFromPositions(vector<glm::vec4> positions) {
    // get vertex data by passing in positions vector
    vector<VertexAttrib> vertexData = createVertexData(positions);
    
    // generate indices vector 
    vector<unsigned int> indices;
    for (int i=0;i<positions.size();i++)
    {
        indices.push_back(i);
    }

    // create mesh
    util::PolygonMesh<VertexAttrib> mesh;

    // give mesh vertex data
    mesh.setVertexData(vertexData);

    // give mesh index data that forms polygons
    mesh.setPrimitives(indices);

    mesh.setPrimitiveType(GL_LINE_STRIP); 
    mesh.setPrimitiveSize(2);  // 2 vertices for each GL_LINE_STRIP

    return mesh;
}
