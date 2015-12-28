#ifndef LIGHTMAN_APPLICATION_H
#define LIGHTMAN_APPLICATION_H

// System
#include <thread>

// GLEW, GLFW
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// Lightman
#include "LightmanCore.h"

// Components
#include "lmViewport.hpp"
//#include "lmTweakbar.hpp"

using namespace LightmanDisplay;

namespace LightmanApplication {

/**
 * Provides display routines and simple interactive interface for
 * workspaces. The application provides a visual presentation
 * of the workspace.
 */
class Application {
public:
  /**
   * Mode of the application.
   */
  typedef enum {

    /// Simply display the current output of the workspace.
    DISPLAY,

    /// Display the output of the workspace and show controls.
    INTERACTIVE,

    /// Renders the animation output of the workspace.
    ANIMATION
  } Mode;

  /**
   * Constructor.
   * Creates a new lightman application instance for the input workspace.
   */
  Application(LightmanCore::Workspace *workspace);

  /**
   * Destructor.
   * Destroys the application and free resources used.
   */
  ~Application();

  /**
   * Set the mode of the application.
   * The mode is set automatically when the workspace is loaded depending
   * on the components of the workspace. Setting a different mode requires
   * the compliance with the workspace. For example, if the workspace does
   * not contain animation, switching to animation mode will have no effect.
   * @param mode application mode to be set.
   */
  void set_mode(Mode mode);

  /**
   * Runs the application.
   * Create a window and runs the application. Note that this creates
   * a new thread that constantly pulls data from the workspace and
   * updates the viewport.
   */
  void run();

  /**
   * Quit the application.
   * This will stop the update thread and close the display. This will frees
   * the application assets but leaves the workspace as is at the time of
   * incovation.
   */
  void quit();

  static void mouse_button_event(GLFWwindow *window, int button, int action,
                                 int mods);
  static void cursor_pos_event(GLFWwindow *window, double xpos, double ypos);
  static void scroll_event(GLFWwindow *window, double xoffset, double yoffset);
  static void key_event(GLFWwindow *window, int key, int scancode, int action,
                        int mods);
  static void char_event(GLFWwindow *window, unsigned int codepoint);
  static void drop_event(GLFWwindow *window, int count, const char **paths);

private:
  /**
   * Display mode of the application.
   */
  Mode mode;

  /**
   * Width of the display.
   */
  size_t w;

  /**
   * Height of the display.
   */
  size_t h;

  /**
   * Window of the application.
   */
  GLFWwindow *window;

  /**
   * Cursor of the application.
   */
  GLFWcursor *cursor;

  /**
   * Viewport of the application.
   */
  Viewport *viewport;

  /**
   * Tweakbars of the application.
   */
  // Tweakbar *tweakbar;

  /**
   * Workspace the application is bound to.
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
   * Animation of the workspace.
   */
  LightmanCore::Animation *animation;

  /**
   * Double buffering, the workspace dumps bitmap output
   * into the draw buffer which is copied to the display
   * buffer which the display thread uses to update the
   * window.
   */
  char *draw_buffer;
  char *disp_buffer;

  /**
   * If the application is running.
   */
  volatile bool running;

  /**
   * The display thread.
   */
  std::thread *update_thread;

  void create_window();
  void update_window();

  void test();
};

}; // namespace LightmanApplication

#endif // LIGHTMAN_APPLICATION_H
