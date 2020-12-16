#include <multitouch_device.hpp>

MultitouchDevice::MultitouchDevice(const std::string &path) : _path{path} {
  std::cout << "Opening device at \"" << _path << "\"" << std::endl;

  _fd = open(_path.c_str(), O_RDONLY | O_NONBLOCK);
  int err = libevdev_new_from_fd(_fd, &_dev);
  if (err != 0) {
    auto error_msg = std::string{strerror(-err)};
    throw std::runtime_error("Failed to open device: " + error_msg);
  }

  _name = std::string{libevdev_get_name(_dev)};

  if (!(libevdev_has_event_type(_dev, EV_ABS) &&
        libevdev_has_event_code(_dev, EV_ABS, ABS_MT_TRACKING_ID) &&
        libevdev_has_event_code(_dev, EV_ABS, ABS_MT_POSITION_X) &&
        libevdev_has_event_code(_dev, EV_ABS, ABS_MT_POSITION_Y))) {
    throw std::runtime_error("Device \"" + _name +
                             "\" is not a multitouch device");
  }

  _num_slots = libevdev_get_num_slots(_dev);
  if (_num_slots < 1) {
    throw std::runtime_error("Device has invalid number of slots!");
  }
  _touch_points = std::vector<std::shared_ptr<TouchPoint>>{_num_slots};

  _x_min = libevdev_get_abs_minimum(_dev, ABS_MT_POSITION_X);
  _x_max = libevdev_get_abs_maximum(_dev, ABS_MT_POSITION_X);
  _y_min = libevdev_get_abs_minimum(_dev, ABS_MT_POSITION_Y);
  _y_max = libevdev_get_abs_maximum(_dev, ABS_MT_POSITION_Y);

  std::cout << "Opened device \"" << _name << "\" at \"" << _path << "\""
            << std::endl;
}

MultitouchDevice::~MultitouchDevice() {
  libevdev_free(_dev);
  std::cout << "Closed device \"" << _name << "\"" << std::endl;
}

const std::string &MultitouchDevice::name() const { return _name; }

int MultitouchDevice::num_slots() const { return _num_slots; }

std::set<std::shared_ptr<TouchPoint>> MultitouchDevice::touch_points() const {
  std::set<std::shared_ptr<TouchPoint>> touch_points;
  for (auto &touch_point : _touch_points) {
    if (nullptr != touch_point) {
      touch_points.insert(touch_point);
    }
  }
  return touch_points;
}

bool MultitouchDevice::update() {
  if (!libevdev_has_event_pending(_dev)) {
    return false;
  }

  int rc = 0;
  while (rc >= 0) {
    struct input_event ev;
    rc = libevdev_next_event(_dev, LIBEVDEV_READ_FLAG_NORMAL, &ev);

    if (rc == LIBEVDEV_READ_STATUS_SUCCESS) {
      if (ev.type == EV_ABS) {
        int slot = libevdev_get_current_slot(_dev);

        if (ev.code == ABS_MT_TRACKING_ID) {
          int id = ev.value;

          if (id >= 0) {
            // new touch
            _touch_points[slot] = std::make_shared<TouchPoint>(id);
          } else {
            // touch was lifted
            _touch_points[slot]->finish();
          }
        } else if (ev.code == ABS_MT_POSITION_X) {
          float x = (ev.value - _x_min) / float(_x_max - _x_min);
          _touch_points[slot]->set_x(x);
        } else if (ev.code == ABS_MT_POSITION_Y) {
          float y = (ev.value - _y_min) / float(_y_max - _y_min);
          _touch_points[slot]->set_y(y);
        }
      } else if (ev.type == EV_SYN && ev.code == SYN_REPORT) {
        // sync event
        for (auto &touch_point : _touch_points) {
          if (nullptr != touch_point) {
            touch_point->update();
          }
        }
      }
    }
  }

  return true;
}
