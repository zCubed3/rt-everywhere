//
// Copyright (c) 2025 Liam Reese
//

#ifndef RTEVERYWHERE_RTE_SHADER_HPP
#define RTEVERYWHERE_RTE_SHADER_HPP

#include <data/rte_ray.hpp>
#include <data/rte_fragment.hpp>

#include <scene/rte_scene.hpp>

#include <string>

struct rteShaderInput {

    const rteState* curState;
    const rteScene* curScene;
    rteRay ray;
    rteFragment fragment;

};

class rteShader {

public:
    virtual ~rteShader() {

    }

    virtual std::string GetShaderID() const = 0;

    virtual void ShadeFragment(const rteShaderInput& input, rteFragment& fragment) const = 0;

};

#endif //RTEVERYWHERE_RTE_SHADER_HPP
