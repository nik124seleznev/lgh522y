/*
 * "$Id: mxml-get.c 451 2014-01-04 21:50:06Z msweet $"
 *
 * Node get functions for Mini-XML, a small XML-like file parsing library.
 *
 * Copyright 2014 by Michael R Sweet.
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

const char *				/*                         */
mxmlGetCDATA(mxml_node_t *node)		/*                 */
{
 /*
                        
  */

  if (!node || node->type != MXML_ELEMENT ||
      strncmp(node->value.element.name, "![CDATA[", 8))
    return (NULL);

 /*
                                                      
  */

  return (node->value.element.name + 8);
}


/*
                                                       
  
                                                                           
              
  
                       
 */

const void *				/*                          */
mxmlGetCustom(mxml_node_t *node)	/*                 */
{
 /*
                        
  */

  if (!node)
    return (NULL);

 /*
                               
  */

  if (node->type == MXML_CUSTOM)
    return (node->value.custom.data);
  else if (node->type == MXML_ELEMENT &&
           node->child &&
	   node->child->type == MXML_CUSTOM)
    return (node->child->value.custom.data);
  else
    return (NULL);
}


/*
                                                         
  
                                                              
  
                       
 */

const char *				/*                          */
mxmlGetElement(mxml_node_t *node)	/*                 */
{
 /*
                        
  */

  if (!node || node->type != MXML_ELEMENT)
    return (NULL);

 /*
                              
  */

  return (node->value.element.name);
}


/*
                                                                  
  
                                                                            
                   
  
                       
 */

mxml_node_t *				/*                         */
mxmlGetFirstChild(mxml_node_t *node)	/*                 */
{
 /*
                        
  */

  if (!node || node->type != MXML_ELEMENT)
    return (NULL);

 /*
                                  
  */

  return (node->child);
}


/*
                                                                            
                                    
  
                                                                               
  
                       
 */

int					/*                        */
mxmlGetInteger(mxml_node_t *node)	/*                 */
{
 /*
                        
  */

  if (!node)
    return (0);

 /*
                               
  */

  if (node->type == MXML_INTEGER)
    return (node->value.integer);
  else if (node->type == MXML_ELEMENT &&
           node->child &&
	   node->child->type == MXML_INTEGER)
    return (node->child->value.integer);
  else
    return (0);
}


/*
                                                                
  
                                                                            
                   
  
                       
 */

mxml_node_t *				/*                        */
mxmlGetLastChild(mxml_node_t *node)	/*                 */
{
 /*
                        
  */

  if (!node || node->type != MXML_ELEMENT)
    return (NULL);

 /*
                           
  */

  return (node->last_child);
}


/*
                                                                     
  
                                                                            
  
                       
 */

mxml_node_t *
mxmlGetNextSibling(mxml_node_t *node)	/*                 */
{
 /*
                        
  */

  if (!node)
    return (NULL);

 /*
                           
  */

  return (node->next);
}


/*
                                                                                
  
                                                                            
              
  
                       
 */

const char *				/*                           */
mxmlGetOpaque(mxml_node_t *node)	/*                 */
{
 /*
                        
  */

  if (!node)
    return (NULL);

 /*
                               
  */

  if (node->type == MXML_OPAQUE)
    return (node->value.opaque);
  else if (node->type == MXML_ELEMENT &&
           node->child &&
	   node->child->type == MXML_OPAQUE)
    return (node->child->value.opaque);
  else
    return (NULL);
}


/*
                                           
  
                                           
  
                       
 */

mxml_node_t *				/*                         */
mxmlGetParent(mxml_node_t *node)	/*                 */
{
 /*
                        
  */

  if (!node)
    return (NULL);

 /*
                           
  */

  return (node->parent);
}


/*
                                                                         
  
                                                                             
  
                       
 */

mxml_node_t *				/*                           */
mxmlGetPrevSibling(mxml_node_t *node)	/*                 */
{
 /*
                        
  */

  if (!node)
    return (NULL);

 /*
                           
  */

  return (node->prev);
}


/*
                                                                      
  
                                                                             
  
                       
 */

double					/*                       */
mxmlGetReal(mxml_node_t *node)		/*                 */
{
 /*
                        
  */

  if (!node)
    return (0.0);

 /*
                               
  */

  if (node->type == MXML_REAL)
    return (node->value.real);
  else if (node->type == MXML_ELEMENT &&
           node->child &&
	   node->child->type == MXML_REAL)
    return (node->child->value.real);
  else
    return (0.0);
}


/*
                                                                      
  
                                                                               
                                         
  
                       
 */

const char *				/*                         */
mxmlGetText(mxml_node_t *node,		/*                 */
            int         *whitespace)	/*                                                        */
{
 /*
                        
  */

  if (!node)
  {
    if (whitespace)
      *whitespace = 0;

    return (NULL);
  }

 /*
                               
  */

  if (node->type == MXML_TEXT)
  {
    if (whitespace)
      *whitespace = node->value.text.whitespace;

    return (node->value.text.string);
  }
  else if (node->type == MXML_ELEMENT &&
           node->child &&
	   node->child->type == MXML_TEXT)
  {
    if (whitespace)
      *whitespace = node->child->value.text.whitespace;

    return (node->child->value.text.string);
  }
  else
  {
    if (whitespace)
      *whitespace = 0;

    return (NULL);
  }
}


/*
                                       
  
                                                           
  
                       
 */

mxml_type_t				/*                  */
mxmlGetType(mxml_node_t *node)		/*                 */
{
 /*
                        
  */

  if (!node)
    return (MXML_IGNORE);

 /*
                           
  */

  return (node->type);
}


/*
                                                              
  
                       
 */

void *					/*                       */
mxmlGetUserData(mxml_node_t *node)	/*                 */
{
 /*
                        
  */

  if (!node)
    return (NULL);

 /*
                                   
  */

  return (node->user_data);
}


/*
                                                              
 */
