#ifndef LIGHTMAN_LAYER_H
#define LIGHTMAN_LAYER_H

#include <string>
#include <vector>

#include "lmPixel.hpp"

namespace LightmanCore {

using namespace std;

/**
 * Layers are defined as arrays of pixels with some modifiers.
 * Layers in lightman have RGB channels only since it does not make
 * sense for an alpha channel to exist in the illuminance space.
 */
class Layer {
public:
  /**
   * Constructor.
   * Creates a new layer from an OpenEXR file.
   */
  Layer(const char *file, const char *name = NULL);

  /**
   * Constructor.
   * Creates a new layer from allocated buffer.
   */
  Layer(Pixel3 *pixel_buffer, size_t w, size_t h, const char *name = NULL);

  /**
   * Destructor.
   */
  ~Layer();

  /**
   * Get the name of the layer.
   */
  string get_name();

  /**
   * Get the width of the layer.
   */
  size_t get_width();

  /**
   * Get the height of the layer.
   */
  size_t get_height();

  /**
   * Get the number of pixels in the layer.
   */
  size_t get_size();

  /**
   * Check if the layer is active.
   */
  bool is_active();

  /**
   * Enables the layer.
   */
  void enable();

  /**
   * Disables the layer.
   */
  void disable();

  /**
   * Get the modulator of the layer.
   */
  vec3 get_modulator();

  /**
   * Set the modulator of the layer.
   */
  void set_modulator(vec3 modulator);

  /**
   * Get a pointer to the pixels.
   */
  Pixel3 *get_pixels();

private:
  /**
   * Name of the layer.
   */
  string name;

  /**
   * Width.
   * The width of the layer.
   */
  size_t w;

  /**
   * Height.
   * The height of the layer.
   */
  size_t h;

  /**
   * Is the layer active.
   * If a layer is not active, it is ignored in composition.
   */
  bool active;

  /**
   * Channel specific modulator of the layer.
   * A vector multiplier that does channel-specific modulation.
   */
  vec3 modulator;

  /**
   * Pixel buffer of the layer.
   * Do note that the pixels have RGB channels only since it does not
   * make sense for an alpha channel to exist in the illuminance space.
   */
  Pixel3 *pixels;
};

}; // namespace LightmanCore

#endif // LIGHTMAN_LAYER_H
