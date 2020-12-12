using System;
using static SDL2.SDL;
using static SDL2.SDL_image;
using static SDL2.SDL_ttf;
using SharpBgfx;
using System.IO;
using System.Reflection;

namespace OpenLSR
{
    class Renderer
    {
        public IntPtr window;
        public IntPtr renderer;

        static readonly string ExePath = Path.GetDirectoryName(Assembly.GetEntryAssembly().Location);
        static readonly string RootPath = Path.Combine(ExePath, "../../../../Assets/");

        public void CreateSDL(int width, int height, string windowTitle = "")
        {
            if (SDL_Init(SDL_INIT_VIDEO) != 0)
                throw new Exception($"Could not init SDL: {SDL_GetError()}");

            SDL_GetVersion(out SDL_version version);
            Console.WriteLine($"Using SDL {version.major}.{version.minor}.{version.patch}");

            var result = SDL_CreateWindowAndRenderer(width, height,
                SDL_WindowFlags.SDL_WINDOW_HIDDEN,
                out IntPtr window,
                out IntPtr renderer);

            if (result != 0)
                throw new Exception($"Could not initialize window: {SDL_GetError()}");

            SDL_SetWindowTitle(window, (windowTitle != string.Empty) ? windowTitle : "OpenLSR Engine");

            this.window = window;
            this.renderer = renderer;
        }

        public static Texture LoadTexture(string name)
        {
            var path = Path.Combine(RootPath, "textures/", name);
            var mem = MemoryBlock.FromArray(File.ReadAllBytes(path));
            return Texture.FromFile(mem, TextureFlags.None, 0);
        }
    }
}
