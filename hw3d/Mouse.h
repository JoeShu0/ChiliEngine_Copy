#pragma once
#include<queue>
#include<optional>

class Mouse
{
	friend class Window;
public:
	//Raw mouse Delta 
	struct RawDelta
	{
		int x, y;
	};
	//mouse event
	class Event
	{
	public:
		enum class Type
		{
			LPress,
			LRelease,
			RPress,
			RRelease,
			WheelUp,
			WheelDown,
			WheelPress,
			WheelRelease,
			Move,
			Enter,
			Leave,
			Invalid
		};
	private:
		Type type;
		bool leftIsPressed;
		bool rightIsPressed;
		bool wheelIsPressed;
		int x;
		int y;
	public:
		Event() noexcept
			:
			type(Type::Invalid),
			leftIsPressed(false),
			rightIsPressed(false),
			wheelIsPressed(false),
			x(0),
			y(0)
		{}
		Event(Type type, const Mouse& parent) noexcept
			:
			type(type),
			leftIsPressed(parent.leftIsPressed),
			rightIsPressed(parent.rightIsPressed),
			x(parent.x),
			y(parent.y)
		{}
		bool IsValid() const noexcept
		{
			return type != Type::Invalid;
		}
		Type GetType() const noexcept
		{
			return type;
		}
		std::pair<int, int> GetPos() const noexcept
		{
			return { x,y };
		}
		int GetPosX() const noexcept
		{
			return x;
		}
		int GetPosY() const noexcept
		{
			return y;
		}
		bool LeftIsPressed() const noexcept
		{
			return leftIsPressed;
		}
		bool RightIsPressed() const noexcept
		{
			return rightIsPressed;
		}
		bool WheelIsPressed() const noexcept
		{
			return wheelIsPressed;
		}
	};
public:
	Mouse() = default;
	Mouse(const Mouse&) = delete;
	Mouse& operator= (const Mouse&) = delete;
	std::pair<int, int> GetPos() const noexcept;
	int GetPosX() const noexcept;
	int GetPosY() const noexcept;
	//std::pair<int, int> GetMouseMoveDeltaDist();
	std::optional<RawDelta> ReadRawDelta() noexcept;
	bool LeftIsPressed() const noexcept;
	bool RightIsPressed() const noexcept;
	bool WheeelIsPressed() const noexcept;
	Event Read() noexcept;
	bool QueueIsEmpty() const noexcept
	{
		return buffer.empty();
	}
	bool IsInWindow() const noexcept;
	void Flush() noexcept;
	void EnableRaw() noexcept;
	void DisableRaw() noexcept;
	bool IsRawEnabled() noexcept;
private:// this part of functions will be called from the Window class
	void OnMouseMove(int in_x, int in_y) noexcept;
	void OnLeftPressed() noexcept;
	void OnRightPressed() noexcept;
	void OnWheelPressed() noexcept;
	void OnLeftReleased() noexcept;
	void OnRightReleased() noexcept;
	void OnWheelReleased() noexcept;
	void OnWheelUp() noexcept;
	void OnWheelDown() noexcept;
	void OnWheelDelta(int delta) noexcept;
	void OnMouseLeave() noexcept;
	void OnMouseEnter() noexcept;
	void OnRawDelta(int dx, int dy) noexcept;
	void TrimBuffer() noexcept;
	void TrimRawInputBuffer() noexcept;
	
private:
	bool leftIsPressed = false;
	bool rightIsPressed = false;
	bool wheelIsPressed = false;
	int x;
	int y;
	//int oldX = 0;
	//int oldY = 0;
	int wheelDeltaCarry = 0;
	bool isInWindow = false;
	bool rawEnabled = false;
	std::queue<Event> buffer;
	static constexpr unsigned int bufferSize = 16u;
	std::queue<RawDelta> rawDeltaBuffer;
};