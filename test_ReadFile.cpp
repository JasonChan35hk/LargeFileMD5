#include "pch.h"

void test_ReadFile(const wchar_t* filename) {
	auto f = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, kCreateFileFlag, nullptr);
	if (f == INVALID_HANDLE_VALUE) {
		throw Error(LOC);
	}

	int64_t filesize = myGetFileSize(f);
	std::vector<uint8_t> buf;

	MyMD5 md5;

	uint64_t readTime = 0;
	uint64_t md5Time = 0;

	int64_t remain = filesize;
	while (remain > 0) {
		auto byteToRead = static_cast<DWORD>(remain > kChunkSize ? kChunkSize : remain);

		buf.resize(byteToRead);
		//------------
		StopWatch sw;
		if (0 == ReadFile(f, buf.data(), byteToRead, nullptr, nullptr)) {
			throw Error(LOC);
		}
		readTime += sw.getTime();

		//------------
		StopWatch sw2;
		md5.update(buf.data(), byteToRead);
		md5Time += sw2.getTime();

		remain -= byteToRead;
	}
	CloseHandle(f);

	md5.printResult();
	printTimeResult(readTime, md5Time);
}
