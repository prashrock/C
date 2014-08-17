//gcc -Wall -g -lpthread -o union_find_test.c
#include <stdio.h>         /* printf */
#include <pthread.h>       /* Pthreads library */
#include <string.h>        /* strerror */
#include "parser.h"        /* Parser API */
#include "multi_thread.h"  /* PThread helper  */
#include "string_api.h"    /* String utilities */
#include "scan_utils.h"    /* input_integer */
#include "union_find.h"    /* Union Find API  */
uf_t *uf;
	
static void print_help_string()
{
	printf("\n\nPress Enter to exit or follow below commands\n");
	printf("\t         ? - Print this text again\n");
	printf("\t         p - Print all connected components\n");
	printf("\t         r - Print connected components and ranks\n");
	printf("\t    find p - Find root of connected Component 'p'\n");
	printf("\t union p q - Connect components 'p' and 'q'\n");
	printf("\t conn  p q - Are 'p' and 'q' connected\n");
	printf("\t         n - Print UF statistics\n");
}

static void multi_char_CLI(const char *c)
{
	int lp;
	const char *space, *space1;
	int root, p, q;
	if(strncmp(c, "find", strlen("find")) == 0)
	{
		if((space = my_strstr_with_strlen(c, " ")))
		{
			lp = atoi(space + 1);
			root = uf_find(&uf, lp);
			printf("Root of '%d' = %d\n", lp, root);
		}
	}
	else if(strncmp(c, "union", strlen("union")) == 0)
	{
		if((space = my_strstr_with_strlen(c, " ")))
		{
			p = atoi(space + 1);
			if((space1 = my_strstr_with_strlen(space+1, " ")))
			{
				q = atoi(space1 + 1);
				if(uf_union(&uf, p, q))
					printf("%d and %d have been connected\n", p, q);
				else
					printf("Error: %d and %d connect failed\n", p, q);
			}
		}
	}
	else if(strncmp(c, "conn", strlen("conn")) == 0)
	{
		int p, q;
		if((space = my_strstr_with_strlen(c, " ")))
		{
			p = atoi(space + 1);
			if((space1 = my_strstr_with_strlen(space+1, " ")))
			{
				q = atoi(space1 + 1);
				if(uf_is_connected(&uf, p, q))
					printf("%d and %d are connected\n", p, q);
				else
					printf("%d and %d are NOT connected\n", p, q);
			}
		}
	}
}

static void single_char_CLI(const char *c)
{
	switch( *c )
	{
	case 'p': case 'P':
		printf("UF has %d elements. Printing all elements\n",
		   uf->elem_cnt);
		uf_iterator(uf, uf_print_id);
		printf("\n");
		break;
	case 'r': case 'R':
		printf("UF has %d elements. Printing elements + ranks\n",
		   uf->elem_cnt);
		uf_iterator(uf, uf_print_id_rank);
		printf("\n");
		break;
	case 'n': case 'N':
		uf_print_stats(uf);
		break;
	case '?':
		print_help_string();
		break;
	default:
		break;
	}
}

int main()
{
	/* Assume create goes through fine, this is just a test-driver */
	uf = uf_create(0);
	uf_set_size_unlimited(uf);
	print_help_string();
	handle_CLI(single_char_CLI, NULL, multi_char_CLI);
	uf_destroy(uf);
	return 0;
}
