#pragma once

#include <thread>
#include <mutex>
#include <shared_mutex>
#include <iostream>
#include <atomic>
#include <string>

class ThreadKoeff {
public:
	ThreadKoeff(size_t start, size_t end) : bStopFlag{ false }, m_start{ start }, m_end{ end } {
		m_half = (end - start) / 2;  
		m_koef = start; 
	};

	void ThreadStart() {
		this_id = std::this_thread::get_id();
		PrintInfo(std::move(" --- Started Koef\n"));
		std::unique_lock<std::mutex> lock(m);
		while (!bStopFlag) {
			cond_var.wait(lock);
		}
	}

	void ChangeKoef(double koef) {
		m_koef = koef;
	}

	double Click(double radians) {
		m_koef = (m_start + m_half) + m_half * sin(radians);
		return m_koef;
	}


	double GetKoef() {
		return m_koef;
	}

	void Stop() {
		std::unique_lock<std::mutex> lock(m);
		bStopFlag = true;
		cond_var.notify_one();
	}

	void PrintInfo(std::string && s) {
		std::lock_guard<std::mutex> lock(m);
		std::cout << s << std::endl;
	}

protected:
	double m_half, m_koef;
	size_t m_start, m_end;

	std::atomic<bool> bStopFlag;
	std::thread::id this_id;
	std::mutex m;
	std::condition_variable cond_var;

};
