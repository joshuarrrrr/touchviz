#pragma once

#include <set>
#include <vector>

#include <glad/glad.h>
#include <spdlog/spdlog.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace gl {

GLuint create_program(const std::set<GLuint>& shaders);

}  // namespace gl