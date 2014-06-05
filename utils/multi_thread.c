#include <stdio.h>
#include <pthread.h>  /* Pthreads library */
#include <string.h>   /* strerror */
#include <stdbool.h>  /* bool, true, false */
#include <stdlib.h>   /* malloc, free */
#include "multi_thread.h"

pthread_mutex_t mutx;
pthread_barrier_t barr;

bool pthread_wrapper_create_mutex(pthread_mutex_t *m)
{
	int err;
	if(!m) return false;
	err = pthread_mutex_init(m, NULL);
	if (err){
		printf("Error: Mutex init failed:%s", strerror(err));
		return false;
	}
	return true;
}

bool pthread_wrapper_create_barrier(pthread_barrier_t *b, int num_threads)
{
	int err;
	if(!b) return false;
	err = pthread_barrier_init(b, NULL, num_threads);
	if (err){
		printf("Error: Barrier init failed:%s", strerror(err));
		return false;
	}
	return true;
}

static void* thread_func(void *args)
{
	struct thread_args *t_args = (struct thread_args *)args;
	int tid = t_args->tid;
	int rc = pthread_barrier_wait(t_args->barrier);
	if(rc != 0 && rc != PTHREAD_BARRIER_SERIAL_THREAD){
		printf("Error: Thread %d could not wait for barrier\n", tid);
		goto thread_func_err;
	}
	
	t_args->user_fn(t_args);
	
thread_func_err:
	free(args);
	pthread_exit(NULL);
}


void handle_multi_threaded_test(const int n_threads,
								void (*user_fn)(struct thread_args *x),
								bool print)
{
	pthread_t tid[n_threads];
	int err, i;

	if(n_threads == 0) return;
	if(pthread_wrapper_create_mutex(&mutx) == false || 
	   pthread_wrapper_create_barrier(&barr, n_threads) == false)
		goto handle_multi_threaded_test_err;
		
	for(i = 0; i < n_threads; i++)
	{
		struct thread_args *args = malloc(sizeof(struct thread_args));
		if(args == NULL){
			printf("Error: Malloc failed during thread create %d\n", i);
			goto handle_multi_threaded_test_err;
		}
		args->tid = i;
		args->mutex = &mutx;
		args->barrier = &barr;
		args->user_fn = user_fn;
		err = pthread_create(&(tid[i]), NULL, &thread_func, (void *)args);
		if (err != 0)
			printf("Error: Create Thread %d failed:[%s]", i, strerror(err));
		else if(print)
			printf("Thread %d created successfully\n", i);
	}
	for(i = 0; i < n_threads; i++)
		err = pthread_join(tid[i], NULL);

	printf("All %d threads completed successfully\n", n_threads);
handle_multi_threaded_test_err:
	pthread_mutex_destroy(&mutx);
	pthread_barrier_destroy(&barr);
	return;
}
