using System;
using System.Text;
using System.Runtime.InteropServices;
namespace lsmsilcrtl
{
    public static unsafe class rtl
    {
        private const int magic = 0x414D534C;
        private static bool mem_is_valid (void *item)
        {
            item = (void *)((byte *)item -8);
            return *(int *)item == magic;
        }
        public static void* malloc(uint size)
        {
            void *rv = Marshal.AllocHGlobal((int)size+8).ToPointer();
            if (rv != null)
            {
                *(int *)rv = magic;
                *(uint *)((byte *)rv + 4) = size;
                rv = (void *)((byte *)rv + 8);
            }
            return rv;
        }

        public static void free(void* item)
        {
            if (item != null && mem_is_valid(item))
            {
                item = (void *)((byte *)item -8);
                Marshal.FreeHGlobal(new System.IntPtr(item));
            }
        }
        public static void* realloc(void *item, uint size)
        {
            if (item == null)
                return malloc(size);
            if (size == 0)
            {
                free(item);
                return null;
            }
            if (mem_is_valid(item))
            {
                uint cp_size = *(uint *)((byte *)item - 4);
                if (size < cp_size)
                    cp_size = size;
                void *newmem = malloc(size);
                if (newmem != null)
                {
                    for (int i=0; i < cp_size; i++)
                        ((byte *)newmem)[i] = ((byte *)item)[i];
                }
                free(item);
                return newmem;
            }
            return null;
        }
        public static void *calloc(uint nitems, uint count)
        {
            uint sz = nitems * count;
            void *rv = malloc(sz);
            if (rv != null)
            {
                byte *zero = (byte *)rv;
                for (int i=0; i < sz; i++)
                {
                    zero[i] = 0;
                }
            }
            return rv;
        }
    }
    public static unsafe class pointer
    {
        public static object box(void *ptr)
        {
            return (object)(IntPtr)(ptr);
        }
        public static void *unbox(object obj)
        {
            return ((IntPtr)obj).ToPointer();
        }
    }
    public unsafe class args
    {
        public args(Object[] n) { _ptr = n; index = 0; }
        private int index;
        private Object[] _ptr;
        private IntPtr _u_ptr;
        bool _u_saved = false;
        public Object GetNextArg()
        {
            if (index < _ptr.Length)
            {
                return _ptr[index++];
            }
            return null;
        }
        public void *GetUnmanaged()
        {
            int sz;
            byte[] mgd;
            IntPtr umgd;
            if (_u_saved)
            {
//                Marshal.FreeHGlobal(_u_ptr);
            }
            if (_ptr == null || _ptr.Length == 0)
            {
                sz = 4;
                mgd = new byte[sz];
                umgd = Marshal.AllocHGlobal(sz);
            }
            else
            {
                int n = _ptr.Length - index;
                sz = n * 4;
                for (int i=index; i < n; i++)
                    if (_ptr[i] is double || _ptr[i] is Int64 || _ptr[i] is UInt64)
                    {
                        sz += 4;
                    }
                mgd = new byte[sz];
                umgd = Marshal.AllocHGlobal(sz);
                int offset = 0;
                for (int i=index; i < _ptr.Length; i++)
                {
                    Byte[] current;
                    object obj = _ptr[i];
                    if (obj is ValueType)
                    {
                    
                        if (obj is bool)
                            current = BitConverter.GetBytes((bool)obj);
                        else if (obj is char)
                            current = BitConverter.GetBytes((char)obj);
                        else if (obj is byte)
                        {
                            current = new byte[1];
                            current[0] = (byte)obj;
                        }
                        else if (obj is Int16)
                            current = BitConverter.GetBytes((Int16)obj);
                        else if (obj is UInt16)
                            current = BitConverter.GetBytes((UInt16)obj);
                        else if (obj is Int32)
                            current = BitConverter.GetBytes((Int32)obj);
                        else if (obj is UInt32)
                            current = BitConverter.GetBytes((UInt32)obj);
                        else if (obj is Int64)
                            current = BitConverter.GetBytes((Int64)obj);
                        else if (obj is UInt64)
                            current = BitConverter.GetBytes((UInt64)obj);
                        else if (obj is float)
                            current = BitConverter.GetBytes((float)obj);
                        else if (obj is double)
                            current = BitConverter.GetBytes((double)obj);
                        else if (obj is IntPtr)
                            current = BitConverter.GetBytes(((IntPtr)obj).ToInt32());
                        else
                        {
                            current = new Byte[4];
                        }
                        if (current.Length < 4)
                        {
                            byte[] old = current;
                            current = new byte[4];
                            Array.Copy(old,current,old.Length);
                        }
                
                    }
                    else
                    {
                        current = new Byte[4];
                    }
                    Array.Copy(current, 0, mgd, offset, current.Length);
                    offset += current.Length;
                }
            }
            Marshal.Copy(mgd, 0, umgd, sz); 
              
            _u_saved = true;
            _u_ptr =  umgd;
            return _u_ptr.ToPointer();
        }
        protected virtual void Finalize()
        {
//            if (_u_ptr != null)
//                Marshal.FreeHGlobal(_u_ptr);
        }
    }
    public unsafe class CString {
        private string myString;
        private sbyte *nativeString;
        private IntPtr freeableString;

        public static sbyte *ToPointer(string str)
        {
            return new CString(str).ToPointer();
        }
        public CString(string str)  { myString = str; }
        public CString(sbyte *str) {nativeString = str; }
        public CString(byte *str) {nativeString = (sbyte *)str; }
        public CString (int len)
        {
            freeableString = Marshal.AllocHGlobal(len);
            nativeString = (sbyte *)freeableString.ToPointer();
        }
        public override string ToString()
        {
            if (nativeString != null)
            {
                int len = 0;
                byte *src = (byte *)nativeString;
                for (len=0; src[len] != 0; len++) ;
                if (len != 0)
                {
                    StringBuilder rv = new StringBuilder();
                    for (int i=0; i < len; i++)
                        rv.Append((char)src[i]);
                    return rv.ToString();
                }
                else
                {
                    return "";
                }
            }
            return null;
        }
        public sbyte *ToPointer()
        {
            if (freeableString.ToPointer() != null)
            {
                return (sbyte *)freeableString.ToPointer();
            }
            if (myString != null)
            {
                System.Text.ASCIIEncoding  encoding=new System.Text.ASCIIEncoding();
                byte[] ba = encoding.GetBytes(myString);
                int slen = myString.Length;
                freeableString = Marshal.AllocHGlobal(myString.Length+1);
                byte *dest = (byte *)freeableString.ToPointer();
                for (int i = 0; i < slen; i++)
                {

                    dest[i] = ba[i];
                }
                dest[slen] = 0;
                return (sbyte *)freeableString.ToPointer();
            }
            else
            {
                return nativeString;
            }
        }
        protected virtual void finalize()
        {
            if (freeableString.ToPointer() != null)
                Marshal.FreeHGlobal(freeableString);
        }
    }
}