#include <iostream>
#include <string>

// has to be included before glfw3
#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include <argh.h>
#include <spdlog/spdlog.h>

#include <multitouch_device.hpp>
#include <program.hpp>
#include <shader.hpp>

struct Vertex {
  glm::vec2 pos;
  glm::vec3 color;
};

int main(int argc, const char* argv[]) {
#ifdef NDEBUG
  spdlog::set_level(spdlog::level::info);
#else
  spdlog::set_level(spdlog::level::debug);
#endif

  auto cmdl = argh::parser(argc, argv);

  if (cmdl[1].empty()) {
    spdlog::error("Please pass a device path as positional parameter!");
    return EXIT_FAILURE;
  }
  std::string device_path = cmdl[1];
  MultitouchDevice mt_device{device_path};

  glfwSetErrorCallback([](int code, const char* desc) {
    spdlog::error("GLFW Error {}: \"{}\"", code, desc);
  });

  if (!glfwInit()) {
    return EXIT_FAILURE;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow* window;
  auto monitor = glfwGetPrimaryMonitor();
  spdlog::info("Using monitor \"{}\"", glfwGetMonitorName(monitor));
  auto mode = glfwGetVideoMode(monitor);
  glm::uvec2 resolution{mode->width, mode->height};
  window = glfwCreateWindow(resolution.x, resolution.y, "touchviz", monitor,
                            nullptr);
  if (!window) {
    glfwTerminate();
    return EXIT_FAILURE;
  }

  glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode,
                                int action, int mods) {
    if (action == GLFW_PRESS) {
      if (key == GLFW_KEY_Q || key == GLFW_KEY_ESCAPE) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
      }
    }
  });

  // init OpenGL
  glfwMakeContextCurrent(window);
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    spdlog::error("Failed to load OpenGL");
    return EXIT_FAILURE;
  }
  glViewport(0, 0, resolution.x, resolution.y);

  auto vert_shader = gl::load_shader_from_file("shaders/simple.vert");
  auto frag_shader = gl::load_shader_from_file("shaders/circle.frag");
  auto program = gl::create_program({vert_shader, frag_shader});

  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  GLuint vbo;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * mt_device.num_slots(), nullptr,
               GL_DYNAMIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (GLvoid*)offsetof(Vertex, pos));
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (GLvoid*)offsetof(Vertex, color));
  glEnableVertexAttribArray(1);

  glUseProgram(program);
  glPointSize(100.0f);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
    mt_device.update();
    auto touch_points = mt_device.touch_points();

    glClear(GL_COLOR_BUFFER_BIT);

    if (!touch_points.empty()) {
      std::vector<Vertex> vertices;
      for (const auto& touch_point : touch_points) {
        if (!touch_point->active()) {
          continue;
        }
        auto pos = touch_point->pos();
        vertices.emplace_back(Vertex{pos, glm::vec3{1.0f}});
      }
      glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * vertices.size(),
                      vertices.data());

      glBindVertexArray(vao);
      glDrawArrays(GL_POINTS, 0, vertices.size());
    }
    glfwSwapBuffers(window);
  }

  // cleanup
  glDeleteVertexArrays(1, &vao);
  glDeleteBuffers(1, &vbo);
  glDeleteProgram(program);

  glfwTerminate();
  return EXIT_SUCCESS;
}
