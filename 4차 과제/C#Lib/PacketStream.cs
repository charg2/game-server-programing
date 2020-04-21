using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace Testing
{
    
    public class PacketStream
    {
        public static readonly int  cpapcity = 65536;
        public static readonly bool is_little_endian = true;
        
        public byte[] buffer = new byte[cpapcity];
        public int write_head = 0;
        public int read_head = 0;


        public PacketStream()
        {}

        public void WriteStructure(object obj)
        {
            int size = Marshal.SizeOf(obj);
            
            IntPtr ptr = Marshal.AllocHGlobal(size);

            Marshal.StructureToPtr(obj, ptr, true);

            Marshal.Copy(ptr, buffer, write_head, size);

            write_head += size;
            if ( write_head > cpapcity )
            {
                throw new Exception();
            }

            Marshal.FreeHGlobal(ptr);
        }

        public void InitReadHead()
        {
            read_head = 0;
        }

        public void ReadStructure<T>(out T obj) where T : struct 
        {
            int size = Marshal.SizeOf(typeof(T));
            //if (size > write_head - read_head)
            //{
            //    throw new Exception();
            //}

            IntPtr ptr = Marshal.AllocHGlobal(size);
            Marshal.Copy(buffer, read_head , ptr, size);
            obj = (T)Marshal.PtrToStructure(ptr, typeof(T));
            read_head += size;
            Marshal.FreeHGlobal(ptr);
        }

        public void PeekStructure<T>(T obj) where T : struct
        {
            int size = Marshal.SizeOf(typeof(T));
            if (size > write_head - read_head)
            {
                throw new Exception();
            }

            IntPtr ptr = Marshal.AllocHGlobal(size);
            Marshal.Copy(buffer, read_head, ptr, size);
            obj = (T)Marshal.PtrToStructure(ptr, typeof(T));
            Marshal.FreeHGlobal(ptr);
        }


        public void rewind()
        {
            write_head = read_head = 0;
        }

        public void write(byte[] src, int length)
        {
            Buffer.BlockCopy(src, 0, buffer, write_head, length);
            write_head += length;
        }
        public void write(bool src)
        {
            Buffer.BlockCopy(BitConverter.GetBytes(src), 0, buffer, write_head, sizeof(bool));
            write_head += sizeof(bool);
        }
        public void write(char src)
        {
            Buffer.BlockCopy(BitConverter.GetBytes(src), 0, buffer, write_head, sizeof(char));
            write_head += sizeof(char);
        }
        public void write(short src)
        {
            Buffer.BlockCopy(BitConverter.GetBytes(src), 0, buffer, write_head, sizeof(short));
            write_head += sizeof(short);
        }
        public void write(ushort src)
        {
            Buffer.BlockCopy(BitConverter.GetBytes(src), 0, buffer, write_head, sizeof(ushort));
            write_head += sizeof(ushort);
        }
        public void write(int src)
        {
            Buffer.BlockCopy(BitConverter.GetBytes(src), 0, buffer, write_head, sizeof(int));
            write_head += sizeof(int);
        }
        public void write(long src)
        {
            Buffer.BlockCopy(BitConverter.GetBytes(src), 0, buffer, write_head, sizeof(long));
            write_head += sizeof(long);
        }
        public void write(uint src)
        {
            Buffer.BlockCopy(BitConverter.GetBytes(src), 0, buffer, write_head, sizeof(uint));
            write_head += sizeof(uint);
        }
        public void write(ulong src)
        {
            Buffer.BlockCopy(BitConverter.GetBytes(src), 0, buffer, write_head, sizeof(ulong));
            write_head += sizeof(ulong);
        }
        public void write(float src)
        {
          //  dest = BitConverter.(buffer, read_head);
            Buffer.BlockCopy(BitConverter.GetBytes(src), 0, buffer, write_head, sizeof(float));
            write_head += sizeof(float);
        }
        public void write(double src)
        {
            Buffer.BlockCopy(BitConverter.GetBytes(src), 0, buffer, write_head, sizeof(double));
            write_head += sizeof(double);
        }


        public void read(ref byte[] dest ,int length)
        {
            //dest = BitConverter.ToBoolean(buffer, read_head);
            Buffer.BlockCopy(buffer, read_head, dest ,0, length);
            read_head += length;
        }

        public void read(ref bool dest) //,int length)
        {
            dest = BitConverter.ToBoolean(buffer, read_head);
            //Buffer.BlockCopy(buffer, read_head, BitConverter.GetBytes(dest), 0, sizeof(bool));
            read_head += sizeof(bool);
        }
        public void read(ref char dest) //,int length)
        {
            dest = BitConverter.ToChar(buffer, read_head);
            //Buffer.BlockCopy(buffer, read_head, BitConverter.GetBytes(dest), 0,  sizeof(char));
            read_head += sizeof(char);
        }
        
        //public void read(ref ref short dest) //,int length)
        //{
        //    short temp = 0;
        //    Buffer.BlockCopy(3, 0, BitConverter.GetBytes(temp), 0, sizeof(short));
        //    dest = temp;
        //    read_head += sizeof(short);
        //}

        public void read(ref ushort dest)//, int length)
        {
            dest = BitConverter.ToUInt16(buffer, read_head);
            //byte[] arr = BitConverter.GetBytes(dest);
            //Buffer.BlockCopy(buffer, read_head, BitConverter.GetBytes(dest), 0,  sizeof(ushort));

            read_head += sizeof(ushort);
        }
        public void read(ref int dest)//, int length)
        {
            dest = BitConverter.ToInt32(buffer, read_head);
            //Buffer.BlockCopy(buffer, read_head, BitConverter.GetBytes(dest), 0,  sizeof(int));
            read_head += sizeof(int);
        }
        public void read(ref long dest)//, int length)
        {
            dest = BitConverter.ToInt64(buffer, read_head);
            //Buffer.BlockCopy(buffer, read_head, BitConverter.GetBytes(dest), 0,  sizeof(long));
            read_head += sizeof(long);
        }
        public void read(ref uint dest)//, int length)
        {
            dest = BitConverter.ToUInt32(buffer, read_head);
            //Buffer.BlockCopy(buffer, read_head, BitConverter.GetBytes(dest), 0,  sizeof(uint));
            read_head += sizeof(uint);
        }
        public void read(ref ulong dest) //,int length)
        {
            dest = BitConverter.ToUInt64(buffer, read_head);
            //Buffer.BlockCopy(buffer, read_head, BitConverter.GetBytes(dest), 0,  sizeof(ulong));
            read_head += sizeof(ulong);
        }
        public void read(ref float dest) //,int length)
        {
            dest = BitConverter.ToSingle(buffer, read_head);
            //Buffer.BlockCopy(buffer, read_head, BitConverter.GetBytes(dest), 0,  sizeof(float));
            read_head += sizeof(float);
        }
        public void read(ref double dest) //,int length)
        {
            dest = BitConverter.ToDouble(buffer, read_head);
            //Buffer.BlockCopy(buffer, read_head, BitConverter.GetBytes(dest), 0,  sizeof(double));
            read_head += sizeof(double);
        }


        void peek(int length)
        {
        }

    }
}
