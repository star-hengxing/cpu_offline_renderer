set_xmakever("2.6.9")
set_version("0.1.0")

set_project("cpu offline renderer")

add_rules("mode.debug", "mode.release")

if is_os("linux") then
    add_ldflags("-fuse-ld=mold")
    add_cxxflags("-fno-math-errno")
end

set_warnings("all")
set_languages("cxx20")

add_includedirs("src")

includes("src")
includes("test")
