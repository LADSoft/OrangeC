/*
 * this example looks for cc386.exe on the path and displays the directory
 * it finds it in
 */
int main()
{
  char *result = searchpath("CC386.EXE");
  if (result)
    printf("Path: %s",result);
  else
    printf("cc386.exe not found on the path");
}