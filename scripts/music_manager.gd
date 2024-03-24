extends Node
class_name MusicManager

@export_group("Streams")
@export var frontend: AudioStream

@export var mr_x: AudioStream

@export_subgroup("City", "city_")
@export var city_race: AudioStream
@export var city_build: AudioStream

@export_subgroup("Desert", "desert_")
@export var desert_race: AudioStream
@export var desert_build: AudioStream

@export_subgroup("Jungle", "jungle_")
@export var jungle_race: AudioStream
@export var jungle_build: AudioStream

@export_subgroup("Arctic", "arctic_")
@export var arctic_race: AudioStream
@export var arctic_build: AudioStream

# Called when the node enters the scene tree for the first time.
func _ready():
	play_song(frontend)

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass
	
func play_song(song: AudioStream):
	if $XMPlayer.stream != song:
		$XMPlayer.stream = song
		$XMPlayer.play()
