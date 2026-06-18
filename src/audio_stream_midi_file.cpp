#include "audio_stream_midi_file.hpp"

String AudioStreamMidiFile::get_path() const {
	return path;
}

void AudioStreamMidiFile::set_path(const String &p_path) {
	path = p_path;

	clear_scheduled_midi_files();
	schedule_midi_file_at_tick(path, 0);
}

void AudioStreamMidiFile::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_path", "path"), &AudioStreamMidiFile::set_path);
	ClassDB::bind_method(D_METHOD("get_path"), &AudioStreamMidiFile::get_path);
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "path"), "set_path", "get_path");
}
