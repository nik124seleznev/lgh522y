/*
 * "$Id: mxml-node.c 451 2014-01-04 21:50:06Z msweet $"
 *
 * Node support code for Mini-XML, a small XML-like file parsing library.
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

static mxml_node_t	*mxml_new(mxml_node_t *parent, mxml_type_t type);


/*
                                      
  
                                                                      
                                                                        
                                                                     
                                                                         
                                                                 
                                                                  
 */

void
mxmlAdd(mxml_node_t *parent,		/*                 */
        int         where,		/*                                                     */
        mxml_node_t *child,		/*                                                */
	mxml_node_t *node)		/*                 */
{
#ifdef DEBUG
  fprintf(stderr, "mxmlAdd(parent=%p, where=%d, child=%p, node=%p)\n", parent,
          where, child, node);
#endif /*       */

 /*
                        
  */

  if (!parent || !node)
    return;

#if DEBUG > 1
  fprintf(stderr, "    BEFORE: node->parent=%p\n", node->parent);
  if (parent)
  {
    fprintf(stderr, "    BEFORE: parent->child=%p\n", parent->child);
    fprintf(stderr, "    BEFORE: parent->last_child=%p\n", parent->last_child);
    fprintf(stderr, "    BEFORE: parent->prev=%p\n", parent->prev);
    fprintf(stderr, "    BEFORE: parent->next=%p\n", parent->next);
  }
#endif /*           */

 /*
                                               
  */

  if (node->parent)
    mxmlRemove(node);

 /*
                     
  */

  node->parent = parent;

  switch (where)
  {
    case MXML_ADD_BEFORE :
        if (!child || child == parent->child || child->parent != parent)
	{
	 /*
                                         
   */

	  node->next = parent->child;

	  if (parent->child)
	    parent->child->prev = node;
	  else
	    parent->last_child = node;

	  parent->child = node;
	}
	else
	{
	 /*
                                     
   */

	  node->next = child;
	  node->prev = child->prev;

	  if (child->prev)
	    child->prev->next = node;
	  else
	    parent->child = node;

	  child->prev = node;
	}
        break;

    case MXML_ADD_AFTER :
        if (!child || child == parent->last_child || child->parent != parent)
	{
	 /*
                                        
   */

	  node->parent = parent;
	  node->prev   = parent->last_child;

	  if (parent->last_child)
	    parent->last_child->next = node;
	  else
	    parent->child = node;

	  parent->last_child = node;
        }
	else
	{
	 /*
                                    
   */

	  node->prev = child;
	  node->next = child->next;

	  if (child->next)
	    child->next->prev = node;
	  else
	    parent->last_child = node;

	  child->next = node;
	}
        break;
  }

#if DEBUG > 1
  fprintf(stderr, "    AFTER: node->parent=%p\n", node->parent);
  if (parent)
  {
    fprintf(stderr, "    AFTER: parent->child=%p\n", parent->child);
    fprintf(stderr, "    AFTER: parent->last_child=%p\n", parent->last_child);
    fprintf(stderr, "    AFTER: parent->prev=%p\n", parent->prev);
    fprintf(stderr, "    AFTER: parent->next=%p\n", parent->next);
  }
#endif /*           */
}


/*
                                                          
  
                                                                      
                                                        
 */

void
mxmlDelete(mxml_node_t *node)		/*                    */
{
  int	i;				/*             */


#ifdef DEBUG
  fprintf(stderr, "mxmlDelete(node=%p)\n", node);
#endif /*       */

 /*
                        
  */

  if (!node)
    return;

 /*
                                              
  */

  mxmlRemove(node);

 /*
                      
  */

  while (node->child)
    mxmlDelete(node->child);

 /*
                               
  */

  switch (node->type)
  {
    case MXML_ELEMENT :
        if (node->value.element.name)
	  free(node->value.element.name);

	if (node->value.element.num_attrs)
	{
	  for (i = 0; i < node->value.element.num_attrs; i ++)
	  {
	    if (node->value.element.attrs[i].name)
	      free(node->value.element.attrs[i].name);
	    if (node->value.element.attrs[i].value)
	      free(node->value.element.attrs[i].value);
	  }

          free(node->value.element.attrs);
	}
        break;
    case MXML_INTEGER :
       /*               */
        break;
    case MXML_OPAQUE :
        if (node->value.opaque)
	  free(node->value.opaque);
        break;
    case MXML_REAL :
       /*               */
        break;
    case MXML_TEXT :
        if (node->value.text.string)
	  free(node->value.text.string);
        break;
    case MXML_CUSTOM :
        if (node->value.custom.data &&
	    node->value.custom.destroy)
	  (*(node->value.custom.destroy))(node->value.custom.data);
	break;
    default :
        break;
  }

 /*
                     
  */

  free(node);
}


/*
                                                                          
  
                                                                           
                                                                       
                   
  
                        
 */

int					/*                     */
mxmlGetRefCount(mxml_node_t *node)	/*          */
{
 /*
                        
  */

  if (!node)
    return (0);

 /*
                                 
  */

  return (node->ref_count);
}


/*
                                              
  
                                                                         
                                                                        
                                                                       
                                                                      
  
                       
 */

mxml_node_t *				/*              */
mxmlNewCDATA(mxml_node_t *parent,	/*                                   */
	     const char  *data)		/*                 */
{
  mxml_node_t	*node;			/*          */


#ifdef DEBUG
  fprintf(stderr, "mxmlNewCDATA(parent=%p, data=\"%s\")\n",
          parent, data ? data : "(null)");
#endif /*       */

 /*
                        
  */

  if (!data)
    return (NULL);

 /*
                                             
  */

  if ((node = mxml_new(parent, MXML_ELEMENT)) != NULL)
    node->value.element.name = _mxml_strdupf("![CDATA[%s]]", data);

  return (node);
}


/*
                                                     
  
                                                                          
                                                                        
                                                                      
                                                              
  
                       
 */

mxml_node_t *				/*              */
mxmlNewCustom(
    mxml_node_t              *parent,	/*                                   */
    void                     *data,	/*                     */
    mxml_custom_destroy_cb_t destroy)	/*                              */
{
  mxml_node_t	*node;			/*          */


#ifdef DEBUG
  fprintf(stderr, "mxmlNewCustom(parent=%p, data=%p, destroy=%p)\n", parent,
          data, destroy);
#endif /*       */

 /*
                                        
  */

  if ((node = mxml_new(parent, MXML_CUSTOM)) != NULL)
  {
    node->value.custom.data    = data;
    node->value.custom.destroy = destroy;
  }

  return (node);
}


/*
                                                  
  
                                                                           
                                                                        
                              
 */

mxml_node_t *				/*              */
mxmlNewElement(mxml_node_t *parent,	/*                                   */
               const char  *name)	/*                     */
{
  mxml_node_t	*node;			/*          */


#ifdef DEBUG
  fprintf(stderr, "mxmlNewElement(parent=%p, name=\"%s\")\n", parent,
          name ? name : "(null)");
#endif /*       */

 /*
                        
  */

  if (!name)
    return (NULL);

 /*
                                               
  */

  if ((node = mxml_new(parent, MXML_ELEMENT)) != NULL)
    node->value.element.name = strdup(name);

  return (node);
}


/*
                                                  
  
                                                                           
                                                                        
                              
 */

mxml_node_t *				/*              */
mxmlNewInteger(mxml_node_t *parent,	/*                                   */
               int         integer)	/*                   */
{
  mxml_node_t	*node;			/*          */


#ifdef DEBUG
  fprintf(stderr, "mxmlNewInteger(parent=%p, integer=%d)\n", parent, integer);
#endif /*       */

 /*
                                               
  */

  if ((node = mxml_new(parent, MXML_INTEGER)) != NULL)
    node->value.integer = integer;

  return (node);
}


/*
                                                  
  
                                                                          
                                                                        
                                                                          
                               
 */

mxml_node_t *				/*              */
mxmlNewOpaque(mxml_node_t *parent,	/*                                   */
              const char  *opaque)	/*                   */
{
  mxml_node_t	*node;			/*          */


#ifdef DEBUG
  fprintf(stderr, "mxmlNewOpaque(parent=%p, opaque=\"%s\")\n", parent,
          opaque ? opaque : "(null)");
#endif /*       */

 /*
                        
  */

  if (!opaque)
    return (NULL);

 /*
                                               
  */

  if ((node = mxml_new(parent, MXML_OPAQUE)) != NULL)
    node->value.opaque = strdup(opaque);

  return (node);
}


/*
                                                   
  
                                                                         
                                                                      
                                          
 */

mxml_node_t *				/*              */
mxmlNewReal(mxml_node_t *parent,	/*                                   */
            double      real)		/*                       */
{
  mxml_node_t	*node;			/*          */


#ifdef DEBUG
  fprintf(stderr, "mxmlNewReal(parent=%p, real=%g)\n", parent, real);
#endif /*       */

 /*
                                               
  */

  if ((node = mxml_new(parent, MXML_REAL)) != NULL)
    node->value.real = real;

  return (node);
}


/*
                                                     
  
                                                                        
                                                                        
                                                                       
                                                                  
                                                                 
 */

mxml_node_t *				/*              */
mxmlNewText(mxml_node_t *parent,	/*                                   */
            int         whitespace,	/*                                               */
	    const char  *string)	/*            */
{
  mxml_node_t	*node;			/*          */


#ifdef DEBUG
  fprintf(stderr, "mxmlNewText(parent=%p, whitespace=%d, string=\"%s\")\n",
          parent, whitespace, string ? string : "(null)");
#endif /*       */

 /*
                        
  */

  if (!string)
    return (NULL);

 /*
                                             
  */

  if ((node = mxml_new(parent, MXML_TEXT)) != NULL)
  {
    node->value.text.whitespace = whitespace;
    node->value.text.string     = strdup(string);
  }

  return (node);
}


/*
                                                                
  
                                                                        
                                                                        
                                                                       
                                                                    
                                                                    
 */

mxml_node_t *				/*              */
mxmlNewTextf(mxml_node_t *parent,	/*                                   */
             int         whitespace,	/*                                               */
	     const char  *format,	/*                               */
	     ...)			/*                               */
{
  mxml_node_t	*node;			/*          */
  va_list	ap;			/*                      */


#ifdef DEBUG
  fprintf(stderr, "mxmlNewTextf(parent=%p, whitespace=%d, format=\"%s\", ...)\n",
          parent, whitespace, format ? format : "(null)");
#endif /*       */

 /*
                        
  */

  if (!format)
    return (NULL);

 /*
                                             
  */

  if ((node = mxml_new(parent, MXML_TEXT)) != NULL)
  {
    va_start(ap, format);

    node->value.text.whitespace = whitespace;
    node->value.text.string     = _mxml_vstrdupf(format, ap);

    va_end(ap);
  }

  return (node);
}


/*
                                                  
  
                                                                     
                                                        
 */

void
mxmlRemove(mxml_node_t *node)		/*                    */
{
#ifdef DEBUG
  fprintf(stderr, "mxmlRemove(node=%p)\n", node);
#endif /*       */

 /*
                        
  */

  if (!node || !node->parent)
    return;

 /*
                         
  */

#if DEBUG > 1
  fprintf(stderr, "    BEFORE: node->parent=%p\n", node->parent);
  if (node->parent)
  {
    fprintf(stderr, "    BEFORE: node->parent->child=%p\n", node->parent->child);
    fprintf(stderr, "    BEFORE: node->parent->last_child=%p\n", node->parent->last_child);
  }
  fprintf(stderr, "    BEFORE: node->child=%p\n", node->child);
  fprintf(stderr, "    BEFORE: node->last_child=%p\n", node->last_child);
  fprintf(stderr, "    BEFORE: node->prev=%p\n", node->prev);
  fprintf(stderr, "    BEFORE: node->next=%p\n", node->next);
#endif /*           */

  if (node->prev)
    node->prev->next = node->next;
  else
    node->parent->child = node->next;

  if (node->next)
    node->next->prev = node->prev;
  else
    node->parent->last_child = node->prev;

  node->parent = NULL;
  node->prev   = NULL;
  node->next   = NULL;

#if DEBUG > 1
  fprintf(stderr, "    AFTER: node->parent=%p\n", node->parent);
  if (node->parent)
  {
    fprintf(stderr, "    AFTER: node->parent->child=%p\n", node->parent->child);
    fprintf(stderr, "    AFTER: node->parent->last_child=%p\n", node->parent->last_child);
  }
  fprintf(stderr, "    AFTER: node->child=%p\n", node->child);
  fprintf(stderr, "    AFTER: node->last_child=%p\n", node->last_child);
  fprintf(stderr, "    AFTER: node->prev=%p\n", node->prev);
  fprintf(stderr, "    AFTER: node->next=%p\n", node->next);
#endif /*           */
}


/*
                                                   
  
                                                                    
                                                      
  
                       
 */

mxml_node_t *				/*                   */
mxmlNewXML(const char *version)		/*                           */
{
  char	element[1024];			/*              */


  snprintf(element, sizeof(element), "?xml version=\"%s\" encoding=\"utf-8\"?",
           version ? version : "1.0");

  return (mxmlNewElement(NULL, element));
}


/*
                                    
  
                                                                     
                               
  
                       
 */

int					/*                         */
mxmlRelease(mxml_node_t *node)		/*          */
{
  if (node)
  {
    if ((-- node->ref_count) <= 0)
    {
      mxmlDelete(node);
      return (0);
    }
    else
      return (node->ref_count);
  }
  else
    return (-1);
}


/*
                                  
  
                       
 */

int					/*                         */
mxmlRetain(mxml_node_t *node)		/*          */
{
  if (node)
    return (++ node->ref_count);
  else
    return (-1);
}


/*
                                    
 */

static mxml_node_t *			/*              */
mxml_new(mxml_node_t *parent,		/*                 */
         mxml_type_t type)		/*               */
{
  mxml_node_t	*node;			/*          */


#if DEBUG > 1
  fprintf(stderr, "mxml_new(parent=%p, type=%d)\n", parent, type);
#endif /*           */

 /*
                                   
  */

  if ((node = calloc(1, sizeof(mxml_node_t))) == NULL)
  {
#if DEBUG > 1
    fputs("    returning NULL\n", stderr);
#endif /*           */

    return (NULL);
  }

#if DEBUG > 1
  fprintf(stderr, "    returning %p\n", node);
#endif /*           */

 /*
                        
  */

  node->type      = type;
  node->ref_count = 1;

 /*
                                   
  */

  if (parent)
    mxmlAdd(parent, MXML_ADD_AFTER, MXML_ADD_TO_PARENT, node);

 /*
                          
  */

  return (node);
}


/*
                                                               
 */
