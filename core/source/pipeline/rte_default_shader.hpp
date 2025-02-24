//
// Copyright (c) 2025 Liam Reese
//

#ifndef RTEVERYWHERE_RTE_DEFAULT_SHADER_HPP
#define RTEVERYWHERE_RTE_DEFAULT_SHADER_HPP

#include <pipeline/rte_shader.hpp>

class rteDefaultShader : public rteShader {

public:
    std::string GetShaderID() const override {
        return "default";
    }

    void ShadeFragment(const rteScene* scene, const rteRay &ray, rteFragment &fragment) const override;

};


#endif //RTEVERYWHERE_RTE_DEFAULT_SHADER_HPP
