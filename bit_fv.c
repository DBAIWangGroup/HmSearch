#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include "bit_fv.h"
#include "header.h"

int max_num_in_dimension;
int bit_per_d;
int bit_dimension_chunk;
int * value_in_dimension;
extern int can5;
extern int N;

void rehash_bit_in_dimension(element_block_t *_element_block_head);

void hash_element_in_dimension(element_block_t *_element_block_head) {
  int i;
  //int buffer;
  int dimension;
  //int * value_in_dimension;
  //int value_in_dimension[64] = {0};
  //unsigned tmp;
  //unsigned order[N];

  element_block_t *element_block_p, *tmp_p;
  element_block_p = _element_block_head;
  value_in_dimension = (int *) calloc(N, sizeof (int));
  //count number of different value in each dimension
  srand((unsigned) (time(NULL)));

  while (element_block_p) {
    for (i = 0; i < element_block_p->last_free; i++) {
      dimension = element_block_p->elements[i].dim;
      element_block_p->elements[i].hash_d = value_in_dimension[dimension];
      value_in_dimension[dimension]++;
      //fprintf(fp_output_element_listlen, "%s\t%d\n", sig_block_p->sigs[i].buffer, times);
      //hash the id to different value
    }
    tmp_p = element_block_p->next;
    element_block_p = tmp_p;
  }

  //find the largest number for hashing
  max_num_in_dimension = 0;

  for (i = 0; i < N; i++) {
    if (value_in_dimension[i] > max_num_in_dimension) {
      max_num_in_dimension = value_in_dimension[i];
    }
  }

  if(max_num_in_dimension == 2){
  bit_per_d = 1;
  }
  else{
  bit_per_d = ceil(log2f((float) (max_num_in_dimension + 1)));
  }
  //bit_per_d = ceil(log2f((float) (max_num_in_dimension)));
  //bit_per_d = 5;
  //rehash_bit_in_dimension(_element_block_head);
  //fprintf(stderr, "%d\n", bit_per_d);
}

int bitwise_record(record_t * rec, int rec_num) {
  int i, j, k, l;
  int hash_value, temp, position;
  unsigned digit;
  //plus one for unknown element in query
  //bit_per_d = (int)log2f((float)(max_num_in_dimension + 1)) + 1;
  bit_dimension_chunk = ceil((double) N / U_PC_LEN);
  //int * bit_buffer;
  //bit_buffer = (int *)malloc(sizeof(int)*bit_per_d);


  //init space 
  unsigned * ptr_data = (unsigned *) calloc(rec_num * bit_per_d * bit_dimension_chunk, sizeof (unsigned));
  unsigned * * ptr_ptr = (unsigned * *) malloc(sizeof (unsigned *) * rec_num * bit_per_d);

  for (i = 0; i < rec_num; i++) {
    rec[i].bit_rec = (unsigned **) (ptr_ptr + i * bit_per_d);
    for (j = 0; j < bit_per_d; j++) {
      rec[i].bit_rec[j] = (unsigned *) (ptr_data + i * bit_per_d * bit_dimension_chunk + j * bit_dimension_chunk);
    }
  }

  //store bit information
  //for each element in each record
  for (i = 0; i < rec_num; i++) {
    for (j = 0; j < rec[i].len; j++) {
      hash_value = rec[i].element_slots[j].element->hash_d;
      //set bit
      //position to set
      k = j / U_PC_LEN;
      position = U_PC_LEN - j % U_PC_LEN - 1;
      temp = hash_value;
      for (l = 0; l < bit_per_d; l++) {
        //get the digit
        digit = temp % 2;
        //temp/2
        temp /= 2;
        //shift to position
        digit = digit << position;
        //add bit to position
        rec[i].bit_rec[l][k] |= digit;
      }
    }
  }
  return EXIT_SUCCESS;
}

int bitwise_query(query_t * rec, int id) {
  int j, k, l;
  int hash_value, temp, position;
  unsigned digit;

  unsigned * ptr_q_data = (unsigned *) calloc(bit_per_d * bit_dimension_chunk, sizeof (unsigned));

  rec[id].bit_rec = (unsigned * *) malloc(sizeof (unsigned *) * bit_per_d);

  for (j = 0; j < bit_per_d; j++) {
    rec[id].bit_rec[j] = (unsigned *) (ptr_q_data + j * bit_dimension_chunk);
  }


  //store bit information
  //for each element in each record
  for (j = 0; j < rec[id].len; j++) {
    hash_value = rec[id].element_slots[j].element->hash_d;
    //set bit
    //position to set
    k = j / U_PC_LEN;
    position = U_PC_LEN - j % U_PC_LEN - 1;
    temp = hash_value;
    for (l = 0; l < bit_per_d; l++) {
      //get the digit
      digit = temp % 2;
      //temp/2
      temp = temp >> 1;
      //shift to position
      digit = digit << position;
      //add bit to position
      rec[id].bit_rec[l][k] |= digit;
    }
  }
  return EXIT_SUCCESS;
}


int bitwise_check_line_based(query_t * query, record_t * rec, int m) {
  int i, j, sum;
  int count[D_CHUNK_MAX] = {0};
  unsigned buffer[D_CHUNK_MAX] = {0};
  unsigned buffer2[D_CHUNK_MAX] = {0};
  //naive
  for (i = 0; i < bit_per_d; i++) {
    sum = 0;
    /*
    #ifdef DEBUG_OUTPUT
        if (i == 1) {
          can5++;
        }
    #endif
     */
    for (j = 0; j < bit_dimension_chunk; j++) {
      buffer2[j] = query->bit_rec[i][j] ^ rec->bit_rec[i][j];
      buffer[j] |= buffer2[j];
      count[j] = bit_count32(buffer[j]);
      sum += count[j];
    }
    if (sum > N - m) {
      return -1;
    }
  }
  return (N - sum);
}

int bitwise_check_once_verify(query_t * query, record_t * rec, int m) {
  int i, j, sum;
  int count[D_CHUNK_MAX] = {0};
  unsigned long long buffer[D_CHUNK_MAX] = {0};
  unsigned long long buffer2[D_CHUNK_MAX] = {0};
  //naive
  sum = 0;
  for (j = 0; j < bit_dimension_chunk; j++) {
    for (i = 0; i < bit_per_d; i++) {
      buffer2[j] = query->bit_rec[i][j] ^ rec->bit_rec[i][j];
      buffer[j] |= buffer2[j];
    }
    count[j] = bit_count32(buffer[j]);
    sum += count[j];
    if (sum > N - m) {
      return -1;
    }
  }
  return (N - sum);
}

/*
inline int bit_count64(unsigned long long i) {
  i = i - ((i >> 1) & 0x5555555555555555UL);
  i = (i & 0x3333333333333333UL) + ((i >> 2) & 0x3333333333333333UL);
  return (int) ((((i + (i >> 4)) & 0xF0F0F0F0F0F0F0FUL) * 0x101010101010101UL) >> 56);
}
 */

inline int bit_count32(unsigned v) {
  v = v - ((v >> 1) & 0x55555555); // reuse input as temporary
  v = (v & 0x33333333) + ((v >> 2) & 0x33333333); // temp
  return (int) ((((v + (v >> 4)) & 0xF0F0F0F) * 0x1010101) >> 24);
}

/*
int transfer_int_to_array(int * buffer, int len, int number){
  int i;
  int number_digit, number_digit_len, number_temp, max;
  max = pow((double)2,(double)(len+1)) - 1;
  if (max < len) {
    printf("error, in transfer_int_to_array illegal argument!\n");
    exit - 1;
  }
  number_temp = number;
  i = 0;
  for(i=0; i < len; i++){
    buffer[i] = number_temp % 2;
    number_temp >> 1;
  }
}
 */

void rehash_bit_in_dimension(element_block_t *_element_block_head) {
  int i, max;
  element_block_t *element_block_p, *tmp_p;
  element_block_p = _element_block_head;

  max = pow((double) 2, (double) bit_per_d);

  element_block_p = _element_block_head;

  //hash the value in each dimension to make 0 and 1 in each level have the same
  //probability to appear
  //0         1             2            3            4              5
  //0      2^t - 1          1         2^t - 2         2           2^t - 3
  while (element_block_p) {
    for (i = 0; i < element_block_p->last_free; i++) {
      if (element_block_p->elements[i].hash_d % 2 == 0) {
        element_block_p->elements[i].hash_d >>= 1;
      } else {
        element_block_p->elements[i].hash_d = max - (element_block_p->elements[i].hash_d) / 2 - 1;
      }
      //fprintf(fp_output_element_listlen, "%s\t%d\n", sig_block_p->sigs[i].buffer, times);
    }
    tmp_p = element_block_p->next;
    element_block_p = tmp_p;
  }
}