#include "LightmanApplication.h"

// System
#include <stdio.h>
#include <iostream>

using namespace LightmanCore;
using namespace LightmanApplication;

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

    // create an application
    Application application = Application(&workspace);

    // run the application
    application.run();

    // hold a bit
    getchar();

    // close the application
    application.quit();

    // prompt for save on exit
    lmpmt("Save file? (y/n)");

    // get input
    string input; lmins(input);
    while (input != "y" && input != "n") {
        lmpmt("Please answer y for yes or n for no");
        lmins(input);
    }

    // if save file
    if (input == "y") workspace.dump_exr("out");

    // close the workspace
    workspace.close();

    // exit
    exit(EXIT_SUCCESS);

}
