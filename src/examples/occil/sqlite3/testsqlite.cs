// A Hello World! program in C#.
using System;
using zzz;
using lsmsilcrtl;
using sqlite3;
namespace HelloWorld
{

    unsafe class Hello 
    {
        static void DumpNames(string file)
        {
            sqlite.sqlite3 *db;
            if (sqlite.sqlite3_open_v2(CString.ToPointer(file), &db, 2/* SQLITE_OPEN_READWRITE*/, null) == 0 /*SQLITE_OK*/)
            {
                string query = "SELECT name FROM names;";
                sqlite.sqlite3_stmt *handle;
                int rc = sqlite.sqlite3_prepare_v2(db, CString.ToPointer(query), query.Length+1, &handle, null);
                if (rc == 0)
                {
                    bool done = false;
                    while (!done)
                    {
                        switch(rc = sqlite.sqlite3_step(handle))
                        {
                            case 5://SQLITE_BUSY:
                                done = true;
                                break;
                            case 101: //SQLITE_DONE:
                                done = true;
                                break;
                            case 100://SQLITE_ROW:
                            {
                                string aa = new CString(sqlite.sqlite3_column_text(handle, 0)).ToString();
                                Console.Write("{0} ", aa);
                                rc = 0;//SQLITE_OK;
                                break;
                            }
                            default:
                                done = true;
                                break;
                        }
                    }
                    sqlite.sqlite3_finalize(handle);
                    
                }
                sqlite.sqlite3_close(db);
            }
            Console.WriteLine();
        }
        static void Main() 
        {
            Console.WriteLine("Hello World!");

            ar.myenum qq1 = ar.myenum.two;
            Console.WriteLine("An enumeration value {0} = {1}", qq1, (int)qq1);
            // get a structure instance from the dll and write its contents
            ar.aa * nn = ar.getqq();

            Console.WriteLine("structure from dll: {0}, {1}, {2}", nn->i, nn->b, nn->j);
            // use CString to make a C-style string for the C code
            ar.myprintf(CString.ToPointer("hello %d %d\n"), 5, 10);

            Console.Write(@"\ntype a string here: ");
            // use CString to make a string buffer the C code can put stuff in
            CString bld = new CString(100);
            ar.mygets(bld.ToPointer(), 101);
            // now convert the string buffer to a string and print it
            Console.WriteLine("{0}", bld.ToString());

            // Dump names table from bzip2.odx
            DumpNames(@"bzip2.odx");
            // Keep the console window open in debug mode.
            Console.WriteLine("Press any key to exit.");
            Console.ReadKey();
        }
    }
}