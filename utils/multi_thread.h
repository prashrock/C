#ifndef _MULTI_THREAD_
#define _MULTI_THREAD_
#include <pthread.h>       /* Pthreads library */
#include <stdbool.h>       /* bool, true, false */

struct thread_args{
	int tid;
	pthread_mutex_t *mutex;
	pthread_barrier_t *barrier;
	void *user_data;
	void (*user_fn)(struct thread_args *x);
};

bool pthread_wrapper_create_mutex(pthread_mutex_t *m);
bool pthread_wrapper_create_barrier(pthread_barrier_t *b, int num_threads);
void handle_multi_threaded_test(const int n_threads,
								void (*user_fn)(struct thread_args *x),
								bool print);

#endif //_MULTI_THREAD_
