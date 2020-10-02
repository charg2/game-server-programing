using System;
using System.Runtime.InteropServices;

public class C2CircularBuffer
{
    //[System.Runtime.Versioning.ResourceExposure(System.Runtime.Versioning.ResourceScope.None)]
    //[System.Runtime.CompilerServices.MethodImplAttribute(System.Runtime.CompilerServices.MethodImplOptions.InternalCall)]
    //private static extern void CopyToNative(Object source, int startIndex, IntPtr destination, int length);

    private byte[] buffer = new byte[capacity];
    private Int32 writeHead = 0;
    private Int32 readHead = 0;
    private const Int32 capacity = 65536 ;

    private IntPtr nativeBuffer = Marshal.AllocHGlobal(100);
    private Int32  nativeBufferCapacity = 100;

    public C2CircularBuffer()    { }

    unsafe public IntPtr GetBuffer()
    {
        fixed (Byte* fixedBuffer = buffer)
        {
            return (IntPtr)(fixedBuffer);
        }
    }

    unsafe public IntPtr GetWriteBuffer()
    {
        fixed (Byte* fixedBuffer = buffer)
        {
            return (IntPtr)(fixedBuffer + writeHead);
        }
    }

    unsafe public byte* GetWriteBufferToByteArray()
    {
        fixed(Byte* bufferWithOffset = &buffer[writeHead] )
        {
            return bufferWithOffset;
        }
    }


    unsafe public IntPtr GetReadBuffer()
    {
        fixed (Byte* fixedBuffer = buffer)
        {
            return (IntPtr)(fixedBuffer + readHead);
        }
    }

    unsafe public Int32 Enqueue<T>(T src)
    {
        Int32 size = Marshal.SizeOf<T>();
        if(size > nativeBufferCapacity)
        {
            // resize space
        }
        Marshal.StructureToPtr(src, nativeBuffer, false);

    /////////////
        Int32 readHeadCapture = readHead;
        Int32 writeHeadCapture = writeHead;

        Int32 useSize = writeHeadCapture >= readHeadCapture ? writeHeadCapture - readHeadCapture : capacity - (readHeadCapture - writeHeadCapture);
        Int32 freeSize = capacity - (useSize + 1);

        if (freeSize == 0)
            return 0;

        Int32 directEnqueueSize = readHeadCapture > writeHeadCapture ? readHeadCapture - (writeHeadCapture + 1) : capacity - writeHeadCapture;
        Int32 sizeToStore = size > freeSize ? freeSize : size;
        Int32 firstSpaceSize = directEnqueueSize < sizeToStore ? directEnqueueSize : sizeToStore;
        Int32 secondSpaceSize = sizeToStore - firstSpaceSize;

        // first copy
        Marshal.Copy(nativeBuffer, buffer, writeHead, firstSpaceSize);

        // second copy.
        if (secondSpaceSize > 0)
        {
            Byte* temp = (Byte*)nativeBuffer.ToPointer();
            temp += firstSpaceSize;

            Marshal.Copy((IntPtr)temp, buffer, 0, secondSpaceSize);
        }

        writeHeadCapture += sizeToStore;
        writeHeadCapture %= capacity;

        writeHead = writeHeadCapture;

        return sizeToStore;
    }

    unsafe public Int32 Dequeue<T>(out T dest)
    {
        Int32 size = Marshal.SizeOf<T>();

        Int32 tempRear = readHead;
        Int32 tempFront = writeHead;

        Int32 useSize = tempFront >= tempRear ? tempFront - tempRear : capacity - tempRear + tempFront;//
        if (useSize == 0)
        {
            dest = default(T);
            return 0;
        }

        Int32 tempDequeueSize = tempRear > tempFront ? capacity - tempRear : tempFront - tempRear;
        Int32 sizeToLoad = size > useSize ? useSize : size;
        Int32 firstSpaceSize = tempDequeueSize > sizeToLoad ? sizeToLoad : tempDequeueSize;
        Int32 secondSpaceSize = sizeToLoad - firstSpaceSize;

        Marshal.Copy(buffer, readHead, nativeBuffer, firstSpaceSize);

        // second copy.
        if (secondSpaceSize > 0)
        {
            Byte* temp = (Byte*)nativeBuffer.ToPointer();
            temp += firstSpaceSize;

            Marshal.Copy(buffer, 0, (IntPtr)temp, secondSpaceSize);
        }
        
        dest = (T)Marshal.PtrToStructure(nativeBuffer, typeof(T));

        tempRear += sizeToLoad;
        tempRear %= capacity;

        readHead = tempRear;

        return sizeToLoad;
    }

    unsafe public Int32 Enqueue(IntPtr src, Int32 size)
    {
        Int32 readHeadCapture = readHead;
        Int32 writeHeadCapture = writeHead;

        Int32 useSize = writeHeadCapture >= readHeadCapture ? writeHeadCapture - readHeadCapture : capacity - ( readHeadCapture - writeHeadCapture );
        Int32 freeSize = capacity - (useSize + 1);

        if (freeSize == 0)
            return 0;

        Int32 directEnqueueSize = readHeadCapture > writeHeadCapture ? readHeadCapture -(writeHeadCapture + 1) : capacity - writeHeadCapture;
        Int32 sizeToStore       = size > freeSize ? freeSize : size;
        Int32 firstSpaceSize    = directEnqueueSize < sizeToStore ? directEnqueueSize : sizeToStore;
        Int32 secondSpaceSize   = sizeToStore - firstSpaceSize;

        // first copy
        Marshal.Copy(src, buffer, writeHead, firstSpaceSize);

        // second copy.
        if (secondSpaceSize > 0)
        {
            Byte* temp = (Byte*)src.ToPointer();
            temp += firstSpaceSize;

            Marshal.Copy((IntPtr)temp, buffer, 0, secondSpaceSize);
        }

        writeHeadCapture += sizeToStore;
        writeHeadCapture %= capacity;

        writeHead = writeHeadCapture;

        return sizeToStore;
    }

    unsafe public Int32 Dequeue(IntPtr dest, Int32 size)
    {
        Int32 tempRear = readHead;
        Int32 tempFront = writeHead;

        Int32 useSize = tempFront >= tempRear ? tempFront - tempRear : capacity - tempRear + tempFront;//
        if (useSize == 0)
            return 0;

        Int32 tempDequeueSize = tempRear > tempFront ? capacity - tempRear : tempFront - tempRear;
        Int32 sizeToLoad = size > useSize ? useSize : size;
        Int32 firstSpaceSize = tempDequeueSize > sizeToLoad ? sizeToLoad : tempDequeueSize;
        Int32 secondSpaceSize= sizeToLoad - firstSpaceSize;

        Marshal.Copy(buffer, readHead, dest, firstSpaceSize);
        //CopyToNative(buffer, readHead, dest, firstSpaceSize);

        // second copy.
        if (secondSpaceSize > 0)
        {
            Byte* temp = (Byte*)dest.ToPointer();
            temp += firstSpaceSize;

            Marshal.Copy(buffer, 0, (IntPtr)temp, secondSpaceSize);
            //CopyToNative(buffer, readHead, (IntPtr)temp, secondSpaceSize);
        }

        tempRear += sizeToLoad;
        tempRear %= capacity;

        readHead = tempRear;

        return sizeToLoad;
    }


    unsafe public Int32 Peek(IntPtr dest, Int32 size)
    {
        Int32 tempRear = readHead;
        Int32 tempFront = writeHead;

        Int32 useSize = tempFront >= tempRear ? tempFront - tempRear : capacity - tempRear + tempFront;//
        if (useSize == 0)
            return 0;

        Int32 tempDequeueSize = tempRear > tempFront ? capacity - tempRear : tempFront - tempRear;
        Int32 sizeToLoad = size > useSize ? useSize : size;
        Int32 firstSpaceSize = tempDequeueSize > sizeToLoad ? sizeToLoad : tempDequeueSize;
        Int32 secondSpaceSize = sizeToLoad - firstSpaceSize;

        Marshal.Copy(buffer, readHead, dest, firstSpaceSize);
        //CopyToNative(buffer, readHead, dest, firstSpaceSize);

        // second copy.
        if (secondSpaceSize > 0)
        {
            Byte* temp = (Byte*)dest.ToPointer();
            temp += firstSpaceSize;

            Marshal.Copy(buffer, 0, (IntPtr)dest, secondSpaceSize);
            //CopyToNative(buffer, readHead, (IntPtr)temp, secondSpaceSize);
        }

        return sizeToLoad;
    }

    public void MoveReadHead(Int32 size)
    {
        readHead += size;
        readHead %= capacity;
    }

    public void MoveWriteHead(Int32 size)
    {
        writeHead += size;
        writeHead %= capacity;
    }

    public Int32 GetUseSize()
    {
        Int32 readHeadCapture = readHead;
        Int32 writeHeadCapture = writeHead;

        return writeHeadCapture >= readHeadCapture ? writeHeadCapture - readHeadCapture : capacity - (readHeadCapture + writeHeadCapture);
    }

    public Int32 GetFreeSize()
    {
        Int32 readHeadCapture = readHead;
        Int32 writeHeadCapture = writeHead;

        Int32 useSize = writeHeadCapture >= readHeadCapture ? writeHeadCapture - readHeadCapture : capacity - (readHeadCapture + writeHeadCapture);

        return capacity - (useSize + 1);
    }

    public Int32 GetDirectEnqueueSize()
    {
        Int32 readHeadCapture = readHead;
        Int32 writeHeadCapture = writeHead;

        if (readHeadCapture > writeHeadCapture)
            return readHeadCapture - writeHeadCapture - 1;
        else
            return capacity - writeHeadCapture;
    }

    public Int32 GetDirectDequeueSize()
    {
        Int32 readHeadCapture = readHead;
        Int32 writeHeadCapture = writeHead;


        if (readHeadCapture > writeHeadCapture)
            return capacity - readHeadCapture;
        else
            return writeHeadCapture - readHeadCapture;//-1;
    }


    public bool Full
    {
        get { return ( (writeHead +1) % capacity ) == readHead; }
    }

    public bool Empty
    {
        get { return writeHead == readHead; }
    }

    public void Clear()
    {
        writeHead = readHead = 0;
    }
}
