add_requires("doctest")

target("scene_file_test")
    set_kind("binary")
    add_files("scene_file_test.cpp")
    add_deps("raytracing")
    add_packages("doctest", "nlohmann_json")
    set_rundir("$(projectdir)")

target("test")
    set_kind("binary")
    add_files("test.cpp")
    add_deps("raytracing")
    set_rundir("$(projectdir)")