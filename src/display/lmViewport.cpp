#include "lmViewport.hpp"

#include <GL/glew.h>

namespace LightmanDisplay {

Viewport::Viewport() {

  // compile shaders
  program = compile_shaders();
  glUseProgram(program);

  const GLfloat vertex_buffer_data[] = {-1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f,
                                        -1.0f, 1.0f,  0.0f, 1.0f, 1.0f,  0.0f,
                                        -1.0f, 1.0f,  0.0f, 1.0f, -1.0f, 0.0f};

  const GLfloat uv_buffer_data[] = {0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
                                    1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f};

  // make buffers
  vertex_buffer = make_buffer(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data),
                              vertex_buffer_data);

  uv_buffer =
      make_buffer(GL_ARRAY_BUFFER, sizeof(uv_buffer_data), uv_buffer_data);
}

Viewport::~Viewport() {

  glDeleteProgram(program);
  glDeleteBuffers(1, &vertex_buffer);
  glDeleteBuffers(1, &uv_buffer);
  glDeleteTextures(1, &texture);
}

int Viewport::bind(char *buffer, size_t w, size_t h) {

  this->w = w;
  this->h = h;
  pixels = buffer;

  texture = make_texture_rgb(pixels, w, h);

  return 0;
}

void Viewport::update() {

  update_texture_rgb(texture, pixels, w, h);
  textureID = glGetUniformLocation(program, "texture_sampler");

  // bind texture
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture);
  glUniform1i(textureID, 0);

  // 1st attribute buffer : vertices
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
  glVertexAttribPointer(0,        // attribute
                        3,        // size
                        GL_FLOAT, // type
                        GL_FALSE, // normalized
                        0,        // stride
                        (void *)0 // array buffer offset
                        );

  // 2nd attribute buffer : UVs
  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, uv_buffer);
  glVertexAttribPointer(1,        // attribute
                        2,        // size : U+V => 2
                        GL_FLOAT, // type
                        GL_FALSE, // normalized?
                        0,        // stride
                        (void *)0 // array buffer offset
                        );

  // draw triangles
  glDrawArrays(GL_TRIANGLES, 0, 3 * 2);
  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
}

}; // namespace LightmanDisplay
