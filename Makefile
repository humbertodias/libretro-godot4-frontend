ARCH := $(shell uname -m)
OS := $(shell uname -s)
CORE_NAME := snes9x
CORE_URL := https://buildbot.libretro.com/nightly/$(OS)/$(ARCH)/latest/$(CORE_NAME)_libretro.so.zip
CORE_URL_LOWER := $(shell echo $(CORE_URL) | tr '[:upper:]' '[:lower:]')

GODOT_BINARY := /opt/Godot_v4.4-stable_linux.x86_64
CC := ccache gcc
CXX := ccache g++

BUILD_DIR := builds/Linux
GODOT_EXPORT_TEMPLATES := ~/.local/share/godot/export_templates/4.4.stable


build-gdretro: build-godot-cpp
	scons

build-godot-cpp:
	cd godot-cpp && scons

clean-godot-cpp:
	cd godot-cpp && scons --clean

clean: clean-godot-cpp
	scons --clean

godot-export: build-gdretro
	mkdir -p $(BUILD_DIR)
	$(GODOT_BINARY) --log-file log.txt --path ./game --headless --export-debug "$(OS)" "$(BUILD_DIR)/libretro-godot4-frontend.x86_64" --export-templates-dir $(GODOT_EXPORT_TEMPLATES)

core-get:
	wget -q -O tmp.zip $(CORE_URL_LOWER)
	unzip -jo tmp.zip -d game/bin
	rm tmp.zip

loader-bin:
	$(CXX) -o loader src/loader.cpp -Isrc `pkg-config --libs --cflags libavformat libavcodec libavutil libswscale libswresample`
