#include <stdio.h>
#include <stdlib.h>

#define BLOCKSIZE 256

#define ALPHA 0.288
#define EPSILON 0.005


typedef struct tr_n_t{
  struct tr_n_t * left;
  struct tr_n_t * right;
  int weight;
} text_t;

text_t * currentblock = NULL;

int size_left;
text_t * free_list = NULL;


text_t * get_node(){

  text_t *tmp;

  if (free_list != NULL){
    tmp = free_list;
    free_list = free_list->left;
  }
  else{

    if (currentblock == NULL || size_left == 0){
      currentblock = (text_t *) malloc(BLOCKSIZE * sizeof(text_t));
      size_left = BLOCKSIZE;
    }
    tmp = currentblock++;
    size_left -= 1;
  }

  return (tmp);
}
void return_node(text_t * node)
{
  node->left = free_list;
  free_list = node;
}

text_t * create_text(void)
{
  text_t * tmp_node;
  tmp_node = get_node();
  tmp_node->left = NULL;
  tmp_node->weight = 0;
  return( tmp_node );
}


/* void right_rotation(text_t  *n) */
/* {  text_t  * tmp_node; */
/*   //key_t        tmp_key; */
/*   int tmp_key; */
/*   tmp_node = n->right;  */
/*   //tmp_key  = n->key; */
/*   tmp_key = n->weight; */
/*   n->right = n->left;         */
/*   // n->key   = n->left->key; */
/*   n->weight = n->left->weight; */
/*   n->left  = n->right->left;   */
/*   n->right->left = n->right->right; */
/*   n->right->right  = tmp_node; */
/*   //n->right->key   = tmp_key; */
/*    n->right->weight = tmp_key; */
/*} */


void left_rotation(text_t *n)
{  text_t *tmp_node;
   
   tmp_node = n->left; 
   n->left  = n->right;        
   n->right = n->left->right;  
   n->left->right = n->left->left;
   n->left->left  = tmp_node;
   
}

void right_rotation(text_t *n)
{  text_t *tmp_node;
 
   tmp_node = n->right;
   n->right = n->left;        
   n->left  = n->right->left;  
   n->right->left = n->right->right;
   n->right->right  = tmp_node;   
}



int length_text(text_t * txt){

  return txt->weight;
}

char * get_line(text_t * txt, int index){

  if (txt->left == NULL){
    printf("*** Tree is empty ***\n");
    return (NULL);
  }
  else{
    text_t * tmp_node;
    tmp_node = txt;

    while (tmp_node->right != NULL){

      if (index <= tmp_node->left->weight){

	//printf("going left \n");
	//printf("%d \n", tmp_node->weight);
	tmp_node = tmp_node->left;
	
      }
      else{ //if (index > tmp_node->right->weight){
	//printf("going right \n");
	//printf("%d \n", tmp_node->weight);
	index = index - tmp_node->left->weight;
	tmp_node = tmp_node->right;
      }
      
    }

    if (tmp_node->weight == index)
      return( (char *) tmp_node->left);

    else{

      printf("*** Line not found returning NULL***\n");
      return (NULL); 
    }
     
    
  }
  
}

void append_line(text_t *txt, char * new_line){

  if (txt->left == NULL){

    txt->left = (text_t *) new_line;
    txt->weight = 1;
    txt->right = NULL;
  }
  else{

    text_t * tmp_node = txt;
    text_t *path_stack[100]; int path_st_p = 0;
    while (tmp_node->right != NULL){
      path_stack[path_st_p++] = tmp_node;
      tmp_node = tmp_node->right;
    }

    text_t *old_leaf, *new_leaf;
    old_leaf = get_node();
    old_leaf->left = tmp_node->left;
    old_leaf->right = NULL;
    old_leaf->weight = 1;
    new_leaf = get_node();
    new_leaf->left = (text_t *) new_line;
    new_leaf->right = NULL;
    new_leaf->weight = 1;
    tmp_node->left = old_leaf;
    tmp_node->right = new_leaf;
    //because it has two nodes beneath it with weight 1 and 1
    tmp_node->weight = 2;


    
    //rebalance
    while( path_st_p > 0 )
        {  tmp_node = path_stack[--path_st_p];
           tmp_node->weight =  tmp_node->left->weight
                             + tmp_node->right->weight;
           if( tmp_node->right->weight
                     < ALPHA*tmp_node->weight )
           {  if( tmp_node->left->left->weight
                     > (ALPHA+EPSILON)*tmp_node->weight )
              {  right_rotation( tmp_node );
                 tmp_node->right->weight =
                                tmp_node->right->left->weight
                              + tmp_node->right->right->weight;
              }
              else
              {  left_rotation( tmp_node->left );
                 right_rotation( tmp_node );
                 tmp_node->right->weight =
                                tmp_node->right->left->weight
                              + tmp_node->right->right->weight;
                 tmp_node->left->weight =
                                tmp_node->left->left->weight
                              + tmp_node->left->right->weight;
              }
           }
           else if ( tmp_node->left->weight
                     < ALPHA*tmp_node->weight )
           {  if( tmp_node->right->right->weight
                     > (ALPHA+EPSILON)*tmp_node->weight )
              {  left_rotation( tmp_node );
                 tmp_node->left->weight =
                                tmp_node->left->left->weight
                              + tmp_node->left->right->weight;
              }
              else
              {  right_rotation( tmp_node->right );
                 left_rotation( tmp_node );
                 tmp_node->right->weight =
                                tmp_node->right->left->weight
                              + tmp_node->right->right->weight;
                 tmp_node->left->weight =
                                tmp_node->left->left->weight
                              + tmp_node->left->right->weight;
              }
           }

        }
  }
    
}

char * set_line(text_t *txt, int index, char * new_line){

  if (txt->left == NULL)
    return NULL;
  else{

    text_t * tmp_node;
    text_t * path_stack[100]; int path_st_p = 0;

    tmp_node = txt;

    while(tmp_node->right != NULL){

      path_stack[path_st_p++] = tmp_node;
      if (index <= tmp_node->left->weight)
	tmp_node = tmp_node->left;
      else{ //if{ //(index > tmp_node->right->weight){

	index = index - tmp_node->left->weight;
	tmp_node = tmp_node->right;
	
      }
      
    }

    if (index == tmp_node->weight){
      text_t * old_line = tmp_node->left;
      tmp_node->left = (text_t *) new_line;
      return (char *) old_line;
      
    }
    
    
  }
  return NULL;
}

void insert_line(text_t * txt, int index, char * new_line){

  
  if (txt->left == NULL){

    txt->left = (text_t *) new_line;
    txt->weight = 1;
    txt->right = NULL;
    
  }

  else{

    text_t * tmp_node = txt;
    text_t * path_stack[100]; int path_st_p = 0;

    while (tmp_node->right != NULL){
      path_stack[path_st_p++] = tmp_node;
      if ( index <= tmp_node->left->weight)
        tmp_node = tmp_node->left;
      else{ //if (index > tmp_node->right->weight){
        index = index - tmp_node->left->weight;
        tmp_node = tmp_node->right;
      }
    }

     //found leaf
    if (index == tmp_node->weight){

      text_t * old_leaf, * new_leaf;
      old_leaf = get_node();
      old_leaf->left = tmp_node->left;
      old_leaf->right = NULL;
      old_leaf->weight = 1;

      new_leaf = get_node();
      new_leaf->left = (text_t *) new_line;
      new_leaf->right = NULL;
      new_leaf->weight = 1;

      tmp_node->left = new_leaf;
      tmp_node->right = old_leaf;
      tmp_node->weight = 2;


    }

    else{

      append_line(txt, new_line);
      
    }

       //rebalance
       while( path_st_p > 0 )
        {  tmp_node = path_stack[--path_st_p];
           tmp_node->weight =  tmp_node->left->weight
                             + tmp_node->right->weight;
           if( tmp_node->right->weight
                     < ALPHA*tmp_node->weight )
           {  if( tmp_node->left->left->weight
                     > (ALPHA+EPSILON)*tmp_node->weight )
              {  right_rotation( tmp_node );
                 tmp_node->right->weight =
                                tmp_node->right->left->weight
                              + tmp_node->right->right->weight;
              }
              else
              {  left_rotation( tmp_node->left );
                 right_rotation( tmp_node );
                 tmp_node->right->weight =
                                tmp_node->right->left->weight
                              + tmp_node->right->right->weight;
                 tmp_node->left->weight =
                                tmp_node->left->left->weight
                              + tmp_node->left->right->weight;
              }
           }
           else if ( tmp_node->left->weight
                     < ALPHA*tmp_node->weight )
           {  if( tmp_node->right->right->weight
                     > (ALPHA+EPSILON)*tmp_node->weight )
              {  left_rotation( tmp_node );
                 tmp_node->left->weight =
                                tmp_node->left->left->weight
                              + tmp_node->left->right->weight;
              }
              else
              {  right_rotation( tmp_node->right );
                 left_rotation( tmp_node );
                 tmp_node->right->weight =
                                tmp_node->right->left->weight
                              + tmp_node->right->right->weight;
                 tmp_node->left->weight =
                                tmp_node->left->left->weight
                              + tmp_node->left->right->weight;
              }
           }

        }					  
    
  }
  
}


char * delete_line(text_t *txt, int index){
    text_t *tmp_node, *upper_node, *other_node;
    char * del_line;

    if (txt->left == NULL)
        return NULL;
    
    else if (txt->right == NULL){
        if (txt->weight == index){
            del_line = (char *) txt->left;
            txt->weight = 0;
            txt->left = NULL;
            return (del_line);
        }
        else
            return NULL;

    }
    else{
        text_t *path_stack[100]; int path_st_p = 0;
        tmp_node = txt;
        while (tmp_node->right != NULL){
            path_stack[path_st_p++] = tmp_node;
            upper_node = tmp_node;

            if (index <= tmp_node->left->weight){
                tmp_node = upper_node->left;
                other_node = upper_node->right;
            }
            else {//if (index > tmp_node->right->weight){
                index = index - tmp_node->left->weight;
                tmp_node = upper_node->right;
                other_node = upper_node->left;

            }
        }

        if (tmp_node->weight != index)
            del_line = NULL;
        else{
	  // upper_node->key = other_node->key;
            upper_node->left = other_node->left;
            upper_node->right = other_node->right;
            upper_node->weight = other_node->weight;
            del_line = (char *) tmp_node->left;
            return_node(tmp_node);
            return_node(other_node);
        }

        /*start rebalance*/
    path_st_p -= 1;
    while( path_st_p > 0 )
    {  tmp_node = path_stack[--path_st_p];
       tmp_node->weight =  tmp_node->left->weight
                         + tmp_node->right->weight;
       if( tmp_node->right->weight
                 < ALPHA*tmp_node->weight )
       {  if( tmp_node->left->left->weight
                 > (ALPHA+EPSILON)*tmp_node->weight )
          {  right_rotation( tmp_node );
             tmp_node->right->weight =
                            tmp_node->right->left->weight
                          + tmp_node->right->right->weight;
          }
          else
          {  left_rotation( tmp_node->left );
             right_rotation( tmp_node );
             tmp_node->right->weight =
                            tmp_node->right->left->weight
                          + tmp_node->right->right->weight;
             tmp_node->left->weight =
                            tmp_node->left->left->weight
                          + tmp_node->left->right->weight;
          }
       }
       else if ( tmp_node->left->weight
                 < ALPHA*tmp_node->weight )
       {  if( tmp_node->right->right->weight
                 > (ALPHA+EPSILON)*tmp_node->weight )
          {  left_rotation( tmp_node );
             tmp_node->left->weight =
                            tmp_node->left->left->weight
                          + tmp_node->left->right->weight;
          }
          else
          {  right_rotation( tmp_node->right );
             left_rotation( tmp_node );
             tmp_node->right->weight =
                            tmp_node->right->left->weight
                          + tmp_node->right->right->weight;
             tmp_node->left->weight =
                            tmp_node->left->left->weight
                          + tmp_node->left->right->weight;
          }
       }
    }
    /*end rebalance*/
    return(del_line );

    }

}
int main()
{  int i, tmp; text_t *txt1, *txt2, *txt3; char *c;
   printf("starting \n");
   txt1 = create_text();
   txt2 = create_text();
   txt3 = create_text();
   append_line(txt1, "line one" );
   if( (tmp = length_text(txt1)) != 1)
   {  printf("Test 1: length should be 1, is %d\n", tmp); exit(-1);
   }
   append_line(txt1, "line hundred" );
   insert_line(txt1, 2, "line ninetynine" );
   insert_line(txt1, 2, "line ninetyeight" );
   insert_line(txt1, 2, "line ninetyseven" );
   insert_line(txt1, 2, "line ninetysix" );
   insert_line(txt1, 2, "line ninetyfive" );
   for( i = 2; i <95; i++ )
     insert_line(txt1, 2, "some filler line between 1 and 95" );
   if( (tmp = length_text(txt1)) != 100)
   {  printf("Test 2: length should be 100, is %d\n", tmp); exit(-1);
   }
   printf("found at line 1:   %s\n",get_line(txt1,  1));
   printf("found at line 2:   %s\n",get_line(txt1,  2));
   printf("found at line 99:  %s\n",get_line(txt1, 99));
   printf("found at line 100: %s\n",get_line(txt1,100));
   for(i=1; i<=10000; i++)
   {  if( i%2==1 )
        append_line(txt2, "A");
      else
        append_line(txt2, "B");
   }
   if( (tmp = length_text(txt2)) != 10000)
   {  printf("Test 3: length should be 10000, is %d\n", tmp); exit(-1);
   }
   c = get_line(txt2, 9876 );
   if( *c != 'B')
     {  printf("Test 4: line 9876 of txt2 should be B, found %s\n", c); exit(-1);
   }
   for( i= 10000; i > 1; i-=2 )
   {  c = delete_line(txt2, i);
      if( *c != 'B')
      {  printf("Test 5: line %d of txt2 should be B, found %s\n", i, c); exit(-1);
      }
      append_line( txt2, c );
   }
   for( i=1; i<= 5000; i++ )
   {  c = get_line(txt2, i);
      if( *c != 'A')
      {  printf("Test 6: line %d of txt2 should be A, found %s\n", i, c); exit(-1);
      }
   }
   for( i=1; i<= 5000; i++ )
     delete_line(txt2, 1 );
   for( i=1; i<= 5000; i++ )
   {  c = get_line(txt2, i);
      if( *c != 'B')
      {  printf("Test 7: line %d of txt2 should be B, found %s\n", i, c); exit(-1);
      }
   }
   set_line(txt1, 100, "the last line");
   for( i=99; i>=1; i-- )
     delete_line(txt1, i );
   printf("found at the last line:   %s\n",get_line(txt1,  1));
   for(i=0; i<1000000; i++)
     append_line(txt3, "line" );
   if( (tmp = length_text(txt3)) != 1000000)
   {  printf("Test 8: length should be 1000000, is %d\n", tmp); exit(-1);
   }
   for(i=0; i<500000; i++)
     delete_line(txt3, 400000 );
   if( (tmp = length_text(txt3)) != 500000)
   {  printf("Test 9: length should be 500000, is %d\n", tmp); exit(-1);
   }
   printf("End of tests\n");
}


/* int main(){ */

/*   /\* create_text(); *\/ */
/*   text_t * searchtree; */
/*   char nextop; */
/*   searchtree = create_text(); */
/*   printf("*** Made Tree: Weight Balanced Tree ***\n"); */

/*   while( (nextop = getchar()) != 'q'){ */

/*     if (nextop == 'l' ){ */

/*       int number_of_lines; */

/*       number_of_lines = length_text(searchtree); */

/*       printf("*** Number of lines in text is %d ***\n", number_of_lines); */
      
/*     } */
/*     else if (nextop == 'a'){ */

/*       char * text_to_insert; */
/*       text_to_insert = (char *) malloc(sizeof(char)); */
/*       scanf("%s", text_to_insert); */
/*       append_line(searchtree, text_to_insert); */
/*       printf("*** Last line added with text %s ***\n ", text_to_insert ); */
/*     } */

/*     else if (nextop == 's'){ */

/*       char *text_to_insert; */
/*       int line_number; */
/*       char * old_line; */
/*       text_to_insert = (char *) malloc (sizeof(char)); */
/*        scanf("%d", &line_number ); */
/*        scanf("%s", text_to_insert); */

/*        old_line = set_line(searchtree, line_number, text_to_insert); */
/*        printf("*** %s replaced %s at line number %d *** \n ", text_to_insert , old_line, line_number); */

      
/*     } */

/*     else if (nextop == 'i'){ */

/*        int line_number; */
/*        char *text_to_insert; */
/*        int success = 0; */
/*        text_to_insert = (char *) malloc(sizeof(char)); */
/*        scanf("%d", &line_number ); */
/*        scanf("%s", text_to_insert ); */

/*        insert_line(searchtree, line_number, text_to_insert); */

/* 	  if (success == 0) */
/* 	    printf("*** Insert successful, line number = %d, text = %s *** \n", line_number, text_to_insert ); */
/* 	  else */
/* 	    printf("*** Insert failed *** \n"); */

      
/*     } */

/*     else if (nextop == 'g'){ */
/*        int key_to_find ; */
/*        scanf("%d", &key_to_find); */
/*        char * text_at_line = get_line(searchtree, key_to_find ); */
/*        if (text_at_line == NULL) */
/* 	 printf("*** Key not found ***\n"); */
/*        else */
/* 	 printf("*** Key %s found ***\n", text_at_line); */
/*     } */

/*     else if (nextop == 'd'){ */

/*       int delkey; */
/*        scanf("%d", &delkey); */
/*        char * line_to_be_deleted; */
/*        line_to_be_deleted = delete_line(searchtree, delkey); */


/*        if (line_to_be_deleted == NULL) */
/*          printf("*** Delete not possible. Line %d does not exist *** \n", delkey); */
/*        else */
/*          printf("*** Line %d deleted *** \n", delkey); */


      
/*     } */

    
/*   } */

/*    return (0);  */
  
/*  }  */
