/*
 * CSmartTimers.cpp
 *
 *  Created on: Apr 1, 2015
 *      Author: rock
 */

#include "SmartTimers.h"

namespace ns_utils {


void thr_fun(const std::set<TimerHandlerPtr_t> &st)
{

}

CSmartTimers::CSmartTimers() {
	// TODO Auto-generated constructor stub

}

CSmartTimers::~CSmartTimers() {
	// TODO Auto-generated destructor stub
}

int32_t CSmartTimers::start() {

	if (m_pThread != nullptr)
	{
		return EET_ERR;
	}

	m_pThread = std::make_shared<std::thread>(thr_fun, m_setTimers);

	return EET_SUC;

}

int32_t CSmartTimers::stop() {

	return EET_SUC;
}

int32_t CSmartTimers::add_timer() {
	return EET_SUC;
}

int32_t CSmartTimers::remove_timer() {
	return EET_SUC;
}

} /* namespace ns_utils */
