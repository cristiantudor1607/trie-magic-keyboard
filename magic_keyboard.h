#ifndef MAGIC_KEYBOARD_H_
#define MAGIC_KEYBOARD_H_

#include <inttypes.h>
#include <string.h>
#include <stdlib.h>
#include "utils.h"
#include "structs.h"
#include "generic_tree.h"

/**
 * @brief Checks if 2 words are different by maximum k characters
 *
 * @param word1 One word
 * @param word1_len The length of the first word
 * @param word2 The second word
 * @param word2_len The length of the second word
 * @param k The maximum number of letter the words can differ
 * @return uint8_t Returns an 8-bit unsigned integer, 0, if the words differ
 * by more than k, or 1, otherwise
 */
uint8_t k_different_word(char *word1, size_t word1_len, char *word2,
						 size_t word2_len, unsigned int k);

/**
 * @brief Searches for k-different words in a trie.
 *
 * @param root The root of the trie
 * @param buff A temporary buffer, to store the words for printing
 * @param bufflen The actual buffer length (I mean how many characters contains
 * at a certain time)
 * @param word The word we want to find the k-different words
 * @param wordlen The word length
 * @param k The k number (maximum letters)
 * @param found The number of words found
 */
void search_kdiff_words(g_node_t *root, char *buff, size_t bufflen, char *word,
						size_t wordlen, unsigned int k, unsigned int *found);
/**
 * @brief Checks if a prefix exists in the trie.
 *
 * @param root The root of the trie
 * @param prefix The prefix we want to find if it exists
 * @param prefix_idx The index for the prefix. Because the function is
 * recursive, this parameter is needed to take letters 1 by 1. The function
 * should be called with this parameter set to 0.
 * @return uint8_t Returns an 8-bit unsigned integer, 1 if the prefix exists,
 * 0 if not.
 */
uint8_t check_prefix(g_node_t *root, char *prefix,  unsigned int prefix_idx);

/**
 * @brief Get the ending node of a prefix if it exists.
 *
 * @param root The root of the trie where we want to search the prefix.
 * @param prefix The prefix we want to search.
 * @param prefix_idx The index for prefix string. The function should be
 * called with this parameter set to 0. It is necessarily because the function
 * is recursive, and it needs it to take letters 1 by 1.
 * @return g_node_t* Returns the ending node if it exists, or NULL otherwise.
 */
g_node_t *get_end_of_prefix(g_node_t *root, char *prefix,
							unsigned int prefix_idx);

/**
 * @brief Prints the first word it find in a subtrie.
 *
 * @param root The root of the subtrie.
 */
void print_first_combination(g_node_t *root);

/**
 * @brief Prints the first lexicographic word that exists in the trie, with
 * the given prefix. It prints an error if there is no such a word.
 *
 * @param prefix_end The node where the prefix ends.
 * @param prefix The prefix as a string.
 */
void print_most_lexic(g_node_t *prefix_end, char *prefix);

/**
 * @brief Get the ending node of the shortest word in a subtrie.
 *
 * @param root The root of the subtrie.
 * @param node A prediction for the node with the shortest key length.
 * @return g_node_t* Returns the ending node of the shortest key in the
 * subtrie.
 */
g_node_t *get_shortestdist_node(g_node_t *root, g_node_t *node);

/**
 * @brief Prints the shortest key with the prefix ending with node prefix_end.
 *
 * @param prefix_end The ending node of the prefix
 */
void print_shortest_key(g_node_t *prefix_end);

/**
 * @brief Gets the node of the maximum frequency key.
 *
 * @param root The root of the subtrie where the node is searched.
 * @param node An initial prediction (it can be very bad, actually it can
 * be just the root), to have a starting point.
 * @return g_node_t* Returns the ending node of the key that was inserted the
 * most.
 */
g_node_t *get_maxfrequency_node(g_node_t *root, g_node_t *node);

/**
 * @brief Prints the key with the maximum frequency with a given prefix.
 *
 * @param prefix_end The node where the prefix ends.
 */
void print_maxfreq_key(g_node_t *prefix_end);

/**
 * @brief Performs the get_maxfrequency_node and get_shortestdist_node at the
 * same time, for a better time of execution, in case we need both.
 *
 * @param root The root of the subtrie where we search.
 * @param shortest Address of a node where to store the shortest key node.
 * @param frequent Address of a node where to store the frequent key node.
 */
void parallel_searching(g_node_t *root, g_node_t **shortest,
						g_node_t **frequent);

/**
 * @brief Prints the result of parallel searching.
 *
 * @param prefix_end The node where the prefix ends, the point where the
 * parallel searching starts.
 */
void print_parallel_search_result(g_node_t *prefix_end);

/**
 * @brief Prints a string in reverse order.
 *
 * @param buff The buffer where the string is stored.
 * @param buff_idx The buffer / string length, but it is used as an decreasing
 * index.
 */
void str_print_reverse(char *buff, unsigned int buff_idx);

/**
 * @brief Prints a word starting from the ending node of the word.
 *
 * @param end The ending node of the key.
 * @param buff A temporarily buffer to store the key.
 * @param buff_idx The index for the buffer.
 */
void print_word_from_end(g_node_t *end, char *buff, unsigned int buff_idx);
#endif  // MAGIC_KEYBOARD_H_
