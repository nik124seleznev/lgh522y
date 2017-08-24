/*
 * "$Id: mxml-index.c 451 2014-01-04 21:50:06Z msweet $"
 *
 * Index support code for Mini-XML, a small XML-like file parsing library.
 *
 * Copyright 2003-2014 by Michael R Sweet.
 *
 * These coded instructions, statements, and computer programs are the
 * property of Michael R Sweet and are protected by Federal copyright
 * law.  Distribution and use rights are outlined in the file "COPYING"
 * which should have been included with this file.  If this file is
 * missing or damaged, see the license at:
 *
 *     http://www.msweet.org/projects.php/Mini-XML
 */

/*
                               
 */

#include "config.h"
#include "mxml.h"


/*
                    
 */

static int	index_compare(mxml_index_t *ind, mxml_node_t *first,
		              mxml_node_t *second);
static int	index_find(mxml_index_t *ind, const char *element,
		           const char *value, mxml_node_t *node);
static void	index_sort(mxml_index_t *ind, int left, int right);


/*
                                         
 */

void
mxmlIndexDelete(mxml_index_t *ind)	/*                     */
{
 /*
                       
  */

  if (!ind)
    return;

 /*
                  
  */

  if (ind->attr)
    free(ind->attr);

  if (ind->alloc_nodes)
    free(ind->nodes);

  free(ind);
}


/*
                                                         
  
                                                       
 */

mxml_node_t *				/*                                        */
mxmlIndexEnum(mxml_index_t *ind)	/*                        */
{
 /*
                        
  */

  if (!ind)
    return (NULL);

 /*
                           
  */

  if (ind->cur_node < ind->num_nodes)
    return (ind->nodes[ind->cur_node ++]);
  else
    return (NULL);
}


/*
                                                   
  
                                                                    
                                                                
                                                                     
                              
 */

mxml_node_t *				/*                                */
mxmlIndexFind(mxml_index_t *ind,	/*                     */
              const char   *element,	/*                                  */
	      const char   *value)	/*                             */
{
  int		diff,			/*                          */
		current,		/*                          */
		first,			/*                        */
		last;			/*                       */


#ifdef DEBUG
  printf("mxmlIndexFind(ind=%p, element=\"%s\", value=\"%s\")\n",
         ind, element ? element : "(null)", value ? value : "(null)");
#endif /*       */

 /*
                        
  */

  if (!ind || (!ind->attr && value))
  {
#ifdef DEBUG
    puts("    returning NULL...");
    printf("    ind->attr=\"%s\"\n", ind->attr ? ind->attr : "(null)");
#endif /*       */

    return (NULL);
  }

 /*
                                                                       
            
  */

  if (!element && !value)
    return (mxmlIndexEnum(ind));

 /*
                                                      
  */

  if (!ind->num_nodes)
  {
#ifdef DEBUG
    puts("    returning NULL...");
    puts("    no nodes!");
#endif /*       */

    return (NULL);
  }

 /*
                                                          
  */

  if (ind->cur_node == 0)
  {
   /*
                                                                     
    */

    first = 0;
    last  = ind->num_nodes - 1;

#ifdef DEBUG
    printf("    find first time, num_nodes=%d...\n", ind->num_nodes);
#endif /*       */

    while ((last - first) > 1)
    {
      current = (first + last) / 2;

#ifdef DEBUG
      printf("    first=%d, last=%d, current=%d\n", first, last, current);
#endif /*       */

      if ((diff = index_find(ind, element, value, ind->nodes[current])) == 0)
      {
       /*
                                                       
 */

#ifdef DEBUG
        puts("    match!");
#endif /*       */

        while (current > 0 &&
	       !index_find(ind, element, value, ind->nodes[current - 1]))
	  current --;

#ifdef DEBUG
        printf("    returning first match=%d\n", current);
#endif /*       */

       /*
                                                                  
 */

        ind->cur_node = current + 1;

	return (ind->nodes[current]);
      }
      else if (diff < 0)
	last = current;
      else
	first = current;

#ifdef DEBUG
      printf("    diff=%d\n", diff);
#endif /*       */
    }

   /*
                                                                 
    */

    for (current = first; current <= last; current ++)
      if (!index_find(ind, element, value, ind->nodes[current]))
      {
       /*
                                               
 */

#ifdef DEBUG
	printf("    returning only match %d...\n", current);
#endif /*       */

	ind->cur_node = current + 1;

	return (ind->nodes[current]);
      }

   /*
                   
    */

    ind->cur_node = ind->num_nodes;

#ifdef DEBUG
    puts("    returning NULL...");
#endif /*       */

    return (NULL);
  }
  else if (ind->cur_node < ind->num_nodes &&
           !index_find(ind, element, value, ind->nodes[ind->cur_node]))
  {
   /*
                                      
    */

#ifdef DEBUG
    printf("    returning next match %d...\n", ind->cur_node);
#endif /*       */

    return (ind->nodes[ind->cur_node ++]);
  }

 /*
                                                  
  */

  ind->cur_node = ind->num_nodes;

#ifdef DEBUG
  puts("    returning NULL...");
#endif /*       */

  return (NULL);
}


/*
                                                               
  
                       
 */

int					/*                              */
mxmlIndexGetCount(mxml_index_t *ind)	/*                    */
{
 /*
                        
  */

  if (!ind)
    return (0);

 /*
                                              
  */

  return (ind->num_nodes);
}


/*
                                         
  
                                                                         
                                                                        
                                                                     
                                                                         
                        
 */

mxml_index_t *				/*               */
mxmlIndexNew(mxml_node_t *node,		/*                   */
             const char  *element,	/*                                      */
             const char  *attr)		/*                                         */
{
  mxml_index_t	*ind;			/*           */
  mxml_node_t	*current,		/*                       */
  		**temp;			/*                              */


 /*
                        
  */

#ifdef DEBUG
  printf("mxmlIndexNew(node=%p, element=\"%s\", attr=\"%s\")\n",
         node, element ? element : "(null)", attr ? attr : "(null)");
#endif /*       */

  if (!node)
    return (NULL);

 /*
                         
  */

  if ((ind = calloc(1, sizeof(mxml_index_t))) == NULL)
  {
    mxml_error("Unable to allocate %d bytes for index - %s",
               sizeof(mxml_index_t), strerror(errno));
    return (NULL);
  }

  if (attr)
    ind->attr = strdup(attr);

  if (!element && !attr)
    current = node;
  else
    current = mxmlFindElement(node, node, element, attr, NULL, MXML_DESCEND);

  while (current)
  {
    if (ind->num_nodes >= ind->alloc_nodes)
    {
      if (!ind->alloc_nodes)
        temp = malloc(64 * sizeof(mxml_node_t *));
      else
        temp = realloc(ind->nodes, (ind->alloc_nodes + 64) * sizeof(mxml_node_t *));

      if (!temp)
      {
       /*
                                                              
 */

        mxml_error("Unable to allocate %d bytes for index: %s",
	           (ind->alloc_nodes + 64) * sizeof(mxml_node_t *),
		   strerror(errno));

        mxmlIndexDelete(ind);
	return (NULL);
      }

      ind->nodes       = temp;
      ind->alloc_nodes += 64;
    }

    ind->nodes[ind->num_nodes ++] = current;

    current = mxmlFindElement(current, node, element, attr, NULL, MXML_DESCEND);
  }

 /*
                                                
  */

#ifdef DEBUG
  {
    int i;				/*             */


    printf("%d node(s) in index.\n\n", ind->num_nodes);

    if (attr)
    {
      printf("Node      Address   Element         %s\n", attr);
      puts("--------  --------  --------------  ------------------------------");

      for (i = 0; i < ind->num_nodes; i ++)
	printf("%8d  %-8p  %-14.14s  %s\n", i, ind->nodes[i],
	       ind->nodes[i]->value.element.name,
	       mxmlElementGetAttr(ind->nodes[i], attr));
    }
    else
    {
      puts("Node      Address   Element");
      puts("--------  --------  --------------");

      for (i = 0; i < ind->num_nodes; i ++)
	printf("%8d  %-8p  %s\n", i, ind->nodes[i],
	       ind->nodes[i]->value.element.name);
    }

    putchar('\n');
  }
#endif /*       */

  if (ind->num_nodes > 1)
    index_sort(ind, 0, ind->num_nodes - 1);

#ifdef DEBUG
  {
    int i;				/*             */


    puts("After sorting:\n");

    if (attr)
    {
      printf("Node      Address   Element         %s\n", attr);
      puts("--------  --------  --------------  ------------------------------");

      for (i = 0; i < ind->num_nodes; i ++)
	printf("%8d  %-8p  %-14.14s  %s\n", i, ind->nodes[i],
	       ind->nodes[i]->value.element.name,
	       mxmlElementGetAttr(ind->nodes[i], attr));
    }
    else
    {
      puts("Node      Address   Element");
      puts("--------  --------  --------------");

      for (i = 0; i < ind->num_nodes; i ++)
	printf("%8d  %-8p  %s\n", i, ind->nodes[i],
	       ind->nodes[i]->value.element.name);
    }

    putchar('\n');
  }
#endif /*       */

 /*
                           
  */

  return (ind);
}


/*
                                                                           
                                                           
  
                                                                   
                                      
 */

mxml_node_t *				/*                                         */
mxmlIndexReset(mxml_index_t *ind)	/*                    */
{
#ifdef DEBUG
  printf("mxmlIndexReset(ind=%p)\n", ind);
#endif /*       */

 /*
                        
  */

  if (!ind)
    return (NULL);

 /*
                                         
  */

  ind->cur_node = 0;

 /*
                            
  */

  if (ind->num_nodes)
    return (ind->nodes[0]);
  else
    return (NULL);
}


/*
                                         
 */

static int				/*                          */
index_compare(mxml_index_t *ind,	/*           */
              mxml_node_t  *first,	/*                */
              mxml_node_t  *second)	/*                 */
{
  int	diff;				/*            */


 /*
                             
  */

  if ((diff = strcmp(first->value.element.name,
                     second->value.element.name)) != 0)
    return (diff);

 /*
                                
  */

  if (ind->attr)
  {
    if ((diff = strcmp(mxmlElementGetAttr(first, ind->attr),
                       mxmlElementGetAttr(second, ind->attr))) != 0)
      return (diff);
  }

 /*
                              
  */

  return (0);
}


/*
                                                     
 */

static int				/*                          */
index_find(mxml_index_t *ind,		/*           */
           const char   *element,	/*                          */
	   const char   *value,		/*                             */
           mxml_node_t  *node)		/*          */
{
  int	diff;				/*            */


 /*
                             
  */

  if (element)
  {
    if ((diff = strcmp(element, node->value.element.name)) != 0)
      return (diff);
  }

 /*
                                
  */

  if (value)
  {
    if ((diff = strcmp(value, mxmlElementGetAttr(node, ind->attr))) != 0)
      return (diff);
  }

 /*
                              
  */

  return (0);
}


/*
                                                  
  
                                                              
 */

static void
index_sort(mxml_index_t *ind,		/*                   */
           int          left,		/*                            */
	   int          right)		/*                             */
{
  mxml_node_t	*pivot,			/*            */
		*temp;			/*           */
  int		templ,			/*                     */
		tempr;			/*                      */


 /*
                                                         
  */

  do
  {
   /*
                                                
    */

    pivot = ind->nodes[left];

    for (templ = left, tempr = right; templ < tempr;)
    {
     /*
                                                  
      */

      while ((templ < right) &&
             index_compare(ind, ind->nodes[templ], pivot) <= 0)
	templ ++;

     /*
                                                   
      */

      while ((tempr > left) &&
             index_compare(ind, ind->nodes[tempr], pivot) > 0)
	tempr --;

     /*
                               
      */

      if (templ < tempr)
      {
	temp              = ind->nodes[templ];
	ind->nodes[templ] = ind->nodes[tempr];
	ind->nodes[tempr] = temp;
      }
    }

   /*
                                                                          
                   
    */

    if (index_compare(ind, pivot, ind->nodes[tempr]) > 0)
    {
      ind->nodes[left]  = ind->nodes[tempr];
      ind->nodes[tempr] = pivot;
    }

   /*
                                                      
    */

    if (left < (tempr - 1))
      index_sort(ind, left, tempr - 1);
  }
  while (right > (left = tempr + 1));
}


/*
                                                                
 */
