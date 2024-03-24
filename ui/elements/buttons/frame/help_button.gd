extends TextureButton

func _ready():
	$HelpFrame/HelpVideo.play()

func _on_pressed():
	$HelpFrame.visible = !$HelpFrame.visible
