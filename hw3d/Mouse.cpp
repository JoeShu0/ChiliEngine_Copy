#include "Mouse.h"
#include "CustomWin.h"

std::pair<int, int> Mouse::GetPos() const noexcept
{
	return std::pair<int, int>(x, y);
}

int Mouse::GetPosX() const noexcept
{
	return Mouse::x;
}

int Mouse::GetPosY() const noexcept
{
	return Mouse::y;
}
/*
std::pair<int, int> Mouse::GetMouseMoveDeltaDist()
{
	const std::pair<int, int> DeltaPos = std::pair<int, int>(x - oldX, y - oldY);

	oldX = x;
	oldY = y;

	return DeltaPos;
}
*/

std::optional<Mouse::RawDelta> Mouse::ReadRawDelta() noexcept
{
	if (rawDeltaBuffer.empty())
	{
		return std::nullopt;
	}
	const RawDelta d = rawDeltaBuffer.front();
	rawDeltaBuffer.pop();
	return d;
}

bool Mouse::LeftIsPressed() const noexcept
{
	return Mouse::leftIsPressed;
}

bool Mouse::RightIsPressed() const noexcept
{
	return Mouse::rightIsPressed;
}

bool Mouse::WheeelIsPressed() const noexcept
{
	return Mouse::wheelIsPressed;
}

Mouse::Event Mouse::Read() noexcept
{
	if (buffer.size() > 0)
	{
		auto E = Mouse::buffer.front();
		buffer.pop();
		return E;
	}
	else
	{
		return Event();
	}
}

bool Mouse::IsInWindow() const noexcept
{
	return Mouse::isInWindow;
}

void Mouse::Flush() noexcept
{
	buffer = std::queue<Event>();
}

void Mouse::OnMouseMove(int in_x, int in_y) noexcept
{
	x = in_x;
	y = in_y;
	buffer.push(Event(Mouse::Event::Type::Move, *this));
	TrimBuffer();
}

void Mouse::OnLeftPressed() noexcept
{
	leftIsPressed = true;
	buffer.push(Event(Mouse::Event::Type::LPress, *this));
	TrimBuffer();
}

void Mouse::OnRightPressed() noexcept
{
	rightIsPressed = true;
	buffer.push(Event(Mouse::Event::Type::RPress, *this));
	TrimBuffer();
}

void Mouse::OnWheelPressed() noexcept
{
	wheelIsPressed = true;
	buffer.push(Event(Mouse::Event::Type::WheelPress, *this));
	TrimBuffer();
}

void Mouse::OnLeftReleased() noexcept
{
	leftIsPressed = false;
	buffer.push(Event(Mouse::Event::Type::LRelease, *this));
	TrimBuffer();
}

void Mouse::OnRightReleased() noexcept
{
	rightIsPressed = false;
	buffer.push(Event(Mouse::Event::Type::RRelease, *this));
	TrimBuffer();
}

void Mouse::OnWheelReleased() noexcept
{
	wheelIsPressed = false;
	buffer.push(Event(Mouse::Event::Type::WheelRelease, *this));
	TrimBuffer();
}

void Mouse::OnWheelUp() noexcept
{
	buffer.push(Event(Mouse::Event::Type::WheelUp, *this));
	TrimBuffer();
}

void Mouse::OnWheelDown() noexcept
{
	buffer.push(Event(Mouse::Event::Type::WheelDown, *this));
	TrimBuffer();
}

void Mouse::OnWheelDelta(int delta) noexcept
{
	wheelDeltaCarry += delta;
	while (wheelDeltaCarry >= WHEEL_DELTA)
	{
		wheelDeltaCarry -= WHEEL_DELTA;
		OnWheelUp();
	}
	while (wheelDeltaCarry <= -WHEEL_DELTA)
	{
		wheelDeltaCarry += WHEEL_DELTA;
		OnWheelDown();
	}
}

void Mouse::OnMouseLeave() noexcept
{
	Mouse::isInWindow = false;
	buffer.push(Event(Mouse::Event::Type::Leave, *this));
	TrimBuffer();
}

void Mouse::OnMouseEnter() noexcept
{
	Mouse::isInWindow = true;
	buffer.push(Event(Mouse::Event::Type::Enter, *this));
	TrimBuffer();
}

void Mouse::OnRawDelta(int dx, int dy) noexcept
{
	rawDeltaBuffer.push({ dx,dy });
	TrimBuffer();
}

void Mouse::TrimBuffer() noexcept
{
	while (buffer.size() > bufferSize)
	{
		buffer.pop();
	}
}

void Mouse::TrimRawInputBuffer() noexcept
{
	while (rawDeltaBuffer.size() > bufferSize)
	{
		rawDeltaBuffer.pop();
	}
}

void Mouse::EnableRaw() noexcept
{
	rawEnabled = true;
}

void Mouse::DisableRaw() noexcept
{
	rawEnabled = false;
}

bool Mouse::IsRawEnabled() noexcept
{
	return rawEnabled;
}



