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
#include <unistd.h>
#include <iostream>

namespace ns_utils
{

const int64_t NANOS_OF_ONE_SECONDS = (1000 * 1000 * 1000);

int32_t CBaseTimer::create()
{
	ST_ASSERT(m_init_expire_nanos < NANOS_OF_ONE_SECONDS && m_interval_nanos < NANOS_OF_ONE_SECONDS);

	if (ECT_REALTIME != m_timer_type && ECT_MONOTONIC != m_timer_type)
	{
		return EEC_ERR;
	}

	int32_t timer_type = ECT_REALTIME == m_timer_type ? CLOCK_REALTIME : CLOCK_MONOTONIC;

	struct timespec now =
	{ 0 };
	if (clock_gettime(timer_type, &now) == -1)
	{
		return EEC_ERR;
	}

	struct itimerspec new_value =
	{ 0 };
	new_value.it_value.tv_sec = now.tv_sec + m_init_expire_seconds + (now.tv_nsec + m_init_expire_nanos) / NANOS_OF_ONE_SECONDS;
	new_value.it_value.tv_nsec = (now.tv_nsec + m_init_expire_nanos) % NANOS_OF_ONE_SECONDS;
	new_value.it_interval.tv_sec = m_interval_seconds + m_interval_nanos / NANOS_OF_ONE_SECONDS;
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
		m_stop_flag(false), m_pthread(nullptr)
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

	if (m_pthread != nullptr)
	{
		ST_ASSERT(false);
		return EEC_ERR;
	}

	/*
	 * In the initial epoll_create() implementation, the size argument informed the kernel of the number  of  file  descriptors  that  the  caller
	 expected  to add to the epoll instance.  The kernel used this information as a hint for the amount of space to initially allocate in inter‐
	 nal data structures describing events.  (If necessary, the kernel would allocate more space if the caller's usage exceeded the  hint  given
	 in  size.)  Nowadays, this hint is no longer required (the kernel dynamically sizes the required data structures without needing the hint),
	 but size must still be greater than zero, in order to ensure backward compatibility when new epoll applications are run on older kernels.
	 * */
	m_epollfd = epoll_create(MAX_TIMERS);
	if (m_epollfd == -1)
	{
		ST_ASSERT(false);
		return EEC_ERR;
	}

	m_pthread = std::make_shared < std::thread > ([this]
	{	handle_timers();});

	return EEC_SUC;
}

int32_t CSmartTimers::stop()
{
	std::lock_guard < std::mutex > lock(m_state_lk);

	if (m_pthread == nullptr)
	{
		ST_ASSERT(false);
		return EEC_ERR;
	}
	m_stop_flag = true;
	m_pthread->join();

	return EEC_SUC;
}

int32_t CSmartTimers::add_timer(timer_ptr_t &ptimer)
{
	std::lock_guard < std::mutex > lock(m_timers_lk);

	std::pair<timers_set_t::iterator, bool> ret = m_timers.insert(ptimer);
	if (!ret.second)
	{
		return EEC_ERR;
	}

	if (ptimer->create() == EEC_ERR)
	{
		m_timers.erase(ret.first);
		return EEC_ERR;
	}

	return EEC_SUC;
}

void CSmartTimers::handle_timers()
{
	for (;;)
	{
		if (m_stop_flag)
		{
			break;
		}

		{
			std::lock_guard < std::mutex > lock(m_timers_lk); ///TheRock_Lhy: not suggest to register timer at runtime..
			///modify epoll set
			for (timers_set_t::iterator itor = m_timers.begin(); itor != m_timers.end();)
			{
				std::cout<<"count: "<<(*itor).use_count()<<std::endl;

				if ((*itor).unique())
				{
					std::cout<<"count1: "<<(*itor).use_count()<<std::endl;
					//TheRock_Lhy: i confirm not to assign it to others:).
					///unregister it now.
					if (epoll_ctl(m_epollfd, EPOLL_CTL_DEL, (*itor)->get_fd(), NULL) == -1)
					{
						ST_ASSERT(false);
					}

					m_timers.erase(++itor);

				}
				else if (!(*itor)->is_registered())
				{
					std::cout<<"count1: "<<(*itor).use_count()<<std::endl;
					///register it
					struct epoll_event ev =
					{ 0 };
					ev.events = EPOLLIN;
					ev.data.ptr = (*itor).get();
					if (epoll_ctl(m_epollfd, EPOLL_CTL_ADD, (*itor)->get_fd(), &ev) == -1)
					{
						ST_ASSERT(false);
					}

					(*itor)->registered();
					itor++;
				}
				else
				{
					itor++;
				}
			}
		}

		struct epoll_event events[MAX_TIMERS] =
		{ 0 };
		int32_t nfds = 0, n = 0;
		const int32_t timeout = 1;
		uint64_t times = 0;
		ssize_t s = 0;
		for (;;)
		{
			nfds = epoll_wait(m_epollfd, events, MAX_TIMERS, timeout);
			if (nfds == -1)
			{
				//ST_ASSERT(false);
				break;
			}

			for (n = 0; n < nfds; ++n)
			{
				CBaseTimer *ptimer = static_cast<CBaseTimer*>(events[n].data.ptr);
				s = read(ptimer->get_fd(), &times, sizeof(uint64_t));
				if (s != sizeof(uint64_t))
				{
					ST_ASSERT(false);
					return;
				}
				ptimer->handle_timer_evt(times);
			}
		}
	}

}

} /* namespace ns_utils */

