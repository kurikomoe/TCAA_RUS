-- add_requires("microsoft-proxy 2.4.0")
add_rules("mode.debug", "mode.release")
add_rules("plugin.compile_commands.autoupdate")

set_languages("cxx20", "c++20")

includes("third/ms-proxy.lua")

add_requires("ms-proxy 3.0.0")

target("proxy")
    set_kind("binary")
    add_files("src/*.cpp")
    add_packages("ms-proxy")
