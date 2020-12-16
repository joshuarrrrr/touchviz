#pragma once

#include <set>
#include <stdexcept>
#include <string>
#include <vector>

#include <libevdev/libevdev.h>
#include <spdlog/spdlog.h>

#include <touch_point.hpp>

class MultitouchDevice {
 public:
  MultitouchDevice(const std::string& path);
  MultitouchDevice(const MultitouchDevice&) = delete;
  MultitouchDevice(MultitouchDevice&&) noexcept = default;
  ~MultitouchDevice();

  MultitouchDevice& operator=(const MultitouchDevice&) = delete;
  MultitouchDevice& operator=(MultitouchDevice&&) noexcept = default;

  const std::string& name() const;
  int num_slots() const;
  std::set<std::shared_ptr<TouchPoint>> touch_points() const;

  bool update();
  void run();

 protected:
  struct libevdev* _dev = nullptr;
  int _fd;

  std::string _path;
  std::string _name;

  unsigned int _num_slots;
  int _x_min, _x_max;
  int _y_min, _y_max;

  std::vector<std::shared_ptr<TouchPoint>> _touch_points;
};