#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "structs.h"
#include "generic_tree.h"
#include "magic_keyboard.h"
#include "utils.h"

/**
 * @brief Associates an unique id to some specific strings. If the string
 * hasn't an associated id, it gives back 0, meaning that it is an
 * unrecognized command / invalid command. Because there are few strings and
 * we don't need many ids the function returns an 8-bit unsigned integer.
 *
 * @param string The string we want to associate and id.
 * @return u8_t The id associated, or 0 in case the string isn't in the list.
 */
u8_t parse_input(char *string)
{
	if (strncmp(string, "INSERT", 6) == 0)
		return 1;

	if (strncmp(string, "LOAD", 4) == 0)
		return 2;

	if (strncmp(string, "REMOVE", 6) == 0)
		return 3;

	if (strncmp(string, "AUTOCORRECT", 11) == 0)
		return 4;

	if (strncmp(string, "AUTOCOMPLETE", 12) == 0)
		return 5;

	if (strncmp(string, "EXIT", 4) == 0)
		return 6;

	return 0;
}

int main(void)
{
	char input[MAX_IN], string[MAX_STR], buff[MAX_BUFF];
	unsigned int k, found;
	u8_t id;
	g_tree_t *trie = create_generic_tree(sizeof(key_t), free_tnode);
	init_trie(trie);
	do {
		scanf("%s", input);
		id = parse_input(input);
		switch (id) {
		case 1:
			scanf("%s", string);
			insert_and_update_trie(trie, string);
			break;
		case 2:
			scanf("%s", string);
			load_file(trie, string);
			break;
		case 3:
			scanf("%s", string);
			remove_and_update_trie(trie, string);
			break;
		case 4:
			scanf("%s", string);
			scanf("%u", &k);
			/**
			 * This works as an reset for the buffer. If the functions is
			 * called multiple times, it will have characters from the
			 * previous calling, so putting '\0' at the start will make the
			 * buffer act as an empty one. The old characters will be
			 * overwritten
			 */
			buff[0] = '\0';
			found = 0;
			search_kdiff_words(trie->root, buff, 0, string, strlen(string),
							   k, &found);
			if (found == 0)
				printf("No words found\n");
			break;
		case 5:
			scanf("%s", string);
			scanf("%u", &k);
			g_node_t *prefix_end = get_end_of_prefix(trie->root, string, 0);

			if (k == 1 || k == 0)
				print_most_lexic(prefix_end, string);
			if (k == 2)
				print_shortest_key(prefix_end);
			if (k == 3)
				print_maxfreq_key(prefix_end);
			if (k == 0)
				print_parallel_search_result(prefix_end);
			break;
		case 6:
			free_trie(trie->root, trie->free_func);
			free(trie);
			break;
		default:
			break;
		}

	} while (id != 6);

	return 0;
}
