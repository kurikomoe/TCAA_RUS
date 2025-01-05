-- add_requires("microsoft-proxy 2.4.0")
add_rules("mode.debug", "mode.release")
add_rules("plugin.compile_commands.autoupdate")

set_languages("cxx20", "c++20")

-- includes("third/ms-proxy.lua")
-- add_requires("ms-proxy 3.0.0")

target("version")
    set_kind("shared")
    set_arch("x86")
    set_toolchains("msvc")
    add_files("src/*.cpp", "src/*.ixx")
    add_shflags("/DEF:src/version.def", {force = true})
    add_links("User32.lib")
