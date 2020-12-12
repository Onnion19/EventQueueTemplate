#pragma once

#include <functional>
#include <thread>
#include <mutex>
#include <optional>
#include <queue>
#include <utility>


/**
 *  Very basic implementation of a generic multithreading event queue 
 * 
 * T = QueueType
 */
template<class T>
class EventQueue
{
public:
	/** 
	 * Adds a new event to the queue 
	 * Event = Event type 
	 * Args = arguments for the consttructor of the given type.
	 */

	template<class Event, class ... Args>
	void ReceiveEvent(Args&& ...args) {
		const std::lock_guard<std::mutex> lock(mMutex);
		mQueue.push(Event(std::forward<Args>(args)...));
	}

	/**
	 * Returns the first event on the queue. If there is non returnt none. 
	 */
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

