#include "pch.h"

#pragma comment(lib, "Onecore.lib")

void test_FileMapping3(const wchar_t* filename) {
	auto f = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, kCreateFileFlag, nullptr);
	if (f == INVALID_HANDLE_VALUE) {
		throw Error(LOC);
	}

	auto fm = CreateFileMapping(f, nullptr, PAGE_READONLY, 0, 0, nullptr);
	if (fm == nullptr) {
		throw Error(LOC);
	}

	int64_t filesize = myGetFileSize(f);

	MyMD5 md5;

	uint64_t readTime = 0;
	uint64_t md5Time = 0;

	int64_t n = filesize;
	int64_t pos = 0;

	auto proc = GetCurrentProcess();

	auto* data = reinterpret_cast<const uint8_t*>(MapViewOfFile2(fm, proc, 0, nullptr, 0, 0, PAGE_READONLY));
	if (!data) {
		auto err = GetLastError();
		throw Error(LOC);
	}

	while (n > 0) {
		auto byteToRead = static_cast<DWORD>(n > kChunkSize ? kChunkSize : n);
		//------------
		StopWatch sw2;
		md5.update(data + pos, byteToRead);
		md5Time += sw2.getTime();

		n -= byteToRead;
		pos += byteToRead;
	}

	UnmapViewOfFile(data);
	CloseHandle(fm);
	CloseHandle(f);

	md5.printResult();
	printTimeResult(readTime, md5Time);
}