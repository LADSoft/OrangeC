//#include <stdio.h>

inline void aa();
inline void bb();
extern inline void cc();
void dd();
extern inline void ee();

inline void aa()
{
	printf("hi\n");
}
extern inline void bb()
{
	printf("hi\n");
}
inline void cc()
{
	printf("hi\n");
}
extern inline void dd()
{
	printf("hi\n");
}
void ee()
{
	printf("hi\n");
}