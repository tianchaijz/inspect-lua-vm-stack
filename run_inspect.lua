local linspect = require "linspect"
local socket = require "socket"


local function inspect(kind, ...)
    local banner = string.rep("=", 20) .. " " .. kind
    return linspect["inspect_" .. kind](...)
end


inspect("package")

inspect("loadfile", "scripts/global.lua")
inspect("loadfile", "scripts/local.lua")
inspect("loadfile", "scripts/module.lua")
