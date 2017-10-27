#include <pthread.h>
#include "caltrain.h"

// Implementation (Documented in headers).
void station_init(struct station *station) {
	// Initialize mutex lock.
	pthread_mutex_init(&station->counters_lock, NULL);
	// Initialize our conditions.
	pthread_cond_init(&station->waiting_passengers, NULL);
	pthread_cond_init(&station->train_boarded, NULL);
	// Initialize our counters.
	station->waiting_passengers_count = 0;
	station->train_empty_places = 0;
	station->confirmed_on_train_count = 0;
}

// Implementation (Documented in headers).
void station_load_train(struct station *station, int count) {
	// If train has no empty places, leave immedialty.
	if (count == 0) {
		return;
	}
	// Update train capacity.
	// Only this function can update and only one train can wait in the station as
	// told in requirements.
	station->train_capacity = count;
	// Take lock to change and read shared variables.
	pthread_mutex_lock(&station->counters_lock);
	// Check if there is any waiting passengers, otherwise leave immediatly.
	if (station->waiting_passengers_count != 0) {
		// Set number of available tickets and number of confirmed passengers.
		station->train_empty_places = count;
		station->confirmed_on_train_count = 0;
		// Announce to all passengers.
		pthread_cond_broadcast(&station->waiting_passengers);
		// Wait in the station until all passengers get on the train.
		pthread_cond_wait(&station->train_boarded, &station->counters_lock);
		// Update the waiting passenger count as the train leaves.
		station->waiting_passengers_count -= station->confirmed_on_train_count;	
	}
	// Unlock.
	pthread_mutex_unlock(&station->counters_lock);

}

// Implementation (Documented in headers).
void station_wait_for_train(struct station *station) {
	// Take lock to update counter.
	pthread_mutex_lock(&station->counters_lock);
	// Add to the number of waiting passengers
	station->waiting_passengers_count++;
	// Unlock the lock.
	pthread_mutex_unlock(&station->counters_lock);
	// Wait until you get an empty seat ticket on a train.
	int boarded = 0;
	while (boarded == 0) {
		// Take lock to check/update counter.
		pthread_mutex_lock(&station->counters_lock);
		// If there is already a train waiting, then go catch it !, otherwise wait.
		if (station->train_empty_places <= 0) {
			pthread_cond_wait(&station->waiting_passengers, &station->counters_lock);
		}
		// Check if any free seats tickets are remaining, if there is take one.
		if (station->train_empty_places > 0) {
			station->train_empty_places--;
			boarded = 1;
		}
		// Unlock the lock.
		pthread_mutex_unlock(&station->counters_lock); 
	}
}

// Implementation (Documented in headers).
void station_on_board(struct station *station) {
	// Take lock to update the confirmation counter.
	pthread_mutex_lock(&station->counters_lock);
	// Update number of confirmed passengers.
	station->confirmed_on_train_count++;
	// Read values of other counters.
	int confirmed = station->confirmed_on_train_count;
	int waiting = station->waiting_passengers_count;
	// Leave lock.
	pthread_mutex_unlock(&station->counters_lock);
	// Check for ending condition, only one thread will ever update station->train_capacity and
	// that's before threads begin entering here.
	if (station->train_capacity == confirmed || waiting == confirmed) {
		// Tell train to leave if at full capacity and all passengers got on or if no more passengers
		// are waiting.  
		pthread_cond_signal(&station->train_boarded);
	}
}
