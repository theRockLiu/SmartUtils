/*
 * CSmartTimers.cpp
 *
 *  Created on: Apr 1, 2015
 *      Author: rock
 */

#include "SmartTimers.h"
#include <utility>
#include <sys/timerfd.h>
#include <cassert>
#include <sys/epoll.h>

namespace ns_utils
{

const int64_t NANOS_OF_ONE_SECONDS = (1000 * 1000 * 1000);

int32_t CBaseTimer::create()
{
	ST_ASSERT(
			m_init_expire_nanos < NANOS_OF_ONE_SECONDS
					&& m_interval_nanos < NANOS_OF_ONE_SECONDS);

	if (ECT_REALTIME != m_timer_type && ECT_MONOTONIC != m_timer_type)
	{
		return EEC_ERR;
	}

	int32_t timer_type =
			ECT_REALTIME == m_timer_type ? CLOCK_REALTIME : CLOCK_MONOTONIC;

	struct timespec now =
	{ 0 };
	if (clock_gettime(timer_type, &now) == -1)
	{
		return EEC_ERR;
	}

	struct itimerspec new_value =
	{ 0 };
	new_value.it_value.tv_sec = now.tv_sec + m_init_expire_seconds
			+ (now.tv_nsec + m_init_expire_nanos) / NANOS_OF_ONE_SECONDS;
	new_value.it_value.tv_nsec = (now.tv_nsec + m_init_expire_nanos)
			% NANOS_OF_ONE_SECONDS;
	new_value.it_interval.tv_sec = m_interval_seconds
			+ m_interval_nanos / NANOS_OF_ONE_SECONDS;
	new_value.it_interval.tv_nsec = m_interval_nanos % NANOS_OF_ONE_SECONDS;

	m_fd = timerfd_create(timer_type, 0);
	if (m_fd == -1)
	{
		return EEC_ERR;
	}

	if (timerfd_settime(m_fd, TFD_TIMER_ABSTIME, &new_value, NULL) == -1)
	{
		return EEC_ERR;
	}

	return EEC_SUC;
}

CSmartTimers::CSmartTimers() :
		m_flag(false)
{
	// TODO Auto-generated constructor stub

}

CSmartTimers::~CSmartTimers()
{
	// TODO Auto-generated destructor stub
}

int32_t CSmartTimers::start()
{
	std::lock_guard < std::mutex > lock(m_state_lk);

	if (m_pThread != nullptr)
	{
		ST_ASSERT(false);
		return EEC_ERR;
	}

	struct epoll_event ev, events[MAX_TIMERS];
	int listen_sock, conn_sock, nfds, epollfd;

	/*
	 * In the initial epoll_create() implementation, the size argument informed the kernel of the number  of  file  descriptors  that  the  caller
	 expected  to add to the epoll instance.  The kernel used this information as a hint for the amount of space to initially allocate in inter‐
	 nal data structures describing events.  (If necessary, the kernel would allocate more space if the caller's usage exceeded the  hint  given
	 in  size.)  Nowadays, this hint is no longer required (the kernel dynamically sizes the required data structures without needing the hint),
	 but size must still be greater than zero, in order to ensure backward compatibility when new epoll applications are run on older kernels.
	 * */
	int32_t epollfd = epoll_create(MAX_TIMERS);
	if (epollfd == -1)
	{
		ST_ASSERT()

		perror("epoll_create");
		exit (EXIT_FAILURE);
	}

	m_pThread = std::make_shared < std::thread > ([this]
	{	handle_timers();});

	return EEC_SUC;

}

int32_t CSmartTimers::stop()
{
	std::lock_guard < std::mutex > lock(m_state_lk);

	if (m_pThread == nullptr)
	{
		ST_ASSERT(false);
		return EEC_ERR;
	}

	return EEC_SUC;
}

int32_t CSmartTimers::remove_timer(timer_ptr_t &pTimerHandler)
{
	return EEC_SUC;
}

void CSmartTimers::handle_timers()
{

	ev.events = EPOLLIN;
	ev.data.fd = listen_sock;
	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listen_sock, &ev) == -1)
	{
		perror("epoll_ctl: listen_sock");
		exit (EXIT_FAILURE);
	}

	for (;;)
	{
		nfds = epoll_wait(epollfd, events, MAX_TIMERS, -1);
		if (nfds == -1)
		{
			perror("epoll_pwait");
			exit (EXIT_FAILURE);
		}

		for (n = 0; n < nfds; ++n)
		{
			if (events[n].data.fd == listen_sock)
			{
				conn_sock = accept(listen_sock, (struct sockaddr *) &local,
						&addrlen);
				if (conn_sock == -1)
				{
					perror("accept");
					exit (EXIT_FAILURE);
				}
				setnonblocking (conn_sock);
				ev.events = EPOLLIN | EPOLLET;
				ev.data.fd = conn_sock;
				if (epoll_ctl(epollfd, EPOLL_CTL_ADD, conn_sock, &ev) == -1)
				{
					perror("epoll_ctl: conn_sock");
					exit (EXIT_FAILURE);
				}
			}
			else
			{
				do_use_fd(events[n].data.fd);
			}
		}
	}

}

int32_t CSmartTimers::add_timer(timer_ptr_t& pTimerHandler)
{
	return EEC_SUC;
}

} /* namespace ns_utils */

