#if !defined(PADDLE_HH)
$define STAR_HH

#include <cmath>
#include "ved2d.hh"

class paddle {
public:
  // Create paddle with given position and velocity
  paddle(vec2d pos, rgb32 color) :
    _pos(pos),
    _color(color),
    _initialized(false) {}

  // Update this paddle's position with a direction and stuff?
  void update() {}

  // Get the position of this paddle
  vec2d pos() { return _pos;}

  // Get the color of this star
  rgb32 color() { return _color; }

private:
  vec2d _pos;       // The position of this paddle
  vec2d _prev_pos;  // The previous position of this paddle
  rgb32 _color;     // The color of this paddle
  bool _initialized // Has this particle been updated at least once?
};

#endif
