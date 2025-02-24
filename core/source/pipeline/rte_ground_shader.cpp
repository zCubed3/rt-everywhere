//
// Copyright (c) 2025 Liam Reese
//

#include "rte_ground_shader.hpp"

void rteGroundShader::ShadeFragment(const rteShaderInput &input, rteFragment &fragment) const {

    glm::vec3 checker = glm::floor(fragment.position * input.curScene->groundCheckerSize);

    float mod = glm::mod(checker.x + glm::mod(checker.z, 2.0F), 2.0F);
    fragment.shaded = mod ? input.curScene->groundColor1 : input.curScene->groundColor2;

    float specular = mod ? input.curScene->groundSpecular1 : input.curScene->groundSpecular2;

    rteFragment metalFrag = fragment;
    metalFrag.shaded = glm::vec3(1, 1, 1);

    input.curScene->BounceMirror(input.curState, input.ray, metalFrag);
    fragment.shaded = glm::mix(fragment.shaded, fragment.shaded * metalFrag.shaded, specular);
    fragment.hitRay = metalFrag.hitRay;

}