extends Node2D

func _ready() -> void:
	test_plugin_functionality()


func test_plugin_functionality() -> void:
	var stream := AudioStreamMidiSequencer.new()
	stream.soundfont_path = "res://midi/VintageDreamsWaves-v2.sf2"
	stream.bpm = 100
	
	stream.schedule_midi_file_at_beat("res://midi/Dance_GM.mid", 4 * 0)
	
	$AudioStreamPlayer.stream = stream
	$AudioStreamPlayer.play()


#func _process(_delta: float) -> void:
	#if not $AudioStreamPlayer.playing:
		#return
	#var real:float = $AudioStreamPlayer.get_playback_position() + AudioServer.get_time_since_last_mix()
	#var beat := real * 80 / 60.0
	#var column := floori(beat / 4)
	#print(column)
