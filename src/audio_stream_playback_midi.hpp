#pragma once

#include "audio_stream_midi_sequencer.hpp"
#include <MidiFile.h>
#include <fluidsynth.h>
#include <godot_cpp/classes/audio_stream.hpp>
#include <godot_cpp/classes/audio_stream_playback.hpp>
#include <godot_cpp/variant/string.hpp>

class AudioStreamPlaybackMidi : public godot::AudioStreamPlayback {
	GDCLASS(AudioStreamPlaybackMidi, AudioStreamPlayback);

private:
	friend class AudioStreamMidiSequencer; // Ummmmm
	Ref<AudioStreamMidiSequencer> base;

	bool playing = false;
	unsigned int end_sequencer_time = 0;

	fluid_settings_t *settings = nullptr;
	fluid_synth_t *synth = nullptr;
	fluid_sequencer_t *sequencer = nullptr;
	fluid_seq_id_t synth_id = -1;

	void schedule_midi_buffer_at_tick(const godot::PackedByteArray &buffer, double start_tick);

	bool event_midi_to_fluid(const smf::MidiEvent &e, fluid_midi_event_t *out);
	double timeline_ticks_to_ms(double ticks) const;

protected:
	static void _bind_methods();

public:
	AudioStreamPlaybackMidi();
	~AudioStreamPlaybackMidi();

	void _start(double p_from_pos) override;
	void _stop() override;
	bool _is_playing() const override;
	int32_t _get_loop_count() const override;
	double _get_playback_position() const override;
	void _seek(double p_position) override;
	int32_t _mix(godot::AudioFrame *buffer, float rate_scale, int32_t frames) override;
	void _tag_used_streams() override;
	void _set_parameter(const godot::StringName &p_name, const godot::Variant &p_value) override;
	godot::Variant _get_parameter(const godot::StringName &p_name) const override;
};
