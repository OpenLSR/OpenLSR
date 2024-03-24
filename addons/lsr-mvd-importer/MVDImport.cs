#if TOOLS
using Godot;

[Tool]
public partial class MVDImport : EditorPlugin
{
    private MVDImportPlugin plugin = new();
    
    public override void _EnterTree()
    {
        AddImportPlugin(plugin);
    }

    public override void _ExitTree()
    {
        RemoveImportPlugin(plugin);
    }
}
#endif