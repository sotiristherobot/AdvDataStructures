#include <stdio.h>
#include <stdlib.h>

#define BLOCKSIZE 256

struct intv {int x_min; int x_max; int y_min; int y_max;};

typedef struct intv object_t;
typedef int key_t;
typedef struct ls_n_t {  
  key_t key_a, key_b;
  struct ls_n_t  *next;
  object_t *object; 
} list_node_t;

//sotiris
struct rect_list_t {
  struct rect_list_t * next;
  int x_min; int x_max;
  int y_min; int y_max;
};

/*typedef struct tr_n_t {
  key_t key;
  struct tr_n_t *left;
  struct tr_n_t *right;
  struct tr_n_t *inner_tree;

  list_node_t   *interval_list;
  struct rect_list_t *intv_list;


} tree_node_t;*/


typedef struct seg_tree_2d_t {
  key_t key;
  struct seg_tree_2d_t *left;
  struct seg_tree_2d_t *right;
  struct seg_tree_2d_t *inner_tree;

  list_node_t   *interval_list;
  struct rect_list_t *intv_list;


} tree_node_t;



tree_node_t *currentblock = NULL;
int    size_left;
tree_node_t *free_list = NULL;

tree_node_t *get_node()
{ tree_node_t *tmp;
  if( free_list != NULL )
  {  tmp = free_list;
     free_list = free_list -> left;
  }
  else
  {  if( currentblock == NULL || size_left == 0)
     {  currentblock =
                (tree_node_t *) malloc( BLOCKSIZE * sizeof(tree_node_t) );
        size_left = BLOCKSIZE;
     }
     tmp = currentblock++;
     size_left -= 1;
  }
  return( tmp );
}


void return_node(tree_node_t *node)
{  node->left = free_list;
   free_list = node;
}

/*list_node_t *get_list_node()
{  return( ( list_node_t *) get_node() );
}*/
struct rect_list_t *get_list_node()
{  return( ( struct rect_list_t *) get_node() );
}


tree_node_t *make_tree(tree_node_t *list)
{
  typedef struct {
    tree_node_t  *node1;
    tree_node_t  *node2;
    list_node_t *intv_list;
    int number; } st_item;
   st_item current, left, right;
   st_item stack[100]; int st_p = 0;
   tree_node_t *tmp, *root;
   int length = 0;
   for( tmp = list; tmp != NULL; tmp = tmp->right )
      length += 1; /* find length of list */

   root = get_node();
   current.node1 = root; /* put root node on stack */
   current.node2 = NULL;
   current.number = length;/* root expands to length leaves */
   stack[ st_p++ ] = current;
   while( st_p >0 )/* there is still unexpanded node */
   {  current = stack[ --st_p ];
      if( current.number > 1 ) /* create (empty) tree nodes */
      { left.node1 = get_node();
        left.node2 = current.node2;
        left.number = current.number / 2;
        right.node1 = get_node();
        right.node2 = current.node1;
        right.number = current.number - left.number;
        (current.node1)->left  = left.node1;
        (current.node1)->right = right.node1;
        stack[ st_p++ ] = right;
        stack[ st_p++ ] = left;
      }
      else /* reached a leaf, must be filled with list item */
      { (current.node1)->left  = list->left;   /* fill leaf */
        (current.node1)->key   = list->key;    /* from list */
       // (current.node1)->intv_list = list->intv_list;
        (current.node1)->right = NULL;
        if( current.node2 != NULL ){
           /* insert comparison key in interior node */
           (current.node2)->key   = list->key;
	        // (current.node2)->intv_list = list->intv_list;
	}
        tmp = list;          /* unlink first item from list */
        list = list->right;  /* content has been copied to */
        return_node(tmp);    /* leaf, so node is returned */
      }
   }
   return( root );
}

void empty_tree( tree_node_t *stree)
{  stree->interval_list  = NULL;
   stree->intv_list = NULL;
   if( stree->right != NULL )
   {  empty_tree( stree->left );
      empty_tree( stree->right );
   }
}

void check_tree( tree_node_t *tr, int depth, int lower, int upper )
{  if( tr->left == NULL )
   {  printf("Tree Empty\n"); return; }
   if( tr->key < lower || tr->key >= upper )
         printf("Wrong Key Order \n");
   if( tr->right == NULL )
   {  if( *( (int *) tr->left) == 42 )
         printf("%d(%d)  ", tr->key, depth );
      else
         printf("Wrong Object \n");
   }
   else
   {  check_tree(tr->left, depth+1, lower, tr->key );
      check_tree(tr->right, depth+1, tr->key, upper );
   }
}



void attach_intv_node(tree_node_t *tree_node,
                     key_t a, key_t b, object_t *object)
{
  list_node_t *new_node;
  struct rect_list_t * new_node_new;

   /*new_node = get_list_node();
   new_node->next = tree_node->interval_list;
   new_node->key_a = a; new_node->key_b = b;
   new_node->object = object;
   tree_node->interval_list = new_node;*/

   new_node_new = get_list_node();
   new_node_new->next = tree_node->intv_list;
   new_node_new->x_min = object->x_min;
   new_node_new->x_max = object->x_max;
   new_node_new->y_min = object->y_min;
   new_node_new->y_max = object->y_max;
   tree_node->intv_list = new_node_new;

  //printf("Attach Intveral Node tree_node %d x_min %d\n",tree_node->key, tree_node->intv_list->x_min);

}

void insert_interval(tree_node_t *tree,
                     key_t a, key_t b, object_t *object)
{  tree_node_t *current_node, *right_path, *left_path;
   list_node_t *current_list, *new_node;
   if( tree->left == NULL )
      exit(-1); /* tree incorrect */
   else
   {  current_node = tree;
      right_path = left_path = NULL;
      while( current_node->right != NULL ) /* not at leaf */
      {   if( b < current_node->key ) /* go left: a < b < key */
              current_node = current_node->left;
          else if( current_node->key < a)
                                     /* go right: key < b < a */
              current_node = current_node->right;
          else if( a < current_node->key &&
               current_node->key < b )  /* split: a < key < b */
	  {   right_path = current_node->right; /* both right */
	      left_path  = current_node->left;    /* and left */
              break;
          }
          else if( a == current_node->key ) /* a = key < b */
	  {   right_path = current_node->right; /* no left */
              break;
          }
          else /*   current_node->key == b, so a < key = b */
	  {   left_path  = current_node->left; /* no right */
              break;
          }
      }
      if( left_path != NULL )
      {  /* now follow the path of the left endpoint a*/
         while( left_path->right != NULL )
         {   if( a < left_path->key )
             {   /* right node must be selected */
                 attach_intv_node(left_path->right, a,b,object);
                 left_path = left_path->left;
             }
             else if ( a == left_path->key )
             {   attach_intv_node(left_path->right, a,b,object);
                 break; /* no further descent necessary */
             }
	     else /* go right, no node selected */
                 left_path = left_path->right;
         }
         /* left leaf needs to be selected if reached in descent*/
         if( left_path->right == NULL && left_path->key == a )
             attach_intv_node(left_path, a,b,object);
      }  /* end left path */
      if( right_path != NULL )
      {  /* and now follow the path of the right endpoint b */
         while( right_path->right != NULL )
         {   if( right_path->key < b )
             {   /* left node must be selected */
                 attach_intv_node(right_path->left, a,b,object);
                 right_path = right_path->right;
             }
             else if ( right_path->key == b)
             {   attach_intv_node(right_path->left, a,b,object);
                 break; /* no further descent necessary */
             }
	     else /* go left, no node selected */
                 right_path = right_path->left;
         }
      /* on the right side, the leaf of b is never attached */
      }  /* end right path */
   }
}


int compint( int *a, int *b)
{  //return( *a>*b );
  return ( *(int*)a - *(int*)b );
}

list_node_t *find_intervals(tree_node_t *tree, key_t query_key)
{  

  int kokos = 0;
  tree_node_t *current_tree_node;
   list_node_t *current_list, *result_list, *new_result;
   if( tree->left == NULL ) {
      printf("tree is empty\n");
      return(NULL);
   }
   else /* tree nonempty, follow search path */
   {  
    printf("searching\n");
    current_tree_node = tree;
      result_list = NULL;
      while( current_tree_node->right != NULL )
      {   if( query_key < current_tree_node->key ){
              current_tree_node = current_tree_node->left;
              
          }
          else{
              current_tree_node = current_tree_node->right;
              
          }


          current_list = current_tree_node->interval_list;
          while( current_list != NULL )
          {  /* copy entry from node list to result list */
             //printf("Found it inner \n");
              kokos += 1;
              
             // new_result = get_list_node();
             // new_result->next = result_list;
             // new_result->key_a = current_list->key_a;
             // new_result->key_b = current_list->key_b;
             // new_result->object = current_list->object;
             // result_list = new_result;
              current_list = current_list->next;
          }
      }
       printf("%d\n", kokos );
      return( result_list );
   }

}

struct rect_list_t *query_seg_tree_2d(tree_node_t *tree, key_t a, key_t b){

  tree_node_t * current_tree_node, * current_inner_tree_node;

  struct rect_list_t * current_list;
  struct rect_list_t * result_list, * new_result;


  current_tree_node = tree;
  new_result = result_list = NULL;
  current_list = NULL;

  while (current_tree_node->right != NULL){

    if (a < current_tree_node->key)
      current_tree_node = current_tree_node->left;
    else
      current_tree_node = current_tree_node->right;

    if (current_tree_node->intv_list != NULL){

      current_inner_tree_node = current_tree_node->inner_tree;

      while (current_inner_tree_node->right != NULL){

        if (b < current_inner_tree_node->key)
          current_inner_tree_node = current_inner_tree_node->left;
        else
          current_inner_tree_node = current_inner_tree_node->right;

        current_list = current_inner_tree_node->intv_list;

        while (current_list != NULL){
          new_result = get_list_node();
          new_result->next = result_list;
          new_result->x_min = current_list->x_min;
          new_result->x_max = current_list->x_max;
          new_result->y_min = current_list->y_min;
          new_result->y_max = current_list->y_max;
          result_list = new_result;
          current_list = current_list->next;


        }



      }

    }


  }


  return result_list;






}

struct seg_tree_2d_t * create_seg_tree_2d(struct rect_list_t *list){

  struct  rect_list_t *tmp_list = list;
  
  //printf("tmp_list\n" , tmp_list->x_min);

  tree_node_t * seg_tree_1d;

  //helpers
  int keys[100000]; int prev_key = 0; int i = 0; int * tmpob = 0;
  struct intv intervals[50000];

  //temporrary list for creating the tree
  tree_node_t * tmp_tree_list; tree_node_t * tmp_tree_helper;

  //keys for x-axis
  while (tmp_list != NULL){

    keys[2*i] = tmp_list->x_min;
    keys[2*i+1] = tmp_list->x_max;

    intervals[i].x_min = tmp_list->x_min;
    intervals[i].x_max = tmp_list->x_max;
    intervals[i].y_min = tmp_list->y_min;
    intervals[i].y_max = tmp_list->y_max;

    tmp_list = tmp_list->next;
    i += 1;
  }


  

  //sort x-axis keys
  qsort(keys, 2*i, sizeof(int), compint);



  tmp_tree_list = get_node();
  tmp_tree_list->right = NULL;

  prev_key = tmp_tree_list->key = keys[2*i-1];

  tmpob = (int *) malloc(sizeof(int));
  *tmpob = 42;
  tmp_tree_list->left = (tree_node_t *) tmpob;

  for (int j = 2*i-2; j>=0; j--){

    if (keys[j] != prev_key){
      tmp_tree_helper = get_node();
      prev_key = tmp_tree_helper->key = keys[j];
      tmp_tree_helper->right = tmp_tree_list;

      tmpob = (int *)malloc(sizeof(int));
      *tmpob = 42;
      tmp_tree_helper->left = (tree_node_t *)tmpob;

      tmp_tree_list = tmp_tree_helper;

    }

  }

  //make the -nfinity
  tmp_tree_helper = get_node();
  tmp_tree_helper->key = -1000;
  tmp_tree_helper->right = tmp_tree_list;
  tmpob = (int *) malloc(sizeof(int));
  *tmpob = 42;
  tmp_tree_helper->left = (tree_node_t *)tmpob;
  tmp_tree_list = tmp_tree_helper;

  tmp_tree_helper = tmp_tree_list;

  //copy the interval list to nodes
  while (tmp_tree_helper != NULL){
    //printf("*** Key of this node is %d **** \n", tmp_tree_helper->key);
   /* for (int j = i - 1; j>=0; j--){
      if (tmp_tree_helper->key == intervals[j].x_min || tmp_tree_helper->key == intervals[j].x_max){

	     //printf("*** Copying interval for key node %d *** \n", tmp_tree_helper->key);
	     tmp_tree_helper->intv_list = get_list_node();
	     tmp_tree_helper->intv_list->x_min = intervals[j].x_min;
	     tmp_tree_helper->intv_list->x_max = intervals[j].x_max;
	     tmp_tree_helper->intv_list->y_min = intervals[j].y_min;
	     tmp_tree_helper->intv_list->y_max = intervals[j].y_max;
      }

    }*/

    tmp_tree_helper = tmp_tree_helper->right;
  }


  tmp_tree_helper = tmp_tree_list;

  while (tmp_tree_helper != NULL){

    if (tmp_tree_helper->intv_list == NULL){
		  //printf("\n");
      // printf("*** Key is  %d *** \n", tmp_tree_helper->key);
    }
    else{

     /* printf("*** Key is  %d *** \n", tmp_tree_helper->key);
      printf("*** X-MIN is  %d *** \n", tmp_tree_helper->intv_list->x_min);
      printf("*** X-MAX is  %d *** \n", tmp_tree_helper->intv_list->x_max);
      printf("*** Y-MIN is  %d *** \n", tmp_tree_helper->intv_list->y_min);
      printf("*** Y-Max is  %d *** \n", tmp_tree_helper->intv_list->y_max);

      printf("\n ********************************************************** \n"); */
    }
     tmp_tree_helper = tmp_tree_helper->right;
  }


  seg_tree_1d = make_tree(tmp_tree_list);
  empty_tree(seg_tree_1d);
  


  //check_tree(seg_tree_1d, 0, -2000, 1000);


  //insert intervals for x-axis
  for (int p = i -1; p>=0; p--){

    struct intv *tmp_ob;
    tmp_ob = (struct intv *)malloc(sizeof(struct intv));

    tmp_ob->x_min = intervals[p].x_min;
    tmp_ob->x_max = intervals[p].x_max;
    tmp_ob->y_min = intervals[p].y_min;
    tmp_ob->y_max = intervals[p].y_max;

    insert_interval(seg_tree_1d, intervals[p].x_min, intervals[p].x_max, tmp_ob);
  }





  // Create the 2-d tree
  int stack_p = 0; tree_node_t * node_stack[100];
  tree_node_t * itmp_ptr;
  tree_node_t * tmp_inner_tree , * tmp_inner_tree_helper;
  struct rect_list_t  * tmp_inner_list;

  //insert the root into the stack
  node_stack[stack_p++] = seg_tree_1d;

  while (stack_p > 0){


    itmp_ptr = node_stack[--stack_p];

    int lolos;
    if (itmp_ptr->intv_list == NULL)
      //printf("\n*** Interval List is NULL Node key is: %d  *** \n", itmp_ptr->key );
    	
      lolos = 15;
      //printf("*** \n ");
    else{
       // printf("\n*** Interval List is not NULL Node key is: %d  *** \n", itmp_ptr->key );

        struct rect_list_t * tmp_sot = itmp_ptr->intv_list;

      
	     int keys_y[100000]; int prev_key_y = 0; int l = 0;
	     tmp_inner_list = itmp_ptr->intv_list;
	     struct intv intervals_y[50000];

       while(tmp_inner_list != NULL){

	         keys_y[2*l] = tmp_inner_list->y_min;
	         keys_y[2*l+1] = tmp_inner_list->y_max;

	         intervals_y[l].x_min = tmp_inner_list->x_min;
	         intervals_y[l].x_max = tmp_inner_list->x_max;
	         intervals_y[l].y_min = tmp_inner_list->y_min;
	         intervals_y[l].y_max = tmp_inner_list->y_max;

	         tmp_inner_list = tmp_inner_list->next;
	         l +=1;

	     }
	//sort the keys in y-axis;
	qsort(keys_y, 2*l, sizeof(int),compint);

  //for( int s = 0; s<2*j; s++ )
   //  printf("%d, ", keys_y[s] );
  // printf("\n");


	tmp_inner_tree = get_node();
	tmp_inner_tree->right = NULL;
	prev_key_y = tmp_inner_tree->key = keys_y[2*l-1];

	tmpob = (int *) malloc(sizeof(int));
	*tmpob = 42;
	tmp_inner_tree->left = (tree_node_t *) tmpob;

	for (int m = 2*l - 2; m >=0; m--){

	  if (keys_y[m] != prev_key_y){

	    tmp_inner_tree_helper = get_node();
	    prev_key_y = tmp_inner_tree_helper->key=keys_y[m];
	    tmp_inner_tree_helper->right = tmp_inner_tree;

	    tmpob = (int *)malloc(sizeof(int));
	    *tmpob = 42;
	    tmp_inner_tree_helper->left = (tree_node_t *)tmpob;

	    tmp_inner_tree = tmp_inner_tree_helper;

	  }

	}

	//make the -nfinity
	tmp_inner_tree_helper = get_node();
	tmp_inner_tree_helper->key = -1000;
	tmp_inner_tree_helper->right = tmp_inner_tree;
	tmpob = (int *) malloc(sizeof(int));
	*tmpob =42;
	tmp_inner_tree_helper->left = (tree_node_t *)tmpob;
	tmp_inner_tree = tmp_inner_tree_helper;

  tmp_inner_tree_helper = tmp_inner_tree;



	itmp_ptr->inner_tree = make_tree(tmp_inner_tree);
	empty_tree(itmp_ptr->inner_tree);
  //check_tree(itmp_ptr->inner_tree, 0, -2000, 1000);


	//insert intervals into the y-axis
	for (int s = l - 1 ; s >=0; s--){
    //printf("TMP INNER LIST %d \n", k);
    //printf("\n Inserting Intervals \n");
	  struct intv *tmp_ob;
	  tmp_ob = (struct intv *)malloc(sizeof(struct intv));
	  tmp_ob->x_min = intervals_y[s].x_min;
	  tmp_ob->x_max = intervals_y[s].x_max;
	  tmp_ob->y_min = intervals_y[s].y_min;
	  tmp_ob->y_max = intervals_y[s].y_max;

   // printf("Interval y y_min %d \n", intervals_y[k].y_max);
	  insert_interval(itmp_ptr->inner_tree, intervals_y[s].y_min, intervals_y[s].y_max, tmp_ob);

	}


    }


    if (itmp_ptr->right != NULL){
      node_stack[stack_p++] = itmp_ptr->left;
      node_stack[stack_p++] = itmp_ptr->right;
    }
  }

  return seg_tree_1d;

}

int main()
{  int i, j, x, y, l,m; 
   struct rect_list_t rectangles[50000];
   struct rect_list_t * tmp;
   struct seg_tree_2d_t *tr;
   for( i=0; i<50000; i++)
   {  rectangles[(17*i)%50000 ].next = rectangles + ((17*(i+1))%50000);  
   }
   rectangles[(17*49999)%50000 ].next = NULL;
   i=0; tmp = rectangles;
   while(tmp->next != NULL )
   {  tmp = tmp->next; i+=1; }
   printf("List of %d rectangles\n",i);
   for(i=0; i<12500; i++)
   {  rectangles[i].x_min = 500000 + 40*i;
      rectangles[i].x_max = 500000 + 40*i + 20;
      rectangles[i].y_min = 0;
      rectangles[i].y_max = 1000000;
   }
   for(i=12500; i<25000; i++)
   {  rectangles[i].x_min = 500000 + 40*(i-12500) + 10;
      rectangles[i].x_max = 500000 + 40*(i-12500) + 20;
      rectangles[i].y_min = 0;
      rectangles[i].y_max = 1000000;
   }
   for(i=25000; i<37500; i++)
   {  rectangles[i].x_min = 20*(i-25000);
      rectangles[i].x_max = 20*(i-25000) + 250000;
      rectangles[i].y_min = 20*(i-25000);
      rectangles[i].y_max = 20*(i-25000) + 250000;
   }
   for(i=37500; i<50000; i++)
   {  rectangles[i].x_min = 40*(i-37500);
      rectangles[i].x_max = 500000;
      rectangles[i].y_min = 300000;
      rectangles[i].y_max = 500000 + 40*(i-37500);
   }
   printf("Defined the 50000 rectangles\n"); fflush(stdout);
   tr = create_seg_tree_2d( rectangles );
   printf("Created 2d segment tree\n"); fflush(stdout);
   /* test 1 */
   for( i= 0; i<1000; i++ )
   {  x = 500000 + 400*i +30;
      y = ((1379*i)%400000) + 10;
      tmp = query_seg_tree_2d( tr, x,y);
      if( tmp != NULL )
      {  printf("point %d,%d should not be covered by any rectangle.\n",x,y);
         printf(" instead reported as covered by [%d,%d]x[%d,%d]\n",
                tmp->x_min, tmp->x_max, tmp->y_min, tmp->y_max); 
         fflush(stdout);
         exit(0);
      }
   }
   printf("Passed Test 1.\n");
   /* test 2 */
   for( i= 0; i<1000; i++ )
   {  x = 500000 + 400*i +5;
      y = ((3791*i)%400000) + 10;
      tmp = query_seg_tree_2d( tr, x,y);
      if( tmp == NULL )
      {  printf("point %d,%d should be covered by a rectangle, not found\n"
    ,x,y);fflush(stdout);
         exit(0);
      }
      if( x< tmp->x_min || x > tmp->x_max || y< tmp->y_min || y > tmp->y_max )
      {  printf("rectangle [%d,%d]x[%d,%d] does not cover point %d,%d\n",
                 tmp->x_min, tmp->x_max, tmp->y_min, tmp->y_max,x,y); 
         fflush(stdout); 
         exit(0);
      } 
   }
   printf("Passed Test 2.\n");
   /* test 3 */
   for( i= 0; i<10; i++ )
   {  x = 300000 + 20000*i + 3;
      y = 400001;
      tmp = query_seg_tree_2d( tr, x,y);
      if( tmp == NULL )
      {  printf("point %d,%d should be covered by a rectangle, not found\n"
                 ,x,y);
         exit(0);
      }
      while( tmp != NULL )
      {  if( x< tmp->x_min|| x > tmp->x_max|| y< tmp->y_min|| y > tmp->y_max )
        {  printf("rectangle [%d,%d]x[%d,%d] does not cover point %d,%d\n",
                 tmp->x_min, tmp->x_max, tmp->y_min, tmp->y_max,x,y); 
           exit(0);
        }
        tmp = tmp->next;
      }
   }
   printf("Passed Test 3.\n");
   /* test 4 */
   for( i= 0; i<10; i++ )
   {  x = 10* (rand()%100000) +4;
      y = 10* (rand()%100000) +4;
      m=0;
      for(j=0; j<50000; j++ )
      {  if( rectangles[j].x_min < x && rectangles[j].x_max > x &&
             rectangles[j].y_min < y && rectangles[j].y_max > y )
      m +=1;
      }
      tmp = query_seg_tree_2d( tr, x,y);
      l=0;
      while( tmp != NULL )
      {  if( x< tmp->x_min|| x > tmp->x_max|| y< tmp->y_min|| y > tmp->y_max )
        {  printf("rectangle [%d,%d]x[%d,%d] does not cover point %d,%d\n",
                 tmp->x_min, tmp->x_max, tmp->y_min, tmp->y_max,x,y); 
           exit(0);
        }
        l+= 1;
        tmp = tmp->next;
      }
      if( l!= m )
      {  printf("test 4.%d: Point %d,%d should be in %d rectangles, but %d found\n", i, x, y, m, l);
         fflush(stdout); 
      }     
   }
   printf("End of tests\n");
}




/*int main(){
	
	struct rect_list_t * tmp_list , * list;
    tree_node_t * tree;
    tmp_list = list;
    list = get_list_node();
	
	
	struct rect_list_t * test;
	test = get_list_node();
    //create dummy values
    list->x_min = 5;
    list->x_max = 10;
    list->y_min = 15;
    list->y_max = 20;
    list->next = get_list_node();
    
    tmp_list = list->next;
    
    tmp_list->x_min = 50;
    tmp_list->x_max = 55;
    tmp_list->y_min = 60;
    tmp_list->y_max = 65;
    
    tree = create_seg_tree_2d(list);
    test = query_seg_tree_2d(tree, 6,16);
	
	while(test != NULL){
	
		printf("\n Found square with x-min: %d, x-max: %d, y_min: %d , y_max: %d \n", test->x_min, test->x_max, test->y_min, test->y_max);
		test=test->next;
	}
  return(0);
}*/
