/*
 * CSmartTimers.cpp
 *
 *  Created on: Apr 1, 2015
 *      Author: rock
 */

#include "SmartTimers.h"
#include <utility>
#include <sys/timerfd.h>

namespace ns_utils
{

int32_t CBaseTimer::create()
{

	if (ECT_REALTIME != m_timer_type && ECT_MONOTONIC != m_timer_type)
	{
		return EEC_ERR;
	}

	m_fd = timerfd_create(ECT_REALTIME == m_timer_type ? CLOCK_REALTIME : CLOCK_MONOTONIC, 0);
	if (-1 == m_fd)
	{
		//print errno.
		return EEC_ERR;
	}

	return EEC_SUC;
}

CSmartTimers::CSmartTimers()
{
	// TODO Auto-generated constructor stub

}

CSmartTimers::~CSmartTimers()
{
	// TODO Auto-generated destructor stub
}

int32_t CSmartTimers::start()
{

	if (m_pThread != nullptr)
	{
		return EEC_ERR;
	}

	m_pThread = std::make_shared < std::thread > ([this]
	{	handle_timers();});

	return EEC_SUC;

}

int32_t CSmartTimers::stop()
{

	return EEC_SUC;
}

int32_t CSmartTimers::remove_timer(timer_ptr_t &pTimerHandler)
{
	return EEC_SUC;
}



void CSmartTimers::handle_timers()
{

#if 0
	struct itimerspec new_value;
	int max_exp, fd;
	struct timespec now;
	uint64_t exp, tot_exp;
	ssize_t s;

	if ((argc != 2) && (argc != 4))
	{
		fprintf(stderr, "%s init-secs [interval-secs max-exp]\n",
				argv[0]);
		exit(EXIT_FAILURE);
	}

	if (clock_gettime(CLOCK_REALTIME, &now) == -1)
	handle_error("clock_gettime");

	/* Create a CLOCK_REALTIME absolute timer with initial
	 expiration and interval as specified in command line */

	new_value.it_value.tv_sec = now.tv_sec + atoi(argv[1]);
	new_value.it_value.tv_nsec = now.tv_nsec;
	if (argc == 2)
	{
		new_value.it_interval.tv_sec = 0;
		max_exp = 1;
	}
	else
	{
		new_value.it_interval.tv_sec = atoi(argv[2]);
		max_exp = atoi(argv[3]);
	}
	new_value.it_interval.tv_nsec = 0;

	fd = timerfd_create(CLOCK_REALTIME, 0);
	if (fd == -1)
	handle_error("timerfd_create");

	if (timerfd_settime(fd, TFD_TIMER_ABSTIME, &new_value, NULL) == -1)
	handle_error("timerfd_settime");

	print_elapsed_time();
	printf("timer started\n");

	for (tot_exp = 0; tot_exp < max_exp;)
	{
		s = read(fd, &exp, sizeof(uint64_t));
		if (s != sizeof(uint64_t))
		handle_error("read");

		tot_exp += exp;
		print_elapsed_time();
		printf("read: %llu; total=%llu\n",
				(unsigned long long) exp,
				(unsigned long long) tot_exp);
	}

	exit(EXIT_SUCCESS);
}
#endif

}

int32_t CSmartTimers::add_timer(timer_ptr_t& pTimerHandler)
{
	return EEC_SUC;
}

} /* namespace ns_utils */


