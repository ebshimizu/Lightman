#ifndef LIGHTMAN_UTILS_H
#define LIGHTMAN_UTILS_H

#include <GL/glew.h>

GLuint compile_shaders();

GLuint make_buffer(GLenum target,
                   GLsizei buffer_size,
                   const void *buffer_data);

GLuint make_texture_rgb(void* pixel_buffer,
                        unsigned int width,
                        unsigned int height);

GLuint make_texture_rgba(void* pixel_buffer,
                         unsigned int width,
                         unsigned int height);

void update_texture_rgb(GLuint textureID,
                        void* pixel_buffer,
                        unsigned int width,
                        unsigned int height);

void update_texture_rgba(GLuint textureID,
                         void* pixel_buffer,
                         unsigned int width,
                         unsigned int height);


#endif // LIGHTMAN_UTILS_H
