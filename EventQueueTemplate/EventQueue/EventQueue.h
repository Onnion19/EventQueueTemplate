#pragma once

#include <functional>
#include <thread>
#include <mutex>
#include <optional>
#include <queue>
#include <utility>


template<class T>
class EventQueue
{
public:
	template<class Event, class ... Args>
	void ReceiveEvent(Args&& ...args) {
		const std::lock_guard<std::mutex> lock(mMutex);
		mQueue.push(Event(std::forward<Args>(args)...));
	}


	std::optional<T> DispatchEvent()
	{
		const std::lock_guard<std::mutex> lock(mMutex);

		if (mQueue.empty())
			return std::nullopt;

		auto currentEvent = std::move(mQueue.front());
		mQueue.pop();
		return std::make_optional<T>(currentEvent);
	}



private:

	std::queue<T> mQueue;
	std::mutex mMutex;
};

