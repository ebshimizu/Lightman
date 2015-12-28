#ifndef LIGHTMAN_TONEMAPPER_H
#define LIGHTMAN_TONEMAPPER_H

#include "lmLayer.hpp"

namespace LightmanCore {

/**
 * Compresses the dynamic range and maps illuminance to colors.
 * The tone mapper converts the illuminance-based pixels of the
 * HDR layer to standard 8-bit RGB colors and outputs clamped RGB bitmap.
 * The default tone mapper does only gamma correction and exposure adjustment.
 */
class ToneMapper {
public:
  /**
   * Constructor.
   */
  ToneMapper();

  /**
   * Destructor.
   */
  ~ToneMapper();

  /**
   * Get the tone mapping algorithm.
   * @return the name of tone mapping algorithm implemented.
   */
  string get_type();

  /**
   * Get display gamma.
   * @return the display gamma.
   */
  float get_gamma();

  /**
   * Set display gamma.
   * @param gamma the display gamma to be set.
   */
  void set_gamma(float gamma);

  /**
   * Get exposure level adjustment.
   * @return the exposure level adjustment (in F-stops).
   */
  float get_level();

  /**
   * Set exposure level adjustment.
   * @param level the exposure level adjustment to be set (in F-stops).
   */
  void set_level(float level);

  /**
   * Set the input of the tonemapper.
   * @param buffer pointer to the Pixel buffer to be set as input.
   * @param w width of the input.
   * @param h height of the input.
   */
  void set_input(Pixel3 *buffer, size_t w, size_t h);

  /**
   * Set the HDR Pixel output buffer of the tonemapper.
   * @param buffer pointer to the Pixel3 buffer to be set as output.
   */
  void set_output_hdr(Pixel3 *buffer);

  /**
   * Set the bitmap output buffer of the tonemapper.
   * @param buffer pointer to the Pixel3 buffer to be set as output.
   */
  void set_output_bmp(char *buffer);

  /**
   * Compresses the dynamic range only, and save the result to
   * the HDR output buffer. If the output buffer has not been
   * set, the call has no effect.
   */
  virtual void apply_hdr();

  /**
   * Compresses the dynamic range and convert illuminance space pixels
   * to color space. The result is save to the the bitmap output buffer.
   * If the output buffer has not been set, the call has no effect.
   */
  virtual void apply_bmp();

  /**
   * Resets all parameters, while preserving input and output bindings.
   */
  virtual void reset();

protected:
  /**
   * Name of the the tone map implementation.
   */
  string type;

  /**
   * Width of the input.
   */
  size_t w;

  /**
   * Height of the input.
   */
  size_t h;

  /**
   * Display gamma.
   * By default the display gamma is set to 2.2.
   */
  float gamma;

  /**
   * Exposure adjustment.
   * The level changes the global exposure level of all pixels in the layer.
   * Note that this is defined in F-stops. A value of 1 would increment
   * the current exposure by a full stop. Likewise and a value of -1 would
   * decrement it by a full stop. By default this is set to 0.
   */
  float level;

  /**
   * The input HDR Pixel buffer of the tone mapper.
   * Note that the tone mapper assumes a valid input buffer.
   * Behavior of applying the tone map is undefined if the
   * input buffer is corrupted.
   */
  Pixel3 *input_buffer;

  /**
   * The output HDR Pixel buffer of the tone mapper.
   * Note that the tone mapper assumes a valid output buffer.
   * Behavior of applying the tone map is undefined if the
   * output buffer is corrupted.
   */
  Pixel3 *hdr_output_buffer;

  /**
   * The output bitmap buffer of the tone mapper.
   * Note that the tone mapper assumes a valid output buffer.
   * Behavior of applying the tone map is undefined if the
   * output buffer is corrupted.
   */
  char *bmp_output_buffer;
};

/**
 * ToneMapper that implementation of Reinhard's algorithm.
 */
class TMReinhard : public ToneMapper {
public:
  /**
   * Constructor.
   */
  TMReinhard();

  /**
   * Destructor.
   */
  ~TMReinhard();

  /**
   * Get key value.
   * @return current key value.
   */
  float get_key();

  /**
   * Set key value.
   * @param key new key value to set
   */
  void set_key(float key);

  /**
   * Get white point value.
   * @return current white point value.
   */
  float get_wht();

  /**
   * Set white point value.
   * @param wht new white point value to set
   */
  void set_wht(float wht);

  /**
   * Automatically adjusts paramenters.
   * This recomputes the exposure value and to have the average luminance
   * of the layer map to that of mid-grey. Ignored if the tone mapper is
   * not bound to any layer.
   */
  void auto_adjust();

  /**
   * Compresses the dynamic range only, and save the result to
   * the HDR output buffer. If the output buffer has not been
   * set, the call has no effect.
   */
  virtual void apply_hdr();

  /**
   * Compresses the dynamic range and convert illuminance space pixels
   * to color space. The result is save to the the bitmap output buffer.
   * If the output buffer has not been set, the call has no effect.
   */
  virtual void apply_bmp();

  /**
   * Resets all parameters, while preserving layer binding.
   */
  void reset();

private:
  /**
   * The key value.
   * The log-average luminance will be mapped to a key value.
   */
  float key;

  /**
   * White point value.
   * The white point value specifies the minimum luminance for a pixel
   * to be converted to white.
   */
  float wht;

  /**
   * Log-average of the luminance of all pixels in the layer.
   * The log-average is computed with respect to the exposure value.
   */
  float avg;
};

/**
 * ToneMapper that implementation of Uncharted 2's algorithm.
 */
class TMUncharted : public ToneMapper {
public:
  /**
   * Constructor.
   */
  TMUncharted();

  /**
   * Destructor.
   */
  ~TMUncharted();

private:
};

}; // namespace LightmanCore

#endif // LIGHTMAN_TONEMAPPER_H
