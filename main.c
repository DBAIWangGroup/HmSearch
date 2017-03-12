/*
 ============================================================================
 Name        : 1-element.c
 Author      : xiaoyang revised from jianbin's Vchunk code
 Version     : 0.0.1
 Copyright   : CSE
 Description : This is a test proelement of 1-element
 ============================================================================
 */
#include <string.h>
#include <stdio.h>
#include <stddef.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <math.h>
#include <hash_dict.h>
#include "header.h"
#include "usage.h"
#include "bit_fv.h"



record_t g_records[RECORD_MAX_LINE_NUM];
query_t g_query[QUERY_MAX_LINE_NUM];
extern hash_dict_t *_index_hash_dict;
extern hash_dict_t *_sig_index_hash_dict;
extern char __usage_information[1024];
//extern char _user_time[256];
extern element_block_t *_element_block_head;
extern sig_block_t *_sig_block_head;
extern int g_element_num;
extern int g_sig_num;
extern char * _buffer;
FILE * fp_query_match;
//FILE * fp_query_cand;
//FILE * fp_query_cand_index;
//FILE * fp_output_order;
unsigned * power_hashmap;
char * sig_str_buffer;
unsigned *order;
int can0, can1, can2, can3, can4, can5, avg_m_num;
int g_curr_record_num;
int g_curr_query_num;
int bit_fv = 0;
int g_m = -1;
int g_q = -1;
int N = -1;
float alpha_odd = 100000000;
float alpha_even = 100000000;
int collapse_filtering = 0;
int order_num = 0;
extern int bit_per_d;
char * data_source = NULL;
char * query_source = NULL;
int mask[U_INT_LEN / 2];
int partition_can0[MAX_PARTI] = {0};
int partition_exact[MAX_PARTI] = {0};
int partition_len[MAX_PARTI] = {0};

void print_usage() {
  fprintf(stderr, "       <-d data source >\n");
  fprintf(stderr, "       <-s query source>\n");
  fprintf(stderr, "       <-m match number>\n");
  fprintf(stderr, "       <-q q_sig length>\n");
  fprintf(stderr, "       <-n total dimension>\n");
  fprintf(stderr, "       [-b bit filtering and verification]\n");
  fprintf(stderr, "       [-i q_sigs random order]\n");
  fprintf(stderr, "       [-o set number of multi_order filtering]\n");
#ifdef Debug
#endif
  exit(0);
}

int main(int argc, char **argv) {
  int c;
  while ((c = getopt(argc, argv, "bd:s:m:q:n:e:j:")) != -1)
    switch (c) {
        /*
              case 'r':
                g_element_random_order = 1;
                break;
              case 'b':
                g_element_idf_avg_order = 1;
                break;
         */
      case 'b':
        bit_fv = 1;
        break;
      case 'd':
        data_source = optarg;
        break;
      case 's':
        query_source = optarg;
        break;
      case 'm':
        g_m = atof(optarg);
        break;
      case 'q':
        g_q = atoi(optarg);
        break;
      case 'n':
        N = atoi(optarg);
        break;
      case 'j':
        alpha_odd = atof(optarg);
        break;
      case 'e':
        alpha_even = atof(optarg);
        break;
      case '?':
        if (optopt == 'd' || optopt == 's' || optopt == 'm' || optopt == 'q')
          fprintf(stderr, "Error: Option -%c requires an argument.\n",
                optopt);
        else if (isprint(optopt))
          fprintf(stderr, "Error: Unknown option `-%c'.\n", optopt);
        else
          fprintf(stderr, "Error: Unknown option character `\\x%x'.\n",
                optopt);
        return 1;
      default:
        print_usage();
    }
  // debug

  //g_m = 90;
  //g_q = 1;

  //debug
  if (g_m < 0) {
    fprintf(stderr, "Error: Missing arguments.\nPlease use -m with arguments and check again\n");
    print_usage();
  }


  if (N < 0) {
    fprintf(stderr, "Error: Missing arguments.\nPlease use -n with arguments and check again\n");
    print_usage();
  }


  if (data_source == NULL) {
    fprintf(stderr, "Error: Missing arguments.\nPlease use -d with arguments and check again\n");
    print_usage();
  }

  if (query_source == NULL) {
    fprintf(stderr, "Error: Missing arguments.\nPlease use -s with arguments and check again\n");
    print_usage();
  }

  /*
    if (g_element_random_order == 1 && g_element_idf_avg_order == 1) {
      fprintf(stderr, "Error: only one element order can be set for a time.\nPlease use -a and -r separately\n");
      print_usage();
    }
   */
  //fprintf(stdout, "m is %d\n", g_m);

  q_sig_search(g_m);

  return EXIT_SUCCESS;
}

int q_sig_search(int g_m) {
  int i, j, k;
  double time;
  int m_num;
  //char can_name_buffer[256];
  //char avr_can_name_buffer[256];
  char match_name_buffer[256];
  double temp0, temp1, switch_point, proba;
  double beta[MAX_PARTI];
  //char can_index_name_buffer[256];
  //char avr_can1_name_buffer[256];
  //sprintf(can_name_buffer, "./can_stat_m=%d_q=%d_o=%d.txt", g_m, g_q, order_num);
  //sprintf(can_index_name_buffer, "./index_stat_m=%d_q=%d_o=%d_can.txt", g_m, g_q, order_num);
  sprintf(match_name_buffer, "./match_m=%d_q=%d_o=%d.txt", g_m, g_q, order_num);

  //init power_hashmap
  power_hashmap = init_power_hashmap(N, power_hashmap);

  //init bitmap mask
  for (i = 0; i < U_INT_LEN / 2; i++) {
    mask[i] = 3 << (i << 1);
  }


  /*
    if (g_element_random_order == 1) {
    order_num = 0;
    }
   */
  // link data to stream
  FILE * fp_record;
  if ((fp_record = fopen(data_source, "rt+")) == NULL) {
    fprintf(stderr, "No Document has been found.\n");
    return EXIT_FAILURE;
  }
  // set a buffer to store combining elements as a sig

  // read data
  g_curr_record_num = read_all_documents(fp_record, g_records);
  fclose(fp_record);
  //calculate best Q based on m 
  g_q = calculate_Q(g_records[0].len, g_m);
  //fprintf(stdout, "partition length is %d\n", g_q);
  //if (g_q <= 15){
  //collapse_filtering = 0;
  //}
  //sig_str_buffer = (char *) malloc(sizeof (char) * 2 * g_q * MAX_ELEMENT_LEN);

  if (g_curr_record_num < 0) {
    fprintf(stderr, "Error: Read data error\n");
    return EXIT_FAILURE;
  }

  // init the hashtable
  init_element_index(MAX_ELEMENT_NUM);
  // create space for elements
  for (i = 0; i < g_curr_record_num; i++) {
    if ((g_records[i].element_slots = (element_slot_t *) malloc(sizeof (element_slot_t) * g_records[i].len)) == NULL) {
      fprintf(stderr, "ERROR: Out of memory\n");
      exit(-1);
    }
    // build the element list
    build_record_elements(g_records[i].str, g_records[i].len, g_records[i].element_slots);
  }

  //random the frq to random the element order
  //if (g_element_random_order == 1) {
  if (bit_fv) {
    hash_element_in_dimension(_element_block_head);
    bitwise_record(g_records, g_curr_record_num);
  }

  free(_buffer);
  //sort_all_element_list_by_freq(g_records, g_curr_record_num);
  //}

  // initiate index
  /*
    if(collapse_filtering){
    rand_bucket_num(_element_block_head);
    //fprintf(stdout,"%u\t%u\t%u\t%u\n", g_records[0].element_slots[0].element->bucket_num, g_records[0].element_slots[1].element->bucket_num, 
    //g_records[0].element_slots[2].element->bucket_num, g_records[0].element_slots[3].element->bucket_num );
    rec_bucket_init(g_records, g_curr_record_num);
    //fprintf(stdout,"%u\t%u\t%u\t%u\n", g_records[0].bucket[0], g_records[0].bucket[1], 
    //g_records[0].bucket[3], g_records[0].bucket[4]);
    }
   */

  init_sig_index(MAX_SIG_NUM);
  // create data sigs for each record from their elements
  for (i = 0; i < g_curr_record_num; i++) {

    //calculate the number of sig_prefix for each record
    g_records[i].sig_num = calculate_sig_num(&g_records[i], g_m);
    //set space for those sigs
    if ((g_records[i].sig_slots = (sig_slot *) malloc(sizeof (sig_slot) * g_records[i].sig_num)) == NULL) {
      fprintf(stderr, "ERROR: Out of memory\n");
      exit(-1);
    }
    //create sigs
    build_sigs(&g_records[i], g_q, g_m);
  }


  //random the frq to random the sig order
  //if (g_sig_random_order == 1) {
  //  random_sig_frq(_sig_block_head);
  //}
  //sort those sigs by idf
  //sort_all_sig_list_by_freq(g_records, g_curr_record_num);
  // build index
  build_sig_idf_list(g_records, g_curr_record_num, g_m, g_q);

  fprintf(stdout, "The number of documents:%d\n", g_curr_record_num);
  printf("\n");
  // print out the average length of the documents in data source
  int sum = 0;
  int avg_len = 0;
  for (i = 0; i < g_curr_record_num; i++) {
    sum += g_records[i].len;
  }
  avg_len = sum / g_curr_record_num;
  fprintf(stdout, "The average length of these documents is:%d\n", avg_len);
  fprintf(stdout, "\n");
  //show the information in the index
  fprintf(stdout, "The number of different elements is %d\n", g_element_num);
  fprintf(stdout, "The number of different sigs is %d\n", g_sig_num);


  //search part

#ifndef DEBUG_INDEX
  //output result

  if ((fp_query_match = fopen(match_name_buffer, "w+")) == NULL) {
    fprintf(stderr, "Error: create file error\n");
    return EXIT_FAILURE;
  }
  //output candidate status
  /*
    if ((fp_query_cand = fopen(can_name_buffer, "w+")) == NULL) {
      fprintf(stderr, "Document creating error.\n");
      return EXIT_FAILURE;
    }

    if ((fp_query_cand_index = fopen(can_index_name_buffer, "w+")) == NULL) {
      fprintf(stderr, "Document creating error.\n");
      return EXIT_FAILURE;
    }
   */
  //FILE * fp_query_stat;
  //if ((fp_query_stat = fopen("/Users/xyzhang/Desktop/q_sig/search_stat.txt", "w+")) == NULL) {
  //    fprintf(stderr, "Document creating error.\n");
  //    return EXIT_FAILURE;
  //}

  // data query
  FILE * fp_query;
  if ((fp_query = fopen(query_source, "rt+")) == NULL) {
    fprintf(stderr, "No Document has been found.\n");
    return EXIT_FAILURE;
  }

  //read query data
  g_curr_query_num = read_all_queries(fp_query, g_query);
  fclose(fp_query);
  //reset timer


  if (g_curr_query_num < 0) {
    fprintf(stderr, "Error: Read query error\n");
    return EXIT_FAILURE;
  }

  double average_can0, average_can1, average_can2, average_can3, average_can4, average_can5, average_esti_can1;
  average_can0 = 0;
  average_can1 = 0;
  average_can2 = 0;
  average_can3 = 0;
  average_can4 = 0;
  average_can5 = 0;
  //average_esti_can1 = 0;

  for (i = 0; i < g_curr_query_num; i++) {
    //for (i = 63; i<64 ;i++){
    //init_query_element_head();
    //set space for query elements
    if ((g_query[i].element_slots = (element_slot_t *) malloc(sizeof (element_slot_t) * g_query[i].len)) == NULL) {
      fprintf(stderr, "ERROR: Out of memory\n");
      exit(-1);
    }
    //create query elements
    build_query_elements(g_query[i].str, g_query[i].len, g_query[i].element_slots);

    if (bit_fv) {
      bitwise_query(g_query, i);
    }
    //set order by dimension

    /*
        if(collapse_filtering){
        //init_query_sig_head();
        que_bucket_init(g_query, i);
         }
     */
  }

  ResetUsage();
  mytimer preptimer;
  preptimer.timesum = 0;
  mytimer probetimer;
  probetimer.timesum = 0;

  init_search(g_curr_record_num);
  
  
  
  for (i = 0; i < g_curr_query_num; i++) {
#ifdef DEBUG_OUTPUT
    can0 = 0;
    can1 = 0;
    can2 = 0;
    can3 = 0;
    can4 = 0;
    can5 = 0;
#endif
    
  proba = 0;
  temp0 = 1;
  temp1 = 0;
    //calculate the number of sig_prefix for each query
    g_query[i].sig_num = calculate_query_sig_num(&g_query[i], g_m);

    if ((g_query[i].sig_slots = (sig_slot *) malloc(sizeof (sig_slot) * g_query[i].sig_num)) == NULL) {
      fprintf(stderr, "ERROR: Out of memory\n");
      exit(-1);
    }
    
    //build query sigs
    build_query_sigs(&(g_query[i]), g_q, g_m);

    k = 0;
     for (j = 0; j < (N- g_m +3)/2; j++) {
        //idf[k]= g_query[i].sig_slots[j].sig->last_idf;
        can0 += partition_can0[j];
        beta[k] = (double)(partition_can0[j] - (partition_len[j] - 1) * partition_exact[j] )/ (double)g_curr_record_num;
        temp0 *= (1 - beta[k]);
        k++;
    }
    
     for (j = 0; j < k; j++) {
       //fprintf(stderr, "%e %e\n", temp0, beta[j]);
        proba = (temp0 / (1 - beta[j])) * beta[j];
        //fprintf(stderr, "%e\n", proba);
        temp1 += proba;
    }
   
   switch_point = (double)can0 / ((double)g_curr_record_num * (temp0 + temp1));
   //average_esti_can1 += ((double)g_curr_record_num * ((double)1 - temp0 - temp1));

    if ((N - g_m) % 2 == 0) {
      if ( switch_point > alpha_even) {
        StartTimer(&probetimer);
        can1 = g_curr_record_num;
        for (j = 0; j < g_curr_record_num; j++) {
          m_num = bitwise_check_line_based(&(g_query[i]), &(g_records[j]), g_m);
          if (m_num >= g_m) {
            can4++;
          }
        }
        PauseTImer(&probetimer);
        can0 = g_curr_record_num;
      } else {
        StartTimer(&probetimer);
        search_in_index(g_query, i, g_records, g_curr_record_num, g_m, g_q);
        PauseTImer(&probetimer);
      }
    } else {
      if ( switch_point > alpha_odd) {
        can1 = g_curr_record_num;
        StartTimer(&probetimer);
        for (j = 0; j < g_curr_record_num; j++) {
          m_num = bitwise_check_line_based(&(g_query[i]), &(g_records[j]), g_m);
          if (m_num >= g_m) {
            can4++;
          }
        }
        PauseTImer(&probetimer);
        can0 = g_curr_record_num;
      } else {
        StartTimer(&probetimer);
        search_in_index_odd(g_query, i, g_records, g_curr_record_num, g_m, g_q);
        PauseTImer(&probetimer);
      }
    }
  
    //free malloc space
    //free(g_query[i].element_slots);
    //free(g_query[i].sig_slots);
#ifdef DEBUG_OUTPUT
    average_can0 += can0;
    average_can1 += can1;
    average_can2 += can2;
    average_can3 += can3;
    average_can4 += can4;
    average_can5 += can5;
    //average_m_num += avg_m_num;
#endif
  }
  //  fprintf(fp_query_stat,"query[%d]\t%d\t%d\n", i, g_query[i].can0, g_query[i].pair_num);
  //}

  /*
  #ifdef dump_one_query
    char order_buffer[256];
    sprintf(order_buffer, "./order_m=%d_q=%d_o=%d.txt", g_m, g_q, order_num);
    if ((fp_output_order = fopen(order_buffer, "w+")) == NULL) {
      fprintf(stderr, "Error: create file error\n");
      return EXIT_FAILURE;
    }
    for (i =0; i<order_num;i++){
    dump_element_random_order(_element_block_head, g_m, g_q, i);
    }
  #endif
   */

  //#ifdef DEBUG_OUTPUT
  time = ShowUsage();
  fprintf(stdout, "Usage: %s\n", __usage_information);

  average_can0 /= g_curr_query_num;
  average_can1 /= g_curr_query_num;
  average_can2 /= g_curr_query_num;
  average_can3 /= g_curr_query_num;
  average_can4 /= g_curr_query_num;
  average_can5 /= g_curr_query_num;
  //average_esti_can1 /= g_curr_query_num;

  /*
          FILE * fp_avr_cand1_num;
      if ((fp_avr_cand1_num = fopen(avr_can1_name_buffer, "rt+")) == NULL) {
        fprintf(stderr, "No Document has been found.\n");
        return EXIT_FAILURE;
      }
   */

  //print out the query's name
  int p;
  char * data;
  data = query_source;
  p = strlen(data) - 1;
  while (data[p] != '/') {
    p--;
  }
  p++;
  while (data[p] != '\0' && data[p] != '.') {
    fprintf(stderr, "%c", data[p]);
    p++;
  }
  fprintf(stderr, " ");

  fprintf(stderr, "m %d partition_len %d ", g_m, g_q);
  fprintf(stderr, "time %.6f can0 %f ", time, average_can0);
  fprintf(stderr, "can1 %f ", average_can1);
  //fprintf(stderr, "esti_can1 %f ", average_esti_can1);
  fprintf(stderr, "can2 %f ", average_can2);
  fprintf(stderr, "can3 %f ", average_can3);
  //fprintf(stderr, "can4 %Lf ", average_can5);
  fprintf(stderr, "can4 %f bitnum: %d\n", average_can4, bit_per_d);
  //fprintf(stderr, "avg_m_num_in_pfx\t%Lf\n", average_m_num);
  //dump useful order
  //dump_element_order(_element_block_head, g_m, g_q);
  //dump_sig_order(_sig_block_head, g_m, g_q);
  //dump_sig_order_in_prefix(_sig_block_head, g_m, g_q);

  //free space
  /*
    destroy_element_index();
    destroy_sig_index();
    for (i = 0; i < g_curr_record_num; i++) {
      free(g_records[i].element_slots);
      free(g_records[i].sig_slots);
    }

    //destroy_query_element_blocks();
    //destroy_query_sig_blocks();

    free(sig_str_buffer);
    free(_buffer);
   */
  //#endif
#endif 
  return EXIT_SUCCESS;
}
