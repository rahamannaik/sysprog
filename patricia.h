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


#endif 
