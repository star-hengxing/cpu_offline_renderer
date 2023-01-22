add_requires("stb")

includes("module.lua")

add_includedirs(".", {public = true})

target("renderer")
    set_default(true)
    set_kind("binary")
    add_files("main.cpp")
    add_deps("raytracing")
    set_rundir("$(projectdir)")
target_end()

target("raytracing")
    set_kind("$(kind)")
    add_rules("module")
    add_files("raytracing/**.cpp")
    add_deps("util")
target_end()

target("util")
    set_kind("$(kind)")
    add_rules("module")
    add_files("util/**.cpp")
    add_packages("stb")

    on_load(function (target)
        if is_host("windows") then
            target:add("defines", "_CRT_SECURE_NO_WARNINGS")
            -- msvc stl source_location cannot work with clang
            target:add("defines", "__cpp_consteval", {public = true})
        end
    end)
target_end()
