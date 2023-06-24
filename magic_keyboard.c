#include "magic_keyboard.h"

u8_t k_different_word(char *word1, size_t word1_len, char *word2,
					  size_t word2_len, unsigned int k)
{
	/**
	 * Take the minimum word size
	 */
	size_t length;
	if (word1_len < word2_len)
		length = word1_len;
	else
		length = word2_len;

	unsigned int count = 0;

	/**
	 * Count the different characters
	 */
	for (size_t i = 0; i < length; i++) {
		if (word1[i] ^ word2[i])
			count++;
	}

	if (count <= k)
		return 1;

	return 0;
}

void search_kdiff_words(g_node_t *root, char *buff, size_t bufflen, char *word,
						size_t wordlen, unsigned int k, unsigned int *found)
{
	/**
	 * If the words are already too different, it should stop searching on
	 * that branch
	 */
	u8_t difference = k_different_word(buff, bufflen, word, wordlen, k);
	if (difference == 0)
		return;

	/**
	 * If there are bigger words, don't search through them
	 */
	if (bufflen > wordlen)
		return;

	/**
	 * If the current key is an ending, print it if matches
	 */
	if (((key_t *)root->data)->ending == END) {
		buff[bufflen] = '\0';

		if (difference == 1 && bufflen == wordlen) {
			printf("%s\n", buff);
			*found = *found + 1;
			return;
		}
	}

	/**
	 * Search through the all possible combinations
	 */
	for (unsigned int i = 0; i < ALPH; i++) {
		if (root->children[i]) {
			char c = i + 'a';
			buff[bufflen] = c;

			search_kdiff_words(root->children[i], buff, bufflen + 1, word,
							   wordlen, k, found);
		}
	}
}

u8_t check_prefix(g_node_t *root, char *prefix, unsigned int prefix_idx)
{
	char c = prefix[prefix_idx];

	/**
	 * If it succeded to make it through, to the end of the string,
	 * it means the prefix exists
	 */
	if (c == '\0')
		return 1;

	/**
	 * If the specific children isn't allocated, there is no chance the prefix
	 * exists
	 */
	if (!root->children[c - 'a'])
		return 0;

	prefix_idx++;
	return check_prefix(root->children[c - 'a'], prefix, prefix_idx);
}

g_node_t *get_end_of_prefix(g_node_t *root, char *prefix,
							unsigned int prefix_idx)
{
	/**
	 * The same as in the function from above, but with root and NULL, instead
	 * of 1 and 0
	 */
	char c = prefix[prefix_idx];
	if (c == '\0')
		return root;

	prefix_idx++;

	if (!root->children[c - 'a'])
		return NULL;

	return get_end_of_prefix(root->children[c - 'a'], prefix, prefix_idx);
}

void print_first_combination(g_node_t *root)
{
	/**
	 * Stop when it meets the first END node
	 */
	if (((key_t *)root->data)->ending == END)
		return;

	for (unsigned int i = 0; i < ALPH; i++) {
		/**
		 * Search for the first allocated children, then break, to find
		 * just the first combination
		 */
		if (root->children[i]) {
			char c = i + 'a';
			printf("%c", c);
			print_first_combination(root->children[i]);
			break;
		}
	}
}

void print_most_lexic(g_node_t *prefix_end, char *prefix)
{
	/**
	 * Check if the prefix exists. If it doesn't exists, the prefix_end
	 * pointer will be NULL
	 */
	if (!prefix_end) {
		printf("No words found\n");
		return;
	}

	/**
	 * Print the part of the key that we already have
	 */
	printf("%s", prefix);

	/**
	 * Print the rest of the key
	 */
	print_first_combination(prefix_end);
	printf("\n");
}

g_node_t *get_shortestdist_node(g_node_t *root, g_node_t *node)
{
	/**
	 * If it reaches the end of a key, return the node associated with the
	 * shortest distance
	 */
	if (((key_t *)root->data)->ending == END) {
		if (((key_t *)root->data)->key_len < ((key_t *)node->data)->key_len)
			node = root;

		return node;
	}

	for (unsigned int i = 0; i < ALPH; i++) {
		if (root->children[i])
			node = get_shortestdist_node(root->children[i], node);
	}

	return node;
}

void print_shortest_key(g_node_t *prefix_end)
{
	if (!prefix_end) {
		printf("No words found\n");
		return;
	}

	g_node_t *key_end = get_shortestdist_node(prefix_end, prefix_end);

	char buff[MAX_BUFF];
	print_word_from_end(key_end, buff, 0);
}

g_node_t *get_maxfrequency_node(g_node_t *root, g_node_t *node)
{
	/**
	 * If it reaches a leaf, return the node associated with the maximum
	 * frequency key
	 */
	if (root->children_num == 0) {
		if (((key_t *)root->data)->freq > ((key_t *)node->data)->freq)
			node = root;

		return node;
	}

	/**
	 * If is just the end of a word, continue searching, because it could have
	 * a bigger word overlapping
	 */
	if (((key_t *)root->data)->ending == END) {
		if (((key_t *)root->data)->freq > ((key_t *)node->data)->freq)
			node = root;
	}

	for (unsigned int i = 0; i < ALPH; i++) {
		if (root->children[i])
			node = get_maxfrequency_node(root->children[i], node);
	}

	return node;
}

void print_maxfreq_key(g_node_t *prefix_end)
{
	if (!prefix_end) {
		printf("No words found\n");
		return;
	}

	g_node_t *key_end = get_maxfrequency_node(prefix_end, prefix_end);

	char buff[MAX_BUFF];
	print_word_from_end(key_end, buff, 0);
}

void parallel_searching(g_node_t *root, g_node_t **shortest,
						g_node_t **frequent)
{
	/**
	 * This functions combines the get_shortestdist_node with the
	 * get_maxfrequency_node, without coming with new ideas. The only
	 * difference is that it comes back from recursion only if it reaches
	 * a leaf
	 */

	s32_t curr_freq = ((key_t *)root->data)->freq;
	size_t curr_len = ((key_t *)root->data)->key_len;

	size_t shortest_len = ((key_t *)((*shortest)->data))->key_len;
	s32_t max_freq = ((key_t *)((*frequent)->data))->freq;

	if (root->children_num == 0) {
		if (curr_freq > max_freq)
			*frequent = root;

		if (curr_len < shortest_len)
			*shortest = root;

		return;
	}

	if (((key_t *)root->data)->ending == END) {
		if (curr_freq > max_freq)
			*frequent = root;

		if (curr_len < shortest_len)
			*shortest = root;
	}

	for (unsigned int i = 0; i < ALPH; i++) {
		if (root->children[i])
			parallel_searching(root->children[i], shortest, frequent);
	}
}

void print_parallel_search_result(g_node_t *prefix_end)
{
	if (!prefix_end) {
		printf("No words found\n");
		printf("No words found\n");
		return;
	}

	g_node_t *shortest = prefix_end;
	g_node_t *frequent = prefix_end;
	parallel_searching(prefix_end, &shortest, &frequent);

	char buff[MAX_BUFF];
	print_word_from_end(shortest, buff, 0);
	print_word_from_end(frequent, buff, 0);
}

void print_word_from_end(g_node_t *end, char *buff, unsigned int buff_idx)
{
	/**
	 * If it reaches the root, the complete word is in buffer, so I should add
	 * '\0' at the end, and print it in reverse order
	 */
	if (((key_t *)end->data)->ending == ROOT) {
		buff[buff_idx] = '\0';
		str_print_reverse(buff, buff_idx);

		return;
	}

	char c = ((key_t *)end->data)->key;
	buff[buff_idx] = c;
	buff_idx++;

	/**
	 * Go from parent to parent until it reaches the root
	 */
	print_word_from_end(end->parent, buff, buff_idx);
}

void str_print_reverse(char *buff, unsigned int buff_idx)
{
	/**
	 * Print char by char from right to left (I mean from the end to
	 * the begging)
	 */
	while (buff_idx > 0) {
		printf("%c", buff[buff_idx - 1]);
		buff_idx--;
	}

	printf("\n");
}
