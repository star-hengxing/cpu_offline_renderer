set_project("cpu_offline_renderer")

add_rules("mode.debug", "mode.release")
-- third lib
add_requires("stb")

add_includedirs("../Hinae/include")
add_includedirs("include")

if is_os("linux") then
    add_ldflags("-B/usr/lib/mold")
    add_cxxflags("-fno-math-errno")
elseif is_os("windows") then
    add_defines("_CRT_SECURE_NO_WARNINGS")
    add_cxflags("/utf-8")
end

set_warnings("all")
set_languages("cxxlatest")

target("raytracing")
    set_kind("static")
    add_files("src/**.cpp")
    add_packages("stb")

target("main")
    set_kind("binary")
    add_files("examples/main.cpp")
    add_deps("raytracing")
    set_rundir("$(projectdir)")

target("test")
    set_kind("binary")
    add_files("examples/test.cpp")
    add_deps("raytracing")
    set_rundir("$(projectdir)")