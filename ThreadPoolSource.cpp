#include <iostream>

#include <vector>

#include <thread>
#include <chrono>
#include <Windows.h>
#include <mutex>
using namespace std::chrono;

#define LENGTH 50000000
#define THREADSIZE 32

std::mutex m;
int sum;

HANDLE events[(THREADSIZE) + 1];
HANDLE main_event;

void m_work_thread()
{
	int ret;
	int local = 0;

	while (true)
	{				
		ret = WaitForMultipleObjects((THREADSIZE) + 1, events, FALSE, INFINITE);
		if (0 == ret) break;
		local = 0;
		
		for (int i = 1; i <= LENGTH / ret; ++i)		
			local++;	

		m.lock();
		sum = local + sum;
		m.unlock();

		if (sum == LENGTH)
			SetEvent(main_event);
	}
	SetEvent(main_event);
}

int main()
{
 	main_event = CreateEvent(NULL, FALSE, FALSE, NULL);
	for (int i = 0; i < (THREADSIZE) + 1; i++)
		events[i] = CreateEvent(NULL, FALSE, FALSE, NULL);

	std::vector<std::thread> threads;
	for (int i = 0; i < THREADSIZE; i++)
		threads.push_back(std::thread{ m_work_thread });

	
	// 2 의 제곱값으로 반복
	for (int i = 1; i <= THREADSIZE; i *= 2)
	{
		// 값 초기화
		sum = 0;
		// 작업 시작 시간
		auto start = high_resolution_clock::now();

		// 대기중인 스레드 활성화
		for (int j = 0; j < i; j++)
			SetEvent(events[i]);

		WaitForSingleObject(main_event, INFINITE);

		std::cout << i << " Thread" << " Sum : ";
		std::cout << sum;

		// 작업 종료 시간
		auto end = high_resolution_clock::now() - start;
		// 총 작업 시간 출력
		std::cout << std::endl << duration_cast<milliseconds>(end).count() << " milliseconds" << std::endl;
		std::cout << std::endl;
	}
	
	for (int i = 0; i < THREADSIZE; i++)
		SetEvent(events[0]);

	for (int i = 0; i < THREADSIZE; i++)
		threads[i].join();
  	threads.clear();
  	return 0;
}
