#ifndef _DY_BST_DS_
#define _DY_BST_DS_
#include <stdio.h>
#include <stdlib.h>        /* atoi  */
#include <stdbool.h>       /* bool, true, false */
#include <ctype.h>         /* isspace */
#include <string.h>        /* memset */
#include <assert.h>        /* assert */
#include <limits.h>        /* INT_MAX */
#include <malloc.h>        /* malloc, free */
#include "compiler_api.h"  /* likely, MAX */
#ifndef BST_OBJ_TYPE
#define BST_OBJ_TYPE int
#endif
#ifndef BST_PRINT_DATA_TYPE
#define BST_PRINT_DATA_TYPE(_data) printf("%d",(_data))
#endif
#define BST_INVALID_KEY 0xDEADDEAD
#define DQ_OBJ_TYPE BST_OBJ_TYPE
#include "dy_queue.h"      /* Dynamic Queue API  */
#define DY_STACK_OBJ_TYPE BST_OBJ_TYPE
#include "dy_stack.h"      /* Dynamic Stack API  */

typedef struct bst_stats
{
	unsigned ins_cnt;
	unsigned del_cnt;
	unsigned dup_ins_cnt;
}bst_stats_t;

typedef struct bst_node
{
	int key;
	dq_t *data_q;
	int cnt; /* Maintain count of number of nodes under this node */
	struct bst_node *left;
	struct bst_node *right;
}bst_node_t;

typedef struct binary_search_tree
{
	bst_node_t *root;
	bst_stats_t stats;
}bst_t;

/*--------------------Helper Functions(Start)--------------------*/
/* Get #keys in BST. Time Complexity = O(1) */
static inline unsigned bst_size_api(bst_node_t *x)
{
	if(x == NULL) return 0;
	else		  return x->cnt;
}
/* Calculate #Keys after Insert/Del. Time Complexity = O(1) */
static inline unsigned bst_size_calc_api(bst_node_t *x)
{
	return (1 + bst_size_api(x->left) + bst_size_api(x->right));
}
/* Get #Levels in BST(zero-based).  Time Complexity = O(lg n) */
static inline int bst_height_api(bst_node_t *x)
{
	if(x == NULL) return -1;
	else
		return (1+MAX(bst_height_api(x->left),bst_height_api(x->right)));
}
static inline bst_node_t *bst_node_create_api(int key, BST_OBJ_TYPE data)
{
	bst_node_t *x = (bst_node_t *) malloc(sizeof(bst_node_t));
	if(unlikely(!x)) return NULL;
	x->data_q = dq_init();
	if(unlikely(!x->data_q)) {
		free(x);
		return NULL;
	}
	x->left = x->right = NULL;
	dq_enq(x->data_q, data);
	x->key = key;
	x->cnt = 1;
	return x;
}
static inline void bst_node_destroy_api(bst_node_t *x)
{
	if(!x) return;
	if(x->data_q) dq_destroy(x->data_q);
	free(x);
}
static inline void print_bst_node_data_comma(BST_OBJ_TYPE *data)
{BST_PRINT_DATA_TYPE(*data); printf(" ,");}
static inline void print_bst_node_key_comma(int *key)
{if(key)printf("%d, ", *key);}
static inline void bst_node_print_data_iterator(bst_node_t *x)
{if(x)dq_iterator(x->data_q, dq_print_element);}
static inline void print_bst_node_key(bst_node_t *x)
{if(x)printf("%d\n", x->key);}
static inline void print_bst_node_key_space(bst_node_t *x)
{if(x)printf("%d  ", x->key);}
static inline void print_bst_node_key_data(bst_node_t *x)
{
	if(!x) return;
	printf("----------------------------------------------\n");
	printf("key = "); print_bst_node_key(x);
	printf("Data:\n");
	dq_iterator(x->data_q, print_bst_node_data_comma);
	printf("\n----------------------------------------------\n");
}
static inline bool is_bst_node_root(bst_t *bst, bst_node_t *x)
{
	if(bst && x && (bst->root->key == x->key))return true;
	else                                      return false;
}
static inline bool is_bst_node_leaf(bst_node_t *x)
{
	if(x->left == NULL && x->right == NULL)   return true;
	else                                      return false;
}
static inline bool bst_node_get_data(bst_node_t *x, dq_t **data)
{
	if(x){
		if(data) *data = x->data_q;
		return true;
	}
	else return false;
}
static inline bool bst_node_get_key(bst_node_t *x, int *key)
{
	if(x){
		if(key) *key = x->key;
		return true;
	}
	else return false;
}
/*----------------------Helper Functions(End)----------------------*/
/*--------------------Internal Functions(Start)--------------------*/
static inline bst_t *bst_init_impl()
{
	bst_t *bst = (bst_t *)malloc(sizeof(bst_t));
	if(bst == NULL) return NULL;
	memset(bst, 0, sizeof(bst_t));
	return bst;
}
/* Assume that the current node’s value is k. Then for each node,*
 * check if all nodes of left subtree contain values that are    *
 * less than k. Also check if all nodes of right subtree contain *
 * values that are greater than k. If all of the nodes satisfy   *
 * this property, then it must be a BST.                         *
 * Note: Duplicate keys are allowed (BST considered valid). If   *
 * you want to disallow duplicate keys, use below comparison     *
 *      if(min && x->key <= *min) return false;                  *
 *      if(max && x->key >= *max) return false;                  */
static inline bool is_BST_impl(bst_node_t *x, int *min, int *max)
{
	if(x == NULL) return true;
	if(min && x->key < *min) return false;
	if(max && x->key > *max) return false;
	else
		return (is_BST_impl(x->left,  min, &x->key) &&
				is_BST_impl(x->right, &x->key, max));
}
static inline bst_node_t *bst_get_min_node_impl(bst_node_t *x)
{
	if(x == NULL || x->left == NULL) return x;
	else                      return bst_get_min_node_impl(x->left);
}
static inline bst_node_t *bst_get_max_node_impl(bst_node_t *x)
{
	if(x == NULL || x->right == NULL) return x;
	else                      return bst_get_max_node_impl(x->right);
}
/* Get #keys less than given key.                           *
 * This will work even if key is not in BST                 */
static inline int bst_get_rank_impl(bst_node_t *x, int key)
{
	if(x == NULL)          return 0;
	else if(key < x->key)  return bst_get_rank_impl(x->left, key);
	else if(key == x->key) return bst_size_api(x->left);
	else
		return 1 + bst_size_api(x->left)+bst_get_rank_impl(x->right,key);
}
/* Get the lowest Common parent given 2 keys */
static inline bst_node_t *bst_get_lcp_node_impl(bst_node_t *x, int key1,
												int key2)
{
	/* Both keys are smaller than current node, go left */
	/* Both keys are larger than current node, go right */
	/* key1<x.key<key2 : This is the lowest common parent */
	if(x == NULL) return x;
	else if(x->key > key1 && x->key > key2)
		return bst_get_lcp_node_impl(x->left, key1, key2);
	else if(x->key < key1 && x->key < key2)
		return bst_get_lcp_node_impl(x->right, key1, key2);
	else
		return x;
}
static inline bst_node_t *bst_get_node_impl(bst_node_t *x, int key)
{
	while(x){
		if(key < x->key)      x = x->left;
		else if(key > x->key) x = x->right;
		else                  return x;
	}
	return NULL;
}
/* Return height of a node i.e., its row number (zero-based) */
static inline int bst_get_node_height_impl(bst_t *bst, int key)
{
	int height = 0;
	bst_node_t *x = bst->root;
	while(x)
	{
		if(key < x->key)      x = x->left;
		else if(key > x->key) x = x->right;
		else                  break;
		height++;
	}
	return height;
}
/* Assumes key is present in BST and gets immediate parent for key */
static inline bst_node_t *bst_get_parent_node_impl(bst_node_t *x,
												   int key)
{
	bst_node_t *parent = NULL;
	while(x){
		if(key == x->key)     break;
		else                  parent = x;	
		if(key < x->key)      x = x->left;
		else if(key > x->key) x = x->right;
	}
	return parent;
}
/* Find the node immediately smaller than given node.               *
 * Note: The given key may or may-not be present in the BST         */
static inline bst_node_t *bst_get_floor_node_impl(bst_node_t *x, int key)
{
	if(x == NULL)          return NULL;
	else if(x->key >= key) return bst_get_floor_node_impl(x->left, key);
	/* If Current Key is less than user key, then two possibilities *
     * 1) Current node is the floor                                 *
	 * 2) Floor resides on right-subtree. First scan right sub-tree */
	bst_node_t *t = bst_get_floor_node_impl(x->right, key);
	if(t)                  return t;
	else                   return x;
}
/* Find the node immediately larger than given node                 *
 * Note: The given key may or may-not be present in the BST         */
static inline bst_node_t *bst_get_ceil_node_impl(bst_node_t *x, int key)
{
	if(x == NULL)          return NULL;
	else if(x->key <= key) return bst_get_ceil_node_impl(x->right, key);
	/* If x->key > key, then there are two possibilities            *
     * 1) Current node is the ceil                                  *
	 * 2) Ceil resides on left-subtree. First scan left sub-tree    */
	bst_node_t *t = bst_get_ceil_node_impl(x->left, key);
	if(t)                  return t;
	else                   return x;
}
static inline bst_node_t *bst_insert_node_impl(bst_t *bst, bst_node_t *x,
											 int key, BST_OBJ_TYPE data)
{
	if(x == NULL)
		return bst_node_create_api(key, data);
	else if(key < x->key)
		x->left = bst_insert_node_impl(bst, x->left, key, data);
	else if(key > x->key)
		x->right= bst_insert_node_impl(bst, x->right, key, data);
	else {
		if(dq_count(x->data_q) >= 1)
			bst->stats.dup_ins_cnt++;
		dq_enq(x->data_q, data);
	}
	x->cnt = bst_size_calc_api(x);
	return x;
}
/* When Min-node is removed, BST property might get dis-satisfied    *
 * Satisfy BST property at min-node's parent and propagate it back   */
static inline bst_node_t *bst_delete_min_node_ensure_bst_property(
	bst_node_t *x)
{
	if(x == NULL) return NULL;
	if(x->left == NULL) return x->right;
	x->left = bst_delete_min_node_ensure_bst_property(x->left);
	x->cnt = bst_size_calc_api(x);
	return x;
}
/* Use Hibbard deletion method to delete a node                      *
 * Note: This method may cause BST to become less balanced           */
static inline bst_node_t *bst_delete_node_impl(bst_node_t *x, int key)
{
	if(x == NULL) return NULL;
	else if(key < x->key) x->left =bst_delete_node_impl(x->left, key);
	else if(key > x->key) x->right=bst_delete_node_impl(x->right, key);
	else
	{
/* We've identified the node 'x' to be deleted now.  3 possibilities  *
 * 0 child: Set parent link to null and go back updating counts       *
 * 1 child: Set parent link to child link and go back updating counts *
 * 2 child: Find min key 'm' in right-sub-tree.                       *
          : Delete 'x', replace it with 'm' & go back updating counts */
		if(x->right == NULL)/* Takes care of 1st 2 cases of Hibbard  */
			return x->left;
		if(x->left == NULL) /* Takes care of 1st 2 cases of Hibbard  */
			return x->right;
		/* Take care of 3rd case of Hibbard Deletion */
		bst_node_t *old_x = x;
		x = bst_get_min_node_impl(x->right);
		x->right = bst_delete_min_node_ensure_bst_property(old_x->right);
		x->left = old_x->left;
		bst_node_destroy_api(old_x);
	}
	x->cnt = bst_size_calc_api(x);
	return x;
}
static inline void bst_preorder_traversal_impl(bst_node_t *x,
											  void (*fn)(bst_node_t *x))
{
	if(x && fn)
	{
		fn(x);
		bst_preorder_traversal_impl(x->left, fn);
		bst_preorder_traversal_impl(x->right, fn);
	}
}
static inline void bst_inorder_traversal_impl(bst_node_t *x,
											  void (*fn)(bst_node_t *x))
{
	if(x && fn)
	{
		bst_inorder_traversal_impl(x->left, fn);
		fn(x);
		bst_inorder_traversal_impl(x->right, fn);
	}
}
static inline void bst_postorder_traversal_impl(bst_node_t *x,
											  void (*fn)(bst_node_t *x))
{
	if(x && fn)
	{
		bst_postorder_traversal_impl(x->left, fn);
		bst_postorder_traversal_impl(x->right, fn);
		fn(x);
	}
}
static inline void bst_dfs_path_print_impl(bst_node_t *x,
										   dstack_t *p_stack)
{
	int key;
	if(x == NULL)  return;
	dy_stack_push(p_stack, x->key);
	if(is_bst_node_leaf(x))
	{
		dy_stack_reverse_iterator(p_stack, print_bst_node_key_comma);
		printf("\n");
	}
	else
	{
		bst_dfs_path_print_impl(x->left, p_stack);
		bst_dfs_path_print_impl(x->right, p_stack);
	}
	dy_stack_pop(p_stack, &key);
}
static inline void bst_dfs_longest_path_print_impl(bst_node_t *x,
												   dstack_t *p_stack)
{
	int key;
	if(x == NULL) return;
	dy_stack_push(p_stack, x->key);
	if(is_bst_node_leaf(x))
	{
		dy_stack_reverse_iterator(p_stack, print_bst_node_key_comma);
		printf("\n");
	}
	else
	{
		bst_node_t *next;
		if(x->left == NULL)		   	             next = x->right;
		else if(x->right == NULL)                next = x->left;
		else
		{
			if(x->left->cnt < x->right->cnt)     next = x->right;
			else if(x->left->cnt > x->right->cnt)next = x->left;
			else{
				next = x->right; 
				bst_dfs_longest_path_print_impl(x->left, p_stack);
			}	
		}
		bst_dfs_longest_path_print_impl(next, p_stack);
	}
	dy_stack_pop(p_stack, &key);
}
/* BFS traversal implies Queue, DFS traversal implies Stack     *
 * This implementation is a bit more convoluted since Dynamic   *
 * Queue implementation does not support 2 types                */
static inline void bst_bfs_traversal_impl(bst_node_t *rt, void (*fn)
										  (bst_node_t *x), dq_t *dq)
{	
	int key;
	dq_t *bfs_traverse;
	if(rt == NULL) return; /* Empty BST, dont traverse */
	bfs_traverse = dq_init();
	dq_enq(bfs_traverse, rt->key);
	while(dq_deq(bfs_traverse, &key))
	{
		if(key == BST_INVALID_KEY)
		{
			if(dq)      dq_enq(dq, key);
			continue;
		}
		bst_node_t *x = bst_get_node_impl(rt, key);
		if(dq)      dq_enq(dq, key);
		if(fn)      fn(x);
		if(x->left) dq_enq(bfs_traverse, x->left->key);
		else        dq_enq(bfs_traverse, BST_INVALID_KEY);
		if(x->right)dq_enq(bfs_traverse, x->right->key);
		else        dq_enq(bfs_traverse, BST_INVALID_KEY);
	}
	dq_destroy(bfs_traverse);
}
/* Do a level order traversal of a given tree but print every   *
 * other level in reverse so that its like a snake traveral     *
 * This is an iterative solution with below complexity:         *
 * Time Complexity = O(n)                                       *
 * Space Complexity = O(2^h) (h is the height(0 based))         *
 * space complexity comes because max # nodes that each stack   *
 * would have to hold = maximum possible number of leaf nodes   *
 * Note: ignoring error checking as this is sample code and     *
 * readability is MY desired characteristic                     */
static inline void bst_spiral_traversal_impl(bst_node_t *rt, void (*fn)
											 (bst_node_t *x), dq_t *dq)
{
	int key;
	dstack_t *s1, *s2;
	bst_node_t *x;
	if(rt == NULL) return;   /* Empty BST, dont traverse        */
	if(fn == NULL) return;   /* Empty traverse fn, nothing to do*/
	s1 = dy_stack_init();    /* Right to Left push stack        */
	s2 = dy_stack_init();    /* Left to Right push stack        */
	dy_stack_push(s1, rt->key);
	while(is_dy_stack_empty(s1) == false || is_dy_stack_empty(s2) == false)
	{
		/* S1 includes nodes added in a right to left order     *
		 * therefore, if its children are pushed into a stack   *
		 * in right to left order, by virtue of stack (LIFO)    *
		 * those nodes can be accessed left to right            */
		while(is_dy_stack_empty(s1) == false) {
			dy_stack_pop(s1, &key);
			x = bst_get_node_impl(rt, key);
			fn(x);
			/* Note: Right pushed before left, need error checks*/
			if(x->right) dy_stack_push(s2, x->right->key);
			if(x->left)  dy_stack_push(s2, x->left->key);
		}
		printf("\n"); /* One level is complete, print new-line  */
		/* After above loop, s2 should now have all the elements*
		 * and s1 will be empty. Go over nodes in left to right *
		 * order and add any available children in a left-right *
		 * order, at end of this loop by virtue of stack (LIFO) *
		 * those nodes can be accessed from right to left       */
		while(is_dy_stack_empty(s2) == false) {
			dy_stack_pop(s2, &key);
			x = bst_get_node_impl(rt, key);
			fn(x);
			/* Note: Left pushed before right, need error checks*/
			if(x->left)  dy_stack_push(s1, x->left->key);
			if(x->right) dy_stack_push(s1, x->right->key);
		}
		printf("\n"); /* One level is complete, print new-line  */
	}
	dy_stack_destroy(s1);
	dy_stack_destroy(s2);
}
/* Do a level order traversal of a given tree but print levels  *
 * from bottom to top (level by level)                          *
 * This is an iterative solution with below complexity:         *
 * Time Complexity = O(n)                                       *
 * Space Complexity = O(n)                                      */
static inline void bst_bottom_up_traversal_impl(bst_node_t *rt, void (*fn)
												(bst_node_t *x), dq_t *dq)
{
	int key, level_size = 1, tree_size = bst_size_api(rt), idx = 0;
	dstack_t *len_st;
	int nodes[tree_size];    /* Hopefully, stack mem is enough  */
	bst_node_t *x;
	if(rt == NULL) return;   /* Empty BST, dont traverse        */
	if(fn == NULL) return;   /* Empty traverse fn, nothing to do*/
	len_st= dy_stack_init(); /* Init stack to store all lengths */
	nodes[idx++] = rt->key;
	dy_stack_push(len_st, level_size);
	/* Do a level order traversal and populate nodes + lengths  */
	while(idx < tree_size)
	{
		int i, cur_level_size = 0, base_idx = idx - level_size;
		for(i = 0; i < level_size; i++)
		{
			key = nodes[base_idx + i];
			x = bst_get_node_impl(rt, key);
			if(x->left) {
				nodes[idx++] = x->left->key;
				cur_level_size++;
			}
			if(x->right) {
				nodes[idx++] = x->right->key;
				cur_level_size++;
			}
		}
		/* the number of nodes in the level just processed      */
		level_size = cur_level_size;
		dy_stack_push(len_st, level_size);
	}
	/* For each level from back (STACK) dump all nodes in level */
	while(is_dy_stack_empty(len_st) == false)
	{
		int num, i;
		dy_stack_pop(len_st, &num); /* Skip error checks        */
		idx -= num;
		for(i = 0; i < num; i++) {
			x = bst_get_node_impl(rt, nodes[idx+i]);
			fn(x);
		}
		printf("\n");
	}
	dy_stack_destroy(len_st);
}
/*--------------------Internal Functions(End)--------------------*/
/*--------------------API Functions(start)--------------------*/
bst_t *bst_init()            {return bst_init_impl();}
unsigned bst_size(bst_t *bst){return bst_size_api(bst->root);}
int    bst_height(bst_t *bst){return bst_height_api(bst->root);}
bool is_bst_empty(bst_t *bst){return (bst->root == NULL);}
bool is_BST(bst_t *bst)
{
	return is_BST_impl(bst->root, NULL, NULL);
}
bool bst_is_root(bst_t *bst, int key)
{
	return is_bst_node_root(bst, bst->root);
}
bst_node_t *bst_get_node(bst_t *bst, int key)
{
	return bst_get_node_impl(bst->root, key);
}
/* Return height of a node i.e., its row number (zero-based) */
int bst_get_node_height(bst_t *bst, int key)
{
	return bst_get_node_height_impl(bst, key);
}
bool bst_get_root_key(bst_t *bst, int *key)
{
	return bst_node_get_key(bst->root, key);
}
bool bst_get_min_key(bst_t *bst, int *key)
{
	bst_node_t *x = bst_get_min_node_impl(bst->root);
	return bst_node_get_key(x, key);
}
bool bst_get_max_key(bst_t *bst, int *key)
{
	bst_node_t *x = bst_get_max_node_impl(bst->root);
	return bst_node_get_key(x, key);
}
bool bst_get_parent_key(bst_t *bst, int key, int *parent_key)
{
	bst_node_t *parent;
	/* First check if user given key is present in BST */
	if(bst_get_node(bst, key) == NULL)
		return false;
	parent = bst_get_parent_node_impl(bst->root, key);
	return bst_node_get_key(parent, parent_key);
}
bool bst_get_lowest_common_parent_key(bst_t *bst, int k1, int k2, int *p)
{
	bst_node_t *x =	bst_get_lcp_node_impl(bst->root, k1, k2);
	return bst_node_get_key(x, p);
}
bool bst_get_floor_key(bst_t *bst, int key, int *floor_key)
{
	bst_node_t *x =	bst_get_floor_node_impl(bst->root, key);
	return bst_node_get_key(x, floor_key);
}
bool bst_get_ceil_key(bst_t *bst, int key, int *ceil_key)
{
	bst_node_t *x =	bst_get_ceil_node_impl(bst->root, key);
	return bst_node_get_key(x, ceil_key);
}
bool bst_get_data(bst_t *bst, int key, dq_t **data_q)
{
	bst_node_t *x = bst_get_node(bst, key);
	return bst_node_get_data(x, data_q);
}
int bst_get_rank(bst_t *bst, int key)
{
	return bst_get_rank_impl(bst->root, key);
}
bool bst_insert(bst_t *bst, int key, BST_OBJ_TYPE data)
{
	bst->root = bst_insert_node_impl(bst, bst->root, key, data);
	return (bst->root != NULL);
}
bool bst_delete(bst_t *bst, int key)
{
	/* First Check if the Key is present in BST */
	if(bst_get_node(bst, key) == false) return false;
	/* Delete and update the Root node accordingly */
	bst->root = bst_delete_node_impl(bst->root, key);
	/* To be really really sure, check if key is still present */
	if(bst_get_node(bst, key) == false) return true;
	else 	                            return false;
}
void bst_bfs_traversal(bst_t *bst, void (*fn)(bst_node_t *x), dq_t *dq)
{
	bst_bfs_traversal_impl(bst->root, fn, dq);
}
void bst_spiral_traversal(bst_t *bst, void (*fn)(bst_node_t *x), dq_t *dq)
{
	bst_spiral_traversal_impl(bst->root, fn, dq);
}
void bst_bottom_up_traversal(bst_t *bst, void (*fn)(bst_node_t *x), dq_t *dq)
{
	bst_bottom_up_traversal_impl(bst->root, fn, dq);
}
void bst_preorder_dfs_traversal(bst_t *bst, void (*fn)(bst_node_t *x))
{
	bst_preorder_traversal_impl(bst->root, fn);
}
void bst_inorder_dfs_traversal(bst_t *bst, void (*fn)(bst_node_t *x))
{
	bst_inorder_traversal_impl(bst->root, fn);
}
void bst_postorder_dfs_traversal(bst_t *bst, void (*fn)(bst_node_t *x))
{
	bst_postorder_traversal_impl(bst->root, fn);
}
void bst_dfs_path_print(bst_t *bst)
{
	dstack_t *p_stack = dy_stack_init();
	if(p_stack == NULL) return;
	bst_dfs_path_print_impl(bst->root, p_stack);
	dy_stack_destroy(p_stack);
}
void bst_dfs_longest_path_print(bst_t *bst)
{
	dstack_t *p_stack = dy_stack_init();
	if(p_stack == NULL) return;
	bst_dfs_longest_path_print_impl(bst->root, p_stack);
	dy_stack_destroy(p_stack);
}
void bst_destroy(bst_t *bst)
{
	bst_postorder_dfs_traversal(bst, bst_node_destroy_api);
	free(bst);
}
/*--------------------API Functions(end)--------------------*/
#endif //_DY_BST_DS_
