/*
 * idf.c
 *
 *  Created on: Oct 22, 2011
 *      Author: jianbinqin
 *      revised:xiaoyang
 */

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include "header.h"


extern int order_num;
extern int g_element_num;
extern unsigned * order;
extern FILE * fp_output_order;
extern int N;

static int _compare_element_slots(const void *a, const void *b) {
  const element_slot_t *sa = (const element_slot_t *) a;
  const element_slot_t *sb = (const element_slot_t *) b;

  const element_t *da = sa->element;
  const element_t *db = sb->element;
  int ret;

  //following frequency, 3>2
  ret = (da->frq > db->frq) - (da->frq < db->frq);
  if (ret == 0) {
    ret = (da->sign > db->sign) - (da->sign < db->sign);
  }
  return ret;
}

/*
static int _compare_order_buffer(const void *a, const void *b) {
  const element_t **sa = (const element_t **) a;
  const element_t **sb = (const element_t **) b;

  const element_t *da = *sa;
  const element_t *db = *sb;
  int ret;

  //following frequency, 3>2
  ret = (da->frq > db->frq) - (da->frq < db->frq);
  if (ret == 0) {
    ret = (da->sign > db->sign) - (da->sign < db->sign);
  }
  return ret;
}
*/

void sort_all_element_list_by_freq(record_t *records, int records_num) {
  int i = 0;
  //int j;
  for (i = 0; i < records_num; i++) {
    // process non-underflow records
    qsort(records[i].element_slots, records[i].len,
            sizeof (element_slot_t), _compare_element_slots);
  }
}

void sort_all_query_element_list_by_freq(query_t *records, int i) {
  // process non-underflow records
  qsort(records[i].element_slots, records[i].len,
          sizeof (element_slot_t), _compare_element_slots);
}

void random_sig_frq(sig_block_t *_sig_block_head) {
  int i;
  sig_block_t *sig_block_p, *tmp_p;
  sig_block_p = _sig_block_head;
  srand(time(NULL));
  while (sig_block_p) {
    for (i = 0; i < sig_block_p->last_free; i++) {
      sig_block_p->sigs[i].frq = rand();
      //fprintf(fp_output_element_listlen, "%s\t%d\n", sig_block_p->sigs[i].buffer, times);
    }
    tmp_p = sig_block_p->next;
    sig_block_p = tmp_p;
  }
}

int calculate_sig_num(record_t *rec, int M) {
  int num;
  num = rec->len;
  return num;
}

int calculate_query_sig_num(query_t *rec, int M) {
  int num;
  num = rec->len;
  return num;
}

/*
int calculate_pfx_length(record_t *ra, int id, int m, int Q) {

  int obj_len = ra->len;
  int i;
  int pfx = -1;

  int p_pfx_len = Q * (obj_len - m) + Q;
  if (obj_len >= p_pfx_len) pfx = p_pfx_len - Q + 1;
  if (pfx < 0) {
    printf("m or Q does not meet the requirement\n");
    exit(-1);
  } else {
    for (i = 0; i < pfx; i++) {
      ra->sig_slots[i].sig->pfx_num++;
    }
    //ra->stat = 1;
  }
  return pfx;
}
*/

void build_sig_idf_list(record_t *records, int records_num, int m, int Q) {
  int i, j;
  sig *sig_q;
  for (i = 0; i < records_num; i++) {
    for (j = 0; j < records[i].sig_num; j++) {
      sig_q = records[i].sig_slots[j].sig;
      if (sig_q->idf_list == NULL) {
        if ((sig_q->idf_list = (idf_node_t *) malloc(sizeof (idf_node_t) * sig_q->frq)) == NULL) {
          fprintf(stderr, "Error: out of memory when allocat idf_list\n");
          exit(-1);
        }
        //element_p->head_idf = 0;
      }
      sig_q->idf_list[sig_q->last_idf].record_id = i;
      sig_q->last_idf++;
    }
  }
}



int dump_sig_order(sig_block_t *_sig_block_head, int g_m, int g_q) {
  int i;
  //int k = 0;
  int times;
  sig_block_t *sig_block_p, *tmp_p;
  sig_block_p = _sig_block_head;

  char sig_name_buffer[256];
  sprintf(sig_name_buffer, "./sig_m=%d_q=%d_o=%d.txt", g_m, g_q, order_num);
  FILE * fp_output_sig_listlen;
  if ((fp_output_sig_listlen = fopen(sig_name_buffer, "w+")) == NULL) {
    fprintf(stderr, "Error: create file error\n");
    return EXIT_FAILURE;
  }

  while (sig_block_p) {
    for (i = 0; i < sig_block_p->last_free; i++) {
        times = sig_block_p->sigs[i].frq;
        //fprintf(fp_output_sig_listlen, "%llu\t%d\n", sig_block_p->sigs[i].sign, times);
    }
    tmp_p = sig_block_p->next;
    sig_block_p = tmp_p;
  }
  return EXIT_SUCCESS;
}

int dump_element_order(element_block_t *_element_block_head, int g_m, int g_q) {
  int i;
  int times;
  element_block_t *element_block_p, *tmp_p;
  element_block_p = _element_block_head;

  char element_prefix_name_buffer[256];
  sprintf(element_prefix_name_buffer, "./ele_pref_m=%d_q=%d_o=%d.txt", g_m, g_q, order_num);
  FILE * fp_output_element_listlen;
  if ((fp_output_element_listlen = fopen(element_prefix_name_buffer, "w+")) == NULL) {
    fprintf(stderr, "Error: create file error\n");
    return EXIT_FAILURE;
  }

  while (element_block_p) {
    for (i = 0; i < element_block_p->last_free; i++) {
      times = element_block_p->elements[i].frq;
      fprintf(fp_output_element_listlen, "%llu\t%d\n", element_block_p->elements[i].sign, times);
    }
    tmp_p = element_block_p->next;
    element_block_p = tmp_p;
  }
  return EXIT_SUCCESS;
}








