#pragma once

#include "hl_md5.h"
#include <iostream>
#include <exception>
#include <vector>
#include <stdint.h>
#include <Windows.h>
#include <conio.h>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

class NonCopyable {
public:
	NonCopyable();
private:
	NonCopyable(const NonCopyable&) = delete;
	void operator=(const NonCopyable&) = delete;
};

class Loc {
public:
	Loc(const char* filename_, int line_) 
		: filename(filename_)
		, line(line_)
	{}

	const char* filename = "";
	int line = 0;
};

#define LOC Loc(__FILE__, __LINE__)

class Error : public std::exception {
public:
	Error(Loc loc) {
		printf("filename %s:%d", loc.filename, loc.line);
	}
};

class StopWatch {
public:
	StopWatch() {
		t = GetTickCount64();
	}

	uint64_t getTime() {
		return GetTickCount64() - t;
	}

	uint64_t t;
};

template<class T> constexpr T& my_const_cast(const T& p) { return const_cast<T&>(p); }
template<class T> constexpr T* my_const_cast(const T* p) { return const_cast<T*>(p); }

class MyMD5 : private MD5 {
public:
	MyMD5() {
		MD5Init(&ctx);
	}

	void update(const uint8_t* data, uint32_t len) {
		MD5Update(&ctx, my_const_cast(data), len);
	}

	void printResult() {
		unsigned char digest[16];
		MD5Final(digest, &ctx);

		std::cout << "MD5: ";
		for (int i=0; i<16; i++)
			printf("%x", digest[i]);
		std::cout << "\n";
	}

private:
	HL_MD5_CTX ctx;
};

const int64_t kChunkSize = 256 * 1024;
const int kChunkCount = 4;

inline
int64_t myGetFileSize(HANDLE f) {
	LARGE_INTEGER tmp;
	if (0 == GetFileSizeEx(f, &tmp))
		throw Error(LOC);

	return tmp.QuadPart;
}

inline
void printTimeResult(uint64_t readTime, uint64_t md5Time) {
	printf("read  time: %6lldms\n", readTime);
	printf("md5   time: %6lldms\n", md5Time);
}

const DWORD kCreateFileFlag = 0; //FILE_FLAG_NO_BUFFERING; // FILE_FLAG_SEQUENTIAL_SCAN;