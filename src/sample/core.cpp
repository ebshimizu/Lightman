#include "LightmanCore.h"

// System
#include <stdio.h>
#include <iostream>

using namespace LightmanCore;

int main(int argc, const char *argv[]) {

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

  // write out as an image
  workspace.dump_exr("out");

  // close the workspace
  workspace.close();

  // exit
  exit(EXIT_SUCCESS);
}
