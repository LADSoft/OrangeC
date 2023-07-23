main(int argc, char **argv)
{
	printf("hi\n");
	int *aa = (int *)malloc(44);
	aa[44/4] = 0;
	free(aa);
  }
}