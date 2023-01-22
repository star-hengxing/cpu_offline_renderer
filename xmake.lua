set_project("cpu_offline_renderer")
set_version("0.0.1")

set_xmakever("2.7.5")

set_warnings("all")
set_languages("c++20")

add_rules("mode.debug", "mode.releasedbg", "mode.release")

if is_mode("debug") then
    set_optimize("none")
    set_runtimes("MDd")
elseif is_mode("releasedbg") then
    set_optimize("fastest")
    set_runtimes("MD")
    set_policy("build.optimization.lto", true)
elseif is_mode("release") then
    set_strip("all")
    set_symbols("hidden")
    set_optimize("fastest")
    set_runtimes("MT")
    add_defines("NDEBUG")
    set_policy("build.optimization.lto", true)
end

if is_mode("debug", "releasedbg") then
    set_symbols("debug")
    set_policy("build.warning", true)
    -- use dynamic libraries acceleration on linking
    add_requireconfs("*", {configs = {shared = true}})
end
-- support utf-8 on msvc
if is_host("windows") then
    add_defines("UNICODE", "_UNICODE")
    add_cxflags("/execution-charset:utf-8", "/source-charset:utf-8", {tools = "cl"})
end
-- disable sqrt function negative check
add_cxxflags("-fno-math-errno", {tools = {"gcc", "clang"}})

includes("src")
includes("test")
