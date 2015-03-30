//gcc -Wall -g -lpthread -o tree_test tree_test.c
#include <stdio.h>         /* printf */
#include <pthread.h>       /* Pthreads library */
#include <string.h>        /* strerror */
#include "parser.h"        /* Parser API */
#include "multi_thread.h"  /* PThread helper  */
#include "string_api.h"    /* String utilities */
#include "bst.h"           /* BST API */
#include "rand_utils.h"    /* init_rand(), get_rand_int_limit() */
bst_t *bst;

#define MAX_RAND_KEY 10
#define LP_CNT 1000
static unsigned long enq_tot, deq_tot;
void tree_test_thread(struct thread_args *x)
{
	int i,  val = 0;
	for(i = 0; i < LP_CNT; i++)
	{
		if(pthread_mutex_lock(x->mutex) == 0)
		{
			//if(queue_enq(i + x->tid))
				enq_tot += (i + x->tid);
			pthread_mutex_unlock(x->mutex);
		}
		else break;
		if(pthread_mutex_lock(x->mutex) == 0)
		{
			//if(queue_deq(&val))
				deq_tot += val;
			pthread_mutex_unlock(x->mutex);
		}
		else break;
	}
	if(i != LP_CNT)
		printf("Thread %d made %d cycles out of %d\n", x->tid, i, LP_CNT);
}

static void print_help_string()
{
	printf("\n\nPress Enter to exit or follow below commands\n");
	printf("\t  ? - Print this text again\n");
	printf("\t  p - Print Binary Search Tree\n");
	printf("\t  h - Height of Binary Search Tree\n");
	printf("\t  n - Print size of Binary Search Tree\n");
	printf("\t +1 - Add a number into Binary Search Tree\n");
	printf("\t -1 - Remove Key from Binary Search Tree\n");
	printf("\t ?1 - Search Binary Search Tree with Key\n");
	printf("\t r1 - Print Rank of given Key in Binary Search Tree\n");
	printf("\t t2 - Create '2' threads to handle %d ins/del\n", LP_CNT);
	printf("\t min- Print the minimum Key\n");
	printf("\t max- Print the maximum Key\n");
	printf("\t rootkey           - Print Root key\n");
	printf("\t bfs_print         - BFS Traversal of BST\n");
	printf("\t spiral_print      - BFS:Level order spiral print of BST\n");
	printf("\t preorder_print    - DFS:Pre-Order Traversal of BST\n");
	printf("\t inorder_print     - DFS:In-Order Traversal of BST\n");
	printf("\t postorder_print   - DFS:Post-Order Traversal of BST\n");
	printf("\t path_print        - DFS:Print all root to leaf paths\n");
	printf("\t longest_path_print- DFS:Print longest root to leaf path\n");
	printf("\t add <key> <data>  - Use if data is diff from key\n");
	printf("\t parent <key>      - Print immediate parent's key\n");
	printf("\t lcp <key1> <key2> - Print lowest common parent's key\n");
	printf("\t floor <key>       - Print next smallest key\n");
	printf("\t print_key <key>   - Print key + data given key\n");
	printf("\t rand_insert<#key> - Insert '#key' random keys\n");
}

/* Cannot pass bst_cpy to iterator fn, so keep static declaration     */
static bst_t *bst_print;
static int bst_print_row_pos, bst_print_column_pos;
static void bst_printer_duplicate_bst_helper(bst_node_t *x)
{
	bst_insert(bst_print, x->key, x->key);
}
static void bst_printer_duplicate_bst(bst_t *bst)
{
	if(!bst || !bst_print) return;
	bst_preorder_dfs_traversal(bst, bst_printer_duplicate_bst_helper);
}

/* Calculate row and column position for each node and store this     *
 * information as "data" into each node in another BST                */
static void bst_printer_get_row_col_info(bst_node_t *x)
{
	/* Store Column (vertical) information */
	bst_insert(bst_print, x->key, bst_print_column_pos++);
    /* Store Row (height) information */
	bst_insert(bst_print, x->key,
			   bst_get_node_height(bst_print, x->key));
}
/* BFS iterator calls this function to print each node's key since    *
 * (x, y) pos info for each node is stored in bst_print               */
static void bst_print_helper(bst_node_t *x)
{
	int i, row, col;
	col = x->data_q->elems[1];
	row = x->data_q->elems[2];
	//printf("Key = %d, row = %d, col = %d\n", x->key, row, col);
	if(row == bst_print_row_pos + 1)
	{
		bst_print_column_pos = 0;
		printf("\n");
	}
	for(i = bst_print_column_pos; i < col; i++)printf("    ");
	printf("%4d", x->key);
	bst_print_column_pos = col + 1;
	bst_print_row_pos = row;
}

/* Replicate BST(Pre-Order traversal) to store each node's (x,y) pos  *
 * Do In-order traversal to get column (y) position of each element   *
 * During traversal, calculate each node's height (x) position        *
 * Finally, do a BFS traversal, to print all the nodes                *
 * Note: The row-column approach does not guarantee that the root will*
 * always be at center of tree (consider, left-leaning tree), but the *
 * drawing will be correct and the algo can be enhanced to fix this   *
 * Time Complexity = O(3n lg n)                                       *
 * Space Complexity = O(n)                                            */
static void bst_printer(bst_t *bst)
{
	bst_print = bst_init();
	if(!bst_print) return;
	bst_printer_duplicate_bst(bst);
	bst_print_column_pos = bst_print_row_pos = 0;
	bst_inorder_dfs_traversal(bst, bst_printer_get_row_col_info);
	bst_print_column_pos = 0;
	bst_bfs_traversal(bst_print, bst_print_helper, NULL);
	printf("\n");
	bst_destroy(bst_print);
}
static void single_char_CLI(const char *c)
{
	switch( *c )
	{
	case 'p': case 'P':
		printf("Printing BST Keys:\n");
		bst_printer(bst);
		break;
	case 'h': case 'H':
		printf("Height of BST (zero-based) = %d\n", bst_height(bst));
		break;
	case 'n': case 'N':
		printf("Size = #Keys in BST = %u\n", bst_size(bst));
		break;
	case '?':
		print_help_string();
		break;
	default:
		break;
	}
}

static void single_char_CLI_with_num(const char *c)
{
	dq_t *data_q;
	int rank;
	int x = atoi(c+1);
	switch(*c)
	{
	case '+':
		if(bst_insert(bst, x, x) == false)
			printf("Error: BST Insert failed. BST Full ?\n");
		break;
	case '-':
		if(bst_delete(bst, x))
			printf("Info: Successful deleted key %d\n", x);
		else
			printf("Error: BST Delete failed. Key %d not found\n", x);
		break;
	case '?':
		if(bst_get_data(bst, x, &data_q)){
			printf("Key '%d' found in BST with data:\n", x);
			dq_iterator(data_q, dq_print_element);
		}
		else
			printf("Key '%d' not found in BST Search\n", x);
		break;
	case 'r': case 'R':
		rank = bst_get_rank(bst, x);
		printf("Rank = %d Keys are less than %d\n", rank, x);
		break;
	case 't': case 'T':
		handle_multi_threaded_test(x, tree_test_thread, false);
		printf("Enq_tot=%lu Deq_tot=%lu\n", enq_tot, deq_tot);
		enq_tot = deq_tot = 0;
		//queue_print_stats(&q_stats);
		break;
	default:
		break;
	}
}

static void multi_char_CLI(const char *c)
{
	const char *space;
	if(strncmp(c, "min", strlen("min")) == 0)
	{
		int key;
		if(bst_get_min_key(bst, &key))
			printf("Min key in BST = %d\n", key);
		else
			printf("No Min key found in BST, BST empty?\n");
	}
	else if(strncmp(c, "max", strlen("max")) == 0)
	{
		int key;
		if(bst_get_max_key(bst, &key))
			printf("Max key in BST = %d\n", key);
		else
			printf("No Max key found in BST, BST empty?\n");
	}
	else if(strncmp(c, "rootkey", strlen("rootkey")) == 0)
	{
		int root_key;
		if(bst_get_root_key(bst, &root_key))
		{
			printf("Root_key for BST = %d, ",root_key);
			if(bst_is_root(bst, root_key))
				printf("validated with is_root(%d)\n", root_key);
			else
				printf("is_root(%d) validation failed\n", root_key);
		}
		else
			printf("Root_key not found for BST, BST Empty ?\n");
	}
	else if(strncmp(c, "parent", strlen("parent")) == 0)
	{
		int key, pk;
		if((space = my_strstr_with_strlen(c, " ")))
			key = atoi(space + 1);
		if(bst_get_parent_key(bst, key, &pk))
			printf("Parent key for '%d' = %d\n", key, pk);
		else{
			printf("No Parent found for '%d'\n", key);
			printf("Possible reasons for this:\n");
			printf("\tBST Empty, is Root Key or Key not found\n");
		}
	}
	else if(strncmp(c, "lcp", strlen("lcp")) == 0)
	{
		int key1 = 0, key2 = 0, parent;
		if((space = my_strstr_with_strlen(c, " ")))
			key1 = atoi(space + 1);
		if((space = my_strstr_with_strlen(space+1, " ")))
			key2 = atoi(space + 1);
		/* Get the second key for lowest common parent command */
		if(bst_get_lowest_common_parent_key(bst, key1, key2,
											&parent))
			printf("LCP key for keys '%d' and '%d' = %d\n",
				   key1, key2, parent);
		else
			printf("No LCP found for keys '%d'&'%d'\n",key1,key2);
	}
	else if(strncmp(c, "add", strlen("add")) == 0)
	{
		int key, val;
		if((space = my_strstr_with_strlen(c, " ")))
			key = atoi(space + 1);
		if((space = my_strstr_with_strlen(space+1, " ")))
			val = atoi(space + 1);
		printf("key=%d, val=%d\n", key, val);
		if(bst_insert(bst, key, val) == false)
			printf("Error: BST Insert failed. BST Full ?\n");
	}
	else if(strncmp(c, "floor", strlen("floor")) == 0)
	{
		int key = 0, floor_key;
		if((space = my_strstr_with_strlen(c, " ")))
			key = atoi(space + 1);
		if(bst_get_floor_key(bst, key, &floor_key))
			printf("Floor key for '%d' = %d\n", key, floor_key);
		else{
			printf("No Floor found for '%d'\n", key);
			printf("Possible reasons for this:\n");
			printf("\tBST Empty, is Root Key or Key not found\n");
		}
	}
	else if(strncmp(c, "bfs_print", strlen("bfs_print")) == 0)
	{
		printf("BFS Traversal of keys in BST:\n");
		bst_bfs_traversal(bst, print_bst_node_key, NULL);
		printf("\n");
	}
	else if(strncmp(c, "spiral_print", strlen("spiral_print")) == 0)
	{
		printf("BFS Spiral Level-order Traversal of keys in BST:\n");
		bst_spiral_traversal(bst, print_bst_node_key_space, NULL);
		printf("\n");
	}
	else if(strncmp(c, "preorder_print", strlen("preorder_print")) == 0)
	{
		printf("Pre-Order Traversal of keys in BST:\n");
		bst_preorder_dfs_traversal(bst, print_bst_node_key);
	}
	else if(strncmp(c, "inorder_print", strlen("inorder_print")) == 0)
	{
		printf("In-Order Traversal of keys in BST:\n");
		bst_inorder_dfs_traversal(bst, print_bst_node_key);
	}
	else if(strncmp(c, "postorder_print", strlen("postorder_print"))== 0)
	{
		printf("Post-Order Traversal of keys in BST:\n");
		bst_postorder_dfs_traversal(bst, print_bst_node_key);
	}
	else if(strncmp(c, "path_print", strlen("path_print"))== 0)
	{
		printf("Printing all Root--Leaf paths in BST:\n");
		bst_dfs_path_print(bst);
	}
	else if(strncmp(c, "longest_path_print", strlen("longest_path_print"))== 0)
	{
		printf("Printing longest Root--Leaf path in BST:\n");
		bst_dfs_longest_path_print(bst);
	}
	else if(strncmp(c, "print_key", strlen("print_key"))== 0)
	{
		int key = 0;
		if((space = my_strstr_with_strlen(c, " ")))
			key = atoi(space + 1);
		print_bst_node_key_data(bst_get_node(bst, key));
	}
	else if(strncmp(c, "rand_insert", strlen("rand_insert")) == 0)
	{
		int i;
		int num_keys = 0;
		if((space = my_strstr_with_strlen(c, " ")))
			num_keys = atoi(space + 1);
		printf("Inserting %u random keys between (0 %d)\n",
			   num_keys, MAX_RAND_KEY);
		for(i = 0; i < num_keys; i++){
			int key;
			/* BST cannot handle duplicate nodes, so generate uniq rand */
			do {
				key = get_rand_int_limit(MAX_RAND_KEY);
			} while(bst_get_node(bst, key));
			if(bst_insert(bst, key, key) == false)
				printf("Error: BST Insert %d failed. BST Full?\n", key);
			printf("Info: Inserted %d into BST\n", key);
		}
	}
}

int main()
{
	bst = bst_init();
	init_rand();
	print_help_string();
	handle_CLI(single_char_CLI, single_char_CLI_with_num, multi_char_CLI);
	return 0;
}
