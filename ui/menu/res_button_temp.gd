extends TextureRect

func _on_640_pressed():
	get_window().set_size(Vector2i(1280, 720))

func _on_800_pressed():
	get_window().set_size(Vector2i(800, 600))
