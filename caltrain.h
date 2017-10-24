#include <pthread.h>

struct station {
	pthread_mutex_t waiting_passengers_mutex;
	int waiting_passengers_count;
	pthread_cond_t waiting_passengers;
	pthread_cond_t train_boarded;
	int train_empty_places;
	int confirmed_on_train_count;
	int train_capacity;
};

void station_init(struct station *station);

void station_load_train(struct station *station, int count);

void station_wait_for_train(struct station *station);

void station_on_board(struct station *station);