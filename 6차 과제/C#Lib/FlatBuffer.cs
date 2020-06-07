using System;
using System.Runtime.InteropServices;
using System.Threading;


public class ThreadLocalBuffer //: IDisposable
{
    enum Konstant
    {
        MAXIMUM_SEGMENT_SIZE = 1460 // 
    }

    public Int32 Capacity { get; set; } = (Int32)Konstant.MAXIMUM_SEGMENT_SIZE;
    public IntPtr Buffer { get; set; } = Marshal.AllocHGlobal((Int32)Konstant.MAXIMUM_SEGMENT_SIZE);

    public ThreadLocalBuffer()
    { }


    public void Resize(Int32 newCapacity)
    {
        Marshal.FreeHGlobal(Buffer);

        Buffer = Marshal.AllocHGlobal(newCapacity);

        Capacity = newCapacity;
    }
}



public class FlatBuffer
{
    private byte[] buffer = new byte[1460];
    private Int32 writeHead = 0;
    private Int32 readHead = 0;
    private static Int32 headerSize = 0;



    private static ThreadLocal<ThreadLocalBuffer> innerBuffer = new ThreadLocal<ThreadLocalBuffer>(() => new ThreadLocalBuffer());


    public FlatBuffer(Int32 initialHeaderSize = 2)
    {
        writeHead = readHead = initialHeaderSize;
        FlatBuffer.headerSize = initialHeaderSize;
    }

    unsafe public IntPtr GetBuffer()
    {
        fixed (Byte* fixedBuffer = buffer)
        {
            return (IntPtr)(fixedBuffer + writeHead);
        }
    }

    unsafe public byte[] GetBufferToByteArray()
    {
        return buffer;
    }

    unsafe public void Wirte<T>(T src)
    {
        Int32 size = Marshal.SizeOf<T>();

        ThreadLocalBuffer localBuffer = innerBuffer.Value;
        if (localBuffer.Capacity < size)
        {
            localBuffer.Resize(size);
        }

        //IntPtr ptr = Marshal.AllocHGlobal(size);
        IntPtr ptr = localBuffer.Buffer;

        Marshal.StructureToPtr(src, ptr, false);

        Marshal.Copy(ptr, buffer, writeHead, size); // ptr to buffer

        writeHead += size;
        if (writeHead > buffer.Length)
        {
            throw new Exception();
        }

        // using inner buffer
        //Marshal.FreeHGlobal(ptr);

        return;
    }

    unsafe public void Wirte<T>(T src, Int32 size)
    {
        ThreadLocalBuffer localBuffer = innerBuffer.Value;
        if (localBuffer.Capacity < size)
        {
            localBuffer.Resize(size);
        }

        //IntPtr ptr = Marshal.AllocHGlobal(size);
        IntPtr ptr = localBuffer.Buffer;

        Marshal.StructureToPtr(src, ptr, false);

        Marshal.Copy(ptr, buffer, writeHead, size); // ptr to buffer

        writeHead += size;
        if (writeHead > buffer.Length)
        {
            throw new Exception();
        }

        // using inner buffer
        //Marshal.FreeHGlobal(ptr);

        return;
    }

    unsafe public void Wirte(IntPtr ptr, Int32 size)
    {
        Marshal.Copy(ptr, buffer, writeHead, size); // ptr to buffer

        writeHead += size;
        if (writeHead > buffer.Length)
        {
            throw new Exception();
        }

        return;
    }


    unsafe public bool Read<T>(out T dest)
    {
        Int32 size = Marshal.SizeOf<T>();
        if ((readHead + size) > writeHead)
        {
            throw new Exception();
            return false;
        }

        ThreadLocalBuffer localBuffer = innerBuffer.Value;
        IntPtr ptr = localBuffer.Buffer;            //IntPtr ptr = Marshal.AllocHGlobal(size);

        Marshal.Copy(buffer, readHead, ptr, size);

        dest = (T)Marshal.PtrToStructure(ptr, typeof(T));

        readHead += size;

        return true;//Marshal.FreeHGlobal(ptr);
    }


    unsafe public bool Read<T>(out T dest, Int32 size)
    {
        if ((readHead + size) <= writeHead)
        {
            throw new Exception();
            return false;
        }

        ThreadLocalBuffer localBuffer = innerBuffer.Value;
        IntPtr ptr = localBuffer.Buffer;            //IntPtr ptr = Marshal.AllocHGlobal(size);

        Marshal.Copy(buffer, readHead, ptr, size);

        dest = (T)Marshal.PtrToStructure(ptr, typeof(T));

        readHead += size;

        return true;//Marshal.FreeHGlobal(ptr);
    }

    unsafe public bool Read(IntPtr ptr, Int32 size)
    {
        if ((readHead + size) <= writeHead)
        {
            throw new Exception();
            return false;
        }

        Marshal.Copy(buffer, readHead, ptr, size);

        readHead += size;

        return true;
    }


    unsafe public bool Peek<T>(out T dest)
    {
        Int32 size = Marshal.SizeOf<T>();
        if ((readHead + size) <= writeHead)
        {
            throw new Exception();
            return false;
        }

        ThreadLocalBuffer localBuffer = innerBuffer.Value;
        IntPtr ptr = localBuffer.Buffer;

        Marshal.Copy(buffer, readHead, ptr, size);

        dest = (T)Marshal.PtrToStructure(ptr, typeof(T));//readHead += size;

        return true;//Marshal.FreeHGlobal(ptr);
    }

    unsafe public bool Peek<T>(T dest, Int32 size)
    {
        if ((readHead + size) <= writeHead)
        {
            throw new Exception();
            return false;
        }

        ThreadLocalBuffer localBuffer = innerBuffer.Value;
        IntPtr ptr = localBuffer.Buffer;            //IntPtr ptr = Marshal.AllocHGlobal(size);

        Marshal.Copy(buffer, readHead, ptr, size);

        dest = (T)Marshal.PtrToStructure(ptr, typeof(T)); //readHead += size;

        return true;//Marshal.FreeHGlobal(ptr);
    }


    public void Rewind()
    {
        writeHead = readHead = headerSize;
    }

    public void MoveReadHead(Int32 size)
    {
        readHead += size;
        if(readHead > writeHead)
        {
            throw new Exception();
        }
    }

    public void MoveWriteHead(Int32 size)
    {
        writeHead += size;
        if (writeHead > Capacity - 1)
        {
            throw new Exception();
        }
    }

    public void Resize(Int32 size)
    {
        throw new Exception();
        // 대충 귀찮아서 안만들었다는 것;
    }

    public Int32 Size
    {
        get{ return writeHead - readHead; }
    }

    public Int32 Capacity
    {
        get { return buffer.Length; }
    }


    public bool Empty
    {
        get { return writeHead == readHead; }
    }

}

