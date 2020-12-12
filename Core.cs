using System;
using System.Numerics;
using static SDL2.SDL;
using static SDL2.SDL_image;
using static System.Console;
using SharpBgfx;
using System.Reflection;

namespace OpenLSR
{
    class Core // use bgfx idiot.
    {
        unsafe static void Main(string[] args)
        {
            int width = 640;
            int height = 480;

            var renderer = new Renderer();

            renderer.CreateSDL(width, height, string.Format("OpenLSR v{0}", Assembly.GetEntryAssembly().GetName().Version.ToString()));

            var wmi = new SDL_SysWMinfo();
            SDL_VERSION(out wmi.version);
            SDL_GetWindowWMInfo(renderer.window, ref wmi);

            var pd = new PlatformData();
            pd.WindowHandle = wmi.info.win.window;

            var init = new InitSettings();
            init.PlatformData = pd;

            Bgfx.Init(init);
            Bgfx.Reset(width, height, ResetFlags.Vsync);

            SDL_ShowWindow(renderer.window);

            // enable debug text
            Bgfx.SetDebugFeatures(DebugFeatures.DisplayText);

            // set view 0 clear state
            Bgfx.SetViewClear(0, ClearTargets.Color | ClearTargets.Depth, 0x303030ff);

            bool quit = false;
            SDL_Event Event;

            // start the frame clock
            var clock = new GameClock();
            clock.Start();

            // main loop
            while (!quit)
            { 
                while (SDL_PollEvent(out Event)!=0)
                {
                    switch (Event.type)
                    {
                        case SDL_EventType.SDL_QUIT:
                            quit = true;
                            break;
                        default:
                            break;
                    }
                }

                // set view 0 viewport
                Bgfx.SetViewRect(0, 0, 0, width, height);

                //  make sure view 0 is cleared if no other draw calls are submitted
                Bgfx.Touch(0);

                // tick the clock
                var elapsed = clock.Frame();
                var time = clock.TotalTime();

                // write some debug text
                Bgfx.DebugTextClear();

                Bgfx.DebugTextWrite(0, 1, DebugColor.White, DebugColor.Magenta, "OpenLSR Test");
                Bgfx.DebugTextWrite(0, 2, DebugColor.White, DebugColor.Magenta, "Frame: {0:F3} ms", elapsed * 1000);
                

                // advance to the next frame. Rendering thread will be kicked to
                // process submitted rendering primitives.
                Bgfx.Frame();
            }

            // clean up
            Bgfx.Shutdown();

            SDL_DestroyRenderer(renderer.renderer);
            SDL_DestroyWindow(renderer.window);

            SDL_Quit();
        }
    }
}
