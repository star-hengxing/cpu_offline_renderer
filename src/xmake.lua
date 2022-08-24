includes("util")
includes("raytracing")

target("main")
    set_default(true)
    set_kind("binary")
    add_files("main.cpp")
    add_deps("raytracing")
    set_rundir("$(projectdir)")