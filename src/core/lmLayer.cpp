#include "lmLayer.hpp"

namespace LightmanCore {

Layer::Layer(const char *file, const char *name) {

  if (name) {
    this->name = name;
  }

  // load single exr file?
  w = 0;
  h = 0;
  pixels = NULL;

  active = true;
  modulator = vec3(1, 1, 1);
}

Layer::Layer(Pixel3 *pixel_buffer, size_t width, size_t height,
             const char *name) {

  if (name) {
    this->name = name;
  }

  w = width;
  h = height;
  pixels = pixel_buffer;

  active = true;
  modulator = vec3(1, 1, 1);
}

Layer::~Layer() { delete[] pixels; }

string Layer::get_name() { return name; }

size_t Layer::get_width() { return w; }

size_t Layer::get_height() { return h; }

size_t Layer::get_size() { return w * h; }

bool Layer::is_active() { return active; }

void Layer::enable() { active = true; }

void Layer::disable() { active = false; }

vec3 Layer::get_modulator() { return modulator; }

void Layer::set_modulator(vec3 modulator) { this->modulator = modulator; }

Pixel3 *Layer::get_pixels() { return pixels; }

}; // namespace LightmanCore
