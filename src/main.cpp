#include <iostream>
#include <string>

// has to be included before glfw3
#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include <argh.h>
#include <spdlog/spdlog.h>

#include <libevdev/libevdev.h>

int main(int argc, const char* argv[]) {
#ifdef NDEBUG
  spdlog::set_level(spdlog::level::info);
#else
  spdlog::set_level(spdlog::level::debug);
#endif

  auto cmdl = argh::parser(argc, argv);

  std::string dev_path = "/dev/input/event0";
  if (cmdl.pos_args().size() >= 2) {
    dev_path = cmdl.pos_args().at(1);
    spdlog::info("Using device '{}'", dev_path);
  }

  struct libevdev* dev = NULL;
  int fd;
  int rc = 1;

  fd = open(cmdl.pos_args().at(1).c_str(), O_RDONLY | O_NONBLOCK);
  rc = libevdev_new_from_fd(fd, &dev);
  if (rc < 0) {
    spdlog::error("Failed to init libevdev: {}", strerror(-rc));
    return EXIT_FAILURE;
  }
  printf("Input device name: \"%s\"\n", libevdev_get_name(dev));
  printf("Input device ID: bus %#x vendor %#x product %#x\n",
         libevdev_get_id_bustype(dev), libevdev_get_id_vendor(dev),
         libevdev_get_id_product(dev));
  if (!libevdev_has_event_type(dev, EV_REL) ||
      !libevdev_has_event_code(dev, EV_KEY, BTN_LEFT)) {
    printf("This device does not look like a mouse\n");
    return EXIT_FAILURE;
  }

  do {
    struct input_event ev;
    rc = libevdev_next_event(dev, LIBEVDEV_READ_FLAG_NORMAL, &ev);
    if (rc == 0)
      printf("Event: %s %s %d\n", libevdev_event_type_get_name(ev.type),
             libevdev_event_code_get_name(ev.type, ev.code), ev.value);
  } while (rc == 1 || rc == 0 || rc == -EAGAIN);

  return EXIT_SUCCESS;
}
