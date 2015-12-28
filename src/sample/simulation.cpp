#include "LightmanSimulation.h"

// System
#include <stdio.h>
#include <iostream>

using namespace LightmanCore;
using namespace LightmanSimulation;

int main(int argc, const char* argv[]) {

    // check arguments
    const char *path;
    if (argc != 2) {
        printf("Usage: basic <path_to_workspace>\n");
        exit(EXIT_SUCCESS);
    } else {
        path = argv[1];
    }

    // create a new workspace
    Workspace workspace = Workspace("example");

    // load a workspace from file
    workspace.load(path);

    // create an simulation
    Simulation simulation = Simulation(&workspace);

    // run the simulation
    simulation.run();

    // hold a bit
    getchar();

    // close the simulation
    simulation.quit();

    // close the workspace
    workspace.close();

    // exit
    exit(EXIT_SUCCESS);

}
