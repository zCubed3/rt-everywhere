//
// Copyright (c) 2025 Liam Reese
//

#ifndef RTEVERYWHERE_RTE_GROUND_SHADER_HPP
#define RTEVERYWHERE_RTE_GROUND_SHADER_HPP

#include <pipeline/rte_shader.hpp>

class rteGroundShader : public rteShader {

public:
    std::string GetShaderID() const override {
        return "ground_shader";
    }

    void ShadeFragment(const rteShaderInput& input, rteFragment& fragment) const override;

};


#endif //RTEVERYWHERE_RTE_GROUND_SHADER_HPP
