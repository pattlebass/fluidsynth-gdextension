#pragma once
#include "godot_cpp/classes/audio_stream.hpp"

using namespace godot;

class AudioStreamMidiSequencer : public AudioStream {
	GDCLASS(AudioStreamMidiSequencer, AudioStream);

protected:
	static void _bind_methods();

public:
	// Should just use the ResourceLoader instead of reinventing the wheel
	struct ScheduledMidiFile {
		PackedByteArray buffer;
		double start_tick;
	};

	static constexpr int TICKS_PER_BEAT = 960;

	void set_soundfont_path(const String &path);
	String get_soundfont_path() const;
	void set_bpm(double p_bpm);
	double get_bpm() const;

	void schedule_midi_file_at_beat(const String &path, double start_beat);
	void schedule_midi_file_at_tick(const String &path, double start_tick);

	void schedule_midi_buffer_at_beat(const PackedByteArray &buffer, double start_beat);
	void schedule_midi_buffer_at_tick(const PackedByteArray &buffer, double start_tick);

	const Vector<ScheduledMidiFile> &get_scheduled_midi_files() const;

	void clear_scheduled_midi_files();

	virtual Ref<AudioStreamPlayback> _instantiate_playback() const override;
	virtual String _get_stream_name() const override;

private:
	String soundfont_path;
	double bpm = 120.0;
	Vector<ScheduledMidiFile> scheduled_midi_files;
};
