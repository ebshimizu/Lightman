#include "lmPixel.hpp"

namespace LightmanCore {

std::ostream &operator<<(std::ostream &os, const Pixel3 &p) {
  return os << '(' << p.r << ',' << p.g << ',' << p.b << ')';
}

std::ostream &operator<<(std::ostream &os, const Pixel4 &p) {
  return os << '(' << p.r << ',' << p.g << ',' << p.b << ',' << p.a << ')';
}

}; // namespace LightmanCore
