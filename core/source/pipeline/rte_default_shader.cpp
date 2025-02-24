//
// Copyright (c) 2025 Liam Reese
//

#include "rte_default_shader.hpp"

void rteDefaultShader::ShadeFragment(const rteScene* scene, const rteRay &ray, rteFragment &fragment) const {
    fragment.shaded = glm::vec3(1, 0, 0);
}