#!/usr/bin/env python
import os
import sys

env = SConscript("godot-cpp/SConstruct")
    
# For the reference:
# - CCFLAGS are compilation flags shared between C and C++
# - CFLAGS are for C-specific compilation flags
# - CXXFLAGS are for C++-specific compilation flags
# - CPPFLAGS are for pre-processor flags
# - CPPDEFINES are for pre-processor defines
# - LINKFLAGS are for linking flags

# tweak this if you want to use different folders, or more folders, to store your source code in.
env.Append(CPPPATH=["src/"])
sources = Glob("src/*.cpp")

if env["target"] in ["editor", "template_debug"]:
    try:
        doc_data = env.GodotCPPDocData("src/gen/doc_data.gen.cpp", source=Glob("doc_classes/*.xml"))
        sources.append(doc_data)
        print("Class reference added.")
    except AttributeError:
        print("Not including class reference as we're targeting a pre-4.3 baseline.")

if env["platform"] == "macos":
    library = env.SharedLibrary(
        "game/bin/libgdretro.{}.{}.framework/libgdretro.{}.{}".format(
            env["platform"], env["target"], env["platform"], env["target"]
        ),
        source=sources,
    )
elif env["platform"] == "web":

    # Set specific flags for WebAssembly compilation
    env.Append(CPPFLAGS=["-DWEBASSEMBLY", "-std=c++17"])
    env.Append(LINKFLAGS=["--no-entry", "-s WASM=1", "-s ALLOW_MEMORY_GROWTH=1", "-s EXPORT_ALL=1", "-s EXPORTED_FUNCTIONS=['_main', '_gdretro_library_init']"])

    # Use Emscripten's emcc and em++ for WebAssembly compilation
    env["CC"] = "emcc"
    env["CXX"] = "em++"
    env["LINK"] = "emcc"

    # For WebAssembly, the output is typically a .wasm file with Emscripten
    library = env.Program(
        "game/bin/libgdretro.{}.{}.wasm".format(env["platform"], env["target"]),
        source=Glob("src/*.cpp"),
    )

else:
    library = env.SharedLibrary(
        "game/bin/libgdretro{}{}".format(env["suffix"], env["SHLIBSUFFIX"]),
        source=sources,
    )

Default(library)
