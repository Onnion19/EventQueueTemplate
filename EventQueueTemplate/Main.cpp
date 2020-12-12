
#include <variant>
#include <iostream>
#include "EventQueue/EventQueue.h"

/** 
 *	 Events spcae, here are defined all I/O events. Should be refactored to their own specific file as well as improve the systems.  
 * 
 *	The current implementation is just for example purposes. 
 */
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



	//Set all types of events as a variant. 
	using InputEvent = std::variant<Mouse::OnClick, Keyboard::OnDown>; 

	/** 
	 *	This struct works with std::visit to check the variant content. It will trigger the matching function according to the content of the varirant. 
	 */
	struct visitor {

		void operator()(Mouse::OnClick  clickPoint){
			std::cout << "Mouse clicked at: " << clickPoint.pos_x << " , " << clickPoint.pos_y << std::endl; 
		}

		void operator() (Keyboard::OnDown Key){
			std::cout << "Key down, keycode = " << Key.key << std::endl; 
		}

	};
}


/** 
 * Given a Queue<EventQ> adds a new event of type <Event>.
 * EventQ = Queue type 
 * Event = Event type 
 * 
 * Args = params for the constructor of the given event . 
 */
template<class EventQ, class Event, class ... Args>
void SendEvent(EventQueue<EventQ> & queue ,Args&& ...args) {
	queue.ReceiveEvent<Event>(std::forward<Args>(args)...); 
}


/** 
 * Given a Queue<EventQ> asks for the first event in the queue. 
 *  
 */
template<class EventQ>
void ReceiveEvent(EventQueue<EventQ>& queue) {

	auto incomingEvent = queue.DispatchEvent(); 
	if(incomingEvent)
	{
		//this could be implemented as lambda functions to provided other kind of handlers.
		std::visit(Events::visitor(), *incomingEvent); 
	}
	else 
	{	
		std::cout << "No event to process " << std::endl; 
	}
}

//Just a dumb class for example purposes
class InvalidClass {};

//User programming scenario showcase:
int main()
{
	//Create a event queue of the type you want. 
	/*
	 * Shared pointer is used to be safe on a multithreading envirronment 
	 */
	std::shared_ptr<EventQueue<Events::InputEvent>> myEventQueue = std::make_shared< EventQueue<Events::InputEvent>>();

	//Thread1: Populates the event queue
	{
		//send events that matches with the queue types. In our case all I/O events are valid. 
		SendEvent<Events::InputEvent, Events::Mouse::OnClick>(*myEventQueue, 10, 15); 
		SendEvent<Events::InputEvent, Events::Keyboard::OnDown>(*myEventQueue,10);
		//SendEvent<Events::InputEvent, InvalidClass>(myEventQueue); //Causes COMPILE error because InvalidClass is not inside the InputEvent variant type
	}

	//Thread2: Reads the queue
	{
		ReceiveEvent< Events::InputEvent>(*myEventQueue);
		ReceiveEvent< Events::InputEvent>(*myEventQueue);
		ReceiveEvent< Events::InputEvent>(*myEventQueue); //Can safely try to receive more events than the current one in the queue.
	}


	return 0; 
}