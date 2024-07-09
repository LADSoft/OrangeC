#include <stdio.h>
#include <sys/stat.h>
// This tests to see if we can replicate this file we included, Tillman super-battleships away!
char vals[]{
#embed <Tillman.jpg>
};

int main()
{
    FILE* fil = fopen("Tillman_testout.jpg", "w+b");
    fwrite(vals, 1, sizeof(vals), fil);
    fclose(fil);
}