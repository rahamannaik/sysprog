#ifndef __PATRICIA_H__
#define __PATRICIA_H__

#include <stdio.h>
#include <stdlib.h> 
#include <assert.h> 

typedef unsigned int DataType;

#define BITi( DX , bit_pos ) \
	( ( ( 0X80000000 >> ( bit_pos -1 ) ) & ( DX ) )  >0  )

#define pviu(V)    printf("%-15s=%-12u\n",#V,V)
#define pvix(V)    printf("%-15s=%#010x\n",#V,V)
#define pvi(V)     printf("%-15s=%-12d\n",#V,V)

typedef struct node  Node;

struct node {
	DataType key;
	Node * leftchild, *rightchild;
	int bitpos;
	int node_count;
};

typedef enum trie_codes {
  TRIE_INSERT_SUCCESS = 0,
  TRIE_INSERT_FAILED,
  TRIE_LOOKUP_SUCCESS,
  TRIE_LOOKUP_FAILED,
  TRIE_DELETE_SUCCESS,
  TRIE_DELETE_FAILED,
  TRIE_ELEM_NOT_FOUND,
  TRIE_DUPLICATE_KEY
} trie_codes_t;

int insert( Node ** root_d_ptr, DataType k);
Node * search(Node * t, DataType k );
int different_bits_pos  ( DataType DX,  DataType DY);
int delete( Node ** root_d_ptr, DataType k);
void display_search_result(Node * t, DataType k );
void depth_first_traversal_core_and_search( Node * t, DataType k,  int level);
void  depth_first_traversal_and_search(Node * t , DataType k);
void depth_first_traversal_core( Node * t, int level);
void  depth_first_traversal(Node * t );


#endif 
