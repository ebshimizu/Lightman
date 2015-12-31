#include "lmCompositor.hpp"
#include "lmConsole.hpp"

#include <string.h>
#include <chrono>

using namespace std;
using namespace chrono;

namespace LightmanCore {

Compositor::Compositor() {

  // layers
  layers = set<Layer *>();
  ambient_layer = NULL;

  // buffers
  compose_buffer = NULL;
  output_buffer = NULL;

  //// threshold
  //amb_diff = 0.01;
}

Compositor::~Compositor() {

  // free frame buffer if allocated
  if (compose_buffer)
    delete[] compose_buffer;

  // free all layers
  layers.clear();
}

size_t Compositor::get_width() { return w; }

size_t Compositor::get_height() { return h; }

set<Layer *> Compositor::get_layers() { return layers; }

void Compositor::add_layer(Layer *layer) {

  if (!layers.size()) {

    // set size when first layer is added
    w = layer->get_width();
    h = layer->get_height();

    // allocate memory for composition buffer
    compose_buffer = new Pixel3[w * h]();
    if (!compose_buffer) {
      lmerr("Cannot create composite buffer");
      return;
    }

  } else {

    // check size consistency

    size_t layer_w = layer->get_width();
    size_t layer_h = layer->get_height();
    if (layer_w != w || layer_h != h) {
      return;
    }
  }

  layers.insert(layer);
}

void Compositor::del_layer(Layer *layer) { layers.erase(layer); }

Layer *Compositor::get_layer_by_name(const char *layer_name) {

  Layer *layer;
  set<Layer *>::iterator it = layers.begin();
  while (it != layers.end()) {

    layer = *it;
    if (layer->get_name() == layer_name) {
      return layer;
    }

    ++it;
  }

  return NULL;
}

void Compositor::del_layer_by_name(const char *layer_name) {

  Layer *layer;
  set<Layer *>::iterator it = layers.begin();
  while (it != layers.end()) {

    layer = *it;
    if (layer->get_name() == layer_name) {
      layers.erase(layer);
      return;
    }

    ++it;
  }

  lmerr("Layer not found: " << layer_name);
}

Layer *Compositor::get_ambient() { return ambient_layer; }

void Compositor::set_ambient(Layer *layer) {

  set<Layer *>::iterator it = layers.find(layer);
  if (it != layers.end()) {

    disable_ambient();
    ambient_layer = layer;
    enable_ambient();
  }
}

void Compositor::enable_ambient() {

  if (ambient_layer) {

    // get ambient pixels and leave out the ambient layer itself
    Pixel3 *amb_pixels = ambient_layer->get_pixels();
    layers.erase(ambient_layer);

    // process each layer
    Layer *layer;
    set<Layer *>::iterator it = layers.begin();
    while (it != layers.end()) {

      layer = *it;
      Pixel3 *pixels = layer->get_pixels();
      for (size_t i = 0; i < w * h; ++i) {

        pixels[i] -= amb_pixels[i];

        // filter low values here ?
        // if (pixels[i].luminance() < amb_diff) {
        //    pixels[i] = Pixel3();
        //}
      }
      ++it;
    }

    // put ambient layer back
    layers.insert(ambient_layer);
  }
}

void Compositor::disable_ambient() {

  if (ambient_layer) {

    // get ambient pixels and leave out the ambient layer itself
    Pixel3 *amb_pixels = ambient_layer->get_pixels();

    // process each layer
    Layer *layer;
    set<Layer *>::iterator it = layers.begin();
    while (it != layers.end()) {

      layer = *it;
      Pixel3 *pixels = layer->get_pixels();
      for (size_t i = 0; i < w * h; ++i) {

        pixels[i] += amb_pixels[i];
      }
      ++it;
    }

    // put ambient layer back
    layers.insert(ambient_layer);

    // disable ambient layer
    ambient_layer = NULL;
  }
}

void Compositor::set_output_buffer(Pixel3 *buffer) {

  if (buffer)
    output_buffer = buffer;
}

void Compositor::render() {

  // time_point<system_clock> sys_begin = system_clock::now();

  // get target buffer
  Pixel3 *target = output_buffer ? output_buffer : compose_buffer;

  // clear previous rendering
  memset((void *)target, 0, sizeof(Pixel3) * w * h);

  // composite all active layers
  Layer *layer;
  set<Layer *>::iterator it = layers.begin();
  while (it != layers.end()) {

    layer = *it;
    vec3 m = layer->get_modulator();
    Pixel3 *pixels = layer->get_pixels();

    for (int i = 0; i < w * h; i++) {
      if (layer->is_active()) {
        target[i].r += m.r * pixels[i].r;
        target[i].g += m.g * pixels[i].g;
        target[i].b += m.b * pixels[i].b;
      }
    }
    it++;
  }

  //////////////////////////////////////
  // FLIP THE LOOP FOR SOME SPEEDUPS? //
  //////////////////////////////////////
  //    set<Layer*>::iterator it; Layer *layer;
  //    for (int i = 0; i < w * h; i++ ) {
  //
  //        it = layers.begin();
  //        while(it != layers.end()) {
  //
  //            layer = *it;
  //            if (layer->is_active()) {
  //
  //                vec3 m = layer->get_modulator();
  //                Pixel3 * __restrict__ pixels = layer->get_pixels();
  //
  //                target[i].r += m.r * pixels[i].r;
  //                target[i].g += m.g * pixels[i].g;
  //                target[i].b += m.b * pixels[i].b;
  //            }
  //
  //            it++;
  //        }
  //    }

  // time_point<system_clock> sys_end = system_clock::now();
  // double elapsed = ((duration<double>) (sys_end - sys_begin)).count();
  // lmmsg("time: " << elapsed * 1000);
}

}; // namespace LightmanCore
