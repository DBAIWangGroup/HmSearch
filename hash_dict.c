

/**
  This is a hash dictionary created by Qinjianbin. Based on the idea of odict.

  date: April 22 2009
  Author: Jianbin Qin
 
 **/


#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <openssl/md5.h>
#include "hash_dict.h"
#include <assert.h>
#include <openssl/sha.h>
/**
   Func: to Create a hash dict from a hash_number
   In: hash number
   Out: != NULL, success .
        == NULL. Failed
 **/

hash_dict_t *hash_dict_create(unsigned int hash_number) {
  hash_dict_t *hdp;
  hash_table_t *htp;
  int i;
  if ((hdp = (hash_dict_t *) malloc(sizeof (hash_dict_t))) == NULL) {
    fprintf(stderr, "Fetal: Can not allocate memory for hash dict\n");
    return NULL;
  }

  if ((htp = (hash_table_t *) malloc(sizeof (hash_table_t) * hash_number)) == NULL) {
    fprintf(stderr, "Fetal: Can not allocate memory for hash table\n");
    free(hdp);
    return NULL;
  }

  hdp->hash_num = hash_number;
  hdp->node_num = 0;
  hdp->hash_table = htp;
  for (i = 0; i < hash_number; i++) {
    hdp->hash_table[i].pnode = NULL;
  }
  return hdp;
}

/**
   Renew the hash dict
   IN hashi dict pointer;
   out Alwayers success.
 **/
int hash_dict_renew(hash_dict_t *hdp) {
  unsigned int i;
  hash_dict_node_t *pnode, *pfree;

  assert(hdp != NULL);

  for (i = 0; i < hdp->hash_num; i++) {
    pnode = hdp->hash_table[i].pnode;
    while (pnode != NULL) {
      pfree = pnode;
      pnode = pnode->next;
      free(pfree);
    }
    hdp->hash_table[i].pnode = NULL;
  }
  return 0;
}

/**
   Func: To destory a hash dict
   IN: hash dict pointer.
   Out: 0  Success. always success..
 **/
int hash_dict_destory(hash_dict_t *hash_dict) {
  assert(hash_dict != NULL);
  hash_dict_renew(hash_dict);
  free(hash_dict->hash_table);
#ifndef ITERATION
  free(hash_dict);
#endif  
  return 0;
}

/**
   Func: to do search on hash_dict.
   In: hash_dict, search node info structure
   Out: RT_HASH_DICT_SEARCH_SUCC, success find the search node.
        RT_HASH_DICT_SEARCH_MISS, node not find.
        -1, input  failed finish the operation.
 **/
int hash_dict_search(hash_dict_t *hdp, hash_dict_node_t *snode) {
  unsigned int hkey;
  hash_dict_node_t *pt_node;
  int ret;

  assert(hdp != NULL);
  assert(snode != NULL);

  if (snode->sign1 == 0 && snode->sign2 == 0) {
    fprintf(stderr, "Fetal: Hash_dict search node error \n");
    return -1;
  }

  hkey = (snode->sign1 + snode->sign2) % hdp->hash_num;
  pt_node = hdp->hash_table[hkey].pnode;
  while (pt_node != NULL) {
    if ((pt_node->sign1 == snode->sign1) && (pt_node->sign2 == snode->sign2))
      break;
    pt_node = pt_node->next;
  }

  if (pt_node == NULL) { // if not find. clean the result
    snode->pointer = NULL;
    snode->code = 0;
    ret = RT_HASH_DICT_SEARCH_MISS;
  } else { // if find ,
    snode->pointer = pt_node->pointer;
    snode->code = pt_node->code;
    ret = RT_HASH_DICT_SEARCH_SUCC;
  }
  return ret;
}

/**
   Func: to do add node  on hash_dict.
   In: hash_dict, add node info. is_overwrite.
   Out: RT_HASH_DICT_ADD_ERROR: memory allocation error
        RT_HASH_DICT_ADD_EXIST: the node is existed and without overwrite.
        RT_HASH_DICT_ADD_SUCC: success to add a node to hash_dict;
        -1, input error the operation.
 **/
int hash_dict_count(hash_dict_t *hdp, hash_dict_node_t *snode) {
  unsigned int hkey;
  hash_dict_node_t *pt_node, *pfree = NULL;


  assert(hdp != NULL);
  assert(snode != NULL);

  //        fprintf(stderr, "Fetal: Hash_dict insert node error \n");
  //     return -1;
  // }

  hkey = (snode->sign1 + snode->sign2) % hdp->hash_num;
  pt_node = hdp->hash_table[hkey].pnode;
  while (pt_node != NULL) {
    if ((pt_node->sign1 == snode->sign1) && (pt_node->sign2 == snode->sign2)) {
      break;
    }
    pfree = pt_node;
    pt_node = pt_node->next;
  }

  if (pt_node == NULL) { // no exist node. craete a new node .
    if ((pfree = (hash_dict_node_t *) malloc(sizeof (hash_dict_node_t))) == NULL) {
      fprintf(stderr, "Fatel: Hass node memory allocate error \n");
      return RT_HASH_DICT_ADD_ERROR;
    }
    pfree->sign1 = snode->sign1;
    pfree->sign2 = snode->sign2;
    pfree->pointer = snode->pointer;
    pfree->code = 1;
    pfree->next = hdp->hash_table[hkey].pnode;
    hdp->hash_table[hkey].pnode = pfree;
    hdp->node_num++;
  } else {
    pt_node -> code++;
  }
  return 0;
}

/**
   Func: to do add node  on hash_dict.
   In: hash_dict, add node info. is_overwrite.
   Out: RT_HASH_DICT_ADD_ERROR: memory allocation error
        RT_HASH_DICT_ADD_EXIST: the node is existed and without overwrite.
        RT_HASH_DICT_ADD_SUCC: success to add a node to hash_dict;
        -1, input error the operation.
 **/
int hash_dict_add(hash_dict_t *hdp, hash_dict_node_t *snode, int is_overwrite) {
  unsigned int hkey;
  hash_dict_node_t *pt_node, *pfree = NULL;
  int ret;

  assert(hdp != NULL);
  assert(snode != NULL);

  if (snode->sign1 == 0 && snode->sign2 == 0) {
    fprintf(stderr, "Fetal: Hash_dict insert node error \n");
    return -1;
  }

  hkey = (snode->sign1 + snode->sign2) % hdp->hash_num;
  pt_node = hdp->hash_table[hkey].pnode;
  while (pt_node != NULL) {
    if ((pt_node->sign1 == snode->sign1) && (pt_node->sign2 == snode->sign2)) {
      break;
    }
    pfree = pt_node;
    pt_node = pt_node->next;
  }

  if (pt_node == NULL) { // no exist node. craete a new node .
    if ((pfree = (hash_dict_node_t *) malloc(sizeof (hash_dict_node_t))) == NULL) {
      fprintf(stderr, "Fatel: Hass node memory allocate error \n");
      return RT_HASH_DICT_ADD_ERROR;
    }
    pfree->sign1 = snode->sign1;
    pfree->sign2 = snode->sign2;
    pfree->pointer = snode->pointer;
    pfree->code = snode->code;

    pfree->next = hdp->hash_table[hkey].pnode;
    hdp->hash_table[hkey].pnode = pfree;
    hdp->node_num++;
    ret = RT_HASH_DICT_ADD_SUCC;
  } else {
    if (is_overwrite) {
      pt_node -> pointer = snode->pointer;
      pt_node -> code = snode->code;
      ret = RT_HASH_DICT_ADD_SUCC;
    } else {
      ret = RT_HASH_DICT_ADD_EXIST;
    }
  }
  return ret;
}

/**
   Func: to do del a node on hash_dict.
   In: hash_dict, search node info.
   Out: 0, success finished the opertaion.
        RT_HASH_DICT_DEL_SUCC  success del the node
        RT_HASH_DICT_DEL_MISS  did not find the node need to be del.
        -1, input error.
 **/
int hash_dict_del(hash_dict_t *hdp, hash_dict_node_t *snode) {
  unsigned int hkey;
  hash_dict_node_t *pt_node, *pfree = NULL;
  int ret;

  assert(hdp != NULL);
  assert(snode != NULL);

  if (snode->sign1 == 0 && snode->sign2 == 0) {
    fprintf(stderr, "Fetal: Hash_dict insert node error \n");
    return -1;
  }

  hkey = (snode->sign1 + snode->sign2) % hdp->hash_num;
  pt_node = hdp->hash_table[hkey].pnode;
  while (pt_node != NULL) {
    if ((pt_node->sign1 == snode->sign1) && (pt_node->sign2 == snode->sign2)) {
      break;
    }
    pfree = pt_node;
    pt_node = pt_node->next;
  }

  if (pt_node != NULL) { // the node exist
    if (pfree == NULL) {
      hdp->hash_table[hkey].pnode = NULL;
      free(pt_node);
      hdp->node_num--;
    } else {
      pfree->next = pt_node->next;
      free(pt_node);
      hdp->node_num--;
    }
    ret = RT_HASH_DICT_DEL_SUCC;
  } else {
    ret = RT_HASH_DICT_DEL_MISS;
  }

  return ret;
}

/**
   Func: To create a md5 sign on the given string.
   In:   Key String, String len, Pointer to sign1, Pointer to sign2
   Out:  0 success
         1 Failed
 **/

/*int create_sign_md5(char *str, unsigned int *sign1, unsigned int *sign2) {
    unsigned int md5res[4];
    int len = strlen(str);
    MD5((unsigned char*) str, (unsigned int) len, (unsigned char*) md5res);
 *sign1 = md5res[0] + md5res[1];
 *sign2 = md5res[2] + md5res[3];
    return 0;
}*/

int create_sign_gram(int *arry, unsigned int *sign1, unsigned int *sign2) {
  int *gram_sign = arry;
  *sign1 = 1000 * gram_sign[0];
  *sign2 = gram_sign[1];
  return 0;
}

//need modified because the hash value would increase to several hundreds million

int create_sign_bi_gram(int *arry, unsigned int *sign1, unsigned int *sign2) {
  int *bi_gram_sign = arry;
  unsigned int high, low;
  high = (unsigned int) bi_gram_sign[0] * 1000 + (unsigned int) bi_gram_sign[1];
  low = (unsigned int) bi_gram_sign[2] * 1000 + (unsigned int) bi_gram_sign[3];
  *sign1 = (high << 16) + low;
  *sign2 = 0;
  if (*sign1 == 0 && *sign2 == 0) {
    printf("Error is here");
    getchar();
  }
  return 0;
}

/**
   Func: To create a bitwise sign on the given string.
   In:   Key String, String len, Pointer to sign1, Pointer to sign2
   Out:  0 success
 **/
int create_sign_bitwise(char *str, unsigned int *sign1, unsigned int *sign2) {
  unsigned int h1 = 1159241;
  unsigned int h2 = 2000011;

  while (*str) {
    h1 ^= (h1 << 5) + (h1 >> 2) + *str;
    h2 ^= (h2 << 5) + (h2 >> 2) + *str++;
  }
  *sign1 = h1 & 0x7fffffff;
  *sign2 = h2 & 0x7fffffff;
  return 0;
}

/**
   Func: To create a bitwise sign on the given string.
   In:   Key String, String len, Pointer to sign1, Pointer to sign2
   Out:  0 success
 **/
int create_sign_bitwise_len(char *str, unsigned int *sign1, unsigned int *sign2, unsigned int slen) {
  unsigned int h1 = 1159241;
  unsigned int h2 = 2000011;

  while (slen) {
    h1 ^= (h1 << 5) + (h1 >> 2) + *str;
    h2 ^= (h2 << 5) + (h2 >> 2) + *str++;
    slen--;
  }
  *sign1 = h1 & 0x7fffffff;
  *sign2 = h2 & 0x7fffffff;
  return 0;
}

int create_sign_md5(char *str, unsigned int *sign1, unsigned int *sign2, int len)
{
  unsigned int md5res[4];
  //int len = strlen(str);
  MD5((unsigned char*)str,(unsigned int)len,(unsigned char*)md5res);
  *sign1=md5res[0]+md5res[1];
  *sign2=md5res[2]+md5res[3];
  return 0;
}

unsigned * init_power_hashmap (int N, unsigned * power_hashmap){
  int i;
  unsigned long long power = 1;
  
  power_hashmap = (unsigned *)malloc(sizeof(unsigned) * N);
  power_hashmap[0] = power;
 
  for(i=0 ;i<=N-1;i++){
    power *= RK_HASH_B;
    power %= RK_HASH_M;
    power_hashmap[i] = power;
  }
  return power_hashmap;
}

/*
long long rkHash(int * key , int len, unsigned * power_hashmap)
{
  long long value = 0;
  //long long power = 1;
  //int len = hp->qlen;

  for (int i = 0; i <= len - 1; i++)
    //for (int i = 0; i < len; i++)
  {
    //use a map
    value += (power_hashmap[i] * key[len - i - 1]);
    value %= RK_HASH_M;
    //power *= RK_HASH_B;
    //power %= RK_HASH_M;
  }
  return value;
}
*/






