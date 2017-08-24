/*
 * "$Id: mxml-search.c 451 2014-01-04 21:50:06Z msweet $"
 *
 * Search/navigation functions for Mini-XML, a small XML-like file
 * parsing library.
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

mxml_node_t *				/*                          */
mxmlFindElement(mxml_node_t *node,	/*                  */
                mxml_node_t *top,	/*              */
                const char  *name,	/*                                  */
		const char  *attr,	/*                                      */
		const char  *value,	/*                                      */
		int         descend)	/*                                                                              */
{
  const char	*temp;			/*                         */


 /*
                        
  */

  if (!node || !top || (!attr && value))
    return (NULL);

 /*
                               
  */

  node = mxmlWalkNext(node, top, descend);

 /*
                                            
  */

  while (node != NULL)
  {
   /*
                                 
    */

    if (node->type == MXML_ELEMENT &&
        node->value.element.name &&
	(!name || !strcmp(node->value.element.name, name)))
    {
     /*
                                                   
      */

      if (!attr)
        return (node);			/*                                   */

     /*
                                  
      */

      if ((temp = mxmlElementGetAttr(node, attr)) != NULL)
      {
       /*
                                                   
 */

	if (!value || !strcmp(value, temp))
	  return (node);		/*                   */
      }
    }

   /*
                                           
    */

    if (descend == MXML_DESCEND)
      node = mxmlWalkNext(node, top, MXML_DESCEND);
    else
      node = node->next;
  }

  return (NULL);
}


/*
                                                      
  
                                                                         
                                                                          
                                                        
  
                                                                           
                                                
  
                       
 */

mxml_node_t *				/*                        */
mxmlFindPath(mxml_node_t *top,		/*              */
	     const char  *path)		/*                     */
{
  mxml_node_t	*node;			/*              */
  char		element[256];		/*                      */
  const char	*pathsep;		/*                   */
  int		descend;		/*                        */


 /*
                        
  */

  if (!top || !path || !*path)
    return (NULL);

 /*
                                      
  */

  node = top;
  while (*path)
  {
   /*
                         
    */

    if (!strncmp(path, "*/", 2))
    {
      path += 2;
      descend = MXML_DESCEND;
    }
    else
      descend = MXML_DESCEND_FIRST;

   /*
                                         
    */

    if ((pathsep = strchr(path, '/')) == NULL)
      pathsep = path + strlen(path);

    if (pathsep == path || (pathsep - path) >= sizeof(element))
      return (NULL);

    memcpy(element, path, pathsep - path);
    element[pathsep - path] = '\0';

    if (*pathsep)
      path = pathsep + 1;
    else
      path = pathsep;

   /*
                               
    */

    if ((node = mxmlFindElement(node, node, element, NULL, NULL,
                                descend)) == NULL)
      return (NULL);
  }

 /*
                                                             
  */

  if (node->child && node->child->type != MXML_ELEMENT)
    return (node->child);
  else
    return (node);
}


/*
                                                                
  
                                                                      
                                                                    
                       
 */

mxml_node_t *				/*                       */
mxmlWalkNext(mxml_node_t *node,		/*                  */
             mxml_node_t *top,		/*              */
             int         descend)	/*                                                                              */
{
  if (!node)
    return (NULL);
  else if (node->child && descend)
    return (node->child);
  else if (node == top)
    return (NULL);
  else if (node->next)
    return (node->next);
  else if (node->parent && node->parent != top)
  {
    node = node->parent;

    while (!node->next)
      if (node->parent == top || !node->parent)
        return (NULL);
      else
        node = node->parent;

    return (node->next);
  }
  else
    return (NULL);
}


/*
                                                                    
  
                                                                       
                                                                          
                                   
 */

mxml_node_t *				/*                           */
mxmlWalkPrev(mxml_node_t *node,		/*                  */
             mxml_node_t *top,		/*              */
             int         descend)	/*                                                                              */
{
  if (!node || node == top)
    return (NULL);
  else if (node->prev)
  {
    if (node->prev->last_child && descend)
    {
     /*
                                                      
      */

      node = node->prev->last_child;

      while (node->last_child)
        node = node->last_child;

      return (node);
    }
    else
      return (node->prev);
  }
  else if (node->parent != top)
    return (node->parent);
  else
    return (NULL);
}


/*
                                                                 
 */
