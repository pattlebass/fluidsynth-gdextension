#pragma once

#include <fluidsynth.h>
#include <godot_cpp/variant/string.hpp>

namespace fluidsynthgd {
bool install_memory_soundfont_loader(fluid_settings_t *settings, fluid_synth_t *synth);

int load_soundfont_from_file(fluid_synth_t *synth, const godot::String &path);
} //namespace fluidsynthgd
