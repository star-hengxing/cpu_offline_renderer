add_requires("nlohmann_json")

target("raytracing")
    set_kind("static")
    add_files("**.cpp")
    add_deps("util")
    add_packages("nlohmann_json")