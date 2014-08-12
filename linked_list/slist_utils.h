/* Link list node */
struct node
{
	int data;
	struct node* next;
};

/* Untested - Function to reverse the linked list
 * Time Complexity  = O(n) */
static void reverse_iterative(struct node** head_ref)
{
	struct node* prev   = NULL;
	struct node* current = *head_ref;
	struct node* next;
	while (current != NULL)
	{
		next  = current->next;
		current->next = prev;
		prev = current;
		current = next;
	}
	*head_ref = prev;
}
