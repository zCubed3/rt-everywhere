#ifndef RTEVERYWHERE_RTE_LUA_SHADER_HPP
#define RTEVERYWHERE_RTE_LUA_SHADER_HPP

#include <pipeline/rte_shader.hpp>

#include <sol/sol.hpp>

class rteLuaShader : public rteShader {

public:
    using LuaShaderFunc = std::function<void(const rteScene*, const rteRay&, rteFragment&)>;

protected:
    std::string shaderID; // Sent from Lua, captured here to prevent boundary crossing
    sol::protected_function shaderFunc; // Lua shader delegate;

public:
    rteLuaShader(const std::string shaderID, sol::protected_function func) : shaderID(shaderID), shaderFunc(func) {}

    std::string GetShaderID() const override {
        return shaderID;
    }

    void ShadeFragment(const rteScene *scene, const rteRay &ray, rteFragment &fragment) const override {
        fragment.shaded = shaderFunc();
    }

};


#endif //RTEVERYWHERE_RTE_LUA_SHADER_HPP
