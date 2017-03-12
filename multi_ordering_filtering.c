/*
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include "header.h"
extern int g_element_num;
extern int order_num;
extern hash_dict_t *_index_hash_dict;

int using_order;


int rand_bucket_num(element_block_t *_element_block_head) {
  int i;
  element_block_t *element_block_p, *tmp_p;
  element_block_p = _element_block_head;
  srand((unsigned) (time(NULL)));
  while (element_block_p) {
    for (i = 0; i < element_block_p->last_free; i++) {
      //for (k = 0; k < order_num; k++) {
        element_block_p->elements[i].bucket_num = (unsigned)(rand()%(DIMENSION-1));
      //}
    }
    tmp_p = element_block_p->next;
    element_block_p = tmp_p;
  }
  return EXIT_SUCCESS;
}

int rec_bucket_init(record_t * rec, int rec_num) {
  int i,j;
  for (i = 0; i < rec_num; i++) {
    for (j = 0; j < rec[i].len; j++) {
      rec[i].bucket[rec[i].element_slots[j].element->bucket_num]++;
    }
  }
  return EXIT_SUCCESS;
}

int que_bucket_init(query_t * rec, int num) {
  int j;
    for (j = 0; j < rec[num].len; j++) {
      rec[num].bucket[rec[num].element_slots[j].element->bucket_num]++;
    }
  return EXIT_SUCCESS;
}
*/

