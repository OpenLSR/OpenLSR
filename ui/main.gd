extends Control

func _ready():
	get_window().set_min_size(Vector2i(800, 600))

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass

func _input(event):
	
	# this is just for music testing, remove this eventually
	if event is InputEventKey and event.pressed:
		match event.keycode:
			KEY_0:
				%MusicManager.play_song(%MusicManager.frontend)
			KEY_1:
				%MusicManager.play_song(%MusicManager.city_race)
			KEY_2:
				%MusicManager.play_song(%MusicManager.city_build)
			KEY_3:
				%MusicManager.play_song(%MusicManager.desert_race)
			KEY_4:
				%MusicManager.play_song(%MusicManager.desert_build)
			KEY_5:
				%MusicManager.play_song(%MusicManager.jungle_race)
			KEY_6:
				%MusicManager.play_song(%MusicManager.jungle_build)
			KEY_7:
				%MusicManager.play_song(%MusicManager.arctic_race)
			KEY_8:
				%MusicManager.play_song(%MusicManager.arctic_build)
			KEY_9:
				%MusicManager.play_song(%MusicManager.mr_x)

func switch_menu(menu: Control):
	get_children().all(func(c): c.hide(); return true)
	menu.show()

func _on_main_menu_pick_race():
	switch_menu($RaceMenu)

func _on_main_menu_load_editor():
	switch_menu($Editor)
	%MusicManager.play_song(%MusicManager.city_build)

func _on_main_menu_options_pressed():
	switch_menu($OptionsMenu)
	

func _on_go_back():
	# TODO: make this actually go to the previous menu
	switch_menu($MainMenu)
	%MusicManager.play_song(%MusicManager.frontend)
