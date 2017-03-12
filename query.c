
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


char *_buffer2;
int _buffer2_pos = 0;

/*
static int compare(const void *a, const void *b) {
  const query_t *ap = (query_t *) a;
  const query_t *bp = (query_t *) b;

  //  return (*da > *db) - (*da < *db);
  return (ap->len > bp->len) - (ap->len < bp->len);
}
*/

/*
 * Read all documents from the files and store it in the record
 * data type. This part can be revised for the large memory
 * Consumption.
 */
int read_all_queries(FILE* fp, query_t * query) {
  int j;
  int num = 0;
  /* buffer for read in */
  char readbuffer[ RECORD_MAX_LINE_LEN ];

  if (fp == NULL) {
    fprintf(stderr, "Error: Input error\n");
    return -1;
  }

  _buffer2 = (char *) malloc(sizeof (char) *MAX_QUERY_FILE_SIZE);
  if (_buffer2 == NULL) {
    fprintf(stderr, "Error; Memory allocate err in raed_all_document for _buffer2 ");
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
    strcpy(_buffer2 + _buffer2_pos, readbuffer);

    query[num].str = _buffer2 + _buffer2_pos;
    query[num].len = 1;
    query[num].sig_num = 0;
    query[num].element_slots = NULL;
    query[num].sig_slots = NULL;
    _buffer2_pos = _buffer2_pos + (len + 1);

    if (_buffer2_pos + RECORD_MAX_LINE_LEN > MAX_FILE_SIZE) {
      fprintf(stderr, "Error input file exist maximal filesize: %d\n", MAX_FILE_SIZE);
      return -1;
    }
    //record num of characters in each document
    for (j = 0; j < len; j++) {
      //case insensitive
      //if (readbuffer[j] >= 'A' && readbuffer[j] <= 'Z')
      //readbuffer[j] = readbuffer[j]-('A' - 'a');
      //count
      if (readbuffer[j] == ' ') {
        //data should be cleared, each element is separated by exactly one space from the other
        query[num].len++;
      }
    }
    num++;
  }


  //to sort the whole record list
  //if (sort_data_flag)
  // qsort(records, num, sizeof (record_t), compare);

  /**
  int i = 0;
  for ( i = 0; i < num ;i ++ ) fprintf ( stderr, "[%d] %s\n", i, records[i].str );
   **/

  return num;
}


















