/*
 * header.h
 *
 *  Created on: 10.22, 2011
 *      Author: jianbinqin
 *      revised:xiaoyang
 */

#ifndef HEADER_H_
#define HEADER_H_

#include "hash_dict.h"


#define RECORD_MAX_LINE_LEN 1024*8    // Line length for per line
#define RECORD_MAX_LINE_NUM 1024*1024 // max line number per data
#define QUERY_MAX_LINE_NUM 1024
#define MAX_ELEMENT_NUM 1024*1024
#define MAX_FILE_SIZE 1024*1024*1024
#define MAX_QUERY_FILE_SIZE 1024*1024*16
#define MAX_SIG_NUM 1024*1024*32
#define ELEMENT_BLOCK_ELEMENT_NUM 1024
#define SIG_BLOCK_SIG_NUM 1024
#define MAX_PARTI 5000
#define VERSION "1.0"


// Filtering Macro
#define U_INT_LEN 32
#define U_INT_BIT_LEN 5
#define U_BIT 2
//#define ROLLING_M 1000000007
                  
#define ROLLING_M 1000000007
#define ROLLING_B 257

// Debug info
#define DEBUG_OUTPUT
//#define DEBUG_TRIANGLE
//#define DEBUG_INDEX
//#define dump_one_query
//#define TEST_INDEX_SPEED
//#define OUTPUT_SUM

typedef struct _bit_map_t {
    unsigned int min;
    unsigned int max;
    unsigned int *bitmap;
    unsigned int bitmap_len;
} bit_map_t;

//use to catch the length division point in the records
/*
typedef struct _len_node{
    int len;
    int id;
} len_node;
*/
typedef struct _idf_node{
  int record_id;
  //int rec_len;
  //int porder;
  //int pos;
  //unsigned short int * * position;
  //len_node *len_list;
  //int pos;
  //int order;
  //int porder;
} idf_node_t;

// This is the element data type.
typedef struct _element_t{
  unsigned long long sign;
  unsigned frq;
  char * str;
  int len;
  int order;
  //unsigned bucket_num;
  int id;
  int hash_d;
  int dim;
  //struct _element_t ** order_p;
  //int pfx_num;
  //int head_idf;
  //int last_idf;
  //idf_node_t *idf_list;
} element_t;

typedef struct _element_block_t{
  struct _element_t elements[ELEMENT_BLOCK_ELEMENT_NUM];
  int last_free;
  struct _element_block_t *next;
} element_block_t;


typedef struct _element_slot{
  //unsigned int pos;
  //unsigned int order;
  struct _element_t *element;
} element_slot_t;

typedef struct _sig{
  //unsigned long long sign;
    unsigned frq;
    int last_idf;
    //int pfx_num;
    //int last_idf;
    //int query_pos;
    idf_node_t *idf_list;
    //char buffer[1000];
}sig;

typedef struct _sig_block_t{
  struct _sig sigs[SIG_BLOCK_SIG_NUM];
  int last_free;
  struct _sig_block_t *next;
} sig_block_t;

typedef struct _sig_slot{
  //unsigned int pos;
  //unsigned int order;
  //int exact;
  struct _sig *sig;
} sig_slot;

/* This structure restore the content of the documents.*/
typedef struct _record{
  char *str;
  int len;
  int sig_num;
  //int stat;
  //int vector[DIMENSION];
  //short bucket[DIMENSION];
  unsigned * * bit_rec;
  struct _element_slot *element_slots;
  struct _sig_slot *sig_slots;
} record_t;

typedef struct _query{
  char *str;
  int len;
  //int can0;
  //int can1;
  //int can2;
  //int can3;
  //int can4;
  //long double avg_m_num;
  int sig_num;
  unsigned * * bit_rec;
  //short bucket[DIMENSION];
  struct _element_slot *element_slots;
  struct _sig_slot *sig_slots;
} query_t;

/*This structure stores the element node and its idf list length for output*/
//typedef struct _multi_p{
//    unsigned short p_num;
//    unsigned short * * query_p[ELEMENT_NUM_PER_LEN];
//    unsigned short * * rec_p[ELEMENT_NUM_PER_LEN];
//}multi_p;


//main.c
int q_sig_search(int g_m);


// interface from record.c
int read_all_documents(FILE* fp, record_t * records);
void dump_records(FILE *stream, int record_num, record_t * records);

// interface from query.c
int read_all_queries(FILE* fp, query_t * query);

// interface from multi_order_filtering.c
int rand_bucket_num(element_block_t *_element_block_head);
int rec_bucket_init(record_t * rec, int rec_num);
int que_bucket_init(query_t * rec, int rec_num);

//interface from idf.c
//static int _compare_element_slots (const void *a, const void *b);
void random_element_frq(element_block_t *_element_block_head, int len);
int get_dimension(char * string, int len);
void set_element_frq(element_block_t *_element_block_head);
void random_sig_frq(sig_block_t *_sig_block_head);

int calculate_sig_num(record_t *rec, int Q);
int calculate_query_sig_num(query_t *rec, int Q);

void sort_all_element_list_by_freq(record_t *records, int records_num);
void sort_all_sig_list_by_freq(record_t *records, int records_num);
void sort_all_query_element_list_by_freq(query_t *records, int i);
void sort_all_query_sig_list_by_freq(query_t *records, int i);

void build_sig_idf_list(record_t *records, int records_num, int m, int Q);

int dump_sig_order_in_prefix(sig_block_t *_sig_block_head, int g_m, int g_q);
int dump_sig_order(sig_block_t *_sig_block_head, int g_m, int g_q);
int dump_element_order(element_block_t *_element_block_head, int g_m, int g_q);



//interface from index.c

void init_element_index(int size);
element_t * new_element();
element_t* insert_element_node(char * str, int len, int dim);
int build_record_elements(char * record_head, int element_num, element_slot_t *element_slots);
int destroy_element_index();

//interface from query_prefix.c
element_t * new_query_element();
int build_query_elements(char * query_head, int element_num, element_slot_t *element_slots);
element_t* insert_query_element_node(char * str, int len, int dim);

//interface from sig_index.c
void init_sig_index(int size);
int build_sigs(record_t *rec, int Q, int M);
sig* insert_sig_node(unsigned long long sign);
int destroy_sig_index();
int calculate_Q(int n, int M);
char *strncpy_without_EOF(char *dest, const char *source, size_t n);

//interface from query_sig.c
sig * insert_query_sig_node(unsigned long long sign);
sig * new_query_sig();
int build_query_sigs(query_t *rec, int Q, int M);

//interface from search.c
int _hamming_distance1(query_t * query, record_t * rec);
int _hamming_distance2(record_t * query, record_t * rec);
int search_in_index(query_t *queries, int num, record_t *records, int record_num, int m, int Q);
int bucket_num_filtering(query_t * query, record_t * rec, int m);
void init_search(int record_num);
int search_in_index_odd(query_t *queries, int num, record_t *records, int record_num, int m, int Q);
#endif /* HEADER_H_ */
