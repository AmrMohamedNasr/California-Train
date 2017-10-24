#include <pthread.h>
#include "caltrain.h"

// Implementation (Documented in headers).
void station_init(struct station *station) {
	// Check for error.
	pthread_mutex_init(&station->counters_lock, NULL);
	pthread_cond_init(&station->waiting_passengers, NULL);
	pthread_cond_init(&station->train_boarded, NULL);
	station->waiting_passengers_count = 0;
	station->train_empty_places = 0;
	station->confirmed_on_train_count = 0;
}

// Implementation (Documented in headers).
void station_load_train(struct station *station, int count) {
	pthread_mutex_lock(&station->counters_lock);
	if (station->waiting_passengers_count != 0 && count != 0) {
		station->train_empty_places = count;
		station->confirmed_on_train_count = 0;
		station->train_capacity = count;
		pthread_cond_broadcast(&station->waiting_passengers);
		pthread_cond_wait(&station->train_boarded, &station->counters_lock);
		station->waiting_passengers_count -= station->confirmed_on_train_count;	
	}
	pthread_mutex_unlock(&station->counters_lock);

}

// Implementation (Documented in headers).
void station_wait_for_train(struct station *station) {
	pthread_mutex_lock(&station->counters_lock);
	station->waiting_passengers_count++;
	int boarded = 0;
	while (boarded == 0) {
		pthread_cond_wait(&station->waiting_passengers, &station->counters_lock);
		if (station->train_empty_places > 0) {
			station->train_empty_places--;
			boarded = 1;
		} 
	}
	pthread_mutex_unlock(&station->counters_lock);
}

// Implementation (Documented in headers).
void station_on_board(struct station *station) {
	pthread_mutex_lock(&station->counters_lock);
	station->confirmed_on_train_count++;
	if (station->confirmed_on_train_count == station->train_capacity || station->waiting_passengers_count == station->confirmed_on_train_count) {
		pthread_cond_signal(&station->train_boarded);
	}
	pthread_mutex_unlock(&station->counters_lock);
}
