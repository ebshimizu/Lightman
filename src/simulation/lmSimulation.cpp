#include "lmSimulation.hpp"

#include <stdio.h>

#ifndef _MSC_VER
  #include <unistd.h>
#endif

#include <iostream>

using namespace LightmanCore;

namespace LightmanSimulation {

Simulation::Simulation(Workspace *workspace) {

  // get workspace components
  this->workspace = workspace;
  compositor = workspace->get_compositor();
  tonemapper = workspace->get_tonemapper();

  // get window dimension
  w = workspace->get_width();
  h = workspace->get_height();

  // not running yet
  running = false;

  // wait till run
  window = NULL;
  viewport = NULL;
}

Simulation::~Simulation() {

  // delete tweakbar;
  delete viewport;
}

void Simulation::run() {

  // check if workspace is initialized
  if (!(w * h)) {
    lmerr("Workspace not initialized");
    return;
  }

  // allocated display buffer
  draw_buffer = new char[3 * w * h];
  disp_buffer = new char[3 * w * h];

  // set to workspace output
  workspace->set_bmp_output_buffer(draw_buffer);

  // create the window
  create_window();

  // change state
  running = true;

  // run display thread
  update_thread = new thread(&Simulation::update_window, this);
}

void Simulation::quit() {

  // change state
  running = false;

  // wait to join update thread
  update_thread->join();

  // terminate glfw
  glfwTerminate();

  // reset workspace output
  workspace->set_bmp_output_buffer(NULL);

  // free lmSimulation buffers
  delete[] draw_buffer;
  delete[] disp_buffer;
}

void Simulation::update_window() {

  glfwMakeContextCurrent(window);

  // initialize glew
  glewExperimental = true;
  if (glewInit() != GLEW_OK) {
    lmerr("GLEW init failure");
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  // opengl stuff
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  // VAO
  GLuint VertexArrayID;
  glGenVertexArrays(1, &VertexArrayID);
  glBindVertexArray(VertexArrayID);

  // bind viewport
  viewport = new Viewport();
  if (viewport->bind(disp_buffer, w, h) < 0) {
    lmerr("Cannot create viewport");
    delete (viewport);
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  // run update loop
  while (running) {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // update workspace output
    workspace->dump_bmp_to_buffer();

    // swap buffers
    memcpy(disp_buffer, draw_buffer, w * h * 3 * sizeof(char));

    // update display
    viewport->update();

    // update window
    glfwSwapBuffers(window);
  }
}

void Simulation::create_window() {

  // initialize GLFW
  if (!glfwInit()) {
    lmerr("GLFW init failure");
    exit(EXIT_FAILURE);
  }

  // window settings
  glfwWindowHint(GLFW_SAMPLES, 16);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

  // create window
  string title = workspace->get_title();
  window = glfwCreateWindow(w, h, title.c_str(), NULL, NULL);
  if (!window) {
    lmerr("GLFW cannot create window");
    glfwTerminate();
    exit(EXIT_FAILURE);
  }
}

}; // namespace LightmanSimulation
