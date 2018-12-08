#include <string>
int main(int argc, char **argv)
{
	std::string aa = "aa";
        std::string bb = "bb";
	printf("%s\n",(argc == 1 ? aa : "hi").c_str());
        printf("%s\n",(argc == 2 ? bb : "bye").c_str());
        printf("%s\n",(argc == 1 ? "try" : aa).c_str());
}