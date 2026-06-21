#include "audio_stream_midi_sequencer.hpp"
#include "audio_stream_playback_midi.hpp"
#include "fluidsynth/synth.h"
#include "utils.hpp"
#include <godot_cpp/classes/audio_server.hpp>
#include <godot_cpp/classes/audio_stream_playback.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>

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

// Adapted from Godot's audio_stream_wav.cpp
Error AudioStreamMidiSequencer::save_to_wav(const String &p_path) {
	PackedByteArray data = render_audio();

	int sub_chunk_2_size = data.size(); //Subchunk2Size = Size of data in bytes
	int format_code = 1; // PCM
	int n_channels = 2;
	long sample_rate = AudioServer::get_singleton()->get_mix_rate();

	int byte_pr_sample = 2; // 16 bits format

	String file_path = p_path;
	if (file_path.substr(file_path.length() - 4, 4).to_lower() != ".wav") {
		file_path += ".wav";
	}

	Ref<FileAccess> file = FileAccess::open(file_path, FileAccess::WRITE); //Overrides existing file if present

	ERR_FAIL_COND_V(file.is_null(), ERR_FILE_CANT_WRITE);

	// Create WAV Header
	file->store_string("RIFF"); //ChunkID
	file->store_32(sub_chunk_2_size + 36); //ChunkSize = 36 + SubChunk2Size (size of entire file minus the 8 bits for this and previous header)
	file->store_string("WAVE"); //Format
	file->store_string("fmt "); //Subchunk1ID
	file->store_32(16); //Subchunk1Size = 16
	file->store_16(format_code); //AudioFormat
	file->store_16(n_channels); //Number of Channels
	file->store_32(sample_rate); //SampleRate
	file->store_32(sample_rate * n_channels * byte_pr_sample); //ByteRate
	file->store_16(n_channels * byte_pr_sample); //BlockAlign = NumChannels * BytePrSample
	file->store_16(byte_pr_sample * 8); //BitsPerSample
	file->store_string("data"); //Subchunk2ID
	file->store_32(sub_chunk_2_size); //Subchunk2Size

	// Add data
	const uint8_t *read_data = data.ptr();
	for (uint64_t i = 0; i < data.size() / 2; i++) {
		uint16_t data_point = decode_uint16(&read_data[i * 2]);
		file->store_16(data_point);
	}

	return OK;
}

// TODO: Should probably pipe it straight to the file
PackedByteArray AudioStreamMidiSequencer::render_audio() {
	PackedByteArray data;
	Ref<AudioStreamPlaybackMidi> playback = _instantiate_playback();
	playback->start();

	const int BLOCK_SIZE = 512;
	int16_t samples[BLOCK_SIZE * 2];

	while (!playback->reached_end()) {
		fluid_synth_write_s16(playback->synth, BLOCK_SIZE, samples, 0, 2, samples, 1, 2);

		const int64_t old_size = data.size();
		data.resize(old_size + sizeof(samples));
		memcpy(data.ptrw() + old_size, samples, sizeof(samples));
	}

	return data;
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

	ClassDB::bind_method(D_METHOD("save_to_wav", "path"), &AudioStreamMidiSequencer::save_to_wav);
}
