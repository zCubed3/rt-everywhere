//
// Copyright (c) 2025 Liam Reese
//

#ifndef RTEVERYWHERE_RTE_SHADER_HPP
#define RTEVERYWHERE_RTE_SHADER_HPP

#include <data/rte_ray.hpp>
#include <data/rte_fragment.hpp>

#include <scene/rte_scene.hpp>

#include <string>

class rteShader {

public:
    virtual ~rteShader() {

    }

    virtual std::string GetShaderID() const = 0;

    virtual void ShadeFragment(const rteScene* scene, const rteRay &ray, rteFragment &fragment) const = 0;

};

#endif //RTEVERYWHERE_RTE_SHADER_HPP
