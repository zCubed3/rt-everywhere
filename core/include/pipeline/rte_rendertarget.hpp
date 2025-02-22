//
// Copyright (c) 2025 Liam Reese
//

#ifndef RTEVERYWHERE_RTE_RENDERTARGET_HPP
#define RTEVERYWHERE_RTE_RENDERTARGET_HPP

#include <scene/rte_camera.hpp>

class rteFramebuffer;

/// @brief Defines the output frame buffer and viewpoint used in rendering, this is to allow parallelization
struct rteRenderTarget {

    rteFramebuffer* pFramebuffer;
    rteCamera camera;

};

#endif //RTEVERYWHERE_RTE_RENDERTARGET_HPP
