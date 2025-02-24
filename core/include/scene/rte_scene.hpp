//
// Copyright (c) 2025 Liam Reese
//

#ifndef RTEVERYWHERE_RTE_SCENE_HPP
#define RTEVERYWHERE_RTE_SCENE_HPP

#include <data/rte_ray.hpp>
#include <data/rte_fragment.hpp>

class rteState;

class rteScene {

protected:
    float farClip = 1000.0F;

public:
    bool hasGroundPlane = true;

    glm::vec3 skyColor = glm::vec3(51 / 255.0F, 0, 255 / 255.0F);

    float groundCheckerSize = 10.0F;
    glm::vec3 groundColor1 = glm::vec3(255 / 255.0F, 0, 137.0F / 255.0F);
    glm::vec3 groundColor2 = glm::vec3(5 / 255.0F, 5 / 255.0F, 5 / 255.0F);
    float groundSpecular1 = 0.5F;
    float groundSpecular2 = 0.0F;

    int numMirrorBounces = 4;

    rteFragment TraceGround(const rteRay& ray);
    void TraceSky(const rteRay& ray, rteFragment& fragment);

    void BounceMirror(const rteState* state, const rteRay& ray, rteFragment& fragment);
    void ShadeFrag(const rteState* state, const rteRay& ray, rteFragment& fragment);

    bool TraceScene(const rteState* state, const rteRay &ray, rteFragment& fragment);
};

#endif //RTEVERYWHERE_RTE_SCENE_HPP
