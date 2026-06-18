// Include your classes, that you want to expose to Godot
#include "audio_stream_midi_file.hpp"
#include "audio_stream_midi_sequencer.hpp"
#include "audio_stream_playback_midi.hpp"
#include <gdextension_interface.h>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

#ifdef __ANDROID__
#include <dlfcn.h>
#endif

using namespace godot;

void initialize_gdextension_types(ModuleInitializationLevel p_level) {
#ifdef __ANDROID__
	dlopen("libfluidsynth.so", RTLD_GLOBAL | RTLD_NOW);
#endif
	UtilityFunctions::print("init called, level: ", (int)p_level);
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
	UtilityFunctions::print("registering classes...");
	// Register your classes here, so they are available in the Godot editor and engine
	// GDREGISTER_CLASS(ItemData)
	GDREGISTER_CLASS(AudioStreamPlaybackMidi)
	GDREGISTER_CLASS(AudioStreamMidiSequencer)
	GDREGISTER_CLASS(AudioStreamMidiFile)
	UtilityFunctions::print("classes registered");
}

void uninitialize_gdextension_types(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
}

extern "C" {
// Initialization
GDExtensionBool GDE_EXPORT fluidsynthgd_init(GDExtensionInterfaceGetProcAddress p_get_proc_address, GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization) {
	GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);
	init_obj.register_initializer(initialize_gdextension_types);
	init_obj.register_terminator(uninitialize_gdextension_types);
	init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

	return init_obj.init();
}
}
