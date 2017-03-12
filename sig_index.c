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

int g_sig_num = 0;
extern int order_num;
extern int N;
extern unsigned * power_hashmap;

hash_dict_t *_sig_index_hash_dict = NULL;
sig_block_t *_sig_block_head = NULL;
sig_block_t *_curr_sig_block = NULL;
extern int partition_len[MAX_PARTI];

/*
 * Init the data of the inverted index and element.
 */
void init_sig_index(int size) {
#ifndef ITERATION
  if (_sig_index_hash_dict != NULL) {
    hash_dict_destory(_sig_index_hash_dict);
  }
#endif
  _sig_index_hash_dict = hash_dict_create(size * 1.5);
  _sig_block_head = (sig_block_t *) malloc(sizeof (sig_block_t));
  if (_sig_block_head == NULL) {
    fprintf(stderr, "MALLOC MEMEORY ERROR\n");
    exit(-1);
  }
  _sig_block_head->last_free = 0;
  _sig_block_head->next = NULL;
  _curr_sig_block = _sig_block_head;

  g_sig_num = 0;
}

/*
 * get a element from a block date s
 */
sig * new_sig() {
  sig_block_t *cbtp;

  if (_curr_sig_block->last_free < ELEMENT_BLOCK_ELEMENT_NUM) {
    return &_curr_sig_block->sigs[_curr_sig_block->last_free++];
  } else {
    cbtp = (sig_block_t *) malloc(sizeof (sig_block_t));
    if (cbtp == NULL) {
      fprintf(stderr, "MALLOC MEMEORY ERROR\n");
      return NULL;
    }
    cbtp->last_free = 0;
    cbtp->next = NULL;
    _curr_sig_block->next = cbtp;
    _curr_sig_block = cbtp;
    return &_curr_sig_block->sigs[_curr_sig_block->last_free++];
  }
}

int build_sigs(record_t *rec, int Q, int M) {
  int i, j;
  int k = 0;
  int l = 0;
  long long * buffer;
  long long chunk_sign;
  unsigned long long variance_sign;
  int first_part = 0;
  buffer = (long long *) malloc(sizeof (long long) * N);
  //int first_part;
  //first_part = N - (N - (((int) ceil((N - M + 1.0) / 2.0)) * Q))*(Q + 1);
  first_part = N - (N - (((N - M + 3) / 2) * Q))*(Q + 1);

  for (i = 0; i < first_part; i += Q) {
    //rolling hash value in each position
    //init chunk sign
    chunk_sign = 0;
    for (j = 0; j < Q; j++) {
      buffer[j] = ((rec->element_slots[i + j].element->id) * power_hashmap[Q - j - 1]);
      chunk_sign += buffer[j];
    }
    insert_sig_node(chunk_sign);
    partition_len[l] = Q;
    l++;
    //chunk_sign %= ROLLING_M;
    //chunk_sign = rkHash(buffer, Q, power_hashmap);
    //memset(buffer, 0, sizeof (char) * 2 * Q * MAX_ELEMENT_LEN);
    for (j = 0; j < Q; j++) {
      //strncpy(buffer, element_slot_st->str, len);
      variance_sign = (unsigned long long) ((chunk_sign - buffer[j]));
      rec->sig_slots[k].sig = insert_sig_node(variance_sign);
      //rec->sig_slots[k].exact = 0;
      //rec->sig_slots[k].pos = i;
      k++;
    }
  }
  //deal with the last part whose length may not be Q

  for (i = first_part; i < rec->len; i += Q + 1) {
    //init chunk_sign
    chunk_sign = 0;
    for (j = 0; j < Q + 1; j++) {
      buffer[j] = ((rec->element_slots[i + j].element->id) * power_hashmap[Q - j]);
      chunk_sign += buffer[j];
      //chunk_sign %= ROLLING_M;
    }

    insert_sig_node(chunk_sign);
    partition_len[l] = Q + 1;
    l++;
    //chunk_sign = rkHash(buffer, Q, power_hashmap);
    //memset(buffer, 0, sizeof (char) * 2 * Q * MAX_ELEMENT_LEN);
    for (j = 0; j < Q + 1; j++) {
      //strncpy(buffer, element_slot_st->str, len);
      variance_sign = (unsigned long long) ((chunk_sign - buffer[j]));
      rec->sig_slots[k].sig = insert_sig_node(variance_sign);
      //rec->sig_slots[k].exact = 0;
      //rec->sig_slots[k].pos = i;
      k++;
    }
  }
  free(buffer);
  return EXIT_SUCCESS;
}

sig* insert_sig_node(unsigned long long sign) {

  hash_dict_node_t snode;
  int res;
  sig *sig_q;
  //char s;
  //if (dimension < 10) s = (char) dimension + '0';
  //else if (dimension < 36) s = (char) dimension - 10 + 'a';
  //else s = (char) dimension - 36 + '_';
  //printf("%llu\n", sign);
  snode.sign1 = (sign >> 32) + 1;
  //printf("%u ", snode.sign1);
  snode.sign2 = (sign << 32) >> 32;
  //printf("%u ;", snode.sign2);

  if (hash_dict_search(_sig_index_hash_dict, &snode) == RT_HASH_DICT_SEARCH_SUCC) {
    sig_q = (sig *) snode.pointer;
    sig_q->frq++;
  } else {
    if ((sig_q = new_sig()) == NULL) {
      fprintf(stderr, "ERROR: Out of memory \n");
      exit(-1);
    }
    //sig_q->sign = ((unsigned long long)snode.sign1 << 32) + (unsigned long long)snode.sign2;
    //sig_q->sign = sign;
    sig_q->idf_list = NULL;
    //sig_q->pfx_num = 0;
    sig_q->frq = 1;
    sig_q->last_idf = 0;
    //sig_q->buffer = (char *) malloc(sizeof (char) * Q * MAX_ELEMENT_LEN);
    //strcpy(sig_q->buffer, sig_buffer);
    g_sig_num++;
    snode.pointer = (void *) sig_q;
    res = hash_dict_add(_sig_index_hash_dict, &snode, 0);
    // insert it into hash_dict
    if (res != RT_HASH_DICT_ADD_SUCC && res != RT_HASH_DICT_ADD_EXIST) {
      fprintf(stderr, "Error, insert a sig node into hash dict error \n");
      return NULL;
    }
  }
  return sig_q;
}

int destroy_sig_index() {
  sig_block_t *sig_block_p, *tmp_p;
  sig_block_p = _sig_block_head;
  int i;
  while (sig_block_p) {
    for (i = 0; i < sig_block_p->last_free; i++) {
      if (sig_block_p->sigs[i].idf_list != NULL) {
        free(sig_block_p->sigs[i].idf_list);
      }
    }
    tmp_p = sig_block_p->next;
    free(sig_block_p);
    sig_block_p = tmp_p;
  }

  if (_sig_index_hash_dict != NULL)
    hash_dict_destory(_sig_index_hash_dict);
  return 0;
}

int calculate_Q(int n, int M) {
  int Q;
  Q = N / ((N - M + 3) / 2);
  return Q;
}

char *strncpy_without_EOF(char *dest, const char *source, size_t n) {
  char *start = dest;

  while (n && (*dest++ = *source++)) n--;
  //if (n) while (--n) *dest++ = '\0';
  return start;
}



