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
#include <math.h>
#include "header.h"
#include "hash_dict.h"

//element_block_t *_query_element_block_head = NULL;
//element_block_t *_curr_query_element_block = NULL;
extern hash_dict_t *_index_hash_dict;
extern int collapse_filtering;
extern int order_num;
extern int bit_per_d;
extern int max_num_in_dimension;
/*
void init_query_element_head() {
  _query_element_block_head = (element_block_t *) malloc(sizeof (element_block_t));
  if (_query_element_block_head == NULL) {
    fprintf(stderr, "MALLOC MEMEORY ERROR\n");
    exit(-1);
  }
  _query_element_block_head->last_free = 0;
  _query_element_block_head->next = NULL;
  _curr_query_element_block = _query_element_block_head;
}
*/


/*
element_t * new_query_element() {
  element_block_t *cbtp;

  if (_curr_query_element_block->last_free < ELEMENT_BLOCK_ELEMENT_NUM) {
    return &_curr_query_element_block->elements[_curr_query_element_block->last_free++];
  } else {
    cbtp = (element_block_t *) malloc(sizeof (element_block_t));
    if (cbtp == NULL) {
      fprintf(stderr, "MALLOC MEMEORY ERROR\n");
      return NULL;
    }
    cbtp->last_free = 0;
    cbtp->next = NULL;
    _curr_query_element_block->next = cbtp;
    _curr_query_element_block = cbtp;
    return &_curr_query_element_block->elements[_curr_query_element_block->last_free++];
  }
}
*/
element_t * new_query_element() {
  element_t *cbtp;
  cbtp = (element_t *) malloc(sizeof (element_t));
  if (cbtp == NULL) {
    fprintf(stderr, "MALLOC MEMEORY ERROR\n");
    return NULL;
  }
  return cbtp;
}
/*
  This function use the record string to build elements for a query.
 */
int build_query_elements(char * query_head, int element_num, element_slot_t *element_slots) {

  int i;
  int element_len;
  char * str = query_head;
  for (i = 0; i < element_num; i++) {
    element_len = 0;
    while (*(str + element_len) != '\0' && *(str + element_len) != ' ') {
      element_len++;
    }
    //no need for hash here
    //if(collapse_filtering){
    element_slots[i].element = insert_query_element_node(str, element_len, i);
    //}
    if (*(str + element_len) == ' ') {
      str = str + element_len + 1;
    }
  }
  return 0;
}

element_t* insert_query_element_node(char * str, int len, int dim) {
  //int i;
  hash_dict_node_t snode;
  element_t *element_p;
/*
  char element_buffer[len + 1];
  char * node_str = str;
  strncpy(element_buffer, node_str, len);
  element_buffer[len] = '\0';
*/
  if (create_sign_bitwise_len(str, &snode.sign1, &snode.sign2, len) != 0) {
    fprintf(stderr, "Error: Error in create query's elements' sign");
    return NULL;
  }

  if (hash_dict_search(_index_hash_dict, &snode) == RT_HASH_DICT_SEARCH_SUCC) {
    element_p = (element_t *) snode.pointer;
    //element_p->frq++;
  } else {
    if ((element_p = new_query_element()) == NULL) {
      fprintf(stderr, "ERROR: Out of memory \n");
      exit(-1);
    }
    //this part can be improved to get better performance
    //element_p->sign = ((unsigned long long)snode.sign1 << 32) + (unsigned long long)snode.sign2;
    //element_p->frq = 0;
    //element_p->str = str;
    //element_p->len = len;
    element_p->id = -2;
    //element_p->bucket_num = (unsigned)DIMENSION;
    //element_p->hash_d = pow((double) 2, (double) bit_per_d) - max_num_in_dimension/2 - 1;
    //caution
    //
    element_p->hash_d = max_num_in_dimension + 1;
    //element_p->hash_d = 1;
    //element_p->idf_list = NULL;
    //element_p->head_idf = 0;
    //element_p->last_idf = 0;
    //element_p->pfx_num = 0;
    //fprintf(stderr, "new element");
  }
  return element_p;
}

/*
int destroy_query_element_blocks() {
  int i;
  element_block_t *element_block_p, *tmp_p;

  element_block_p = _query_element_block_head;
  while (element_block_p) {
    tmp_p = element_block_p->next;
    free(element_block_p);
    element_block_p = tmp_p;
  }
  return 0;
}
*/
