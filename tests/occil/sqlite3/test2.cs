using System;
using System.IO;

using sqlite3;
using lsmsilcrtl;

namespace sqliteil
{
    unsafe class Program
    {
        static string[] Names { get; } = new string[]
        {
            "Bob",
            "Tom",
            "Carlos",
            "Marcos",
            "Alexandre",
            "Alex",
            "Morgana",
            "Maria",
            "Jose",
            "Joao",
            "Marcos",
            "Gustavo",
            "Roberto",
            "Rodrigo",
            "Teste"
        };

        static int Main(string[] args)
        {
            String dbName = "dbtest.db";

            if (File.Exists(dbName))
                File.Delete(dbName);

            sqlite.sqlite3* db;
            // Create the database
            int rc = sqlite.sqlite3_open(CString.ToPointer(dbName), &db);
            if (rc != 0)
            {
                Console.WriteLine("Fail to create the database :(");
                return -1;
            }

            // Create the table
            sqlite.sqlite3_stmt* stmt;
            sqlite3.sqlite.sqlite3_prepare_v2(db, CString.ToPointer
            ("CREATE TABLE demo (name TEXT, age INTEGER);"), -1, &stmt, null);
            rc = sqlite.sqlite3_step(stmt);
            if (rc != 101)
            {
                Console.WriteLine("Fail to create the table :(");
                return -1;
            }
            sqlite.sqlite3_finalize(stmt);

            Random random = new Random(0);
            // Insert some data in table
            foreach (var name in Names)
            {
                var insertLine = String.Format("insert into demo (name, age) values ('{0}', {1});", name, random.Next(1, 99));
                var query = CString.ToPointer(insertLine);
                sqlite.sqlite3_prepare_v2(db, query, insertLine.Length, &stmt, null);
                rc = sqlite.sqlite3_step(stmt);
                if (rc != 101)
                {
                    Console.WriteLine("Fail to insert the name: {0}", name);
                }
                sqlite.sqlite3_finalize(stmt);
            }

            // Read the inserted data...
            var select = "SELECT * FROM demo;";
            rc = sqlite.sqlite3_prepare_v2(db, CString.ToPointer(select), select.Length, &stmt, null);
            if(rc == 0)
            {
                bool done = false;
                while(!done)
                {
                    switch(rc = sqlite.sqlite3_step(stmt))
                    {
                        case 5:
                        case 101:
                            done = true;
                            break;
                        case 100:
                            {
                                string name = 
                                   new CString(sqlite.sqlite3_column_text(stmt, 0)).ToString();
                                int age = sqlite.sqlite3_column_int(stmt, 1);
                                Console.WriteLine("Name: {0} -- Age: {1}", name, age);
                                rc = 0;
                            }
                            break;
                        default:
                            done = true;
                            break;
                    }
                }
            }

            sqlite.sqlite3_close(db);
            return 0;
        }
    }
}