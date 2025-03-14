extends Node

var is_running = false

func _ready():
	get_status()

func _process(delta):
	if is_running:
		$gd_retro.core_run()

func _on_button_core_pressed():
	if $gd_retro.is_initialized():
		$gd_retro.core_unload()
		get_status()
	else:
		$file_dialog_core.popup()

func _on_button_game_pressed():
	if not $gd_retro.is_initialized(): return
	$file_dialog_game.popup()

func _on_button_exit_pressed():
	get_tree().quit()

func _on_button_run_toggled(button_pressed):
	is_running = button_pressed
	$ui_canvas_layer/ui/buttons_container_left/button_run.text = "STOP" if is_running else "RUN"

func _on_file_dialog_core_file_selected(path):
	$gd_retro.core_load(path)
	get_status()

func get_user_data_path():
	var home_dir = OS.get_environment("HOME")
	var project_name = ProjectSettings.get_setting("application/config/name")
	
	var user_data_path = ""
	if OS.get_name() == "MacOS":
		user_data_path = home_dir + "/Library/Application Support/Godot/app_userdata/" + project_name + "/"
	elif OS.get_name() == "Linux":
		user_data_path = home_dir + "/.local/share/godot/app_userdata/" + project_name + "/"
	elif OS.get_name() == "Windows":
		user_data_path = OS.get_environment("APPDATA") + "/Godot/app_userdata/" + project_name + "/"
	return user_data_path

func _on_file_dialog_game_file_selected(path: String):
	var realpath = path.replace("user://", get_user_data_path())
	print("realpath: %s" % realpath)
	if $gd_retro.core_load_game(realpath):
		$ui_canvas_layer/ui/buttons_container_left/button_run.disabled = false

func get_status():
	print("core system info: %s" % $gd_retro.core_get_system_info())
	print($gd_retro.core_get_system_av_info())
	var text = ""
	if $gd_retro.is_initialized():
		var status = $gd_retro.core_get_system_info()
		text += "%s %s" % [status["library_name"], status["library_version"]]
		$ui_canvas_layer/ui/buttons_container_left/button_core.text = "UNLOAD CORE"
		$ui_canvas_layer/ui/buttons_container_left/button_game.disabled = false
	else:
		text += "No core loaded."
		$ui_canvas_layer/ui/buttons_container_left/button_core.text = "LOAD CORE"
		$ui_canvas_layer/ui/buttons_container_left/button_game.disabled = true
	$ui_canvas_layer/ui/label_info.text = text
