package.cpath = package.cpath .. ";lib/?.so;lib/?.dylib;;"

local linspect = require "linspect"
local trim_required = require "trim"


local function inspect(kind, ...)
    local banner = string.rep("=", 20) .. " " .. kind
    print(banner)

    return linspect["inspect_" .. kind](...)
end


inspect("package")

inspect("loadfile", "scripts/global.lua")
inspect("loadfile", "scripts/local.lua")
inspect("loadfile", "scripts/module.lua")
