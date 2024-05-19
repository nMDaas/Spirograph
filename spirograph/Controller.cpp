#include "Controller.h"
#include <cstdio>
#include <cstdlib>
#include <fstream>

// Implementation of Controller of Program.

Controller::Controller(Model* m,View* v) {
    model = m;
    view = v;
}

Controller::~Controller()
{

}

// program runs in a loop until window is closed
void Controller::run()
{
    view->init(model);
    while (!view->shouldWindowClose()) {
        view->display();
        glfwPollEvents();
    }
    view->closeWindow();
    exit(EXIT_SUCCESS);
}