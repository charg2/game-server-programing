using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Threading;

public static class Profiler
{
    private static ThreadLocal<SortedDictionary<string, FunctionInfo>> profilingDict;
    private static ThreadLocal<string>  localFuncName;
    private static ThreadLocal<long>    localStartTick;

    public static int spinLock = 0;

    struct FunctionInfo
    {
        public long callCount;
        public long totalCallTick;
    }

    static Profiler()
    {
        profilingDict = new ThreadLocal<SortedDictionary<string, FunctionInfo>>
        (
             valueFactory: () => new SortedDictionary<string, FunctionInfo>()
            , trackAllValues: true
        );

        localFuncName = new ThreadLocal<string>();
        localStartTick = new ThreadLocal<long>();
    }

    public static void BeginProfiling(string funcName)
    {
        localFuncName.Value = funcName;
        localStartTick.Value = DateTime.Now.Ticks;
    }

    public static void EndProfiling(string funcName)
    {
        //localFuncName.Value = funcName;
        if (funcName != localFuncName.Value)
            return;

        long elapsedTick = DateTime.Now.Ticks - localStartTick.Value;


        FunctionInfo functionInfo;
        if (true == profilingDict.Value.TryGetValue(funcName, out functionInfo))
        {
            functionInfo.callCount += 1;
            functionInfo.totalCallTick += elapsedTick;
            profilingDict.Value[funcName] = functionInfo;
        }
        else
        {
            profilingDict.Value.Add(funcName, new FunctionInfo { callCount = 1, totalCallTick = elapsedTick });
        }

    }

    public static void DoConsoleOut()//TimeUnit unit = TimeUnit.NanoSec)
    {
        Console.WriteLine("Thread No. \\ Function Name          \\  averageTime  \\ totalTime \\ callCount");
        int threadOrder = 0;
        foreach (var localProfilingDict in profilingDict.Values)
        {
            foreach (KeyValuePair<string, FunctionInfo> pair in localProfilingDict)
            {
                double averageTime = pair.Value.totalCallTick / pair.Value.callCount;
                Console.WriteLine($"[{threadOrder}] {pair.Key}              avgTime : {averageTime}00 ns, totalTime :{pair.Value.totalCallTick}, call count :{pair.Value.callCount} ");
            }
            ++threadOrder;
        }

    }

    // log4net을 쓰자.
    public static void DoFileOut()
    {}

    [DllImport("winmm.dll", EntryPoint = "timeBeginPeriod")]
    public static extern uint timeBeginPeriod(uint uMilliseconds);

    [DllImport("winmm.dll", EntryPoint = "timeEndPeriod")]
    public static extern uint timeEndPeriod(uint uMilliseconds);

    public static void Init()
    {
        timeBeginPeriod(1);
    }

    public static void Fin()
    {
        timeEndPeriod(1);
    }
}
