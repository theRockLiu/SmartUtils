//============================================================================
// Name        : SmartUtilsTest.cpp
// Author      : theRockLhy
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
using namespace std;

#include <timers/SmartTimers.h>

class CMyTimerHandler: public ns_utils::CBaseTimer
{
public:
	CMyTimerHandler(const ns_utils::ETimerType timer_type, int64_t init_expire_seconds, int64_t init_expire_nanos, int64_t interval_seconds,
			int64_t interval_nanos) :
			CBaseTimer(timer_type, init_expire_seconds, init_expire_nanos, interval_seconds, interval_nanos)
	{
	}

public:
	void handle_timer_evt(uint64_t ui64Times)
	{
		cout << "times: " << ui64Times << endl;
	}

};

int main()
{
	ns_utils::timer_ptr_t ptr = std::make_shared < CMyTimerHandler > (ns_utils::ETimerType::ETT_REALTIME, 1, 1, 1, 1);

	ns_utils::CSmartTimers tms;
	tms.start();

	cout << "sss: " << ptr.use_count() << endl; // prints !!!Hello World!!!
	tms.add_timer(ptr);
	cout << "sss: " << ptr.use_count() << endl; // prints !!!Hello World!!!

	std::this_thread::sleep_for(10s);

	ptr = nullptr;

	cout << "!!!stop!!!" << endl; // prints !!!Hello World!!!

	std::this_thread::sleep_for(10s);

	ptr = std::make_shared < CMyTimerHandler > (ns_utils::ETimerType::ETT_MONOTONIC, 1, 1, 1, 1);
	tms.add_timer(ptr);

	std::this_thread::sleep_for(10s);

	tms.stop();

	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!
	return 0;
}
