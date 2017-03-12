/*
 * index.c
 *
 *  Created on: Apr 22, 2009
 *      Author: jianbinqin
 */

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stddef.h>
#include "header.h"
#include "hash_dict.h"

int g_element_num = 0;

extern int order_num;
hash_dict_t *_index_hash_dict = NULL;
element_block_t *_element_block_head = NULL;
element_block_t *_curr_element_block = NULL;
extern int  * order_table;
/*
 * Init the data of the inverted index and element.
 */
void init_element_index(int size) {
#ifndef ITERATION
  if (_index_hash_dict != NULL) {
    hash_dict_destory(_index_hash_dict);
  }
#endif
  _index_hash_dict = hash_dict_create(size * 1.5);
  _element_block_head = (element_block_t *) malloc(sizeof (element_block_t));
  if (_element_block_head == NULL) {
    fprintf(stderr, "MALLOC MEMEORY ERROR\n");
    exit(-1);
  }
  _element_block_head->last_free = 0;
  _element_block_head->next = NULL;
  _curr_element_block = _element_block_head;
  g_element_num = 0;
}

/*
 * get a element from a block date s
 */
element_t * new_element() {
  element_block_t *cbtp;

  if (_curr_element_block->last_free < ELEMENT_BLOCK_ELEMENT_NUM) {
    return &_curr_element_block->elements[_curr_element_block->last_free++];
  } else {
    cbtp = (element_block_t *) malloc(sizeof (element_block_t));
    if (cbtp == NULL) {
      fprintf(stderr, "MALLOC MEMEORY ERROR\n");
      return NULL;
    }
    cbtp->last_free = 0;
    cbtp->next = NULL;
    _curr_element_block->next = cbtp;
    _curr_element_block = cbtp;
    return &_curr_element_block->elements[_curr_element_block->last_free++];
  }
}

/*
  This function use the record string to build a list elements.
  and those elements is in the data set.
 */
int build_record_elements(char * record_head, int element_num, element_slot_t *element_slots) {

  int i;
  int element_len;
  char * str = record_head;
  for (i = 0; i < element_num; i++) {
    element_len = 0;
    while (*(str + element_len) != '\0' && *(str + element_len) != ' ') {
      element_len++;
    }
    element_slots[i].element = insert_element_node(str, element_len, i);
    //for building sigs' convenience;
    if (*(str + element_len) == ' ') {
      str = str + element_len + 1;
    }
  }
  return 0;
}

element_t* insert_element_node(char * str, int len, int dim) {

  hash_dict_node_t snode;
  int res;
  element_t *element_p;
  /*
    char * node_str =str;
    char * element_buffer;
    element_buffer = (char *)malloc(sizeof(char) * (len + 1));
    strncpy(element_buffer, node_str, len);
   */
  
  if (create_sign_bitwise_len(str, &snode.sign1, &snode.sign2, len) != 0) {
    fprintf(stderr, "Error: Error in create sign for records' elements\n");
    return NULL;
  }

  if (hash_dict_search(_index_hash_dict, &snode) == RT_HASH_DICT_SEARCH_SUCC) {
    element_p = (element_t *) snode.pointer;
    //element_p->frq++;
  } else {
    if ((element_p = new_element()) == NULL) {
      fprintf(stderr, "ERROR: Out of memory \n");
      exit(-1);
    }
    //element_p->sign = ((unsigned long long)snode.sign1 << 32) + (unsigned long long)snode.sign2;
    //element_p->frq = 1;
    element_p->str = str;
    element_p->len = len;
    //element_p->bucket_num = (unsigned)0;
    element_p->id = g_element_num;
    element_p->dim = dim;
    g_element_num++;
    // insert it into hash_dict
    snode.pointer = (void *) element_p;
    res = hash_dict_add(_index_hash_dict, &snode, 0);
    if (res != RT_HASH_DICT_ADD_SUCC && res != RT_HASH_DICT_ADD_EXIST) {
      fprintf(stderr, "Error, insert a element node into hash dict error \n");
      return NULL;
    }
  }
  return element_p;
}

int destroy_element_index() {
  //int i;
  element_block_t *element_block_p, *tmp_p;

  element_block_p = _element_block_head;
  while (element_block_p) {
    tmp_p = element_block_p->next;
    free(element_block_p);
    element_block_p = tmp_p;
  }

  if (_index_hash_dict != NULL)
    hash_dict_destory(_index_hash_dict);
  return 0;
}

