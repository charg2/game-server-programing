using System;
using System.Diagnostics;
using System.Runtime.InteropServices;


public class C2PayloadVector
{
    enum Konstant
    {
        MAXIMUM_SEGMENT_SIZE = 1460 // 
    }

    byte[] buffer = new byte[64 * 1024];
    public Int32 writeHead = 0;
    private Int32 readHead = 0;

    // For geneic method..
    private IntPtr nativeBuffer = Marshal.AllocHGlobal((Int32)Konstant.MAXIMUM_SEGMENT_SIZE);
    private Int32  nativeBufferCapacity = (Int32)Konstant.MAXIMUM_SEGMENT_SIZE;

    public C2PayloadVector() { }

    unsafe public Int32 Wirte<T>(T src)
    {
        Int32 size = Marshal.SizeOf<T>();
        
        Marshal.StructureToPtr(src, nativeBuffer, false);

        Marshal.Copy(nativeBuffer, buffer, writeHead, size); // ptr to buffer

        writeHead += size;
        if (writeHead > buffer.Length)
        {
            throw new Exception();
            return 0;
        }

        return size;
    }

    unsafe public Int32 Wirte<T>(T src, Int32 size)
    {
        Marshal.StructureToPtr(src, nativeBuffer, false);

        Marshal.Copy(nativeBuffer, buffer, writeHead, size); // ptr to buffer

        writeHead += size;
        if (writeHead > buffer.Length)
        {
            throw new Exception();
            return 0;
        }

        return size;
    }

    unsafe public Int32 Wirte(IntPtr ptr, Int32 size)
    {
        Marshal.Copy(ptr, buffer, writeHead, size); // ptr to buffer

        writeHead += size;
        if (writeHead > buffer.Length)
        {
            throw new Exception();
            return 0;
        }

        return size;
    }


    unsafe public Int32 Read<T>(out T dest)
    {
        Int32 size = Marshal.SizeOf<T>();
        if ((readHead + size) > writeHead)
        {
            throw new Exception();
            return 0;
        }

        Marshal.Copy(buffer, readHead, nativeBuffer, size);

        dest = (T)Marshal.PtrToStructure(nativeBuffer, typeof(T));

        readHead += size;

        return size;//Marshal.FreeHGlobal(ptr);
    }


    unsafe public Int32 Read<T>(out T dest, Int32 size)
    {
        if ((readHead + size) <= writeHead)
        {
            throw new Exception();
            return 0;
        }

        Marshal.Copy(buffer, readHead, nativeBuffer, size);

        dest = (T)Marshal.PtrToStructure(nativeBuffer, typeof(T));

        readHead += size;

        return size;
    }

    unsafe public Int32 Read(IntPtr ptr, Int32 size)
    {
        if ((readHead + size) <= writeHead)
        {
            throw new Exception();
            return 0;
        }

        Marshal.Copy(buffer, readHead, ptr, size);

        readHead += size;

        return size;
    }


    unsafe public Int32 Peek<T>(out T dest)
    {
        Int32 size = Marshal.SizeOf<T>();
        if ((readHead + size) > writeHead)
        {
            throw new Exception();
            return 0;
        }

        Marshal.Copy(buffer, readHead, nativeBuffer, size);

        dest = (T)Marshal.PtrToStructure(nativeBuffer, typeof(T));

        return size;//Marshal.FreeHGlobal(ptr);
    }

    unsafe public Int32 Peek<T>(out T dest, Int32 size)
    {
        if ((readHead + size) > writeHead)
        {
            //throw new Exception();
            dest = default;
            return 0;
        }

        Marshal.Copy(buffer, readHead, nativeBuffer, size);

        dest = (T)Marshal.PtrToStructure(nativeBuffer, typeof(T));

        return size;//Marshal.FreeHGlobal(ptr);
    }


    public void Rewind()
    {
        if (readHead > 0)
        {
            //Marshal.Copy(buffer, readHead, buffer, writeHead - readHead); // ptr to buffer
            writeHead -= readHead;
            Buffer.BlockCopy(buffer, readHead, buffer, 0, writeHead);
            readHead = 0;
        }
    }

    public void MoveReadHead(Int32 size)
    {
        readHead += size;
        if (readHead > writeHead)
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

    public byte[] GetBuffer()
    {
        return buffer;
    }


    public Int32 Size
    {
        get { return writeHead - readHead; }
    }

    public Int32 FreeSize
    {
        get { return buffer.Length - writeHead; }
    }


    public Int32 Capacity
    {
        get { return buffer.Length; }
    }


    public bool Empty
    {
        get { return writeHead == readHead; }
    }

    public Int32 ReadHead
    {
        get { return readHead; }
    }

    public Int32 WriteHead
    {
        get { return writeHead; }
    }


    // 대충 귀찮아서 안구현한 부분 ㅋ
    private void ResizeNativeBuffer()
    {

    }

}
