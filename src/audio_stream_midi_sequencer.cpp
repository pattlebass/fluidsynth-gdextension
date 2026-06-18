#include "audio_stream_midi_sequencer.hpp"
#include "audio_stream_playback_midi.hpp"
#include "godot_cpp/classes/audio_stream_playback.hpp"
#include "godot_cpp/classes/file_access.hpp"

Ref<AudioStreamPlayback> AudioStreamMidiSequencer::_instantiate_playback() const {
	Ref<AudioStreamPlaybackMidi> playback = memnew(AudioStreamPlaybackMidi);
	playback->base = Ref<AudioStreamMidiSequencer>(const_cast<AudioStreamMidiSequencer *>(this));
	return playback;
}

String AudioStreamMidiSequencer::_get_stream_name() const {
	return "AudioStreamMidiSequencer";
}

void AudioStreamMidiSequencer::set_soundfont_path(const String &path) {
	soundfont_path = path;
}

String AudioStreamMidiSequencer::get_soundfont_path() const {
	return soundfont_path;
}

void AudioStreamMidiSequencer::set_bpm(double p_bpm) {
	bpm = p_bpm;
}

double AudioStreamMidiSequencer::get_bpm() const {
	return bpm;
}

void AudioStreamMidiSequencer::schedule_midi_file_at_beat(const String &path, double start_beat) {
	schedule_midi_file_at_tick(path, start_beat * AudioStreamMidiSequencer::TICKS_PER_BEAT);
}

void AudioStreamMidiSequencer::schedule_midi_file_at_tick(const String &path, double start_tick) {
	Ref<FileAccess> fa = FileAccess::open(path, FileAccess::READ);
	if (!fa.is_valid()) {
		print_line("Error " + godot::String::num(FileAccess::get_open_error()) + ": Failed to open MIDI file at path: " + path);
		return;
	}
	const PackedByteArray buffer = fa->get_buffer(fa->get_length());
	schedule_midi_buffer_at_tick(buffer, start_tick);
}

void AudioStreamMidiSequencer::schedule_midi_buffer_at_beat(const PackedByteArray &buffer, double start_beat) {
	schedule_midi_buffer_at_tick(buffer, start_beat * AudioStreamMidiSequencer::TICKS_PER_BEAT);
}

void AudioStreamMidiSequencer::schedule_midi_buffer_at_tick(const PackedByteArray &buffer, double start_tick) {
	ScheduledMidiFile scheduled_file;
	scheduled_file.buffer = buffer;
	scheduled_file.start_tick = start_tick;
	scheduled_midi_files.push_back(scheduled_file);
}

const Vector<AudioStreamMidiSequencer::ScheduledMidiFile> &AudioStreamMidiSequencer::get_scheduled_midi_files() const {
	return scheduled_midi_files;
}

void AudioStreamMidiSequencer::clear_scheduled_midi_files() {
	scheduled_midi_files.clear();
}

void AudioStreamMidiSequencer::_bind_methods() {
	BIND_CONSTANT(TICKS_PER_BEAT);

	ClassDB::bind_method(D_METHOD("set_soundfont_path", "path"), &AudioStreamMidiSequencer::set_soundfont_path);
	ClassDB::bind_method(D_METHOD("get_soundfont_path"), &AudioStreamMidiSequencer::get_soundfont_path);
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "soundfont_path"), "set_soundfont_path", "get_soundfont_path");

	ClassDB::bind_method(D_METHOD("set_bpm", "bpm"), &AudioStreamMidiSequencer::set_bpm);
	ClassDB::bind_method(D_METHOD("get_bpm"), &AudioStreamMidiSequencer::get_bpm);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "bpm"), "set_bpm", "get_bpm");

	ClassDB::bind_method(D_METHOD("schedule_midi_file_at_beat", "path", "start_beat"), &AudioStreamMidiSequencer::schedule_midi_file_at_beat);
	ClassDB::bind_method(D_METHOD("schedule_midi_file_at_tick", "path", "start_tick"), &AudioStreamMidiSequencer::schedule_midi_file_at_tick);
	ClassDB::bind_method(D_METHOD("schedule_midi_buffer_at_beat", "buffer", "start_beat"), &AudioStreamMidiSequencer::schedule_midi_buffer_at_beat);
	ClassDB::bind_method(D_METHOD("schedule_midi_buffer_at_tick", "buffer", "start_tick"), &AudioStreamMidiSequencer::schedule_midi_buffer_at_tick);
	ClassDB::bind_method(D_METHOD("clear_scheduled_midi_files"), &AudioStreamMidiSequencer::clear_scheduled_midi_files);
}
