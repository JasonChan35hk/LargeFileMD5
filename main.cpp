#include "pch.h"

#define DO_TEST(func) \
	void func(const wchar_t* filename); \
	do { \
		StopWatch sw; \
		std::cout << "---- " << #func << " ----\n"; \
		func(filename); \
		printf("total time: %6lldms\n\n", sw.getTime()); \
	} while(false); \
//------

int main() {
	const wchar_t* filename = L"C:/Users/Jason/Downloads/CentOS-7-x86_64-Everything-1611.iso";
	//const wchar_t* filename = L"C:/Users/Jason/Downloads/ubuntu-16.04.2-desktop-amd64.iso";	

	DO_TEST(test_FileMapping3);
	DO_TEST(test_FileMapping2);
	DO_TEST(test_FileMapping);
	DO_TEST(test_ReadFile);
	DO_TEST(test_ReadFile2);

	printf("=== End ===\nPlease any key to exit\n");
	_getch();
	return 0;
}