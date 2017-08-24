/*
 * "$Id: mxml-set.c 451 2014-01-04 21:50:06Z msweet $"
 *
 * Node set functions for Mini-XML, a small XML-like file parsing library.
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

int					/*                                 */
mxmlSetCDATA(mxml_node_t *node,		/*                 */
             const char  *data)		/*                     */
{
 /*
                        
  */

  if (node && node->type == MXML_ELEMENT &&
      strncmp(node->value.element.name, "![CDATA[", 8) &&
      node->child && node->child->type == MXML_ELEMENT &&
      !strncmp(node->child->value.element.name, "![CDATA[", 8))
    node = node->child;

  if (!node || node->type != MXML_ELEMENT || !data ||
      strncmp(node->value.element.name, "![CDATA[", 8))
    return (-1);

 /*
                                                       
  */

  if (node->value.element.name)
    free(node->value.element.name);

  node->value.element.name = _mxml_strdupf("![CDATA[%s]]", data);

  return (0);
}


/*
                                                                         
  
                                                                           
  
                       
 */

int					/*                                 */
mxmlSetCustom(
    mxml_node_t              *node,	/*                 */
    void                     *data,	/*                      */
    mxml_custom_destroy_cb_t destroy)	/*                             */
{
 /*
                        
  */

  if (node && node->type == MXML_ELEMENT &&
      node->child && node->child->type == MXML_CUSTOM)
    node = node->child;

  if (!node || node->type != MXML_CUSTOM)
    return (-1);

 /*
                                                       
  */

  if (node->value.custom.data && node->value.custom.destroy)
    (*(node->value.custom.destroy))(node->value.custom.data);

  node->value.custom.data    = data;
  node->value.custom.destroy = destroy;

  return (0);
}


/*
                                                        
  
                                                        
 */

int					/*                                 */
mxmlSetElement(mxml_node_t *node,	/*                 */
               const char  *name)	/*                     */
{
 /*
                        
  */

  if (!node || node->type != MXML_ELEMENT || !name)
    return (-1);

 /*
                                                       
  */

  if (node->value.element.name)
    free(node->value.element.name);

  node->value.element.name = strdup(name);

  return (0);
}


/*
                                                         
  
                                                                             
 */

int					/*                                 */
mxmlSetInteger(mxml_node_t *node,	/*                 */
               int         integer)	/*                   */
{
 /*
                        
  */

  if (node && node->type == MXML_ELEMENT &&
      node->child && node->child->type == MXML_INTEGER)
    node = node->child;

  if (!node || node->type != MXML_INTEGER)
    return (-1);

 /*
                                   
  */

  node->value.integer = integer;

  return (0);
}


/*
                                                       
  
                                                                            
 */

int					/*                                 */
mxmlSetOpaque(mxml_node_t *node,	/*                 */
              const char  *opaque)	/*                   */
{
 /*
                        
  */

  if (node && node->type == MXML_ELEMENT &&
      node->child && node->child->type == MXML_OPAQUE)
    node = node->child;

  if (!node || node->type != MXML_OPAQUE || !opaque)
    return (-1);

 /*
                                                      
  */

  if (node->value.opaque)
    free(node->value.opaque);

  node->value.opaque = strdup(opaque);

  return (0);
}


/*
                                                         
  
                                                                                
 */

int					/*                                 */
mxmlSetReal(mxml_node_t *node,		/*                 */
            double      real)		/*                       */
{
 /*
                        
  */

  if (node && node->type == MXML_ELEMENT &&
      node->child && node->child->type == MXML_REAL)
    node = node->child;

  if (!node || node->type != MXML_REAL)
    return (-1);

 /*
                                   
  */

  node->value.real = real;

  return (0);
}


/*
                                                  
  
                                                                         
 */

int					/*                                 */
mxmlSetText(mxml_node_t *node,		/*                 */
            int         whitespace,	/*                                               */
	    const char  *string)	/*            */
{
 /*
                        
  */

  if (node && node->type == MXML_ELEMENT &&
      node->child && node->child->type == MXML_TEXT)
    node = node->child;

  if (!node || node->type != MXML_TEXT || !string)
    return (-1);

 /*
                                                      
  */

  if (node->value.text.string)
    free(node->value.text.string);

  node->value.text.whitespace = whitespace;
  node->value.text.string     = strdup(string);

  return (0);
}


/*
                                                                         
  
                                                                         
 */

int					/*                                 */
mxmlSetTextf(mxml_node_t *node,		/*                 */
             int         whitespace,	/*                                               */
             const char  *format,	/*                                */
	     ...)			/*                                    */
{
  va_list	ap;			/*                      */


 /*
                        
  */

  if (node && node->type == MXML_ELEMENT &&
      node->child && node->child->type == MXML_TEXT)
    node = node->child;

  if (!node || node->type != MXML_TEXT || !format)
    return (-1);

 /*
                                                      
  */

  if (node->value.text.string)
    free(node->value.text.string);

  va_start(ap, format);

  node->value.text.whitespace = whitespace;
  node->value.text.string     = _mxml_strdupf(format, ap);

  va_end(ap);

  return (0);
}


/*
                                                              
  
                       
 */

int					/*                                 */
mxmlSetUserData(mxml_node_t *node,	/*                 */
                void        *data)	/*                       */
{
 /*
                        
  */

  if (!node)
    return (-1);

 /*
                                           
  */

  node->user_data = data;
  return (0);
}


/*
                                                              
 */
