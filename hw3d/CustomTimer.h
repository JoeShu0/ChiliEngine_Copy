#pragma once
#include <chrono>

class CustomTimer
{
public:
	CustomTimer();
	float Mark();
	float Peek() const;
private:
	std::chrono::steady_clock::time_point last;
};