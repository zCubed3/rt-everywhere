//
// Copyright (c) 2025 Liam Reese
//

#ifndef RTEVERYWHERE_RTE_STATE_HPP
#define RTEVERYWHERE_RTE_STATE_HPP

#include <scene/rte_scene.hpp>
#include <pipeline/rte_rendertarget.hpp>

/// @brief All of the data required for RTE to function
class rteState {

    //
    // Public Members
    //
public:
    rteScene scene;

    //
    // Rendering quality
    //
    int maxNumMirrorBounces = 3;

    //
    // Functions
    //
public:

    /// @brief Immediately renders the active scene to the render target
    void Render(rteRenderTarget rt);

    // TODO: RenderAsync

};

#endif //RTEVERYWHERE_RTE_STATE_HPP
