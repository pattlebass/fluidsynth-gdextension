#include "audio_stream_playback_midi.hpp"
#include "memory_soundfont_loader.hpp"
#include <MidiFile.h>
#include <fluidsynth/midi.h>
#include <godot_cpp/classes/audio_server.hpp>
#include <godot_cpp/classes/project_settings.hpp>
#include <sstream>

using namespace godot;

AudioStreamPlaybackMidi::AudioStreamPlaybackMidi() {
	double mix_rate = AudioServer::get_singleton()->get_mix_rate();
	settings = new_fluid_settings();
	fluid_settings_setstr(settings, "audio.driver", "none");
	fluid_settings_setnum(settings, "synth.sample-rate", mix_rate);
	fluid_settings_setint(settings, "synth.threadsafe-api", 1);
	fluid_settings_setint(settings, "synth.cpu-cores", 1);

	synth = new_fluid_synth(settings);
	sequencer = new_fluid_sequencer2(0);
	synth_id = fluid_sequencer_register_fluidsynth(sequencer, synth);

	fluidsynthgd::install_memory_soundfont_loader(settings, synth);
}

AudioStreamPlaybackMidi::~AudioStreamPlaybackMidi() {
	delete_fluid_synth(synth);
	delete_fluid_settings(settings);
	delete_fluid_sequencer(sequencer);
}

void AudioStreamPlaybackMidi::schedule_midi_buffer_at_tick(const godot::PackedByteArray &buffer, double start_tick) {
	std::string str(reinterpret_cast<const char *>(buffer.ptr()), buffer.size());
	std::istringstream stream(str, std::ios::binary);

	smf::MidiFile midi;
	bool ok = midi.read(stream);

	if (!ok || !midi.status()) {
		print_line("Failed to parse MIDI buffer.");
		return;
	}

	midi.absoluteTicks();

	int file_tpq = midi.getTicksPerQuarterNote();

	for (int track = 0; track < midi.getTrackCount(); track++) {
		for (int i = 0; i < midi[track].size(); i++) {
			smf::MidiEvent &parsed_event = midi[track][i];

			fluid_midi_event_t *fluid_midi_event = new_fluid_midi_event();

			if (!event_midi_to_fluid(parsed_event, fluid_midi_event)) {
				delete_fluid_midi_event(fluid_midi_event);
				continue;
			}

			fluid_event_t *seq_event = new_fluid_event();

			fluid_event_set_source(seq_event, -1);
			fluid_event_set_dest(seq_event, synth_id);

			if (fluid_event_from_midi_event(seq_event, fluid_midi_event) == FLUID_OK) {
				double event_beats = double(parsed_event.tick) / double(file_tpq);
				double event_timeline_ticks = event_beats * double(AudioStreamMidiSequencer::TICKS_PER_BEAT);

				double scheduled_timeline_tick = start_tick + event_timeline_ticks;
				double scheduled_ms = timeline_ticks_to_ms(scheduled_timeline_tick);
				unsigned int fire_at = static_cast<unsigned int>(scheduled_ms);
				fluid_sequencer_send_at(sequencer, seq_event, fire_at, 1);

				end_sequencer_time = godot::Math::max(end_sequencer_time, fire_at);
			}

			delete_fluid_event(seq_event);
			delete_fluid_midi_event(fluid_midi_event);
		}
	}
}

bool AudioStreamPlaybackMidi::event_midi_to_fluid(const smf::MidiEvent &e, fluid_midi_event_t *out) {
	if (e.empty()) {
		return false;
	}

	unsigned char status = e[0];

	// Meta events
	if (status == 0xFF) {
		return false;
	}

	// SysEx events
	if (status == 0xF0 || status == 0xF7) {
		fluid_midi_event_set_type(out, status);
		fluid_midi_event_set_sysex(
				out,
				(void *)&e[1],
				(int)e.size() - 1,
				false);
		return true;
	}

	// Only MIDI channel messages from here.
	if (status < 0x80 || status > 0xEF) {
		return false;
	}

	int type = status & 0xF0;
	int channel = status & 0x0F;

	fluid_midi_event_set_type(out, type);
	fluid_midi_event_set_channel(out, channel);

	switch (type) {
		case 0x80: // Note Off
			if (e.size() < 3) {
				return false;
			}
			fluid_midi_event_set_key(out, e[1]);
			fluid_midi_event_set_velocity(out, e[2]);
			return true;

		case 0x90: // Note On
			if (e.size() < 3) {
				return false;
			}
			fluid_midi_event_set_key(out, e[1]);
			fluid_midi_event_set_velocity(out, e[2]);
			return true;

		case 0xA0: // Polyphonic Key Pressure
			if (e.size() < 3) {
				return false;
			}
			fluid_midi_event_set_key(out, e[1]);
			fluid_midi_event_set_value(out, e[2]);
			return true;

		case 0xB0: // Control Change
			if (e.size() < 3) {
				return false;
			}
			fluid_midi_event_set_control(out, e[1]);
			fluid_midi_event_set_value(out, e[2]);
			return true;

		case 0xC0: // Program Change
			if (e.size() < 2) {
				return false;
			}
			fluid_midi_event_set_program(out, e[1]);
			return true;

		case 0xD0: // Channel Pressure
			if (e.size() < 2) {
				return false;
			}
			fluid_midi_event_set_value(out, e[1]);
			return true;

		case 0xE0: { // Pitch Bend
			if (e.size() < 3) {
				return false;
			}

			int pitch = e[1] | (e[2] << 7); // 0..16383, center = 8192
			fluid_midi_event_set_pitch(out, pitch);
			return true;
		}

		default:
			return false;
	}
}

double AudioStreamPlaybackMidi::timeline_ticks_to_ms(double ticks) const {
	return ticks * 60000.0 / (base->get_bpm() * AudioStreamMidiSequencer::TICKS_PER_BEAT);
}

bool AudioStreamPlaybackMidi::reached_end() {
	return fluid_sequencer_get_tick(sequencer) >= end_sequencer_time && fluid_synth_get_active_voice_count(synth) == 0;
}

void AudioStreamPlaybackMidi::_start(double p_from_pos) {
	fluidsynthgd::load_soundfont_from_file(synth, base->get_soundfont_path()); // move to constructor
	for (const AudioStreamMidiSequencer::ScheduledMidiFile &file : base->get_scheduled_midi_files()) {
		schedule_midi_buffer_at_tick(file.buffer, file.start_tick);
	}
	playing = true;
}

void AudioStreamPlaybackMidi::_stop() {
	playing = false;
	fluid_synth_system_reset(synth);
}

bool AudioStreamPlaybackMidi::_is_playing() const {
	return playing;
}

int32_t AudioStreamPlaybackMidi::_get_loop_count() const {
	return 0;
}

double AudioStreamPlaybackMidi::_get_playback_position() const {
	double time_scale = fluid_sequencer_get_time_scale(sequencer); // ticks per second
	double seconds = fluid_sequencer_get_tick(sequencer) / time_scale;
	return seconds;
}

void AudioStreamPlaybackMidi::_seek(double p_position) {
	// TODO
}

int32_t AudioStreamPlaybackMidi::_mix(AudioFrame *buffer, float rate_scale, int32_t frames) {
	if (!playing) {
		for (int i = 0; i < frames; i++) {
			buffer[i] = AudioFrame{ 0.0f, 0.0f };
		}
		return 0;
	}

	fluid_synth_write_float(synth, frames,
							buffer, 0, 2, // left
							buffer, 1, 2); // right

	if (reached_end()) {
		playing = false;
	}

	return frames;
}

void AudioStreamPlaybackMidi::_tag_used_streams() {
}

void AudioStreamPlaybackMidi::_set_parameter(const godot::StringName &p_name, const godot::Variant &p_value) {
}

godot::Variant AudioStreamPlaybackMidi::_get_parameter(const godot::StringName &p_name) const {
	return Variant();
}

void AudioStreamPlaybackMidi::_bind_methods() {
}
