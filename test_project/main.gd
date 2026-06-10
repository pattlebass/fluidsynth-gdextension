extends Node2D

func _ready() -> void:
	test_plugin_functionality()

func test_plugin_functionality() -> void:
	var stream := AudioStreamMidiSequencer.new()
	#stream.midi_path = "res://Sony Ericsson MusicDJ.mid"
	#stream.midi_path = "res://Sony Ericsson MusicDJ_fixed.mid"
	#stream.midi_path = "res://samples/samples2/sample_0_16.mid"
	stream.soundfont_path = "res://Sony Ericsson A100 DB2020 Synth 1.0.4(1).sf2"
	stream.bpm = 80
	
	#stream.schedule_midi_file_at_beat("res://samples/samples2/sample_3_1.mid", 4 * 0)
	for i in 10:
		stream.schedule_midi_file_at_beat("res://samples/samples2/sample_2_%s.mid" % (i+1), 4 * i)
	
	#for i in 32:
		#stream.schedule_midi_file_at_beat("res://samples/samples2/sample_0_%s.mid" % (i+1), 4 * i)
	
	$AudioStreamPlayer.stream = stream
	$AudioStreamPlayer.play()
	
	#var playback := $AudioStreamPlayer.get_stream_playback() as AudioStreamPlaybackMidi
	#playback.set_bpm(180)
	#playback.load_soundfont("res://Sony Ericsson A100 DB2020 Synth 1.0.4(1).sf2")
	#playback.play_midi("res://Sony Ericsson MusicDJ.mid")
	#playback.start()

func _process(_delta: float) -> void:
	if not $AudioStreamPlayer.playing:
		return
	var real:float = $AudioStreamPlayer.get_playback_position() + AudioServer.get_time_since_last_mix()
	var beat := real * 80 / 60.0
	var column := floori(beat / 4)
	print(column)
	#print(real - $AudioStreamPlayer.get_playback_position())
	#return
	#var tick: int = $AudioStreamPlayer.get_stream_playback().get_playback_position_ticks()
	#var beat: int = tick / AudioStreamMidiSequencer.TICKS_PER_BEAT
	#var column := beat / 4
	#print(tick)
