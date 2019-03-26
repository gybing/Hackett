#pragma once

template <typename WFUNC = std::function<void(int)>, typename PFUNC = std::function<void(int)>>
class ThreadPool final
{
public:
	ThreadPool(int waitcnt, int idlecnt, int maxcnt, WFUNC& waitfunc, PFUNC& procfunc)
		: _waitCount(waitcnt)
		, _idleCount(idlecnt)
		, _maxCount(maxcnt)
		, _waitFunc(waitfunc)
		, _procFunc(procfunc)
		, _stop(false)
	{
		m_hevent = CreateSemaphore(NULL, waitcnt, idlecnt, NULL);

		for (int i=0; i<idlecnt; ++i)
		{
			_threads.emplace_back(std::thread(std::bind(&ThreadPool::run, this, i)));
		}
// 		for (int i=0; i<waitcnt; ++i)
// 		{
// 			_cv.notify_one();
// 		}
	}

	~ThreadPool()
	{
		_stop = true;
		
		for (auto& t : _threads)
		{
			t.join();
		}
	}

private:
	void run(int id)
	{
		while (!_stop)
		{
			{
				// �ȴ���ΪLeader
				//_cv.wait(std::unique_lock<std::mutex>(_mutex));
				// std::lock_guard<std::mutex> lock(_mutex);
				// �ȴ�����

				WaitForSingleObject(m_hevent, INFINITE);

				_waitFunc(id);

				// ����Leader

				ReleaseSemaphore(m_hevent, 1, NULL);
			}

			// ����
			_procFunc(id);
		}
	}

private:
	HANDLE m_hevent;
	std::deque<std::thread> _threads;
	std::mutex _mutex;
	std::condition_variable _cv;
	int _waitCount;
	int _idleCount;
	int _maxCount;
	WFUNC _waitFunc;
	PFUNC _procFunc;
	std::atomic<bool> _stop;

	HDECLARE_NON_COPYABLE (ThreadPool)
};