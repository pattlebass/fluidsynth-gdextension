#pragma once
#include "audio_stream_midi_sequencer.hpp"

using namespace godot;

class AudioStreamMidiFile : public AudioStreamMidiSequencer {
	GDCLASS(AudioStreamMidiFile, AudioStreamMidiSequencer);

protected:
	static void _bind_methods();

public:
	String get_path() const;
	void set_path(const String &path);

private:
	String path = "";
};
