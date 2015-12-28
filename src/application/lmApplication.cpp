#include "lmApplication.hpp"

#include <stdio.h>
#include <unistd.h>
#include <iostream>

using namespace LightmanCore;

namespace LightmanApplication {

Application::Application(Workspace *workspace) {

  // get workspace components
  this->workspace = workspace;
  compositor = workspace->get_compositor();
  tonemapper = workspace->get_tonemapper();
  animation = workspace->get_animation();

  // get window dimension
  w = workspace->get_width();
  h = workspace->get_height();

  // default mode
  mode = DISPLAY;

  // use animation mode if workspace contains animation
  if (animation) {
    mode = ANIMATION;
  }

  // not running yet
  running = false;

  // wait till run
  window = NULL;
  cursor = NULL;
  viewport = NULL;
  // tweakbar = NULL;
}

Application::~Application() {

  // delete tweakbar;
  delete viewport;
}

void Application::set_mode(Mode mode) {

  switch (mode) {
  case DISPLAY:
    this->mode = DISPLAY;
    break;
  case INTERACTIVE:
    this->mode = INTERACTIVE;
    break;
  case ANIMATION:
    if (animation)
      this->mode = ANIMATION;
    break;
  default:
    return;
  }
}

void Application::run() {

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
  update_thread = new thread(&Application::update_window, this);
}

void Application::quit() {

  // change state
  running = false;

  // wait to join update thread
  update_thread->join();

  // terminate glfw
  glfwTerminate();

  // reset workspace output
  workspace->set_bmp_output_buffer(NULL);

  // free application buffers
  delete[] draw_buffer;
  delete[] disp_buffer;
}

void Application::update_window() {

  glfwMakeContextCurrent(window);

  // initialize glew
  glewExperimental = true; // Needed in core profile
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

  // bind tweakbar
  //    if (tweakbar_init(w, h) < 0) {
  //        lmerr("Cannot create tweakbars");
  //        delete(viewport);
  //        glfwTerminate();
  //        exit(EXIT_FAILURE);
  //    } tweakbar_bind(compositor);

  // mode specific initializers
  switch (mode) {
  case DISPLAY:
    break;
  case INTERACTIVE:
    break;
  case ANIMATION:
    animation->start();
    break;
  }

  // run update loop
  while (running) {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // mode specific update
    switch (mode) {
    case DISPLAY:
      break;
    case INTERACTIVE:
      break;
    case ANIMATION:
      animation->update();
      break;
    }

    // update workspace output
    workspace->dump_bmp_to_buffer();

    // swap buffers
    memcpy(disp_buffer, draw_buffer, w * h * 3 * sizeof(char));

    // update display
    viewport->update();

    // tweakbar->render();
    // [ TwDraw(); ]

    // update window
    glfwSwapBuffers(window);

    // get events
    // glfwPollEvents();
    // if ( glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS ||
    //     glfwWindowShouldClose(window)) {
    //    running = false;
    //}
  }
}

void Application::create_window() {

  // initialize GLFW
  if (!glfwInit()) {
    lmerr("GLFW init failure");
    exit(EXIT_FAILURE);
  }

  // window settings
  glfwWindowHint(GLFW_SAMPLES, 16);              // 4x4 antialiasing
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // want OpenGL 3 context
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // want OpenGL 3 context
  glfwWindowHint(GLFW_OPENGL_PROFILE,
                 GLFW_OPENGL_CORE_PROFILE);            // use core profile
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // fowrad compatible
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);            // no resizing

  // create window
  string title = workspace->get_title();
  window = glfwCreateWindow(w, h, title.c_str(), NULL, NULL);
  if (!window) {
    lmerr("GLFW cannot create window");
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  // create cursor
  cursor = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
  glfwSetCursor(window, cursor);

  // set window inputs
  glfwSetInputMode(window, GLFW_STICKY_KEYS, 1);
  glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, 1);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

  // set event handlers
  glfwSetMouseButtonCallback(window, mouse_button_event);
  glfwSetCursorPosCallback(window, cursor_pos_event);
  glfwSetScrollCallback(window, scroll_event);
  glfwSetDropCallback(window, drop_event);
  glfwSetKeyCallback(window, key_event);
  glfwSetCharCallback(window, char_event);
}

//*** Event handling routines ***//

void Application::mouse_button_event(GLFWwindow *window, int button, int action,
                                     int mods) {

  // forward to tweakbar
  // TwEventMouseButtonGLFW(button, action);
}

void Application::cursor_pos_event(GLFWwindow *window, double xpos,
                                   double ypos) {

  // forward to tweakbar
  // TwEventMousePosGLFW(xpos, ypos);
}

void Application::scroll_event(GLFWwindow *window, double xoffset,
                               double yoffset) {
  // forward to tweakbar
  // TwEventMouseWheelGLFW(yoffset);
}

void Application::key_event(GLFWwindow *window, int key, int scancode,
                            int action, int mods) {
  // forward to tweakbar
  // TwEventKeyGLFW(key, action);
}

void Application::char_event(GLFWwindow *window, unsigned int codepoint) {
  // forward to tweakbar?
  // TwEventCharGLFW() -> this still uses glfw2 definition
}

void Application::drop_event(GLFWwindow *window, int count,
                             const char **paths) {

  // TODO: create layer for new input file (drag and drop)

  // for (size_t i = 0; i < count; ++i) compositor->add_layer(paths[i]);
}

}; // namespace LightmanApplication
