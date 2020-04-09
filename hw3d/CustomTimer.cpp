#include "CustomTimer.h"

using namespace std::chrono;

CustomTimer::CustomTimer()
{
	last = steady_clock::now();
}

float CustomTimer::Mark()
{
	const auto old = last;
	last = steady_clock::now();
	const duration<float> frameTime = last - old;
	return frameTime.count();
}

float CustomTimer::Peek() const
{
	return duration<float>(steady_clock::now() - last).count();
}

