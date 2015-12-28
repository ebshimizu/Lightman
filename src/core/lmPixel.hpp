#ifndef LIGHTMAN_PIXEL_H
#define LIGHTMAN_PIXEL_H

// GLM
#include <glm.hpp>

#include <iostream>

namespace LightmanCore {

using namespace glm;

typedef vec3 GLMVector3;
typedef vec4 GLMVector4;

/**
 * Pixels are defined as vectors of illuminance of the color channels.
 * Lightman uses GLM's single precision floating point vectors as a basis
 * for its pixel representation. Therefore they are compatible with all
 * the vector operations provided by GLM.
 */
class Pixel {
public:

    /**
     * Compute the illuminance value of the pixel.
     */
    virtual inline float illum() = 0;

};

/**
 * Pixel with RGB channels.
 * You may access individual channel through members r,g,b, or x,y,z.
 */
class Pixel3 : public Pixel, public GLMVector3  {
public:

    using vec3::vec3;
    using vec3::operator=;

    inline float illum() {
        return dot((vec3)*this, vec3(0.2126, 0.7152, 0.0722));
    }
};

/**
 * Pixel with RGBA channels.
 * You may access individual channel through members r,g,b,a or x,y,z,w.
 */
class Pixel4 : public Pixel, public GLMVector4 {
public:

    using vec4::vec4;
    using vec4::operator=;

    inline float illum(){
        return dot((vec4)*this, vec4(0.2126, 0.7152, 0.0722, 0));
    }

};

/**
 * Clamp an illuminance value to 8 bit color
 */
inline int clamp(float x){ return x < 0 ? 0 : x > 1 ? 255 : int(x * 255); }

/**
 * Serialization
 */
std::ostream& operator<<( std::ostream& os, const Pixel3& p );
std::ostream& operator<<( std::ostream& os, const Pixel4& p );

}; // namespace LightmanCore

#endif // LIGHTMAN_PIXEL_H
