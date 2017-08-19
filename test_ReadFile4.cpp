#include "pch.h"

class TestReadFile4 {
public:

	void threadProc() {
		std::vector<uint8_t> buf;
		buf.reserve(kChunkSize);

		for(;;) {
			DWORD byteToRead = 0;
			{
				std::unique_lock<std::mutex> readLock(readMutex);
				if (remain == 0)
					return;

				byteToRead = static_cast<DWORD>(remain > kChunkSize ? kChunkSize : remain);

				{
					StopWatch sw;
					if (0 == ReadFile(file, buf.data(), byteToRead, nullptr, nullptr)) {
						throw Error(LOC);
					}
					readTime += sw.getTime();
				}
				remain -= byteToRead;
			}

			{
				std::unique_lock<std::mutex> readLock(readMutex);
				StopWatch sw;
				md5.update(buf.data(), byteToRead);
				md5Time += sw.getTime();
			}
		}
	}

	void run(const wchar_t* filename) {
		file = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, kCreateFileFlag, nullptr);
		if (file == INVALID_HANDLE_VALUE) {
			throw Error(LOC);
		}

		filesize = myGetFileSize(file);
		remain = filesize;

		std::thread t0(&TestReadFile4::threadProc, this);
		std::thread t1(&TestReadFile4::threadProc, this);

		t0.join();
		t1.join();

		CloseHandle(file);

		md5.printResult();
		printTimeResult(readTime, md5Time);
	}

	std::mutex md5Mutex;
	MyMD5 md5;
	uint64_t md5Time = 0;

	std::mutex readMutex;
	HANDLE file = INVALID_HANDLE_VALUE;
	uint64_t readTime = 0;
	int64_t remain = 0;
	int64_t filesize = 0;
};

void test_ReadFile4(const wchar_t* filename) {
	TestReadFile4 o;
	o.run(filename);
}