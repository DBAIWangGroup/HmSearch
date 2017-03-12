
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include "header.h"
#include "bit_fv.h"
//extern int *underflow_list;
//extern int underflow_num;
//extern int unsigned underflow_records;
extern hash_dict_t *_sig_index_hash_dict;
extern FILE *fp_query_match;
//extern FILE *fp_query_cand;
//extern FILE *fp_query_cand_index;
extern int collapse_filtering;
extern int can0, can1, can2, can3, can4, avg_m_num;
extern int N;
extern int bit_fv;
extern int mask[U_INT_LEN/2];

int calculate_query_prefix_length(query_t *ra, int m, int Q, int id) {
  int obj_len = ra->len;
  int pfx = -1;
  int p_pfx_len = Q * (obj_len - m) + Q;
  if (obj_len >= p_pfx_len) pfx = p_pfx_len - Q + 1;
  if (pfx < 0) {
    //underflow
    //underflow_records++;
    //ra->stat = 0;
    //pfx = obj_len - Q + 1;
    //for (i = 0; i < obj_len; i++) {
    //ra->sig_slots[i].sig->pfx_num++;
    //}
    fprintf(stderr, "%d length is %d, prefix length is %d, m or Q does not meet the requirement\n", id, obj_len, p_pfx_len);
    exit(-1);
  }
  return pfx;
}

inline static int _min(int a, int b) {
  return a > b ? b : a;
}

inline static int _min3(int a, int b, int c) {
  if (a <= b && a <= c) return a;
  if (b <= a && b <= c) return b;
  return c;
}

//_jaccard

int compare(element_t * da, element_t *db) {
  int ret;

  //following frequency, 3>2
  ret = (da->sign > db->sign) - (da->sign < db->sign);
  return ret;
}

int _match_number(query_t * query, record_t * rec, int m) {
  int match_num;
  int i, j, ret;
  match_num = 0;
  i = 0;
  j = 0;
  while (i < query->len && j < rec->len) {
    ret = compare(query->element_slots[i].element, rec->element_slots[j].element);
    if (ret == 0) {
      match_num++;
    }
    i++;
    j++;
#ifndef dump_one_query
    if (match_num < m - query->len + i) {
      return -1;
    }
#endif
  }
  //printf("%d\n",match_num);
  return match_num;
}



inline static void bit_map_init(bit_map_t *bm, int len) {
  bm->min = 0xffffffff;
  bm->max = 0;
  bm->bitmap_len = len / U_INT_LEN + 2;
  bm->bitmap = (unsigned int *) malloc(sizeof ( int) * bm->bitmap_len);
  memset(bm->bitmap, 0, sizeof (int) * bm->bitmap_len);
}

inline static void bit_map_clear(bit_map_t *bm) {
  unsigned j, m;
  if (bm->min != 0xffffffff) {
    j = bm->min;
    m = bm->max - bm->min + 1;
    memset(&bm->bitmap[j], 0, sizeof (int) * m);
    bm->min = 0xffffffff;
    bm->max = 0;
  }
}

inline static int bit_map_set(bit_map_t *bm, unsigned int y) {
  unsigned j, m;
  j = y >> U_INT_BIT_LEN;
  m = y - (j << U_INT_BIT_LEN);

  if (bm->bitmap[j] & (1 << m)) return 1;

  if (j < bm->min) bm->min = j;
  if (j > bm->max) bm->max = j;

  bm->bitmap[j] = bm->bitmap[j] | (1 << m);
  return 0;
}

inline static int bit_map_check(bit_map_t *bm, int y) {
  int j, m;
  j = y >> U_INT_BIT_LEN;
  m = y - (j << U_INT_BIT_LEN);
  if (bm->bitmap[j] & (1 << m)) return 1;
  return 0;
}

inline static void bit_matrix_init(bit_map_t *bm, int len) {
  bm->min = 0xffffffff;
  bm->max = 0;
  bm->bitmap_len = len / U_INT_LEN + 2;
  bm->bitmap = (unsigned int *) malloc(sizeof (int) * U_BIT * bm->bitmap_len);
  memset(bm->bitmap, 0, sizeof (int) * U_BIT * bm->bitmap_len);
}

inline static void bit_matrix_clear(bit_map_t *bm) {
  unsigned j, m;
  if (bm->min != 0xffffffff) {
    j = bm->min;
    m = bm->max - bm->min + 1;
    memset(&bm->bitmap[j], 0, sizeof (int) * m);
    bm->min = 0xffffffff;
    bm->max = 0;
  }
}

inline static int bit_matrix_set(bit_map_t *bm, unsigned int y) {
  unsigned j, m;
  j = y >> 4;
  m = (y - (j << 4)) << 1;
  //if (bm->bitmap[j] & (1 << m)) return 1;
  if (bm->bitmap[j] & (2 << m)) return 0;
  if (j < bm->min) bm->min = j;
  if (j > bm->max) bm->max = j;
  if(bm->bitmap[j] & (1 << m)){
    bm->bitmap[j] += 1 << m;
    return 1;
  }
  bm->bitmap[j] += 1 << m;
  return 0;
}

inline static int bit_matrix_check(bit_map_t *bm, int y) {
  int j, m;
  j = y >> 4;
  m = (y - (j << 4)) << 1;
  //if (bm->bitmap[j] & (1 << m)) return 1;
  if (bm->bitmap[j] & (2 << m)) return 1;
  return 0;
}

inline static int bit_matrix_tri_set(bit_map_t *bm, unsigned int y) {
  unsigned j, m;
  j = y >> 4;
  m = (y - (j << 4)) << 1;
  //how many bits to shift to the right end
  //printf("k is %d\n",k);
  //if (bm->bitmap[j] & (1 << m)) return 1;
  if (!((bm->bitmap[j] & mask[m/2])^(3 << m)) )return 0;
  if (j < bm->min) bm->min = j;
  if (j > bm->max) bm->max = j;
  if (bm->bitmap[j] & (1 << m)) {
    bm->bitmap[j] += 1 << m;
    return 0;
  }
  if (bm->bitmap[j] & (2 << m)) {
    bm->bitmap[j] += 1 << m;
    return 1;
  }
  bm->bitmap[j] += 1 << m;
  return 0;
}
/*
int idflist_bisearch(idf_node_t *head, int len, int key) {
  int low = 0, high = len - 1, mid;
  if (head[low].rec_len == key || head[low].rec_len > key || low == high)return low;
  else if (head[high].rec_len < key) return (high + 1);
  while (low <= high) {
    mid = low + ((high - low) / 2);
    if (head[mid].rec_len == key && head[mid - 1].rec_len < key)return mid;
    if (head[mid].rec_len >= key) high = mid - 1;
    else low = mid + 1;
  }
  return low;
}
*/
bit_map_t bm;
bit_map_t bma;

void init_search(int record_num) {
  bit_map_init(&bm, record_num);
  bit_matrix_init(&bma, record_num);
}

int search_in_index(query_t *queries, int num, record_t *records, int record_num, int m, int Q) {
  query_t *query = queries;
  sig *sig_q;
  record_t *rec = records;
  int i, rec_id, h;
  int m_num;
  //bit_map_t bm;
  //bit_map_clear(&bm);
  //bit_map_clear(&bm);
  bit_matrix_clear(&bma); 
  //find candidates
  //#ifdef DEBUG_OUTPUT   
  //fprintf(fp_query_cand_index, "%d\t", num);
  //#endif
  //char * buffer = (char *) malloc(sizeof (char) * Q * MAX_ELEMENT_LEN);
  //query[num].prefix_len = calculate_query_prefix_length(&query[num], m, Q, num);
  for (i = 0; i < query[num].sig_num; i++) {
    sig_q = query[num].sig_slots[i].sig;
    if (sig_q == NULL) {
      continue;
    }

/*
#ifdef DEBUG_OUTPUT
    can0 += sig_q->frq;
#endif
*/

    
    //probe and store a qualified list
    for (h = 0; h < sig_q->last_idf; h++) {
      rec_id = sig_q->idf_list[h].record_id;
      //if the record appears the first time and pass the positional filtering
      //we put it into the qualified list
      if (bit_matrix_set(&bma, rec_id)) {
        
#ifdef DEBUG_OUTPUT
    can1++;
#endif
          //m_num = _match_number(&(query[num]), &(rec[rec_id]), m);
        if (bit_fv) {
          m_num = bitwise_check_line_based(&(query[num]), &(rec[rec_id]), m);
        } else {
          m_num = _match_number(&(query[num]), &(rec[rec_id]), m);
        }
        //printf("%f\n", jacd);
          if (m_num >= m) {
#ifdef DEBUG_OUTPUT
    can4++;
#endif
            //fprintf(fp_query_match, "%d\t%d\t%d\n", num, rec_id, m_num);
            //fprintf(stdout, "%d\t%d\t%d\n", num, rec_id, m_num);
        }
      }
    }
  }
/*
#ifdef DEBUG_OUTPUT
  fprintf(stdout, "query_id %d ", num);
  fprintf(stdout, "m %d partition_len %d ", m, Q);
  fprintf(stdout, "can0 %d ", can0);
  fprintf(stdout, "can1 %d ", can1);
  fprintf(stdout, "can2 %d ", can2);
  fprintf(stdout, "can3 %d ", can3);
  fprintf(stdout, "can4 %d\n", can4);
#endif  
*/
  //fprintf(stderr, "avg_m_num_in_pfx\t%Lf\n", average_m_num);
  return EXIT_SUCCESS;
}

int search_in_index_odd(query_t *queries, int num, record_t *records, int record_num, int m, int Q) {
  query_t *query = queries;
  sig *sig_q;
  record_t *rec = records;
  int i, rec_id, h;
  int m_num;
  //bit_map_t bm;
  //bit_map_clear(&bm);
  //bit_map_clear(&bm);
  bit_matrix_clear(&bma); 
  //find candidates
  //#ifdef DEBUG_OUTPUT   
  //fprintf(fp_query_cand_index, "%d\t", num);
  //#endif
  //char * buffer = (char *) malloc(sizeof (char) * Q * MAX_ELEMENT_LEN);
  //query[num].prefix_len = calculate_query_prefix_length(&query[num], m, Q, num);
  for (i = 0; i < query[num].sig_num; i++) {
    sig_q = query[num].sig_slots[i].sig;
    if (sig_q == NULL) {
      continue;
    }

/*
#ifdef DEBUG_OUTPUT
    can0 += sig_q->frq;
#endif
*/

    
    //probe and store a qualified list
    for (h = 0; h < sig_q->last_idf; h++) {
      rec_id = sig_q->idf_list[h].record_id;
      //if the record appears the first time and pass the positional filtering
      //we put it into the qualified list
      if (bit_matrix_tri_set(&bma, rec_id)) {
      //if (!bit_map_set(&bm, rec_id)) {
#ifdef DEBUG_OUTPUT
    can1++;
#endif
          //m_num = _match_number(&(query[num]), &(rec[rec_id]), m);
        if (bit_fv) {
          m_num = bitwise_check_line_based(&(query[num]), &(rec[rec_id]), m);
        } else {
          m_num = _match_number(&(query[num]), &(rec[rec_id]), m);
        }
        //printf("%f\n", jacd);
          if (m_num >= m) {
#ifdef DEBUG_OUTPUT
    can4++;
#endif
            //fprintf(fp_query_match, "%d\t%d\t%d\n", num, rec_id, m_num);
            //fprintf(stdout, "%d\t%d\t%d\n", num, rec_id, m_num);
        }
      }
    }
  }
/*
#ifdef DEBUG_OUTPUT
  fprintf(stdout, "query_id %d ", num);
  fprintf(stdout, "m %d partition_len %d ", m, Q);
  fprintf(stdout, "can0 %d ", can0);
  fprintf(stdout, "can1 %d ", can1);
  fprintf(stdout, "can2 %d ", can2);
  fprintf(stdout, "can3 %d ", can3);
  fprintf(stdout, "can4 %d\n", can4);
#endif  
*/
  //fprintf(stderr, "avg_m_num_in_pfx\t%Lf\n", average_m_num);
  return EXIT_SUCCESS;
}


