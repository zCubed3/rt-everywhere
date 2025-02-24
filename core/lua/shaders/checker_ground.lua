-- checker_ground.lua - Re-implementation of the RTE ground shader in Lua

ground_shader = Shader:new("ground_shader", function()
    return vec3.new(0, 1, 0)
end)

--RTEState:register_shader(ground_shader)