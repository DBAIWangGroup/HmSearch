/**
  This is a hash dictionary created by Qinjianbin. Based on the idea of odict.
  date: Oct 10 2011
  Author: Jianbin Qin
  revised:xiaoyang
 **/

#ifndef __HASH_DICT_H_
#define __HASH_DICT_H_


#define HASH_DICT_FIND_ONLY 0
#define HASH_DICT_FIND_ENTER 1
#define HASH_DICT_FIND_DEL 3

// return of search function
#define RT_HASH_DICT_SEARCH_SUCC 0
#define RT_HASH_DICT_SEARCH_MISS 1

// return of add function
#define RT_HASH_DICT_ADD_SUCC 0
#define RT_HASH_DICT_ADD_ERROR -1
#define RT_HASH_DICT_ADD_EXIST 2

// return of del function
#define RT_HASH_DICT_DEL_MISS 1
#define RT_HASH_DICT_DEL_SUCC 0

#define RK_HASH_M 1000000007
#define RK_HASH_B 257


// hash node type for restore the sign and information.

typedef struct _hash_dict_node_t {
    unsigned int sign1; // signature 1
    unsigned int sign2; // signature 2
    void *pointer; // custom data pointer.
    int code;
    struct _hash_dict_node_t *next; //next node
} hash_dict_node_t;

typedef struct _hash_table_t {
    struct _hash_dict_node_t *pnode; // hash table
} hash_table_t;

typedef struct _hash_dict_t {
    unsigned int hash_num; // number of hash
    unsigned int node_num; // number of nodes
    hash_table_t *hash_table;
} hash_dict_t;





/*
   Func: to Create a hash dict from a hash_number
   In: hash number
   Out: != NULL, success .
        == NULL. Failed
 */
hash_dict_t *hash_dict_create(unsigned int hash_number);




/*
   Renew the hash dict
   IN hashi dict pointer;
   out Alwayers success.
 */
int hash_dict_renew(hash_dict_t *hdp);


/*
   Func: To destory a hash dict
   IN: hash dict pointer.
   Out: 0  Success.
        1  Failed.
 */
int hash_dict_destory(hash_dict_t *hash_dict);


/*
   Func: to do search on hash_dict.
   In: hash_dict, search node info structure
   Out: RT_HASH_DICT_SEARCH_SUCC, success find the search node.
        RT_HASH_DICT_SEARCH_MISS, node not find.
        -1, input  failed finish the operation.
 */
int hash_dict_search(hash_dict_t *hdp, hash_dict_node_t *snode);



/*
   Func: to do search on hash_dict.
   In: hash_dict, search node info structure
   Out: RT_HASH_DICT_SEARCH_SUCC, success find the search node.
        RT_HASH_DICT_SEARCH_MISS, node not find.
        -1, input  failed finish the operation.
 */
int hash_dict_count(hash_dict_t *hdp, hash_dict_node_t *snode);



/*
   Func: to do add node  on hash_dict.
   In: hash_dict, add node info. is_overwrite.
   Out: RT_HASH_DICT_ADD_ERROR: memory allocation error
        RT_HASH_DICT_ADD_EXIST: the node is existed and without overwrite.
        RT_HASH_DICT_ADD_SUCC: success to add a node to hash_dict;
        -1, input error the operation.
 */
int hash_dict_add(hash_dict_t *hdp, hash_dict_node_t *snode, int is_overwrite);



/*
   Func: to do del a node on hash_dict.
   In: hash_dict, search node info.
   Out: 0, success finished the opertaion.
        RT_HASH_DICT_DEL_SUCC  success del the node
        RT_HASH_DICT_DEL_MISS  did not find the node need to be del.
        -1, input error.
 */
int hash_dict_del(hash_dict_t *hdp, hash_dict_node_t *snode);



/*
   Func: To create a md5 sign on the given string.
   In:   Key String, String len, Pointer to sign1, Pointer to sign2
   Out:  0 success
         1 Failed
 */
//int create_sign_md5(char *str, unsigned int *sign1, unsigned int *sign2);

int create_sign_gram(int *arry, unsigned int *sign1, unsigned int *sign2);

/*
   Func: To create a bitwise sign on the given string.
   In:   Key String, String len, Pointer to sign1, Pointer to sign2
   Out:  0 success
         1 Failed
 */
int create_sign_bitwise(char *str, unsigned int *sign1, unsigned int *sign2);


/*
   Func: To create a bitwise sign on the given string.
   In:   Key String, String len, Pointer to sign1, Pointer to sign2
   Out:  0 success
         1 Failed
 */
int create_sign_bitwise_len(char *str, unsigned int *sign1, unsigned int *sign2, unsigned int slen);



//rolling hash
unsigned * init_power_hashmap (int N, unsigned * power_hashmap);
long long rkHash(int * key , int len, unsigned * power_hashmap);


#endif
