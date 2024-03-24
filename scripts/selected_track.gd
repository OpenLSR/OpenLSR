extends Label


# Called when the node enters the scene tree for the first time.
func _ready():
	update_text()

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass

func _on_track_list_item_selected(index):
	update_text()

func update_text():
	self.text = $"../TrackList".get_item_text($"../TrackList".get_selected_items()[0])
