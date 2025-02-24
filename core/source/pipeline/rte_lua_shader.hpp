//
// Copyright (c) 2025 Liam Reese
//

#ifndef RTEVERYWHERE_RTE_LUA_SHADER_HPP
#define RTEVERYWHERE_RTE_LUA_SHADER_HPP

#include <pipeline/rte_shader.hpp>

class rteLuaShader : public rteShader {

public:
    using LuaShaderFunc = std::function<void(const rteScene*, const rteRay&, rteFragment&)>;

protected:
    std::string shaderID; // Sent from Lua, captured here to prevent boundary crossing

public:
    std::string GetShaderID() const override {
        return shaderID;
    }

    void ShadeFragment(const rteShaderInput& input, rteFragment& fragment) const override {
        // TODO: This is really broken!
    }

};


#endif //RTEVERYWHERE_RTE_LUA_SHADER_HPP
