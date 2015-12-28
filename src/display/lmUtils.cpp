#include "lmUtils.hpp"
#include "lmConsole.hpp"

#include <stdio.h>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>

GLuint compile_shaders() {

  lmmsg("Creating shader program");

  // Create the shaders
  GLuint vert_shader = glCreateShader(GL_VERTEX_SHADER);
  GLuint frag_shader = glCreateShader(GL_FRAGMENT_SHADER);

  const char *vert_shader_src =
      "#version 330 core"
      "\nlayout(location = 0) in vec3 vertex_position_modelspace;"
      "\nlayout(location = 1) in vec2 vertex_uv;"
      "\nout vec2 uv;"
      "\nvoid main() {"
      "\n    gl_Position = vec4(vertex_position_modelspace, 1);"
      "\n    uv = vertex_uv;"
      "\n}";

  const char *frag_shader_src =
      "#version 330 core"
      "\nin vec2 uv;"
      "\nout vec3 color;"
      "\nuniform sampler2D texture_sampler;"
      "\nvoid main() {"
      "\n    color = texture( texture_sampler, uv ).rgb;"
      "\n}";

  GLint result = GL_FALSE;
  int info_length;

  // Compile Vertex Shader
  lmout(" - Compiling vertex shader");
  glShaderSource(vert_shader, 1, &vert_shader_src, NULL);
  glCompileShader(vert_shader);

  // Check Vertex Shader
  glGetShaderiv(vert_shader, GL_COMPILE_STATUS, &result);
  glGetShaderiv(vert_shader, GL_INFO_LOG_LENGTH, &info_length);
  if (info_length > 0) {
    std::vector<char> vert_shader_errmsg(info_length + 1);
    glGetShaderInfoLog(vert_shader, info_length, NULL, &vert_shader_errmsg[0]);
    printf("%s\n", &vert_shader_errmsg[0]);
  }

  // Compile Fragment Shader
  lmout(" - Compiling fragment shader");
  glShaderSource(frag_shader, 1, &frag_shader_src, NULL);
  glCompileShader(frag_shader);

  // Check Fragment Shader
  glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &result);
  glGetShaderiv(frag_shader, GL_INFO_LOG_LENGTH, &info_length);
  if (info_length > 0) {
    std::vector<char> frag_shader_errmsg(info_length + 1);
    glGetShaderInfoLog(frag_shader, info_length, NULL, &frag_shader_errmsg[0]);
    printf("%s\n", &frag_shader_errmsg[0]);
  }

  // Link the program
  lmout(" - Attaching shader program");
  GLuint program = glCreateProgram();
  glAttachShader(program, vert_shader);
  glAttachShader(program, frag_shader);
  glLinkProgram(program);

  // Check the program
  glGetProgramiv(program, GL_LINK_STATUS, &result);
  glGetProgramiv(program, GL_INFO_LOG_LENGTH, &info_length);
  if (info_length > 0) {
    std::vector<char> program_errmsg(info_length + 1);
    glGetProgramInfoLog(program, info_length, NULL, &program_errmsg[0]);
    printf("%s\n", &program_errmsg[0]);
  }

  glDetachShader(1, vert_shader);
  glDetachShader(1, frag_shader);
  glDeleteShader(vert_shader);
  glDeleteShader(frag_shader);

  return program;
}

GLuint make_buffer(GLenum target, GLsizei buffer_size,
                   const void *buffer_data) {
  GLuint buffer;
  glGenBuffers(1, &buffer);
  glBindBuffer(target, buffer);
  glBufferData(target, buffer_size, buffer_data, GL_STATIC_DRAW);
  return buffer;
}

GLuint make_texture_rgb(void *pixel_buffer, unsigned int width,
                        unsigned int height) {

  // Create one OpenGL texture
  GLuint textureID;
  glGenTextures(1, &textureID);
  glBindTexture(GL_TEXTURE_2D, textureID);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
               GL_UNSIGNED_BYTE, pixel_buffer);

  // trilinear filtering
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glGenerateMipmap(GL_TEXTURE_2D);

  // Return the ID of the texture we just created
  return textureID;
}

GLuint make_texture_rgba(void *pixel_buffer, unsigned int width,
                         unsigned int height) {

  // Create one OpenGL texture
  GLuint textureID;
  glGenTextures(1, &textureID);
  glBindTexture(GL_TEXTURE_2D, textureID);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, pixel_buffer);

  // trilinear filtering
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glGenerateMipmap(GL_TEXTURE_2D);

  // Return the ID of the texture we just created
  return textureID;
}

void update_texture_rgb(GLuint textureID, void *pixel_buffer,
                        unsigned int width, unsigned int height) {
  glBindTexture(GL_TEXTURE_2D, textureID);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
               GL_UNSIGNED_BYTE, pixel_buffer);
}

void update_texture_rgba(GLuint textureID, void *pixel_buffer,
                         unsigned int width, unsigned int height) {
  glBindTexture(GL_TEXTURE_2D, textureID);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, pixel_buffer);
}
