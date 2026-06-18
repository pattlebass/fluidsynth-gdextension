#include "memory_soundfont_loader.hpp"

#include <cinttypes>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <fluidsynth.h>

using namespace godot;

namespace fluidsynthgd {
struct MemorySoundFont {
	PackedByteArray data;
	fluid_long_long_t position = 0;
};

static constexpr const char *MEMORY_SF_PREFIX = "memsf:";

static void *memorySfOpen(const char *filename) {
	if (filename == nullptr) {
		return nullptr;
	}

	const size_t prefixLength = std::strlen(MEMORY_SF_PREFIX);

	if (std::strncmp(filename, MEMORY_SF_PREFIX, prefixLength) != 0) {
		return nullptr;
	}

	const char *addressText = filename + prefixLength;
	uintptr_t address = static_cast<uintptr_t>(
			std::strtoull(addressText, nullptr, 16));

	MemorySoundFont *soundFont = reinterpret_cast<MemorySoundFont *>(address);

	if (soundFont == nullptr) {
		return nullptr;
	}

	soundFont->position = 0;
	return soundFont;
}

static int memorySfRead(void *buffer, fluid_long_long_t count, void *handle) {
	MemorySoundFont *soundFont = static_cast<MemorySoundFont *>(handle);

	if (soundFont == nullptr || buffer == nullptr || count < 0) {
		return FLUID_FAILED;
	}

	const fluid_long_long_t size = static_cast<fluid_long_long_t>(
			soundFont->data.size());

	if (soundFont->position < 0 || soundFont->position > size) {
		return FLUID_FAILED;
	}

	const fluid_long_long_t remaining = size - soundFont->position;

	if (count > remaining) {
		return FLUID_FAILED;
	}

	const uint8_t *source = soundFont->data.ptr();
	std::memcpy(
			buffer,
			source + soundFont->position,
			static_cast<size_t>(count));

	soundFont->position += count;

	return FLUID_OK;
}

static int memorySfSeek(void *handle, fluid_long_long_t offset, int origin) {
	MemorySoundFont *soundFont = static_cast<MemorySoundFont *>(handle);

	if (soundFont == nullptr) {
		return FLUID_FAILED;
	}

	const fluid_long_long_t size = static_cast<fluid_long_long_t>(
			soundFont->data.size());

	fluid_long_long_t base = 0;

	if (origin == SEEK_SET) {
		base = 0;
	} else if (origin == SEEK_CUR) {
		base = soundFont->position;
	} else if (origin == SEEK_END) {
		base = size;
	} else {
		return FLUID_FAILED;
	}

	const fluid_long_long_t newPosition = base + offset;

	if (newPosition < 0 || newPosition > size) {
		return FLUID_FAILED;
	}

	soundFont->position = newPosition;

	return FLUID_OK;
}

static fluid_long_long_t memorySfTell(void *handle) {
	MemorySoundFont *soundFont = static_cast<MemorySoundFont *>(handle);

	if (soundFont == nullptr) {
		return FLUID_FAILED;
	}

	return soundFont->position;
}

static int memorySfClose(void *handle) {
	return FLUID_OK;
}

bool install_memory_soundfont_loader(fluid_settings_t *settings, fluid_synth_t *synth) {
	if (settings == nullptr || synth == nullptr) {
		UtilityFunctions::printerr("Cannot install memory SoundFont loader: null FluidSynth objects.");
		return false;
	}

	fluid_sfloader_t *loader = new_fluid_defsfloader(settings);

	if (loader == nullptr) {
		UtilityFunctions::printerr("Failed to create FluidSynth SoundFont loader.");
		return false;
	}

	const int callbackResult = fluid_sfloader_set_callbacks(
			loader,
			memorySfOpen,
			memorySfRead,
			memorySfSeek,
			memorySfTell,
			memorySfClose);

	if (callbackResult == FLUID_FAILED) {
		delete_fluid_sfloader(loader);
		UtilityFunctions::printerr("Failed to set FluidSynth SoundFont loader callbacks.");
		return false;
	}

	fluid_synth_add_sfloader(synth, loader);

	return true;
}

int load_soundfont_from_file(fluid_synth_t *synth, const String &path) {
	if (synth == nullptr) {
		UtilityFunctions::printerr("Cannot load SoundFont: synth is null.");
		return FLUID_FAILED;
	}

	Ref<FileAccess> file = FileAccess::open(path, FileAccess::READ);

	if (file.is_null()) {
		UtilityFunctions::printerr(
				"Could not open SoundFont: ",
				path,
				" error: ",
				String::num_int64(FileAccess::get_open_error()));

		return FLUID_FAILED;
	}

	MemorySoundFont memorySoundFont;
	memorySoundFont.data = file->get_buffer(file->get_length());
	memorySoundFont.position = 0;

	char fakeFilename[128];

	std::snprintf(
			fakeFilename,
			sizeof(fakeFilename),
			"%s%" PRIxPTR,
			MEMORY_SF_PREFIX,
			reinterpret_cast<uintptr_t>(&memorySoundFont));

	const int soundFontId = fluid_synth_sfload(synth, fakeFilename, 1);

	if (soundFontId == FLUID_FAILED) {
		UtilityFunctions::printerr("FluidSynth failed to load SoundFont from memory: ", path);
		return FLUID_FAILED;
	}

	return soundFontId;
}
} //namespace fluidsynthgd
