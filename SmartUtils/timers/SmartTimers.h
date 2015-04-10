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

namespace ns_utils
{

enum EClockType
{
	ECT_REALTIME = 0, ECT_MONOTONIC = 1
};

enum EErrCode
{
	EEC_SUC = 0, EEC_ERR = -1
};

class CBaseTimer
{
protected:
	CBaseTimer(int32_t timer_type, int64_t init_expire_seconds,
			int64_t init_expire_nanos, int64_t interval_seconds,
			int64_t interval_nanos) :
			m_fd(-1), m_timer_type(timer_type), m_init_expire_seconds(
					init_expire_seconds), m_init_expire_nanos(
					init_expire_nanos), m_interval_seconds(interval_seconds), m_interval_nanos(
					interval_nanos)
	{
	}
	virtual ~CBaseTimer()
	{
	}

	int32_t create();

public:
	virtual void handle_interval_evt(uint64_t ui64Times) = 0;

private:
	int32_t m_fd;
	int32_t m_timer_type;
	int64_t m_init_expire_seconds;
	int64_t m_init_expire_nanos;
	int64_t m_interval_seconds;
	int64_t m_interval_nanos;

};
typedef std::shared_ptr<ns_utils::CBaseTimer> TimerHandlerPtr_t;

class CSmartTimers
{

public:
	CSmartTimers();
	virtual ~CSmartTimers();

public:

	int32_t start();
	int32_t stop();

	int32_t add_timer(TimerHandlerPtr_t &pTimerHandler);
	int32_t remove_timer(TimerHandlerPtr_t &pTimerHandler);

	void handle_timers();

private:

	volatile bool m_bStopFlag;
	typedef std::set<TimerHandlerPtr_t> TimerHanderSet_t;
	TimerHanderSet_t m_setTimers;
	typedef std::shared_ptr<std::thread> ThreadPtr_t;
	ThreadPtr_t m_pThread;

};

} /* namespace ns_utils */

#endif /* SMARTTIMERS_H_ */
