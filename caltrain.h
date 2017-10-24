#include <pthread.h>

// Define our station structure.
struct station {
	// MUTEX :
		// the lock to keep our train counters synchronized.
		pthread_mutex_t counters_lock;
	// Conditions :
		// Condition to keep waiting passengers on.
		pthread_cond_t waiting_passengers;
		// Condition to tell the train when it is full and all passengers that will ride it are confirmed to be on it.
		pthread_cond_t train_boarded;
	// Counters :
		// The number of passengers waiting in the station.
		int waiting_passengers_count;
		// The current number of empty seats unclaimed in the train.
		int train_empty_places;
		// The number of passengers confirmed on train.
		int confirmed_on_train_count;
		// The current train stopping in the station capacity.
		int train_capacity;
};
/**
	- Initializes our station parameters.
**/
void station_init(struct station *station);

/**
	- Loads a train with waiting passengers in its free seats.
	@count
	- The number of empty seats in the train.
	@station
	- The station in which the train stops.
	@return
	- Will return only when the number of confirmed passengers that got on the train is
	equal to the number of empty seats. If the train is already full or the station is empty,
	it should return immediatly.
**/
void station_load_train(struct station *station, int count);
/**
	- Informs a station of the arrival of a new passenger.
	@station
	- The station in which the passenger will wait.
	@return
	- Will only return when the passenger gets an empty place on a train. However,
	it will not inform the train that a passenger has arrived as he might be still
	moving towards the train. 
**/
void station_wait_for_train(struct station *station);
/**
	- Informs the current train waiting in the station that a passenger has actually boarded
	the train and got in an empty seat.
	@return
	- Immediatly. 
**/
void station_on_board(struct station *station);