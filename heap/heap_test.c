#include <stdio.h>         /* printf */
#include <pthread.h>       /* Pthreads library */
#include <string.h>        /* strerror */
#include "parser.h"        /* Parser API */
#include "multi_thread.h"  /* PThread helper  */
#include "rand_utils.h"    /* get_rand_int(), init_rand()*/
#include "compiler_api.h"  /* block_api() */
#define  MAX_HEAP_IMPL
#include "heap.h"          /* Dynamic Heap API */

#define AUTO_MAX_ELEMS 500   /* Max #elements for auto test */
#define AUTO_MAX_VALUE 100   /* Each element is between +/- 10 */
heap_t *hp;


static void print_help_string()
{
	printf("\n\nPress Enter to exit or follow below commands\n");
	printf("\t  ? - Print this text again\n");
	printf("\t  q - Quit\n");
	printf("\t +1 - Add a number into Heap\n");
	printf("\t  - - Remove and print root of Heap\n");
	printf("\t  p - Print Heap\n");
	printf("\t  n - Print Size information of Heap\n");
	printf("\t  r - Print Root key in the Heap\n");
	printf("\t  a - Assert Heap invariant\n");
	printf("\t  t - Test heap with random input\n");
	printf("\t  h - Insert 10 random elements via heapify\n");
}

static void heap_and_heap_statistics_printer()
{
	printf("Printing Heap Elements:\n");
	heap_print(hp);
	heap_print_stats(hp);
}

static void heap_auto_test()
{
	
	unsigned long sum = 0;
	int n, i, old_val = 0;
	n = get_rand_int_limit(AUTO_MAX_ELEMS+1);
	/* Sanity check first -- Empty heap */
	if(is_heap_empty(hp) == false){
		printf("Error: Heap should be empty to run auto test\n");
		return;
	}
	/* Insert a bunch of random numbers and verify if heap order is maintained */
	printf("Inserting %d numbers:\n", n);
	for(i = 0; i < n; i++) {
		int val = get_rand_int_limit(AUTO_MAX_VALUE*2+1) + (-AUTO_MAX_VALUE);
		printf("%d, ", val);
		sum += val;
		if(heap_insert(hp, val) == false) {
			printf("\nError: Heap insert failed for %d\n", val);
			block_api();
		}
	}
	printf("\n--------------------------------------------\n");
	heap_and_heap_statistics_printer();
	printf("--------------------------------------------\n");
	printf("\nPulling out elements from the heap:\n");
	/* Pull out elements from the heap and validate */
	for(i = 0; i < n; i++) {
		int val = 0;
		if(heap_delete_root(hp, &val) == false) {
			printf("\nError: Heap delete failed for %d item\n", i+1);
			block_api();
		}
		printf("%d, ", val);
		if(i && CMP(old_val, val)) {
			printf("\nError: Heap invariant broken - %d found before %d\n",
				   old_val, val);
			block_api();
		}
		sum  -= val;
		old_val = val;
	}
	if(sum != 0) {
		printf("\nError: sum(%lu) nonzero, heap gobbled something?\n", sum);
		block_api();
	}
	printf("\n--------------------------------------------\n");
}

static void heap_heapify_test(int n)
{
	int i;
	int arr[n];
	
	for(i = 0; i < n; i++) {
		arr[i] = get_rand_int_limit(AUTO_MAX_VALUE);
		printf("%d, ", arr[i]);
	}
	printf("\n");
	if(heap_heapify(hp, arr, n) == false)
		printf("Error: Heapify failed\n");
}

static void single_char_CLI(const char *c)
{
	int x;
	switch( *c )
	{
	case 'p': case 'P':
		heap_and_heap_statistics_printer();
		break;
	case 'n': case 'N':
		printf("Heap Count=%d Size=%d\n", heap_count(hp), heap_size(hp));
		break;
	case '-':
		if(heap_delete_root(hp, &x) == false)
			printf("Error: Heap Delete Root Failed - Heap empty?\n");
		else
			printf("Deleted key = %d\n", x);
		break;
	case 'r':
		if(heap_peak_root(hp, &x) == false)
			printf("Error: Heap Peak Root Failed - Heap empty?\n");
		else
			printf("Root key (%s) = %d\n", HEAP_TYPE_STR, x);
		break;
	case '?':
		print_help_string();
		break;
	case 'a':
		printf("%s invariant = %s\n", HEAP_TYPE_STR,
			   (heap_validate(hp)?"True":"False") );
		break;
	case 't':
		printf("%s - Automatic test(random numbers):\n", HEAP_TYPE_STR);
		heap_auto_test();
		break;
	case 'h':
		printf("%s - Heapify test(add 10 rand numbers):\n", HEAP_TYPE_STR);
		heap_heapify_test(10);
		break;
	case 'q':
		exit(0);
		break;
	default:
		break;
	}
	
}
static void single_char_CLI_with_num(const char *c)
{
	int x = atoi(c+1);
	switch(*c)
	{
	case '+':
		if(heap_insert(hp, x) == false)
			printf("Error: Heap Insert failed. Heap Full ?\n");
		break;
	default:
		break;
	}
}

int main()
{
	init_rand();
	hp = heap_init(0, 4);
	print_help_string();
	handle_CLI(single_char_CLI, single_char_CLI_with_num, NULL);
	heap_destroy(hp);
	return 0;
}

