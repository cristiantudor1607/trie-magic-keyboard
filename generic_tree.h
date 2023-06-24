#ifndef GENERIC_TREE_H_
#define GENERIC_TREE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "structs.h"
#include "utils.h"

/**
 * @brief Creates a generic tree structure.
 *
 * @param data_size The size of the data that will be stored in nodes.
 * @param free_func The function that will free the data stored in nodes.
 * @return g_tree_t* The newly created generic tree, with uninitalized root.
 */
g_tree_t *create_generic_tree(u64_t data_size, void (*free_func)(void *));

/**
 * @brief Initialize a trie node.
 *
 * @param key The character to store in the node.
 * @param end_of_word The state of the node. It can be END, NOT_END, ot ROOT.
 * This will help later. It is useful because some words can overlap.
 * @param freq The frequency of the character. Actually, it is an optional
 * feature, it is never used at all.
 * @return g_node_t* The newly created node, with the given parameters.
 */
g_node_t *init_tnode(char key, state_t end_of_word, s32_t freq);

/**
 * @brief Initializes the root of a trie, and set the params of the generic
 * tree. It initializez the root of the generic tree  with a '\0' key, a ROOT
 * state, and -1 frequency. This will help at searches, because the root will
 * not influence the maximum frequency, and it won't be considered as the end
 * of a word.
 *
 * @param tree The generic tree created by the create_generic_tree function,
 * that we want to be a trie.
 */
void init_trie(g_tree_t *tree);

/**
 * @brief The function that frees a key_t structure. It is actually just
 * a simple free.
 *
 * @param data The data that we want to free. It should be a key_t structure
 * dynamically allocated inside a node, but it would work for other types of
 * data, too.
 */
void free_tnode(void *data);

/**
 * @brief Frees all the nodes, and the data stored in a trie.
 *
 * @param root The root of the trie we want to delete.
 * @param free_func The function that frees the data within a node.
 */
void free_trie(g_node_t *root, void (*free_func)(void *));

/**
 * @brief Insert a node into a subtrie starting at node given as first
 * parameter. It is designed to start from the root of the trie, but it
 * should work for subtries too.
 *
 * @param root The root of subtrie / trie where we want to add the key
 * @param key_ptr A pointer to the current letter in the key buffer.
 * It should be positioned at the begining of the string.
 * @param key_len The length of the actual key (I mean the length of the word
 * we want to insert). It will help when we'll try to search the shortest word.
 */
void insert_key(g_node_t *root, char *key_ptr, size_t key_len);

/**
 * @brief Inserts a given key into the trie structure. If they key already
 * exists, it increases the frequency of that key. Also, the number of keys,
 * I mean the keys_no field of trie structure is incresed if the key is a new
 * one.
 *
 * @param tree The trie where we want to insert the key.
 * @param key The key that needs to be inserted.
 */
void insert_and_update_trie(g_tree_t *tree, char *key);

/**
 * @brief Checks if a trie contains a specific key. This functions is extra, in
 * the final version of the program it is not used, but it helped me in the
 * past while testing.
 *
 * @param root The root of the trie / subtrie where we want to check the key.
 * It was designed to start from the root of the trie, but I think it will
 * work with subtries, too.
 * @param key_ptr A poiner to the buffer where the key is stored. The function
 * was designed with a pointer at the begging of the string.
 * @return u8_t It returns an 8-bit unsigned integer, that acts as a boolean
 * value, 0, if the trie doesn't contain the key, or 1, otherwise.
 */
u8_t has_key(g_node_t *root, char *key_ptr);

/**
 * @brief Get the ending node of a key
 *
 * @param root The root of the trie, where the search process is starting.
 * @param key_ptr A pointer set to the begginig of the buffer where the key
 * is temporarily stored, for this purpose.
 * @return g_node_t* Returns the ending node of the key, if the key exists (and
 * it is not necessary to be a leaf, because, again, some words overlap), or
 * NULL otherwise.
 */
g_node_t *get_ending_node(g_node_t *root, char *key_ptr);

/**
 * @brief Remove the key that ends with the "end" node given as parameter. It
 * is guaranteed that end has the END state, and it is not NULL.
 *
 * @param end The node where the key we want to delete ends.
 * @param free_func The function that frees the data within a node.
 */
void remove_key(g_node_t *end, void (*free_func)(void *));

/**
 * @brief This functions solves the problem that the remove_key function has,
 * it discover the end of the key, and test if it is a valid node. If it's
 * NULL, it won't do anything, otherwise, it will call the function that
 * removes a key. Also, it updates the number of keys from the structure.
 *
 * @param trie The trie where the key should be stored.
 * @param key The key that we want to remove.
 */
void remove_and_update_trie(g_tree_t *trie, char *key);

/**
 * @brief Loads all the words from a file into a trie. It is guaranteed that
 * the file is well-formated, containing just lowercase and delimiters such as
 * space, enter, dot, and so on.
 *
 * @param trie The trie where we want to store the words.
 * @param filename The name of the file we want to load.
 */
void load_file(g_tree_t *trie, char *filename);

/**
 * @brief Prints all the words stored in a trie, in lexicographic order. It is
 * an extra function, I used it for testing. Also, it shows the frequency of
 * all the keys.
 *
 * @param root The root of the trie.
 * @param buff A temporary buffer, for storing words, 1 by 1.
 * @param buff_idx An index for the buffer, which have to start from 0. The
 * index is used to place a character to buffer[buff_idx].
 */
void print_tree(g_node_t *root, char *buff, unsigned int buff_idx);

#endif  // GENERIC_TREE_H_
