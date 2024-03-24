#if TOOLS
using System.IO;
using Godot;
using Godot.Collections;
using LSRutil;
using LSRutil.MVD;
using FileAccess = Godot.FileAccess;

[Tool]
public partial class MVDImportPlugin: EditorImportPlugin
{
    public override string _GetImporterName() => "openlsr.mvd";
    public override string _GetVisibleName() => "MotoVideo";
    public override string[] _GetRecognizedExtensions() => new[] {"mvd"};
    public override string _GetResourceType() => nameof(SpriteFrames);
    public override float _GetPriority() => 1f;
    public override int _GetPresetCount() => 0;
    public override int _GetImportOrder() => 0;
    public override string _GetSaveExtension() => "res";
    public override bool _GetOptionVisibility(string path, StringName optionName, Dictionary options) => true;
    
    public override Array<Dictionary> _GetImportOptions(string path, int presetIndex)
    {
        return new Array<Dictionary>
        {
            new Dictionary
            {
                { "name", "myOption" },
                { "default_value", false },
            }
        };
    }

    public override Error _Import(string sourceFile, string savePath, Dictionary options, Array<string> platformVariants, Array<string> genFiles)
    {
        using (var fa = FileAccess.Open(sourceFile, FileAccess.ModeFlags.Read))
        {
            if (fa == null) return FileAccess.GetOpenError();
            using (var stream = new MemoryStream(fa.GetBuffer((long)fa.GetLength())))
            {
                var reader = new MvdReader();
                MotoVideo video = reader.ReadVideo(stream);

                var spriteframes = new SpriteFrames();
                spriteframes.SetAnimationSpeed("default", 20f);
                //spriteframes.AddFrame("default", new Texture2D());

                foreach (var frame in video.frames)
                {
                    var img = new Image();
                    img.SetData(frame.width, frame.height, false, Image.Format.Rgb565, frame.bytes);
                    spriteframes.AddFrame("default", ImageTexture.CreateFromImage(img));
                }

                return ResourceSaver.Save(spriteframes, $"{savePath}.{_GetSaveExtension()}");
            }
        }
    }
}
#endif