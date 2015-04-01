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

namespace ns_utils {

class CTimerHandler{
protected:
	CTimerHandler(){}
	virtual ~CTimerHandler(){}

public:
	virtual void handle_timer_evt(uint64_t ui64Times) = 0;

};
typedef std::shared_ptr<ns_utils::CTimerHandler> TimerHandlerPtr_t;


class CSmartTimers {

public:
	enum EClockType
	{
		ECT_BASE = 0,
		ECT_REALTIME = ECT_BASE + 1,
		ECT_MONOTONIC = ECT_BASE + 2
	};

public:
	CSmartTimers();
	virtual ~CSmartTimers();

public:
	int32_t start();
	int32_t stop();

	int32_t add_timer();
	int32_t remove_timer();


};

} /* namespace ns_utils */

#endif /* SMARTTIMERS_H_ */
