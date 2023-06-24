#include "generic_tree.h"

g_tree_t *create_generic_tree(u64_t data_size, void (*free_func)(void *))
{
	g_tree_t *new_tree = (g_tree_t *)malloc(sizeof(g_tree_t));
	DIE(!new_tree, MEMFAIL);

	new_tree->data_size = data_size;
	new_tree->free_func = free_func;
	new_tree->keys_no = 0;
	new_tree->root = NULL;

	return new_tree;
}

g_node_t *init_tnode(char key, state_t end_of_word, s32_t freq)
{
	g_node_t *new_node = (g_node_t *)malloc(sizeof(g_node_t));
	DIE(!new_node, MEMFAIL);

	new_node->data  = (key_t *)malloc(sizeof(key_t));
	DIE(!new_node->data, MEMFAIL);

	((key_t *)new_node->data)->key = key;
	((key_t *)new_node->data)->ending = end_of_word;
	((key_t *)new_node->data)->freq = freq;

	/**
	 * All the nodes will be initialized with INF distance, because it will
	 * help to find the shortest key, and we won't have to take care if a node
	 * isn't and ending node
	 */
	((key_t *)new_node->data)->key_len = INF;

	new_node->children = (g_node_t **)malloc(ALPH * sizeof(g_node_t *));
	DIE(!new_node->children, MEMFAIL);

	/**
	 * Set all possible children to NULL
	 */
	for (unsigned int i = 0; i < ALPH; i++)
		new_node->children[i] = NULL;

	new_node->children_num = 0;

	/**
	 * The link with the parent will be made inside the function that request
	 * the creation of this node
	 */
	new_node->parent = NULL;

	return new_node;
}

void init_trie(g_tree_t *tree)
{
	g_node_t *root = init_tnode('\0', ROOT, -1);

	/**
	 * The Root doesn't have a parent, it is the parent of all the possible
	 * keys.
	 */
	root->parent = NULL;

	tree->root = root;
}

void free_tnode(void *data)
{
	free(data);
}

void free_trie(g_node_t *root, void (*free_func)(void *))
{
	/**
	 * Free all the node's children
	 */
	for (unsigned int i = 0; i < ALPH; i++) {
		if (root->children[i])
			free_trie(root->children[i], free_func);
	}

	/**
	 * Free the node itself
	 */
	free(root->children);
	free_func(root->data);
	free(root);
}

void insert_key(g_node_t *root, char *key_ptr, size_t key_len)
{
	/**
	 * If the pointer to the string reaches a '\0', it means that it met the
	 * end of the string at the previous iteration, and the root is actually
	 * the ending letter.
	 *
	 */
	if (*key_ptr == '\0') {
		((key_t *)root->data)->ending = END;
		((key_t *)root->data)->freq = 1;
		((key_t *)root->data)->key_len = key_len;
		return;
	}

	/**
	 * Transform the letter into an index
	 */
	char c = key_ptr[0];
	unsigned int idx = c - 'a';

	/**
	 * Create a new node if it doesn't exist
	 */
	if (!root->children[idx]) {
		root->children[idx] = init_tnode(c, NOT_END, 0);
		root->children[idx]->parent = root;
		root->children_num++;
	}

	/**
	 * Call the function for the next letter and the corresponding child.
	 */
	key_ptr++;
	insert_key(root->children[idx], key_ptr, key_len);
}

void insert_and_update_trie(g_tree_t *trie, char *key)
{
	char *key_ptr = key;
	g_node_t *key_node = get_ending_node(trie->root, key_ptr);
	if (key_node) {
		((key_t *)key_node->data)->freq++;
		return;
	}

	key_ptr = key;
	insert_key(trie->root, key_ptr, strlen(key));
	trie->keys_no++;
}

u8_t has_key(g_node_t *root, char *key_ptr)
{
	state_t is_end = ((key_t *)root->data)->ending;
	if (*key_ptr == '\0' && is_end == END)
		return 1;
	else if (*key_ptr == '\0' && is_end == NOT_END)
		return 0;

	char c = key_ptr[0];
	unsigned int idx = c - 'a';

	if (!root->children[idx])
		return 0;

	key_ptr++;
	return has_key(root->children[idx], key_ptr);
}

g_node_t *get_ending_node(g_node_t *root, char *key_ptr)
{
	state_t is_end = ((key_t *)root->data)->ending;

	/**
	 * Two possible base cases: if it reaches the end of the string, and it
	 * coincide with the END state of a node, and if it reaches the end of the
	 * string, but the state is NOT_END. In the second case, it means that the
	 * key doesn't exists in the trie.
	 *
	 */
	if (*key_ptr == '\0' && is_end == END)
		return root;
	else if (*key_ptr == '\0' && is_end == NOT_END)
		return NULL;

	char c = key_ptr[0];
	unsigned int idx = c - 'a';

	/**
	 * Another base case: if the children of the node isn't allocated, it means
	 * that the key wasn't introduced or it has been removed
	 *
	 */
	if (!root->children[idx])
		return NULL;

	key_ptr++;

	return get_ending_node(root->children[idx], key_ptr);
}

void remove_key(g_node_t *end, void (*free_func)(void *))
{
	/**
	 * If it reaches the trie's root, I don't want it to be deleted, so I have
	 * to come back from recursion.
	 *
	 */
	if (((key_t *)end->data)->ending == ROOT)
		return;

	/**
	 * If the node has children, I don't want to delete it at all, because the
	 * character is probablly used by another word. Just set the state to
	 * NOT_END, and the key_len to INF, to ignore the key at searches.
	 *
	 */
	if (end->children_num != 0) {
		((key_t *)end->data)->ending = NOT_END;
		((key_t *)end->data)->key_len = INF;
		return;
	}

	g_node_t *parent = end->parent;

	/**
	 * Turn the letter into an index
	 */
	char temp_c = ((key_t *)end->data)->key;
	unsigned int idx = temp_c - 'a';

	/**
	 * One child will be deleted, so update the number of childs
	 */
	parent->children_num--;

	free_func(end->data);
	free(end->children);
	free(end);

	parent->children[idx] = NULL;

	/**
	 * If there were 2 overlapping words, I have to stop when it reaches one
	 * ending of a word.
	 */
	if (((key_t *)parent->data)->ending == END)
		return;

	remove_key(parent, free_func);
}

void remove_and_update_trie(g_tree_t *trie, char *key)
{
	char *key_ptr = key;
	g_node_t *end = get_ending_node(trie->root, key_ptr);

	if (!end)
		return;

	remove_key(end, trie->free_func);
	trie->keys_no--;
}

void load_file(g_tree_t *trie, char *filename)
{
	char buff[MAX_BUFF];
	FILE *file = fopen(filename, "rt");
	DIE(!file, "Couldn't open the file. Please try again\n");

	/**
	 * Read all the words and insert them using the functions made by now.
	 * Read until there is nothing left to read
	 */
	while (fscanf(file, "%s", buff) == 1)
		insert_and_update_trie(trie, buff);

	fclose(file);
}

void print_tree(g_node_t *root, char *buff, unsigned int buff_idx)
{
	/**
	 * If it reaches a leaf node end, print the infos and stop searching down
	 * in the trie
	 */
	if (root->children_num == 0) {
		buff[buff_idx] = '\0';
		printf("%s %d\n", buff, ((key_t *)root->data)->freq);
		return;
	}

	/**
	 * If it reaches and end that is not a leaf, print the infos and continue
	 * searching down in the trie
	 */
	if (((key_t *)root->data)->ending == END) {
		buff[buff_idx] = '\0';
		printf("%s %d\n", buff, ((key_t *)root->data)->freq);
	}

	/**
	 * Search for the next letter in the possible word
	 */
	for (unsigned int i = 0; i < ALPH; i++)
		if (root->children[i]) {
			char c = i + 'a';
			buff[buff_idx] = c;
			buff_idx++;
			print_tree(root->children[i], buff, buff_idx);
			buff_idx--;
		}
}
