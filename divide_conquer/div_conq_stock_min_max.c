/* gcc -g -o div_conq_stock_min_max stock_min_max_price.c  */
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

#define AUTO_MAX_ELEMS 20   /* Max #elements for auto test */
#define AUTO_MAX_VALUE 100  /* Each element is between +/- 10 */

/* Problem:                                                     *
 * Given a set of stock prices, find a pair of prices (p1, p2)  *
 * such that idx(p1) < idx(p2) and diff = p2-p1 is the maximum  */

struct min_max{
   int min;
   int max;
   int profit;
};

/* Divide & Conquer algorithm. Split array in half and find maximum   *
 * profit in each sub-array. Instead of scanning each sub-array during*
 * merge, just compare the min and max of both sub-arrays:            *
 * Recurrence Relation:                                               *
 * T(1) = O(1)                                                        *
 * T(N) = 2 * T(N/2) + O(1)                                           *
 * Time complexity = O(n) (Master's method 2nd case, a=2, b=2, d=0)   *
 * Space Complexity = O(1) + O(log n) for recursive stacks            */
struct min_max dc_stock_maximum_diff(int a[], const int n)
{
   struct min_max ret = {0}, l, r, m;
   if(n == 0) return ret; /* If user passes empty array              */
   if(n == 1) {           /* Base case                               */
      ret.min = ret.max = a[0];
      return ret;
   }
   l = dc_stock_maximum_diff(a, n/2);
   r = dc_stock_maximum_diff(&a[n/2], n-n/2);
   m.max = r.max;
   m.min = l.min;
   m.profit = m.max - m.min;
   ret.profit = MAX(MAX(l.profit, r.profit), m.profit);
   ret.max = MAX(l.max, r.max);
   ret.min = MIN(l.min, r.min);
   return ret;
}

/* Dynamic Programming. Avoid recursion and complete in a single pass *
 * Time Complexity = O(n) and Space Complexity = O(1)                 */
int dy_stock_maximum_diff(int a[], const int n)
{
   int i;
   struct min_max ret = {0};
   /* If user passes empty array or array has only one element       */
   if(n == 0 || n == 1) return 0;
   ret.profit = a[1] - a[0];
   if(ret.profit < 0) ret.profit = 0;
   ret.min = a[0];
   for(i = 1; i < n; i++)
      {
         if(a[i] - ret.min > ret.profit){
            ret.profit = a[i] - ret.min;
            ret.max = a[i];
         }
         if(a[i] < ret.min)
            ret.min = a[i];
      }
   return ret.profit;
}

/*--------------- Helper functions and test functions ----------------*/

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
      int val = get_rand_int_limit(AUTO_MAX_VALUE);
      dq_enq(dq, val);
   }
}

static bool print_input = true;
static void stock_max_diff_test(int auto_mode)
{
   dq_t *dq = dq_init();
   struct min_max m_dc, m_dy;
   const char *name;
   if(!dq) return;
   if(auto_mode){
      input_integer_array_auto(dq);
      name = "Automatically";
   }
   else{
      input_integer_array_manual(dq);
      name = "Manually";
   }
   if(print_input)
      {
         printf("%s generated %d integers:", name, dq_count(dq));
         dq_iterator(dq, dq_print_element);
         printf("\n");
      }
   m_dc = dc_stock_maximum_diff(dq->elems, dq_count(dq));
   m_dy.profit = dy_stock_maximum_diff(dq->elems, dq_count(dq));
   if(m_dc.profit != m_dy.profit)
      {
         printf("Error: divide and conquer and DP results dont match\n");
         printf("   Min: DC = %d\n", m_dc.min);
         printf("   Max: DC = %d\n", m_dc.max);
         printf("Profit: DC = %d, DP = %d\n", m_dc.profit, m_dy.profit);
         block_api();
      }
   if(print_input)
      printf("DC_Min = %d DC_Max = %d Diff = %d\n",
             m_dc.min, m_dc.max, m_dc.profit);
   dq_destroy(dq);
}

static void print_help_string()
{
   printf("\n\nPress Enter to exit or follow below commands\n");
   printf("\t ?  - Print this text again\n");
   printf("\t q  - Quit\n");
   printf("\t auto* - Auto-mode: generate seq + compute max diff\n");
   printf("\t manual* - Manual-mode: enter seq+ compute max diff\n");
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
         while(lp--) stock_max_diff_test(1);
         printf("Info: Auto mode successfully completed %d runs\n", user_lp);
      }
   else if(strncmp(c, "manual", strlen("manual")) == 0)
      {
         while(lp--) stock_max_diff_test(0);
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
   if(argc > 2 || (argc == 2 && strstr(argv[1], "print_input") == NULL))
      {
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
