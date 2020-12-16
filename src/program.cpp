#include <program.hpp>

namespace gl {

GLuint create_program(const std::set<GLuint>& shaders) {
  auto program = glCreateProgram();

  // attach shaders
  for (auto shader : shaders) {
    glAttachShader(program, shader);
  }

  // link program
  glLinkProgram(program);
  GLint is_linked;
  glGetProgramiv(program, GL_LINK_STATUS, &is_linked);
  if (is_linked != GL_TRUE) {
    GLint max_length;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &max_length);
    std::vector<GLchar> info_log_vec(max_length);
    glGetProgramInfoLog(program, max_length, &max_length, &info_log_vec[0]);

    // log the linking error messages
    std::string info_log(std::begin(info_log_vec), std::end(info_log_vec));
    spdlog::error("Linking program {} failed:\n{}", program, info_log);
    throw std::runtime_error("Linking shader program failed!");
  }

  // detach shaders
  for (auto shader : shaders) {
    glDetachShader(program, shader);
  }

  return program;
}

}  // namespace gl