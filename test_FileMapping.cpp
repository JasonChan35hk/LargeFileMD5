#include "pch.h"

void test_FileMapping(const wchar_t* filename) {
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

	int64_t remain = filesize;
	int64_t pos = 0;

	auto proc = GetCurrentProcess();

	while (remain > 0) {
		auto byteToRead = static_cast<DWORD>(remain > kChunkSize ? kChunkSize : remain);

		//------------
		StopWatch sw;
		auto* data = MapViewOfFileEx(fm, FILE_MAP_READ, static_cast<DWORD>(pos >> 32), static_cast<DWORD>(pos), byteToRead, nullptr);
		if (data == nullptr) {
			throw Error(LOC);
		}

		//WIN32_MEMORY_RANGE_ENTRY memRange;
		//memRange.VirtualAddress = data;
		//memRange.NumberOfBytes = byteToRead;

		//! doesn't help, 300ms slower
		//PrefetchVirtualMemory(proc, 1, &memRange, 0);

		readTime += sw.getTime();

		//------------
		StopWatch sw2;
		md5.update(reinterpret_cast<const uint8_t*>(data), byteToRead);
		md5Time += sw2.getTime();

		UnmapViewOfFile(data);

		remain -= byteToRead;
		pos += byteToRead;
	}

	CloseHandle(fm);
	CloseHandle(f);

	md5.printResult();
	printTimeResult(readTime, md5Time);
}