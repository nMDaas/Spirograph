#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

#include <map>
#include "View.h"
#include "Model.h"

// Header for Controller of Spirograph program.

class Controller
{
public:
    Controller(Model* m,View* v);
    ~Controller();
    void run(); // program runs in a loop

private:
    View* view;
    Model* model;
};

#endif
