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
	CMyTimerHandler(const ns_utils::ETimerType timer_type, int64_t interval_seconds, int64_t interval_nanos) :
			CBaseTimer(timer_type, interval_seconds, interval_nanos)
	{
	}

public:
	void handle_timer_evt(uint64_t ui64Times)
	{
		cout << "interval: " << get_interval_seconds() <<", times: " << ui64Times << endl;
	}

};

int main()
{
	ns_utils::timer_ptr_t ptr = std::make_shared < CMyTimerHandler > (ns_utils::ETimerType::ETT_REALTIME, 1, 1);

	ns_utils::CSmartTimers tms;
	tms.start();

	//cout << "sss: " << ptr.use_count() << endl; // prints !!!Hello World!!!
	tms.register_timer(ptr);
	//cout << "sss: " << ptr.use_count() << endl; // prints !!!Hello World!!!

	std::this_thread::sleep_for(10s);

	ptr = nullptr;

	cout << "!!!stop!!!" << endl; // prints !!!Hello World!!!

	std::this_thread::sleep_for(10s);

	ptr = std::make_shared < CMyTimerHandler > (ns_utils::ETimerType::ETT_MONOTONIC, 2, 1);
	tms.register_timer(ptr);

	ns_utils::timer_ptr_t ptr1 = std::make_shared < CMyTimerHandler > (ns_utils::ETimerType::ETT_REALTIME, 3, 1);
	tms.register_timer(ptr1);

	ns_utils::timer_ptr_t ptr2 = std::make_shared < CMyTimerHandler > (ns_utils::ETimerType::ETT_MONOTONIC, 5, 1);
	tms.register_timer(ptr2);

	std::this_thread::sleep_for(20s);

	ptr1 = nullptr;

	std::this_thread::sleep_for(20s);

	ptr2 = nullptr;

	std::this_thread::sleep_for(20s);

	tms.stop();

	cout << "!!!test over!!!" << endl; // prints !!!Hello World!!!
	return 0;
}
