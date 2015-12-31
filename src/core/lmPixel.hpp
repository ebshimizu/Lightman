#ifndef LIGHTMAN_PIXEL_H
#define LIGHTMAN_PIXEL_H

// GLM
#include <glm.hpp>

#include <iostream>

namespace LightmanCore {

using namespace glm;

/**
 * Pixel with RGB channels.
 * You may access individual channel through members r,g,b, or x,y,z.
 */
typedef vec3 Pixel3;
typedef vec4 Pixel4;

inline float illum(Pixel3 p) {
	return dot(p, vec3(0.2126, 0.7152, 0.0722));
}

inline float illum(Pixel4 p) {
	return dot(p, vec4(0.2126, 0.7152, 0.0722, 0));
}

/**
 * Clamp an illuminance value to 8 bit color
 */
inline int clamp(float x) { return x < 0 ? 0 : x > 1 ? 255 : int(x * 255); }

/**
 * Serialization
 */
std::ostream &operator<<(std::ostream &os, const Pixel3 &p);
std::ostream &operator<<(std::ostream &os, const Pixel4 &p);

}; // namespace LightmanCore

#endif // LIGHTMAN_PIXEL_H
