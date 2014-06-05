#include <stdio.h>
#include <stdbool.h>
#include <malloc.h>

struct node
{
  int data;
  struct node *left;
  struct node *right;
};

static bool bst_recurse_lookup(struct node *n, int val)
{
  if(n == NULL)
    return false;
  
  if(n->data < val)
    return bst_recurse_lookup(n->left, val);
  else if(n->data > val)
    return bst_recurse_lookup(n->right, val);
  else
    return true;
}

 
/* Print nodes at a given level */
void printGivenLevel(struct node* root, int level)
{
  if(root == NULL)
    return;
  if(level == 1)
    printf("%d ", root->data);
  else if (level > 1)
    {
      printGivenLevel(root->left, level-1);
      printGivenLevel(root->right, level-1);
      printf(" ");
    }
}

/* Function to print level order traversal a tree*/
void printLevelOrder(struct node* root)
{
  int h = height(root);
  int i, j;
  for(i=1; i<=h; i++)
  {
    for(j = h; j > h/2;  j--) printf(" ");
    printGivenLevel(root, i);
    printf("\n");
  }
}     

 
/* Compute the "height" of a tree -- the number of
    nodes along the longest path from the root node
    down to the farthest leaf node.*/
int height(struct node* node)
{
  if (node==NULL)
    return 0;
  else
    {
      /* compute the height of each subtree */
      int lheight = height(node->left);
      int rheight = height(node->right);
 
      /* use the larger one */
      if (lheight > rheight)
	return(lheight+1);
      else return(rheight+1);
    }
} 
 


int main(int argc, char **argv)
{
  struct node *head = NULL;
  if(argc != 2)
  {
    printf("Usage : %s <number_of_elements>\n", argv[0]);
    return -1;
  }
  int n = atoi(argv[1]);
  int val, i;
  printf("Please input %d numbers\n", n);
  for(i = 0; i < n; i++)
  {
    scanf("%d", &val);
    head = insert(head, val);
  }
  printLevelOrder(head);
  return 0;
}
