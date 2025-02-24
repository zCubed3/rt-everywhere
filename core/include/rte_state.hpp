//
// Copyright (c) 2025 Liam Reese
//

#ifndef RTEVERYWHERE_RTE_STATE_HPP
#define RTEVERYWHERE_RTE_STATE_HPP

#include <scene/rte_scene.hpp>
#include <pipeline/rte_rendertarget.hpp>
#include <pipeline/rte_shader.hpp>

#include <sol/sol.hpp>

#include <string>
#include <unordered_map>
#include <memory>

typedef struct lua_State lua_State;

/// @brief All of the data required for RTE to function
class rteState {

protected:
    std::unordered_map<std::string, rteShader*> loadedShaders = {};

    void LoadLuaModules();

    //
    // Public Members
    //
public:
    rteScene scene;

    // TODO: Store this data somewhere else!
    uint64_t highestShadeTime;

    //
    // Rendering quality
    //
    bool visualizeBounceHeat = false;
    bool visualizeShadeTime = false;

    sol::state luaState;

    //
    // Functions
    //
public:

    // TODO: Load more than 1 lua state to allow concurrency

    /// @brief Initializes the state
    void Setup();

    /// @brief Immediately renders the active scene to the render target
    void Render(rteRenderTarget rt);

    /// @brief Registers an instance of the provided shader, lifetime is transferred
    void RegisterShader(rteShader* shader);

    const rteShader* GetShader(const std::string& id) const {

        if (loadedShaders.find(id) == loadedShaders.end())
            abort(); // OOB

        return loadedShaders.at(id);

    }

    // TODO: RenderAsync
};

#endif //RTEVERYWHERE_RTE_STATE_HPP
