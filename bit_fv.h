/* 
 * File:   bit_fv.h
 * Author: xyzhang
 *
 * Created on November 5, 2012, 1:12 PM
 */

#ifndef BIT_FV_H
#define	BIT_FV_H

#include "hash_dict.h"
#include "header.h"
#define D_CHUNK_MAX 8
#define U_PC_LEN 32

void hash_element_in_dimension(element_block_t *_element_block_head);
int bitwise_record(record_t * rec, int rec_num);
int bitwise_query(query_t * rec, int rec_num);
int bitwise_check_line_based(query_t * query, record_t * rec, int m);
int bit_count64(unsigned long long i);
int bit_count32(unsigned v); 
int bitwise_check_once_verify(query_t * query, record_t * rec, int m);


#endif	/* BIT_FV_H */

