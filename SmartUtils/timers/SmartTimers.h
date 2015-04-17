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

#define MAX_TIMERS (1024)

namespace ns_utils
{

enum EClockType
{
	ECT_NONE = 0, ECT_REALTIME = 1, ECT_MONOTONIC = 2
};

enum EErrCode
{
	EEC_SUC = 0, EEC_ERR = -1
};

class CSmartTimers;

class CBaseTimer
{
	friend CSmartTimers;

protected:
	CBaseTimer() :
			m_registered(false), m_fd(-1), m_timer_type(ECT_NONE), m_init_expire_seconds(-1), m_init_expire_nanos(-1), m_interval_seconds(-1), m_interval_nanos(
					-1)
	{
		//int32_t r = create();
		//ST_ASSERT(EEC_SUC == r);
	}

	virtual ~CBaseTimer()
	{
	}

	int32_t create();

	virtual void handle_timer_evt(uint64_t ui64Times) = 0;

public:
	inline void set_timer_type(int32_t timer_type)
	{
		m_timer_type = timer_type;
	}
	inline void set_init_expire_time(int64_t init_expire_seconds, int64_t init_expire_nanos)
	{
		m_init_expire_seconds = init_expire_seconds;
		m_init_expire_nanos = init_expire_nanos;
	}
	inline void set_interval_time(int64_t interval_seconds, int64_t interval_nanos)
	{
		m_interval_seconds = interval_seconds;
		m_interval_nanos = interval_nanos;
	}

	inline uint32_t get_max_timers()
	{
		return MAX_TIMERS;
	}

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
	typedef std::shared_ptr<std::thread> ThreadPtr_t;
	ThreadPtr_t m_pthread;
	std::mutex m_state_lk, m_timers_lk;
	int32_t m_epollfd;

};

} /* namespace ns_utils */

#endif /* SMARTTIMERS_H_ */
