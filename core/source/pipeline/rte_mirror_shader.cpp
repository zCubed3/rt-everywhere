//
// Copyright (c) 2025 Liam Reese
//

#include "rte_mirror_shader.hpp"

void rteMirrorShader::ShadeFragment(const rteShaderInput& input, rteFragment& fragment) const {

    rteFragment mirrorFrag = fragment;
    mirrorFrag.shaded = glm::vec3(0.9F);

    input.curScene->BounceMirror(input.curState, input.ray, mirrorFrag);
    fragment.shaded = mirrorFrag.shaded;
    fragment.hitRay = mirrorFrag.hitRay;

}