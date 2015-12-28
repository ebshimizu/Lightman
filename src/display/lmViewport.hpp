#ifndef LIGHTMAN_VIEWPORT_H
#define LIGHTMAN_VIEWPORT_H

#include "lmUtils.hpp"

namespace LightmanDisplay {

/**
 * Draw bitmaps in the application window. The viewport is simply in charge
 * of drawing a bitmap on screen. The application uses it to display the
 * results from the workspace. Note that the viewport is completely
 * independent from the application, which uses glfw3 for creating its
 * OpenGL context. It is possible to use the viewport with other GL
 * context libraries.
 */
class Viewport {
public:

    /**
     * Constructor.
     */
    Viewport();

    /**
     * Destructor.
     */
    ~Viewport();

    /**
     * Bind the viewport to a bitmap buffer.
     * @param buffer buffer that holds the bitmap pixels
     * @param w width of the bitmap
     * @param h height of the bitmap
     * @return 0 if successful
     *        -1 if failed
     */
    int bind(char *buffer, size_t w, size_t h);

    /**
     * Update the viewport.
     */
    void update();

private:

    /**
     * Width of the window context.
     */
    size_t w;

    /**
     * Height of the window context.
     */
    size_t h;

    /**
     * Pointer to the the content pixels (in GL_RGB).
     */
    void *pixels;

    /**
     * Vertex buferr;
     */
    GLuint vertex_buffer;

    /**
     * UV-coord buferr;
     */
    GLuint uv_buffer;

    /**
     * Texture of display port.
     */
    GLuint texture;

    /**
     * Texture ID of the displayed texture.
     */
    GLuint textureID;

    /**
     * Shader program.
     */
    GLuint program;

};

}; // namespace LightmanDisplay

#endif // LIGHTMAN_VIEWPORT_H
