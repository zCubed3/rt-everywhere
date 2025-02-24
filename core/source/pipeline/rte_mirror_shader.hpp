//
// Copyright (c) 2025 Liam Reese
//

#ifndef RTEVERYWHERE_RTE_MIRROR_SHADER_HPP
#define RTEVERYWHERE_RTE_MIRROR_SHADER_HPP

#include <pipeline/rte_shader.hpp>

class rteMirrorShader : public rteShader {

public:
    std::string GetShaderID() const override {
        return "mirror";
    }

    void ShadeFragment(const rteShaderInput& input, rteFragment& fragment) const override;

};


#endif //RTEVERYWHERE_RTE_MIRROR_SHADER_HPP
