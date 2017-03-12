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

char * buffer;
extern hash_dict_t *_sig_index_hash_dict;
extern int N;
extern unsigned * power_hashmap;
extern int partition_can0[MAX_PARTI];
extern int partition_exact[MAX_PARTI];

sig * new_query_sig() {
  sig *cbtp;
  cbtp = (sig *) malloc(sizeof (sig));
  if (cbtp == NULL) {
    fprintf(stderr, "MALLOC MEMEORY ERROR\n");
    return NULL;
  }
  return cbtp;
}

int build_query_sigs(query_t *rec, int Q, int M) {
  int i, j;
  int k = 0;
  int l = 0;
  long long * buffer;
  long long chunk_sign;
  unsigned long long variance_sign;
  sig * sig_q;

  buffer = (long long *) malloc(sizeof (long long) * N);
  //int first_part;
  int first_part;
  first_part = N - (N - (((N - M + 3) / 2) * Q))*(Q + 1);
  for (i = 0; i < first_part; i += Q) {
    //init chunk_sign
    chunk_sign = 0;
    //rolling hash value in each position


    for (j = 0; j < Q; j++) {
      buffer[j] = ((rec->element_slots[i + j].element->id) * power_hashmap[Q - j - 1]);
      chunk_sign += buffer[j];
      //chunk_sign %= ROLLING_M;
    }
    sig_q = insert_query_sig_node(chunk_sign);

    if (sig_q != NULL) {
      partition_exact[l] = sig_q->frq;
    } else {
      partition_exact[l] = 0;
    }
    partition_can0[l] = 0;
    //chunk_sign = rkHash(buffer, Q, power_hashmap);
    //memset(buffer, 0, sizeof (char) * 2 * Q * MAX_ELEMENT_LEN);
    for (j = 0; j < Q; j++) {
      //strncpy(buffer, element_slot_st->str, len);
      variance_sign = (unsigned long long) ((chunk_sign - buffer[j]));
      rec->sig_slots[k].sig = insert_query_sig_node(variance_sign);
      //rec->sig_slots[k].pos = i;
      if (rec->sig_slots[k].sig != NULL) {
        partition_can0[l] += rec->sig_slots[k].sig->frq;
      }
      k++;
    }
    l++;
  }
  //deal with the last part whose length may not be Q

  for (i = first_part; i < rec->len; i += Q + 1) {
    //init chunk_sign
    chunk_sign = 0;
    for (j = 0; j < Q + 1; j++) {
      buffer[j] = ((rec->element_slots[i + j].element->id) * power_hashmap[Q - j]);
      chunk_sign += buffer[j];
    }
    sig_q = insert_query_sig_node(chunk_sign);

    if (sig_q != NULL) {
      partition_exact[l] = sig_q->frq;
    } else {
      partition_exact[l] = 0;
    }
    partition_can0[l] = 0;
    //chunk_sign %= ROLLING_M;
    //chunk_sign = rkHash(buffer, Q, power_hashmap);
    //memset(buffer, 0, sizeof (char) * 2 * Q * MAX_ELEMENT_LEN);
    for (j = 0; j < Q + 1; j++) {
      //strncpy(buffer, element_slot_st->str, len);
      variance_sign = (unsigned long long) ((chunk_sign - buffer[j]));
      rec->sig_slots[k].sig = insert_query_sig_node(variance_sign);
      //rec->sig_slots[k].pos = i;
      if (rec->sig_slots[k].sig != NULL) {
        partition_can0[l] += rec->sig_slots[k].sig->frq;
      }
      k++;
    }
    l++;
  }
  free(buffer);
  return EXIT_SUCCESS;
}

sig * insert_query_sig_node(unsigned long long sign) {

  hash_dict_node_t snode;
  sig *sig_q;

  //printf("%llu\n", sign);
  snode.sign1 = (sign >> 32) + 1;
  //printf("%u ", snode.sign1);
  snode.sign2 = (sign << 32) >> 32;
  //printf("%u ;", snode.sign2);

  if (hash_dict_search(_sig_index_hash_dict, &snode) == RT_HASH_DICT_SEARCH_SUCC) {
    sig_q = (sig *) snode.pointer;
  } else {
    return NULL;
  }
  return sig_q;
}




/*
int destroy_query_sig_blocks() {
  sig_block_t *sig_block_p, *tmp_p;
  sig_block_p = _query_sig_block_head;
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
  return 0;
}
 */












