#pragma once
#include <vector>
#include <winsock2.h>
#include <MSWSock.h>
#pragma comment(lib,"ws2_32.lib")
#include "DC_WinSock.h"
#include "DC_IOCP.h"
#include "DC_Any.h"
#include <mutex>
#include <atomic>
using namespace DC::IOCP;

class Funique_id {
public:
	Funique_id() :uniqueid(DC::randomer(0, 21476836476)) {
		if (uniqueid < 0) {
			std::cout << "fuck";
		}
	}

	Funique_id(const int32_t input) :uniqueid(input) {}

	Funique_id(const Funique_id&) = default;

public:
	bool operator==(const Funique_id& input)const {
		if (this == nullptr) return false;
		if (uniqueid == input.uniqueid) return true;
		return false;
	}

public:
	inline int32_t getUniqueID()const {
		return uniqueid;
	}

private:
	const int32_t uniqueid;
};

template <typename T>
class Pool {
public:
	Pool() :removeTime(0), cleanLimit(1) {}

public:
	inline void setCleanLimit(const int32_t& input) {
		cleanLimit.store(input, std::memory_order_release);
	}

	template <typename ...U>
	inline T* make(U&& ...args)noexcept {
		Cleanif();
		std::unique_lock<std::mutex> LwriteLock(writeLock);
		try {
			m.emplace_back(new T(std::forward<U>(args)...));
		}
		catch (...) {
			return nullptr;
		}
		return m.rbegin()->get();
	}

	template <typename ...U>
	inline T* put(U&& ...args)noexcept {//only unique_ptr
		Cleanif();
		std::unique_lock<std::mutex> LwriteLock(writeLock);
		try {
			m.emplace_back(std::forward<U>(args)...);
		}
		catch (...) {
			return nullptr;
		}
		return m.rbegin()->get();
	}

	T* release(const T *input) {
		std::unique_lock<std::mutex> LwriteLock(writeLock);
		for (auto i = m.begin(); i != m.end(); i++) {
			if (*input == *(i->get())) {
				removeTime++;
				return i->release();
			}
		}
		return nullptr;
	}

	bool drop(const T *input) {
		std::unique_lock<std::mutex> LwriteLock(writeLock);
		auto fres = std::find_if(m.begin(), m.end(), std::bind(&Pool::compare, this, std::placeholders::_1, input));

		if (fres == m.end()) 
			return false;

		removeTime++;
		fres->get_deleter()(fres->release());
		return true;
	}

	void clean() {
		std::unique_lock<std::mutex> LwriteLock(writeLock);
		realClean();
		removeTime.store(0, std::memory_order_release);
	}

	inline void clear() {
		std::unique_lock<std::mutex> LwriteLock(writeLock);
		m.clear();
		removeTime.store(0, std::memory_order_release);
	}

private:
	void realClean() {
		for (auto i = m.begin(); i != m.end();) {
			if (!(*i)) { i = m.erase(i); continue; }
			i++;
		}
	}

	inline void Cleanif() {
		if (removeTime.load(std::memory_order_acquire) >= cleanLimit.load(std::memory_order_acquire)) {
			clean();
		}
	}

	inline bool compare(const std::unique_ptr<T>& target, const T* const value) {
		if (target.get() == value) return true;
		return false;
	}

public:
	std::vector<std::unique_ptr<T>> m;
	std::mutex writeLock;
	std::atomic_int32_t removeTime, cleanLimit;
};

void test(Pool<Funique_id>& pool) {	
	std::vector<Funique_id*> ptrs;

	for (int i = 0; i < 1000; i++)
		ptrs.push_back(pool.make());
	for (const auto& p : ptrs) {
		if(p->getUniqueID()<0)
			std::cout << "shit\n";
	}

	for (int i = 0; i < 1000; i++) {
		if (!pool.drop(*ptrs.begin())) {
			std::cout << "drop err\n";
		}
		ptrs.erase(ptrs.begin());
	}
}

int main() {
	Pool<Funique_id> pool;

	std::thread t[10];
	for (int i = 0; i < 10; i++)
		t[i] = std::thread(test, std::ref(pool));
	for (int i = 0; i < 10; i++)
		t[i].join();
	for (const auto& p : pool.m) {
		if(p.get()!=nullptr)
			std::cout << "shit\n";
	}
	pool.clean();
}