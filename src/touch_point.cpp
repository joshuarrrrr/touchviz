#include <touch_point.hpp>

TouchPoint::TouchPoint(int id) : _id{id}, _active{true}, _x{-1.0f}, _y{-1.0f} {}

bool TouchPoint::operator<(const TouchPoint& other) const {
  return _id < other.id();
}

int TouchPoint::id() const {
  return _id;
}

bool TouchPoint::active() const {
  return _active;
}

glm::vec2 TouchPoint::pos() const {
  if (_positions.empty()) {
    return glm::vec2{-1.0f, -1.0f};
  }
  return _positions.back();
}

glm::vec2 TouchPoint::start_pos() const {
  if (_positions.empty()) {
    return glm::vec2{-1.0f, -1.0f};
  }
  return _positions.front();
}

const std::vector<glm::vec2>& TouchPoint::positions() const {
  return _positions;
}

void TouchPoint::set_x(float x) {
  _x = x;
}

void TouchPoint::set_y(float y) {
  _y = y;
}

void TouchPoint::update() {
  if (_x > 0 && _y > 0) {
    _positions.emplace_back(glm::vec2{_x, _y});
  }
}

void TouchPoint::finish() {
  _active = false;
}
