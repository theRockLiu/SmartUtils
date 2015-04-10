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

class CMyTimerHandler: public ns_utils::CTimer
{
public:
	void handle_timer_evt(uint64_t ui64Times)
	{
		cout << "times: " << ui64Times << endl;
	}

};

int main()
{
	auto ptr = std::make_shared < CMyTimerHandler > (ns_utils::CSmartTimers::ECT_REALTIME, 10, 5);

	ns_utils::CSmartTimers tms;
	tms.start();
	tms.add_timer(ptr);

    std::this_thread::sleep_for(100s);

    tms.stop();


	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!
	return 0;
}
