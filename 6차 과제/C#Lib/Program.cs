using System;
using System.Runtime.InteropServices;

namespace Testin_g
{
    class Program
    {
        struct TTT
        {
            public Int32 tt1;
            public Int32 tt2;
            public Int32 tt3;
            public Int32 tt4;
        }



        static void Main(string[] args)
        {
            Profiler.Init();

            
            Console.WriteLine("Hello World!");

            TTT tt = new TTT();
            tt.tt1 = 1;
            tt.tt2 = 2;
            tt.tt3 = 3;
            tt.tt4 = 4;


            FlatBuffer buffer = new FlatBuffer(2);
            CircularBuffer circularBuffer = new CircularBuffer();

            buffer.Wirte<TTT>(tt);
            circularBuffer.Enqueue<TTT>(tt);
            tt.tt1 = 5;
            tt.tt2 = 6;
            tt.tt3 = 7;
            tt.tt4 = 8;


            buffer.Wirte<TTT>(tt);
            circularBuffer.Enqueue<TTT>(tt);

            tt.tt1 = 0;
            tt.tt2 = 0;
            tt.tt3 = 0;
            tt.tt4 = 0;

            buffer.Read<TTT>(out tt);


            tt.tt1 = 0;
            tt.tt2 = 0;
            tt.tt3 = 0;
            tt.tt4 = 0;

            circularBuffer.Dequeue<TTT>(out tt);


            tt.tt1 = 0;
            tt.tt2 = 0;
            tt.tt3 = 0;
            tt.tt4 = 0;

            buffer.Read<TTT>(out tt);



            tt.tt1 = 0;
            tt.tt2 = 0;
            tt.tt3 = 0;
            tt.tt4 = 0;

            circularBuffer.Dequeue<TTT>(out tt);





            Profiler.Fin();
        }
    }
}
