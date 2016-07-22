/* gcc -g -o dyn_maximum_subarray dyn_maximum_subarray.c  */
#include <stdio.h>
#include <stdbool.h>       /* bool, true, false */
#include <pthread.h>       /* Pthreads library */
#include <stdlib.h>        /* atoi  */
#include <string.h>        /* memset */
#include <limits.h>        /* ULONG_MAX */
#include "parser.h"        /* Parser API */
#include "scan_utils.h"    /* input_integer */
#include "string_api.h"    /* String utilities */
#include "compiler_api.h"  /* MAX(), block_api() */
#include "rand_utils.h"    /* get_rand_int(), init_rand()*/

#define DQ_OBJ_TYPE int
#define QUEUE_PRINT_TYPE(_data) printf(" %d,", (_data));
#include "dy_queue.h"      /* Dynamic Queue API */

#define AUTO_MAX_ELEMS 10   /* Max #elements for auto test */
#define AUTO_MAX_VALUE 10   /* Each element is between +/- 10 */

/* Problem:                                                     *
 * Given a set of integers(+/-) find the contiguous subset that *
 * gives the maximum sum when summed up                         *
 * Note: this is simpler than subset sum problem                */

/* Linear time complexity O(n) -- works for +/- integers        *
 * Recurrence Used:                                             *
 * Sub_Problem(i) = MAX(0, Sub_Problem(i-1) + A[i])             */
int maximum_subset_dp(int a[], const int n)
{
   int i, max_sum = 0, cur_sum = 0;
   /* Note, to get start position and len implies maintaing a  *
    * match table, so avoiding for DP approach                 */
   for(i = 0; i < n; i++) {
      cur_sum = MAX(0, (cur_sum + a[i]));
      max_sum = MAX(max_sum, cur_sum);
   }
   return max_sum;
}

/* Time complexity is O(n^3) -- works for +/- integers          *
 * Brute forced approach                                        */
int maximum_subset_brute(int a[], const int n, int *start, int *len)
{
   int i, j, k, max_sum = 0;
   if(start || len) {
      if(!start || !len) {
         printf("Error: Both start and len should be specified\n");
         return -1;
      }
      *start = 0;
      *len = 1;
   }
   for(i = 0; i < n; i++) {
      for(j = i; j < n; j++) {
         int cur_sum = 0;
         for(k = i; k <= j; k++) cur_sum += a[k];
         /* Accounting - calculate start/len if required */
         if(start) {
            if(cur_sum > max_sum && cur_sum > 0){
               *start = i;
               *len = j-i+1;
               max_sum = cur_sum;
            }
         }
      }
   }
   return max_sum;
}

/*--------------- Helper functions and test functions ---------------*/

static void input_integer_array_manual(dq_t *dq)
{
   int n;
   while(1){
      printf("Please Enter next Integer for Input Array(Enter (empty line) to stop):\n");
      if(input_integer(&n))	dq_enq(dq, n);
      else break;
   }
}

static void input_integer_array_auto(dq_t *dq)
{
   int n, i;
   n = get_rand_int_limit(AUTO_MAX_ELEMS+1);
   for(i = 0; i < n; i++) {
      int val = get_rand_int_limit(AUTO_MAX_VALUE*2+1) + (-AUTO_MAX_VALUE);
      dq_enq(dq, val);
   }
}

static bool print_input = true;
static void maximum_subset_test(int auto_mode)
{
   dq_t *dq = dq_init();
   int bt_start, bt_len, max_sum_brute;
   int	max_sum_dp;
   const char *name;
   if(!dq) return;
   if(auto_mode) {
      input_integer_array_auto(dq);
      name = "Automatically";
   }
   else {
      input_integer_array_manual(dq);
      name = "Manually";
   }
   if(print_input)
   {
      printf("%s generated %d integers:", name, dq_count(dq));
      dq_iterator(dq, dq_print_element);
      printf("\n");
   }
   max_sum_dp = maximum_subset_dp(dq->elems, dq_count(dq));
   max_sum_brute = maximum_subset_brute(dq->elems, dq_count(dq),
                                        &bt_start, &bt_len);
   
   if(max_sum_dp != max_sum_brute){
      printf("Error: Max subset sum incorrect, DP = %d, Brute = %d\n",
             max_sum_dp, max_sum_brute);
      block_api();
   }
   if(print_input)
      printf("Maximum subset sum = %d @ index=%d, len=%d\n\n",
             max_sum_dp, bt_start+1, bt_len);

   dq_destroy(dq);
}

static void print_help_string()
{
   printf("\n\nPress Enter to exit or follow below commands\n");
   printf("\t ?  - Print this text again\n");
   printf("\t q  - Quit\n");
   printf("\t auto* - Auto-mode: generate seq + compute max subsetsum\n");
   printf("\t manual* - Manual-mode: enter seq+compute max subsetsum\n");
   printf("\t NOTE - * CMDS take optional loop_count, eg m 2\n");
}

static void multi_char_CLI(const char *c)
{
   int lp = 1;
   const char *space;
   if((space = my_strstr_with_strlen(c, " ")))
   {
      lp = atoi(space + 1);
      if(lp == 0) lp = 1;
      //printf("lp = %d\n", lp);
   }
   if(strncmp(c, "auto", strlen("auto")) == 0)
   {
      int user_lp = lp;
      while(lp--) maximum_subset_test(1);
      printf("Info: Auto mode successfully completed %d runs\n", user_lp);
   }
   else if(strncmp(c, "manual", strlen("manual")) == 0) {
      while(lp--) maximum_subset_test(0);
   }
}

static void single_char_CLI(const char *c)
{
   switch( *c )
   {
   case '?':
      print_help_string();
      break;
   case 'q':
      exit(0);
      break;
   default:
      break;
   }
}

int main(int argc, char **argv)
{
   if(argc > 2 || (argc == 2 && strstr(argv[1], "print_input") == NULL)) {
      printf("Usage: %s [print_input | no_print_input] \n", argv[0]);
      return -1;
   }
   if(argc == 2 && strcmp(argv[1], "no_print_input") == 0)
      print_input = false;

   init_rand();
   print_help_string();
   handle_CLI(single_char_CLI, NULL, multi_char_CLI);
   return 0;
}
