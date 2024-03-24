using Godot;
using System;
using System.IO;
using LSRutil;
using LSRutil.TRK;
using FileAccess = Godot.FileAccess;

public partial class lsrutil_test : Node
{
	private TrkReader reader;
	
	// Called when the node enters the scene tree for the first time.
	public override void _Ready()
	{
		reader = new TrkReader();
	}

	// Called every frame. 'delta' is the elapsed time since the previous frame.
	public override void _Process(double delta)
	{
	}

	public void _on_pressed()
	{
		// TODO: figure out how to pack tracks and read godot packed resources from here
		//var track = reader.ReadTrack("tracks/editor/canyon.trk");
		var track = reader.ReadTrack(new MemoryStream(FileAccess.GetFileAsBytes("tracks/editor/canyon.trk")));
	
		GD.Print("## Track information ##");
		GD.Print("Track Size: ", track.size);
		GD.Print("Track Theme: ", track.theme);
		GD.Print("Time of Day: ", track.time);
		GD.Print("---");
		track.GetElements()[0].GetInfo();
	}
}
