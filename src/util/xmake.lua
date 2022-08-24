add_requires("stb")

if is_os("windows") then
    add_defines("_CRT_SECURE_NO_WARNINGS")
end

target("util")
    set_kind("static")
    add_files("**.cpp")
    add_packages("stb")