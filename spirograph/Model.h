#ifndef __MODEL_H__
#define __MODEL_H__

#include <PolygonMesh.h>
#include "VertexAttrib.h"
#include <vector>
using namespace std;

// Header for Model of Spirograph program.
class Model 
{
public:
    Model();
    ~Model();
    vector<util::PolygonMesh<VertexAttrib> > getCircleMesh(); // returns mesh for a circle
    vector<util::PolygonMesh<VertexAttrib> > getCurveMesh(); // returns mesh for a circle
    int getSmallCircRadius(); // returns the radius of smaller inner circle
    int getBigCircRadius(); // returns the radius of bigger outer circle
    void changeInnerCircRadius(int amt); // changes the inner circle radius value by amt & remakes mesh

private:
    vector<util::PolygonMesh<VertexAttrib> > circleMesh; // makes mesh for a unit circle
    vector<util::PolygonMesh<VertexAttrib> > curveMesh; // makes mesh for a curve
    int bigCircRadius; // radius of outer circle
    int smallCircRadius; // radius of inner circle
    float twicePi; // constant for PI
    double smallCircPosX; // x-coordinate of inner circle
    double smallCircPosY; // y-coordinate of inner circle
    void makeCircleMesh(); // creates mesh for a circle
    void makeDrawingCurveMesh(); // creates mesh for the curve

    // finds seed position based on seed and inner circle position and angles provided
    glm::vec4 doubleRotate(glm::vec4 cp, glm::vec4 sp, float deltaTheta1, float deltaTheta2);

    // calculates distance between first and given points
    float calcDistance(float x1, float y1, float x2, float y2); 

    // creates mesh from positions vector given
    util::PolygonMesh<VertexAttrib> createMeshFromPositions(vector<glm::vec4> positions); 

    // creates vertex points from positions vector given
    vector<VertexAttrib> createVertexData(vector<glm::vec4> positions); 

};
#endif
