extends Control

signal pick_race()
signal load_editor()
signal options_pressed()

func _on_race_button_pressed():
	pick_race.emit()

func _on_construct_button_pressed():
	load_editor.emit()
	
func _on_options_button_pressed():
	options_pressed.emit()

func _on_quit_button_pressed():
	get_tree().quit()
