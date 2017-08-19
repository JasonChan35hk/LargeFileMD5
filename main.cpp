#include "pch.h"
#include <Shellapi.h>

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
	//const wchar_t* filename = L"C:/Users/Jason/Downloads/CentOS-7-x86_64-Everything-1611.iso";
	//const wchar_t* filename = L"C:/Users/Jason/Downloads/ubuntu-16.04.2-desktop-amd64.iso";	
	//const wchar_t* filename = L"C:/Users/Jason/Downloads/FreeBSD-11.1-RELEASE-amd64-disc1.iso.xz";	

	int argc;
	auto** argv = CommandLineToArgvW(GetCommandLine(), &argc);

	if (argc < 2) {
		printf("LargeFileMD5 <filename>\n");
		throw Error(LOC);
	}

	auto* filename = argv[1];

	wprintf(L"MD5 of file: %s\n", filename);

	DO_TEST(test_FileMapping3);
	DO_TEST(test_FileMapping2);
	DO_TEST(test_FileMapping);
	DO_TEST(test_ReadFile);
	DO_TEST(test_ReadFile2);
	DO_TEST(test_ReadFile4);

	printf("=== End ===\nPlease any key to exit\n");
	_getch();
	return 0;
}