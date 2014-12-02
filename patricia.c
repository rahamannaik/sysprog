#include "patricia.h"

Node * root=NULL;

int different_bits_pos  ( DataType DX,  DataType DY)
{
	int i;

	for ( i=1;  i<=  sizeof(DataType)*8 ; i++)
	{ 
		if ( BITi(DX,i) != BITi(DY,i) )
			return i;
	}  

	return 0;// no difference 
} 

Node * search(Node * t, DataType k )
{
	if ( !t) return NULL;   // root is null

	Node * current, *next;

	current = t;

	next     = t-> leftchild;

	while( next-> bitpos > current -> bitpos)
	{
		current  = next;

		next     =  ( BITi(k, next->bitpos ) ) ? 
			next->rightchild:  next->leftchild;

	}

	return next; 
}

void insert( Node ** root_d_ptr, DataType k)
{
	int  static node_count=9900;

#ifdef DEBUG
	printf("Insert %#x, ", (k&0xff000000)>>24 );
#endif

	if( ! (*root_d_ptr) ) 
	{
		*root_d_ptr= malloc(sizeof(Node) );
		assert(*root_d_ptr);

		(*root_d_ptr) ->bitpos = 0; 
		(*root_d_ptr) -> key = k;
		(*root_d_ptr) -> leftchild = (*root_d_ptr);
		(*root_d_ptr) -> node_count = node_count++;
#ifdef DEBUG
		printf("inserted root\n\n");
#endif
		return ;
	}

	Node * current, * parent, * lastnode, * newnode;
	int differingPos;

	lastnode = search(  (*root_d_ptr) , k);
	if ( k == lastnode-> key ) 
	{ 
		printf("Duplicate %x\n", k); 
		exit(1); 
	}
#ifdef DEBUG
		printf("last node : %#x, ", 
				(lastnode->key & 0xff000000) >>24 );
#endif


	differingPos = different_bits_pos( k, lastnode->key);

#ifdef DEBUG
	pviu(differingPos);
#endif

	parent  = (*root_d_ptr);
	current = (*root_d_ptr) -> leftchild;

	while( (current-> bitpos > parent -> bitpos) &&
			current->bitpos < differingPos) 
	{
		parent = current;

		current = (  BITi( k, current-> bitpos )?
				current->rightchild  : current->leftchild) ; 
	}

	newnode= malloc(sizeof(Node) );
	assert(newnode);

	newnode ->bitpos = differingPos;

	newnode -> key = k;
	newnode-> node_count = node_count++;

	newnode -> leftchild = ( BITi(k, differingPos)) ? current: newnode;
	newnode -> rightchild = ( BITi(k, differingPos)) ? newnode: current;

	if ( current == parent -> leftchild)
		parent->leftchild = newnode;
	else
		parent->rightchild = newnode;

#ifdef DEBUG
	printf("inserted\n\n"); 
#endif
}

void delete( Node ** root_d_ptr, DataType k)
{

	Node * parent, *current, *child, *header;

	header = *root_d_ptr;


	printf("Delete: %#x\n", (k&0xff000000)>>24);  

	if ( !header ) return ;   // root is null

	parent  = header;

	current      = parent-> leftchild;

	if ( current == parent)
	{
		free(*root_d_ptr);
		*root_d_ptr = NULL;
	
		return ;
	} 

	while(1) {
		child =  BITi(k, current->bitpos) ? 
			current->rightchild: current->leftchild;

pvix(parent->key);
pvix(current->key);
pvi(current->bitpos);
pvix(child->key);
pvi(child->bitpos);
printf("\n"); 
		if ( child->bitpos > current->bitpos)
		{
			parent=current;
			current=child;

			continue;
		}

		if ( child->key != k) 
		{
			printf("Not found\n"); 
			return;
		}




		DataType key_to_change = current->key;

		//Seach for key_to_change.
		Node * search_current = current;
		Node * search_child;
	 while(1) 	
		{
		search_child =   BITi(key_to_change, search_current->bitpos) ?
                        		search_current->rightchild : 
					search_current->leftchild;
		
		if ( search_current->bitpos < search_child->bitpos)
			search_current = search_child;
		else
			break;
		}

		child->key = current->key;

pvix(search_child->key);
pvix(search_current->key);
printf("\n"); 


		if ( search_child== search_current->leftchild)
		{
			search_current->leftchild = child;
		}
		else
		{
			search_current->rightchild = child; 		
		}
		

		if ( current == parent->leftchild)
		{
			if ( child == current-> leftchild ) 
				parent->leftchild = current->rightchild;
			else
				parent->leftchild = current->leftchild;
		} 
		else
		{
			if ( child == current-> leftchild ) 
				parent->rightchild = current->rightchild;
			else
				parent->rightchild = current->leftchild;
		} 
		break;
	}
}


void display_search_result(Node * t, DataType k )
{

	Node * search_result = search (root, k);

	if ( search_result -> key == k)
		printf("Found %#x \n", k);  
	else 
		printf("Not Found %#x \n", k);  
}

void depth_first_traversal_core_and_search( Node * t, DataType k,  int level)
{
	Node * left, * right;

	left = t -> leftchild;
	right = t -> rightchild;

  if ( left ) {
    if (  left-> bitpos > t->bitpos) 
    {
      printf("%*s",  level*2," ");
      printf("%d)Left Branch:bit %d %d\n",level ,
          left-> bitpos, left->node_count); 
      printf("\n"); 
      depth_first_traversal_core_and_search(left, k, level+1);
    }
    else
    {
      printf("%*s",  level*2," ");
      printf("%d)Left:Data %#x %d\n", level, 
          (left->key&0xff000000u)>>24, 
          left->node_count); 
      if ( left->key == k)
      {
        printf("%*s",  level*2," ");
        printf("  Found %#x \n", (k&0xff000000)>>24);  
      }

    }
  }
	printf("\n"); 
  if ( right ) {
    if (  right-> bitpos >t->bitpos) 
    {
      printf("%*s",  level*2," ");
      printf("%d)Right Branch:bit %d %d\n",level, 
          right-> bitpos ,right->node_count); 
      printf("\n"); 
      depth_first_traversal_core_and_search(right, k, level+1);
    }
    else
    {
      printf("%*s",  level*2," ");
      printf("%d)Right:Data %#x %d\n",level, 
          (right->key&0xff000000)>>24, 
          right->node_count); 
      if ( right->key == k)
      { 
        printf("%*s",  level*2," ");
        printf("  Found %#x \n", (k&0xff000000)>>24);  
      }
    }
  }
}

void  depth_first_traversal_and_search(Node * t , DataType k)
{
	if ( !t) return ;   // root is null

	Node * current, *next;

	current = t;

	next     = t-> leftchild;

	if (  next-> bitpos > t->bitpos) 
	{
		printf("0)Left Branch:bit %d %d\n", 
				next-> bitpos, next->node_count); 
	}
	else
	{
		printf("0)Left:Data %#x %d\n",  
				(next->key&0xff000000u)>>24, 
				next->node_count); 

		if ( next->key == k)
		{
			printf("  Found %#x \n", 
					(k&0xff000000)>>24);  
		}

	}
	printf("\n"); 

	depth_first_traversal_core_and_search(next, k, 1);

	return;
}

void depth_first_traversal_core( Node * t, int level)
{
	Node * left, * right;

	left = t -> leftchild;
	right = t -> rightchild;

  if ( left ) {
    if (  left-> bitpos > t->bitpos) 
    {
      printf("%*s",  level*2," ");
      printf("%d)Left Branch:bit %d %d\n",level ,
          left-> bitpos, left->node_count); 
      printf("\n"); 
      depth_first_traversal_core(left, level+1);
    }
    else
    {
      printf("%*s",  level*2," ");
      printf("%d)Left:Data %#x %d\n", level, 
          (left->key&0xff000000u)>>24, 
          left->node_count); 
    }
  }
	printf("\n"); 
  if ( right ) {
    if (  right-> bitpos >t->bitpos) 
    {
      printf("%*s",  level*2," ");
      printf("%d)Right Branch:bit %d %d\n",level, 
          right-> bitpos ,right->node_count); 
      printf("\n"); 
      depth_first_traversal_core(right, level+1);
    }
    else
    {
      printf("%*s",  level*2," ");
      printf("%d)Right:Data %#x %d\n",level, 
          (right->key&0xff000000)>>24, 
          right->node_count); 
    }
  }
}

void  depth_first_traversal(Node * t )
{
	if ( !t) return ;   // root is null

	Node * current, *next;

	current = t;

	next     = t-> leftchild;

	if (  next-> bitpos > t->bitpos) 
	{
		printf("0)Left Branch:bit %d %d\n", 
				next-> bitpos, next->node_count); 
	}
	else
	{
		printf("0)Left:Data %#x %d\n",  
				(next->key&0xff000000u)>>24, 
				next->node_count); 
	}
	printf("\n"); 

	depth_first_traversal_core(next, 1);

	return;
}

void insert_nodes_demo()
{
	printf("insert nodes demo\n"); 

	insert( &root, 0x90000000); 
	depth_first_traversal(root);
	printf("----------------------------------------------\n\n"); 

	insert( &root, 0x20000000); 
	depth_first_traversal(root);
	printf("----------------------------------------------\n\n"); 

	insert( &root, 0xc0000000); 
	depth_first_traversal(root);
	printf("----------------------------------------------\n\n"); 

	insert( &root, 0x80000000); 
	depth_first_traversal(root);
	printf("----------------------------------------------\n\n"); 

	insert( &root, 0x00000000); 
	depth_first_traversal(root);
	printf("----------------------------------------------\n\n"); 

	insert( &root, 0xb0000000); 
	depth_first_traversal(root);
	printf("----------------------------------------------\n\n"); 

	insert( &root, 0xf0000000); 
	depth_first_traversal(root);
	printf("----------------------------------------------\n\n"); 

	insert( &root, 0x40000000); 
	depth_first_traversal(root);
	printf("--------------------------------------------------\n\n"); 

	insert( &root, 0xa0000000); 
	depth_first_traversal(root);
	printf("--------------------------------------------------\n\n"); 

	insert( &root, 0x50000000); 
	depth_first_traversal(root);
	printf("--------------------------------------------------\n\n"); 

}


void clean_up_insert()
{

	while(root!=NULL)
	{

		delete( &root, root->key);

		depth_first_traversal(root);	
	} 
/*
	insert( &root, 0x90000000); 
	insert( &root, 0x20000000); 
	insert( &root, 0xc0000000); 
	insert( &root, 0x80000000); 
	insert( &root, 0x00000000); 
	insert( &root, 0xb0000000); 
	insert( &root, 0xf0000000); 
	insert( &root, 0x40000000); 
	insert( &root, 0xa0000000); 
	insert( &root, 0x50000000); 
*/


}
#if 0
int main(int argc, char * argv[])
{
	insert_nodes_demo();

	clean_up_insert();

	printf("rebuilt\n"); 
	printf("--------------------------------------------------\n\n"); 

	depth_first_traversal(root);

	/*
	   delete(&root,0x20000000);
	   depth_first_traversal(root);
	   printf("--------------------------------------------------\n\n"); 

	   delete(&root,0x50000000);
	   depth_first_traversal(root);
	   printf("--------------------------------------------------\n\n"); 

	   delete(&root,0x80000000);
	   depth_first_traversal(root);
	   printf("--------------------------------------------------\n\n"); 

	   delete(&root,0x90000000);
	   depth_first_traversal(root);
	   printf("--------------------------------------------------\n\n"); 

	   delete(&root,0xc0000000);
	   depth_first_traversal(root);
	   printf("--------------------------------------------------\n\n"); 


	   delete(&root,0xf0000000);
	   depth_first_traversal(root);
	   printf("--------------------------------------------------\n\n"); 
	 */
} 
#endif


/*
   1001	9
   0010	2
   1100	c
   1000	8
   0000	0
   1011	b
   1111	f
   0100	4
   1010	a	
   0101	5

 */
