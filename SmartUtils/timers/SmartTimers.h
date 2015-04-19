/*
 * CSmartTimers.h
 *
 *  Created on: Apr 1, 2015
 *      Author: rock
 */

#ifndef SMARTTIMERS_H_
#define SMARTTIMERS_H_

#include <cstdint>
#include <memory>
#include <thread>
#include <set>
#include <atomic>
#include <mutex>

#ifdef NDEBUG
#define ST_ASSERT(expr) assert(expr) ///to do ...
#else
#define ST_ASSERT(expr) assert(expr)
#endif

#define DISABLE_COPY(class_name) \
	class_name ( class_name &) = delete; \
	class_name & operator= (class_name & ) = delete;

#define DISABLE_MOVE(class_name) \
	class_name ( class_name && ) = delete; \
	class_name & operator= ( class_name && ) = delete;

#define MAX_TIMERS (1024)

namespace ns_utils
{

enum ETimerType
{
	ETT_REALTIME = 0, ETT_MONOTONIC = 1
};

enum EErrCode
{
	EEC_SUC = 0, EEC_ERR = -1
};

class CSmartTimers;

class CBaseTimer
{
	friend CSmartTimers;

	DISABLE_COPY(CBaseTimer)
	DISABLE_MOVE(CBaseTimer)

protected:
	CBaseTimer(const ETimerType timer_type, int64_t init_expire_seconds, int64_t init_expire_nanos, int64_t interval_seconds, int64_t interval_nanos) :
			m_registered(false), m_fd(-1), m_timer_type(timer_type), m_init_expire_seconds(init_expire_seconds), m_init_expire_nanos(
					init_expire_nanos), m_interval_seconds(interval_seconds), m_interval_nanos(interval_nanos)
	{
	}

	virtual ~CBaseTimer();

	int32_t create();

	bool is_registered()
	{
		return m_registered;
	}
	void registered()
	{
		m_registered = true;
	}

	int32_t get_fd()
	{
		return m_fd;
	}

	virtual void handle_timer_evt(uint64_t ui64Times) = 0;

public:

	static inline uint32_t get_max_timers()
	{
		return MAX_TIMERS;
	}

private:
	bool m_registered;
	int32_t m_fd;
	int32_t m_timer_type;
	int64_t m_init_expire_seconds;
	int64_t m_init_expire_nanos;
	int64_t m_interval_seconds;
	int64_t m_interval_nanos;
};
typedef std::shared_ptr<ns_utils::CBaseTimer> timer_ptr_t;

class CSmartTimers
{
	DISABLE_COPY(CSmartTimers)
	DISABLE_MOVE(CSmartTimers)

public:
	explicit CSmartTimers();
	virtual ~CSmartTimers();

public:

	int32_t start();
	int32_t stop();

	int32_t add_timer(timer_ptr_t &ptimer);
	uint32_t get_max_timers();

	void handle_timers();

private:
	volatile bool m_stop_flag;
	typedef std::set<timer_ptr_t> timers_set_t;
	timers_set_t m_timers;
	typedef std::shared_ptr<std::thread> thread_ptr_t;
	thread_ptr_t m_pthread;
	std::mutex m_state_lk, m_timers_lk;
	int32_t m_epollfd;

};

} /* namespace ns_utils */

#endif /* SMARTTIMERS_H_ */
