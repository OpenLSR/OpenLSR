extends Resource
class_name TrackData

enum TrackTheme {JUNGLE, ARCTIC, DESERT, CITY}
enum TrackSize {MULTIPLAYER, SINGLEPLAYER}
enum TrackTime {DAY, NIGHT}

@export_group("Environment")

@export var theme: TrackTheme = TrackTheme.CITY
@export var size: TrackSize = TrackSize.SINGLEPLAYER
@export var time: TrackTime = TrackTime.DAY

@export_group("Flags")
## When set to true, the track can be completed start to finish.
@export var playable: bool

@export_group("Data")
## An Array of all pieces in this track.
@export var pieces: Array[TrackPieceData]
