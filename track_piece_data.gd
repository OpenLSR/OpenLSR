extends Resource
class_name TrackPieceData

enum Rotation {WEST, NORTH, EAST, SOUTH}

@export var position: Vector3i
@export var rotation: Rotation
@export var origin: Vector2i
@export var theme: TrackData.TrackTheme
@export var piece_id: int
