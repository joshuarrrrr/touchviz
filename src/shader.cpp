#include <shader.hpp>

namespace gl {

GLenum detect_shader_type_from_filename(const std::string &filename) {
  auto file_ending_2 = filename.substr(filename.length() - 2, 2);
  auto file_ending_4 = filename.substr(filename.length() - 4, 4);
  if (file_ending_4 == "vert" || file_ending_2 == "vs") {
    return GL_VERTEX_SHADER;
    //   } else if (file_ending_4 == "tesc") {
    //     return GL_TESS_CONTROL_SHADER;
    //   } else if (file_ending_4 == "tese") {
    //     return GL_TESS_EVALUATION_SHADER;
  } else if (file_ending_4 == "geom") {
    return GL_GEOMETRY_SHADER;
  } else if (file_ending_4 == "frag" || file_ending_2 == "fs") {
    return GL_FRAGMENT_SHADER;
  }

  std::cout << "Failed to detect shader type from filename \"" << filename
            << "\"" << std::endl;
  return GL_NONE;
}

std::string load_shader_source_from_file(const std::string &filename) {
  std::ifstream filestream(filename);
  if (!filestream.is_open()) {
    throw std::runtime_error("Failed to load shader from file \"" + filename +
                             "\"");
  }

  return std::string(std::istreambuf_iterator<char>(filestream),
                     std::istreambuf_iterator<char>());
}

bool compile_shader(GLuint shader) {
  glCompileShader(shader);

  GLint compiled = 0;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
  if (compiled == GL_FALSE) {
    GLint max_length = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &max_length);

    std::vector<GLchar> error_log_vec(max_length);
    glGetShaderInfoLog(shader, max_length, &max_length, &error_log_vec[0]);
    std::string error_log(std::begin(error_log_vec), std::end(error_log_vec));
    std::cout << "Compiling shader " << shader << " failed:" << std::endl
              << error_log << std::endl;

    glDeleteShader(shader);
    return false;
  }

  return true;
}

GLuint create_shader(const std::string &source, GLenum type) {
  GLuint shader = glCreateShader(type);
  auto shader_cstring = source.c_str();
  glShaderSource(shader, 1, &shader_cstring, nullptr);
  if (!compile_shader(shader)) {
    throw std::runtime_error("Failed to create shader!");
  }

  return shader;
}

GLuint load_shader_from_file(const std::string &filename, GLenum type) {
  // auto-detect shader type if none was provided
  if (type == GL_NONE) {
    type = detect_shader_type_from_filename(filename);
  }

  auto source = load_shader_source_from_file(filename);
  return create_shader(source, type);
}

} // namespace gl
