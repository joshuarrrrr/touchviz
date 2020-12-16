#pragma once

#include <vector>

#include <glm/glm.hpp>

class TouchPoint {
 public:
  TouchPoint(int id);
  TouchPoint(const TouchPoint&) = delete;
  TouchPoint(TouchPoint&&) noexcept = default;
  ~TouchPoint() = default;

  TouchPoint& operator=(const TouchPoint&) = delete;
  TouchPoint& operator=(TouchPoint&&) noexcept = default;

  bool operator<(const TouchPoint& other) const;

  int id() const;
  bool active() const;
  glm::vec2 pos() const;
  glm::vec2 start_pos() const;
  const std::vector<glm::vec2>& positions() const;

  void set_x(float x);
  void set_y(float y);

  void update();
  void finish();

 protected:
  int _id;
  bool _active;
  float _x, _y;

  std::vector<glm::vec2> _positions;
};