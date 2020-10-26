#include <cstdio>
int main(int argc, char** argv)
{
        printf("Arguments passed: %d\n", argc);
        printf("Arguments: ");
        for (int i = 0; i < argc; i++)
        {
            printf("%s, ", argv[i]);
        }
        printf("\n");
}