#include "pch.h"

class TestReadFile2 {
public:
	class Chunk {
	public:
		Chunk() {
			buf.reserve(kChunkSize);
		}

		std::vector<uint8_t> buf;
		bool fileEnded = false;
	};

	class AtomicQueue {
	public:
		Chunk* pop() {
			std::unique_lock<std::mutex> lock(_mutex);
			while (_q.empty()) {
				_cv.wait(lock);
			}

			auto* c = _q.front();
			_q.pop();
			return c;
		}

		void push(Chunk* p) {
			{
				std::unique_lock<std::mutex> lock(_mutex);
				_q.push(p);
			}
			_cv.notify_all();
		}

	private:
		std::mutex _mutex;
		std::condition_variable _cv;
		std::queue<Chunk*> _q;
	};

	void doRead() {
		auto f = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, kCreateFileFlag, nullptr);
		if (f == INVALID_HANDLE_VALUE) {
			throw Error(LOC);
		}

		int64_t filesize = myGetFileSize(f);
		int64_t remain = filesize;

		while (remain > 0) {
			auto byteToRead = static_cast<DWORD>(remain > kChunkSize ? kChunkSize : remain);

			auto* c = freeQueue.pop();
			c->buf.resize(byteToRead);

			{
				StopWatch sw;
				if (0 == ReadFile(f, c->buf.data(), byteToRead, nullptr, nullptr)) {
					throw Error(LOC);
				}
				readTime += sw.getTime();
			}

			bufQueue.push(c);
			remain -= byteToRead;
		}

		{
			auto* c = freeQueue.pop();
			c->fileEnded = true;
			bufQueue.push(c);
		}

		CloseHandle(f);
	}

	void run(const wchar_t* filename_) {
		filename = filename_;
		Chunk buf[kChunkCount];

		for (int i = 0; i < kChunkCount; i++) {
			freeQueue.push(&buf[i]);
		}

		std::thread readThread(&TestReadFile2::doRead, this);


		for(;;) {
			auto* c = bufQueue.pop();
			if (c->fileEnded) 
				break;

			StopWatch sw;
			md5.update(c->buf.data(), static_cast<uint32_t>(c->buf.size()));
			md5Time += sw.getTime();

			freeQueue.push(c);
		}

		readThread.join();

		md5.printResult();
		printTimeResult(readTime, md5Time);
	}


	MyMD5 md5;
	uint64_t readTime = 0;
	uint64_t md5Time = 0;


	const wchar_t* filename;
	AtomicQueue bufQueue;
	AtomicQueue freeQueue;
};

void test_ReadFile2(const wchar_t* filename) {
	TestReadFile2 o;
	o.run(filename);
}