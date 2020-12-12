using System.Diagnostics;

namespace OpenLSR
{
    public class GameClock
    {
        long frequency;
        long lastFrame;
        long initialTick;

        public GameClock()
        {
            frequency = Stopwatch.Frequency;
        }

        public void Start()
        {
            initialTick = Stopwatch.GetTimestamp();
        }

        public float Frame()
        {
            long tick = Stopwatch.GetTimestamp();

            float elapsed = ((float)(tick - lastFrame)) / frequency;
            lastFrame = tick;

            return elapsed;
        }

        public float TotalTime()
        {
            long tick = Stopwatch.GetTimestamp();
            return ((float)(tick - initialTick)) / frequency;
        }
    }
}
