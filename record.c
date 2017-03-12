/*
 * records.c
 *
 *  Created on: Oct 22, 2011
 *      Author: jianbinqin
 *      revised:xiaoyang
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include "header.h"
#include "hash_dict.h"



char *_buffer;
int _buffer_pos = 0;
extern int sort_data_flag;
//extern len_node lnode[RECORD_MAX_LINE_NUM];;
extern int len_node_num;
hash_dict_t *_index_hash_dict2 = NULL;

/*
static int compare(const void *a, const void *b) {
  const record_t *ap = (record_t *) a;
  const record_t *bp = (record_t *) b;

  //  return (*da > *db) - (*da < *db);
  return (ap->len > bp->len) - (ap->len < bp->len);
}
*/

/*
 * Read all documents from the files and store it in the record
 * data type. This part can be revised for the large memory
 * Consumption.
 */

/*
void init_index2(int size) {
    if (_index_hash_dict2 != NULL) {
        hash_dict_destory(_index_hash_dict2);
    }
    _index_hash_dict2 = hash_dict_create(size * 1.5);
}
 */
int read_all_documents(FILE* fp, record_t * records) {
  int j;
  //int temp = 0;
  int num = 0;
  /* buffer for read in */
  char readbuffer[ RECORD_MAX_LINE_LEN ];

  if (fp == NULL) {
    fprintf(stderr, "Error: Input error\n");
    return -1;
  }
  _buffer = (char *) malloc(sizeof (char) *MAX_FILE_SIZE);
  if (_buffer == NULL) {
    fprintf(stderr, "Error; Memory allocate err in raed_all_document for _buffer ");
    return -1;
  }
  while (fgets(readbuffer, RECORD_MAX_LINE_LEN, fp) != NULL) {
    int len = strlen(readbuffer);

    // wipe out the new line char
    while (len > 0 && (readbuffer[len - 1] == '\n' || readbuffer[len - 1] == '\r')) {
      readbuffer[len - 1] = '\0';
      len--;
    }
    if (len <= 0) continue;

    strcpy(_buffer + _buffer_pos, readbuffer);

    records[num].str = _buffer + _buffer_pos;
    records[num].len = 1;
    records[num].sig_num = 0;
    records[num].element_slots = NULL;
    records[num].sig_slots = NULL;
    _buffer_pos = _buffer_pos + (len + 1);

    if (_buffer_pos + RECORD_MAX_LINE_LEN > MAX_FILE_SIZE) {
      fprintf(stderr, "Error input file exist maximal filesize: %d\n", MAX_FILE_SIZE);
      return -1;
    }
    //calculate size of each object
    for (j = 0; j < len; j++) {
      //case insensitive
      //if (readbuffer[j] >= 'A' && readbuffer[j] <= 'Z')
      //readbuffer[j] = readbuffer[j]-('A' - 'a');
      if (readbuffer[j] == ' ') {
        //data should be cleared, each element is separated by exactly one space from the other
        records[num].len++;
      }
    }
    num++;
  }


  //to sort the whole record list
  //if (sort_data_flag)
  //qsort(records, num, sizeof (record_t), compare);

  //catch the length division point in the records
  /*
  init_index2(RECORD_MAX_LINE_LEN);
  hash_dict_node_t snode;
  len_node *p;
  int sign;
    
  for (i = 0; i < num; i++) {
      if (records[i].len ==temp) continue;
          temp = records[i].len;
          lnode[len_node_num].len = temp;
          lnode[len_node_num].id = i;
          p= &lnode[len_node_num];
          sign=temp;
          create_sign_lnode(sign, &snode.sign1, &snode.sign2);
          snode.pointer = (void *) p;
          res = hash_dict_add(_index_hash_dict2, &snode, 0);
          len_node_num++;
  }
  int i = 0;
  for ( i = 0; i < num ;i ++ ) fprintf ( stderr, "[%d] %s\n", i, records[i].str );
   */

  return num;
}




/* int read_all_bin_documents(char *filename, record_t * records) { */
/*   int j; */
/*   //int temp = 0; */
/*   int num = 0; */
  
/*   char readbuffer[ RECORD_MAX_LINE_LEN ]; */

/*   if (fp == NULL) { */
/*     fprintf(stderr, "Error: Input error\n"); */
/*     return -1; */
/*   } */
/*   _buffer = (char *) malloc(sizeof (char) *MAX_FILE_SIZE); */
  
/*   if (_buffer == NULL) { */
/*     fprintf(stderr, "Error; Memory allocate err in raed_all_document for _buffer "); */
/*     return -1; */
/*   } */
 
/*   FILE *fp = fopen(filename, "rb"); */
/*   uint64_t N = 0; */
/*   int B, D; */

/*   // Read the dimentionality informations. */
/*   fread(&N, sizeof(uint64_t), 1, fp); */
/*   fread(&D, sizeof(int), 1, fp); */
/*   fread(&B, sizeof(int), 1, fp); */

/*   int Dim = D; */
/*   int DataBytes = B; */
/*   uint64_t  NumData = N; */

/*   uint8_t* Data = new uint8_t[mNumData*mDataBytes]; */
/*   fread(mData, sizeof(uint8_t), mNumData*mDataBytes, fp); */
/*   fclose(fp); */

/*   for (int i = 0; i < N; i ++) { */
    



    
/*   } */




  
  
/*   while (fgets(readbuffer, RECORD_MAX_LINE_LEN, fp) != NULL) { */
/*     int len = strlen(readbuffer); */

/*     // wipe out the new line char */
/*     while (len > 0 && (readbuffer[len - 1] == '\n' || readbuffer[len - 1] == '\r')) { */
/*       readbuffer[len - 1] = '\0'; */
/*       len--; */
/*     } */
/*     if (len <= 0) continue; */

/*     strcpy(_buffer + _buffer_pos, readbuffer); */

/*     records[num].str = _buffer + _buffer_pos; */
/*     records[num].len = 1; */
/*     records[num].sig_num = 0; */
/*     records[num].element_slots = NULL; */
/*     records[num].sig_slots = NULL; */
/*     _buffer_pos = _buffer_pos + (len + 1); */

/*     if (_buffer_pos + RECORD_MAX_LINE_LEN > MAX_FILE_SIZE) { */
/*       fprintf(stderr, "Error input file exist maximal filesize: %d\n", MAX_FILE_SIZE); */
/*       return -1; */
/*     } */
/*     //calculate size of each object */
/*     for (j = 0; j < len; j++) { */
/*       //case insensitive */
/*       //if (readbuffer[j] >= 'A' && readbuffer[j] <= 'Z') */
/*       //readbuffer[j] = readbuffer[j]-('A' - 'a'); */
/*       if (readbuffer[j] == ' ') { */
/*         //data should be cleared, each element is separated by exactly one space from the other */
/*         records[num].len++; */
/*       } */
/*     } */
/*     num++; */
/*   } */


/*   //to sort the whole record list */
/*   //if (sort_data_flag) */
/*   //qsort(records, num, sizeof (record_t), compare); */

/*   //catch the length division point in the records */
/*   /\* */
/*   init_index2(RECORD_MAX_LINE_LEN); */
/*   hash_dict_node_t snode; */
/*   len_node *p; */
/*   int sign; */
    
/*   for (i = 0; i < num; i++) { */
/*       if (records[i].len ==temp) continue; */
/*           temp = records[i].len; */
/*           lnode[len_node_num].len = temp; */
/*           lnode[len_node_num].id = i; */
/*           p= &lnode[len_node_num]; */
/*           sign=temp; */
/*           create_sign_lnode(sign, &snode.sign1, &snode.sign2); */
/*           snode.pointer = (void *) p; */
/*           res = hash_dict_add(_index_hash_dict2, &snode, 0); */
/*           len_node_num++; */
/*   } */
/*   int i = 0; */
/*   for ( i = 0; i < num ;i ++ ) fprintf ( stderr, "[%d] %s\n", i, records[i].str ); */
/*    *\/ */

/*   return num; */
/* } */

















