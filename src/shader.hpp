#pragma once

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <streambuf>
#include <string>
#include <vector>

#include <glad/glad.h>

namespace gl {

GLenum detect_shader_type_from_filename(const std::string &filename);
std::string load_shader_source_from_file(const std::string &filename);
bool compile_shader(GLuint shader);
GLuint create_shader(const std::string &source, GLenum type);
GLuint load_shader_from_file(const std::string &filename,
                             GLenum type = GL_NONE);

} // namespace gl
