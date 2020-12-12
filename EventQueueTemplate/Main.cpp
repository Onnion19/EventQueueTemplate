
#include <variant>
#include <iostream>
#include "EventQueue/EventQueue.h"

namespace Events{

	namespace Mouse{
	
		struct OnClick{
			explicit OnClick(int x, int y) : pos_x(x), pos_y(y) {}
			int pos_x, pos_y; 
		};

		//Other events here
	}


	namespace Keyboard{
	
		struct OnDown{
			explicit OnDown(int KeyCode) : key(KeyCode) {}

			int key; 
		};

		//Other events here
	}




	using InputEvent = std::variant<Mouse::OnClick, Keyboard::OnDown>; 

	struct visitor {

		void operator()(Mouse::OnClick  clickPoint){
			std::cout << "Mouse clicked at: " << clickPoint.pos_x << " , " << clickPoint.pos_y << std::endl; 
		}

		void operator() (Keyboard::OnDown Key){
			std::cout << "Key down, keycode = " << Key.key << std::endl; 
		}

	};
}



template<class EventQ, class Event, class ... Args>
void SendEvent(EventQueue<EventQ> & queue ,Args&& ...args) {
	queue.ReceiveEvent<Event>(std::forward<Args>(args)...); 
}

template<class EventQ>
void ReceiveEvent(EventQueue<EventQ>& queue) {

	auto incomingEvent = queue.DispatchEvent(); 
	if(incomingEvent)
	{
		std::visit(Events::visitor(), *incomingEvent); 
	}
	else 
	{	
		std::cout << "No event to process " << std::endl; 
	}
}

int main()
{

	EventQueue<Events::InputEvent> myEventQueue; 
	//send events
	SendEvent<Events::InputEvent, Events::Mouse::OnClick>(myEventQueue, 10, 15); 
	SendEvent<Events::InputEvent, Events::Keyboard::OnDown>(myEventQueue,10);

	//Receive events
	ReceiveEvent< Events::InputEvent>(myEventQueue); 
	ReceiveEvent< Events::InputEvent>(myEventQueue); 
	ReceiveEvent< Events::InputEvent>(myEventQueue); 

	return 0; 
}