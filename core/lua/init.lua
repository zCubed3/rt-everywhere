-- init.lua, this initializes all shader modules

require "shaders.checker_ground"

print("[RTE] JIT = " .. tostring(jit.status()))

print("[RTE]: Lua initialized!")