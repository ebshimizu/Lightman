#ifndef LIGHTMAN_SIMULATION_H
#define LIGHTMAN_SIMULATION_H

// System
#include <thread>

// GLEW, GLFW
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// Lightman
#include "LightmanCore.h"

// Viewport
#include "lmViewport.hpp"

using namespace LightmanDisplay;

namespace LightmanSimulation {

/**
 * Similar to the functionalities of applications but provides optimized
 * routines for faster response. The simulation is built on top of the
 * workspace and renders the current state of the workspace in realtime.
 */
class Simulation {
public:
  /**
   * Constructor.
   * Creates a new lightman simulation instance for the input workspace.
   */
  Simulation(LightmanCore::Workspace *workspace);

  /**
   * Destructor.
   * Destroys the simulation and free resources used.
   */
  ~Simulation();

  /**
   * Runs the simulation.
   * Create a window and runs the simulation. Note that this creates
   * a new thread that constantly pulls data from the workspace and
   * updates the viewport.
   */
  void run();

  /**
   * Quit the simulation.
   * This will stop the update thread and close the display and free
   * memory used by the simulation but leaves the workspace as is at
   * the time of incovation.
   */
  void quit();

private:
  /**
   * Width of the display.
   */
  size_t w;

  /**
   * Height of the display.
   */
  size_t h;

  /**
   * Window of the simulation.
   */
  GLFWwindow *window;

  /**
   * Viewport of the simulation.
   */
  Viewport *viewport;

  /**
   * Workspace the simulation is bound to.
   */
  LightmanCore::Workspace *workspace;

  /**
   * Compositor of the workspace.
   */
  LightmanCore::Compositor *compositor;

  /**
   * Tonemapper of the workspace.
   */
  LightmanCore::ToneMapper *tonemapper;

  /**
   * Double buffering, the workspace dumps bitmap output
   * into the draw buffer which is copied to the display
   * buffer which the display thread uses to update the
   * window.
   */
  char *draw_buffer;
  char *disp_buffer;

  /**
   * If the simulation is running.
   */
  volatile bool running;

  /**
   * The display thread.
   */
  std::thread *update_thread;

  void create_window();
  void update_window();
};

}; // namespace LightmanSimulation

#endif // LIGHTMAN_SIMULATION_H
