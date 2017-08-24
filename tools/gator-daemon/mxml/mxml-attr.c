/*
 * "$Id: mxml-attr.c 451 2014-01-04 21:50:06Z msweet $"
 *
 * Attribute support code for Mini-XML, a small XML-like file parsing library.
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

static int	mxml_set_attr(mxml_node_t *node, const char *name,
		              char *value);


/*
                                                   
  
                       
 */

void
mxmlElementDeleteAttr(mxml_node_t *node,/*             */
                      const char  *name)/*                    */
{
  int		i;			/*             */
  mxml_attr_t	*attr;			/*                   */


#ifdef DEBUG
  fprintf(stderr, "mxmlElementDeleteAttr(node=%p, name=\"%s\")\n",
          node, name ? name : "(null)");
#endif /*       */

 /*
                        
  */

  if (!node || node->type != MXML_ELEMENT || !name)
    return;

 /*
                             
  */

  for (i = node->value.element.num_attrs, attr = node->value.element.attrs;
       i > 0;
       i --, attr ++)
  {
#ifdef DEBUG
    printf("    %s=\"%s\"\n", attr->name, attr->value);
#endif /*       */

    if (!strcmp(attr->name, name))
    {
     /*
                                
      */

      free(attr->name);
      free(attr->value);

      i --;
      if (i > 0)
        memmove(attr, attr + 1, i * sizeof(mxml_attr_t));

      node->value.element.num_attrs --;

      if (node->value.element.num_attrs == 0)
        free(node->value.element.attrs);
      return;
    }
  }
}


/*
                                             
  
                                                                  
                                  
 */

const char *				/*                             */
mxmlElementGetAttr(mxml_node_t *node,	/*                  */
                   const char  *name)	/*                       */
{
  int		i;			/*             */
  mxml_attr_t	*attr;			/*                   */


#ifdef DEBUG
  fprintf(stderr, "mxmlElementGetAttr(node=%p, name=\"%s\")\n",
          node, name ? name : "(null)");
#endif /*       */

 /*
                        
  */

  if (!node || node->type != MXML_ELEMENT || !name)
    return (NULL);

 /*
                             
  */

  for (i = node->value.element.num_attrs, attr = node->value.element.attrs;
       i > 0;
       i --, attr ++)
  {
#ifdef DEBUG
    printf("    %s=\"%s\"\n", attr->name, attr->value);
#endif /*       */

    if (!strcmp(attr->name, name))
    {
#ifdef DEBUG
      printf("    Returning \"%s\"!\n", attr->value);
#endif /*       */
      return (attr->value);
    }
  }

 /*
                                            
  */

#ifdef DEBUG
  puts("    Returning NULL!\n");
#endif /*       */

  return (NULL);
}


/*
                                             
  
                                                                    
                                                                  
                                                                   
                  
 */

void
mxmlElementSetAttr(mxml_node_t *node,	/*                  */
                   const char  *name,	/*                       */
                   const char  *value)	/*                     */
{
  char	*valuec;			/*               */


#ifdef DEBUG
  fprintf(stderr, "mxmlElementSetAttr(node=%p, name=\"%s\", value=\"%s\")\n",
          node, name ? name : "(null)", value ? value : "(null)");
#endif /*       */

 /*
                        
  */

  if (!node || node->type != MXML_ELEMENT || !name)
    return;

  if (value)
    valuec = strdup(value);
  else
    valuec = NULL;

  if (mxml_set_attr(node, name, valuec))
    free(valuec);
}


/*
                                                                     
  
                                                                    
                                                                         
                                                                       
                     
  
                       
 */

void
mxmlElementSetAttrf(mxml_node_t *node,	/*                  */
                    const char  *name,	/*                       */
                    const char  *format,/*                                  */
		    ...)		/*                                    */
{
  va_list	ap;			/*                  */
  char		*value;			/*       */


#ifdef DEBUG
  fprintf(stderr,
          "mxmlElementSetAttrf(node=%p, name=\"%s\", format=\"%s\", ...)\n",
          node, name ? name : "(null)", format ? format : "(null)");
#endif /*       */

 /*
                        
  */

  if (!node || node->type != MXML_ELEMENT || !name || !format)
    return;

 /*
                       
  */

  va_start(ap, format);
  value = _mxml_vstrdupf(format, ap);
  va_end(ap);

  if (!value)
    mxml_error("Unable to allocate memory for attribute '%s' in element %s!",
               name, node->value.element.name);
  else if (mxml_set_attr(node, name, value))
    free(value);
}


/*
                                                               
 */

static int				/*                                 */
mxml_set_attr(mxml_node_t *node,	/*                  */
              const char  *name,	/*                    */
              char        *value)	/*                     */
{
  int		i;			/*             */
  mxml_attr_t	*attr;			/*               */


 /*
                             
  */

  for (i = node->value.element.num_attrs, attr = node->value.element.attrs;
       i > 0;
       i --, attr ++)
    if (!strcmp(attr->name, name))
    {
     /*
                                       
      */

      if (attr->value)
        free(attr->value);

      attr->value = value;

      return (0);
    }

 /*
                          
  */

  if (node->value.element.num_attrs == 0)
    attr = malloc(sizeof(mxml_attr_t));
  else
    attr = realloc(node->value.element.attrs,
                   (node->value.element.num_attrs + 1) * sizeof(mxml_attr_t));

  if (!attr)
  {
    mxml_error("Unable to allocate memory for attribute '%s' in element %s!",
               name, node->value.element.name);
    return (-1);
  }

  node->value.element.attrs = attr;
  attr += node->value.element.num_attrs;

  if ((attr->name = strdup(name)) == NULL)
  {
    mxml_error("Unable to allocate memory for attribute '%s' in element %s!",
               name, node->value.element.name);
    return (-1);
  }

  attr->value = value;

  node->value.element.num_attrs ++;

  return (0);
}


/*
                                                               
 */
