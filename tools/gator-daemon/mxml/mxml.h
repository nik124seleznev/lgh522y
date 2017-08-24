/*
 * "$Id: mxml.h 451 2014-01-04 21:50:06Z msweet $"
 *
 * Header file for Mini-XML, a small XML-like file parsing library.
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

#ifndef _mxml_h_
#  define _mxml_h_

/*
                               
 */

#  include <stdio.h>
#  include <stdlib.h>
#  include <string.h>
#  include <ctype.h>
#  include <errno.h>


/*
               
 */

#  define MXML_MAJOR_VERSION	2	/*                      */
#  define MXML_MINOR_VERSION	8	/*                      */

#  define MXML_TAB		8	/*                      */

#  define MXML_NO_CALLBACK	0	/*                           */
#  define MXML_INTEGER_CALLBACK	mxml_integer_cb
					/*                            */
#  define MXML_OPAQUE_CALLBACK	mxml_opaque_cb
					/*                          */
#  define MXML_REAL_CALLBACK	mxml_real_cb
					/*                                */
#  define MXML_TEXT_CALLBACK	0	/*                        */
#  define MXML_IGNORE_CALLBACK	mxml_ignore_cb
					/*                                */

#  define MXML_NO_PARENT	0	/*                        */

#  define MXML_DESCEND		1	/*                              */
#  define MXML_NO_DESCEND	0	/*                                    */
#  define MXML_DESCEND_FIRST	-1	/*                        */

#  define MXML_WS_BEFORE_OPEN	0	/*                              */
#  define MXML_WS_AFTER_OPEN	1	/*                             */
#  define MXML_WS_BEFORE_CLOSE	2	/*                               */
#  define MXML_WS_AFTER_CLOSE	3	/*                              */

#  define MXML_ADD_BEFORE	0	/*                                */
#  define MXML_ADD_AFTER	1	/*                               */
#  define MXML_ADD_TO_PARENT	NULL	/*                             */


/*
                
 */

typedef enum mxml_sax_event_e		/*                       */
{
  MXML_SAX_CDATA,			/*            */
  MXML_SAX_COMMENT,			/*              */
  MXML_SAX_DATA,			/*           */
  MXML_SAX_DIRECTIVE,			/*                           */
  MXML_SAX_ELEMENT_CLOSE,		/*                */
  MXML_SAX_ELEMENT_OPEN			/*                */
} mxml_sax_event_t;

typedef enum mxml_type_e		/*                          */
{
  MXML_IGNORE = -1,			/*                                             */
  MXML_ELEMENT,				/*                             */
  MXML_INTEGER,				/*               */
  MXML_OPAQUE,				/*               */
  MXML_REAL,				/*            */
  MXML_TEXT,				/*               */
  MXML_CUSTOM				/*                                  */
} mxml_type_t;

typedef void (*mxml_custom_destroy_cb_t)(void *);
					/*                              */

typedef void (*mxml_error_cb_t)(const char *);
					/*                               */

typedef struct mxml_attr_s		/*                                                 */
{
  char			*name;		/*                */
  char			*value;		/*                 */
} mxml_attr_t;

typedef struct mxml_element_s		/*                                       */
{
  char			*name;		/*                 */
  int			num_attrs;	/*                      */
  mxml_attr_t		*attrs;		/*            */
} mxml_element_t;

typedef struct mxml_text_s		/*                                    */
{
  int			whitespace;	/*                     */
  char			*string;	/*                 */
} mxml_text_t;

typedef struct mxml_custom_s		/*                                      */
{
  void			*data;		/*                                    */
  mxml_custom_destroy_cb_t destroy;	/*                                */
} mxml_custom_t;

typedef union mxml_value_u		/*                                    */
{
  mxml_element_t	element;	/*         */
  int			integer;	/*                */
  char			*opaque;	/*               */
  double		real;		/*             */
  mxml_text_t		text;		/*               */
  mxml_custom_t		custom;		/*                                  */
} mxml_value_t;

struct mxml_node_s			/*                              */
{
  mxml_type_t		type;		/*           */
  struct mxml_node_s	*next;		/*                             */
  struct mxml_node_s	*prev;		/*                                 */
  struct mxml_node_s	*parent;	/*             */
  struct mxml_node_s	*child;		/*                  */
  struct mxml_node_s	*last_child;	/*                 */
  mxml_value_t		value;		/*            */
  int			ref_count;	/*           */
  void			*user_data;	/*           */
};

typedef struct mxml_node_s mxml_node_t;	/*                    */

struct mxml_index_s			 /*                                    */
{
  char			*attr;		/*                                     */
  int			num_nodes;	/*                          */
  int			alloc_nodes;	/*                          */
  int			cur_node;	/*              */
  mxml_node_t		**nodes;	/*            */
};

typedef struct mxml_index_s mxml_index_t;
					/*                          */

typedef int (*mxml_custom_load_cb_t)(mxml_node_t *, const char *);
					/*                                          */

typedef char *(*mxml_custom_save_cb_t)(mxml_node_t *);
					/*                                          */

typedef int (*mxml_entity_cb_t)(const char *);
					/*                             */

typedef mxml_type_t (*mxml_load_cb_t)(mxml_node_t *);
					/*                              */

typedef const char *(*mxml_save_cb_t)(mxml_node_t *, int);
					/*                              */

typedef void (*mxml_sax_cb_t)(mxml_node_t *, mxml_sax_event_t, void *);
					/*                             */


/*
                 
 */

#  ifdef __cplusplus
extern "C" {
#  endif /*             */

/*
                
 */

extern void		mxmlAdd(mxml_node_t *parent, int where,
			        mxml_node_t *child, mxml_node_t *node);
extern void		mxmlDelete(mxml_node_t *node);
extern void		mxmlElementDeleteAttr(mxml_node_t *node,
			                      const char *name);
extern const char	*mxmlElementGetAttr(mxml_node_t *node, const char *name);
extern void		mxmlElementSetAttr(mxml_node_t *node, const char *name,
			                   const char *value);
extern void		mxmlElementSetAttrf(mxml_node_t *node, const char *name,
			                    const char *format, ...)
#    ifdef __GNUC__
__attribute__ ((__format__ (__printf__, 3, 4)))
#    endif /*          */
;
extern int		mxmlEntityAddCallback(mxml_entity_cb_t cb);
extern const char	*mxmlEntityGetName(int val);
extern int		mxmlEntityGetValue(const char *name);
extern void		mxmlEntityRemoveCallback(mxml_entity_cb_t cb);
extern mxml_node_t	*mxmlFindElement(mxml_node_t *node, mxml_node_t *top,
			                 const char *name, const char *attr,
					 const char *value, int descend);
extern mxml_node_t	*mxmlFindPath(mxml_node_t *node, const char *path);
extern const char	*mxmlGetCDATA(mxml_node_t *node);
extern const void	*mxmlGetCustom(mxml_node_t *node);
extern const char	*mxmlGetElement(mxml_node_t *node);
extern mxml_node_t	*mxmlGetFirstChild(mxml_node_t *node);
extern int		mxmlGetInteger(mxml_node_t *node);
extern mxml_node_t	*mxmlGetLastChild(mxml_node_t *node);
extern mxml_node_t	*mxmlGetNextSibling(mxml_node_t *node);
extern const char	*mxmlGetOpaque(mxml_node_t *node);
extern mxml_node_t	*mxmlGetParent(mxml_node_t *node);
extern mxml_node_t	*mxmlGetPrevSibling(mxml_node_t *node);
extern double		mxmlGetReal(mxml_node_t *node);
extern int		mxmlGetRefCount(mxml_node_t *node);
extern const char	*mxmlGetText(mxml_node_t *node, int *whitespace);
extern mxml_type_t	mxmlGetType(mxml_node_t *node);
extern void		*mxmlGetUserData(mxml_node_t *node);
extern void		mxmlIndexDelete(mxml_index_t *ind);
extern mxml_node_t	*mxmlIndexEnum(mxml_index_t *ind);
extern mxml_node_t	*mxmlIndexFind(mxml_index_t *ind,
			               const char *element,
			               const char *value);
extern int		mxmlIndexGetCount(mxml_index_t *ind);
extern mxml_index_t	*mxmlIndexNew(mxml_node_t *node, const char *element,
			              const char *attr);
extern mxml_node_t	*mxmlIndexReset(mxml_index_t *ind);
extern mxml_node_t	*mxmlLoadFd(mxml_node_t *top, int fd,
			            mxml_type_t (*cb)(mxml_node_t *));
extern mxml_node_t	*mxmlLoadFile(mxml_node_t *top, FILE *fp,
			              mxml_type_t (*cb)(mxml_node_t *));
extern mxml_node_t	*mxmlLoadString(mxml_node_t *top, const char *s,
			                mxml_type_t (*cb)(mxml_node_t *));
extern mxml_node_t	*mxmlNewCDATA(mxml_node_t *parent, const char *string);
extern mxml_node_t	*mxmlNewCustom(mxml_node_t *parent, void *data,
			               mxml_custom_destroy_cb_t destroy);
extern mxml_node_t	*mxmlNewElement(mxml_node_t *parent, const char *name);
extern mxml_node_t	*mxmlNewInteger(mxml_node_t *parent, int integer);
extern mxml_node_t	*mxmlNewOpaque(mxml_node_t *parent, const char *opaque);
extern mxml_node_t	*mxmlNewReal(mxml_node_t *parent, double real);
extern mxml_node_t	*mxmlNewText(mxml_node_t *parent, int whitespace,
			             const char *string);
extern mxml_node_t	*mxmlNewTextf(mxml_node_t *parent, int whitespace,
			              const char *format, ...)
#    ifdef __GNUC__
__attribute__ ((__format__ (__printf__, 3, 4)))
#    endif /*          */
;
extern mxml_node_t	*mxmlNewXML(const char *version);
extern int		mxmlRelease(mxml_node_t *node);
extern void		mxmlRemove(mxml_node_t *node);
extern int		mxmlRetain(mxml_node_t *node);
extern char		*mxmlSaveAllocString(mxml_node_t *node,
			        	     mxml_save_cb_t cb);
extern int		mxmlSaveFd(mxml_node_t *node, int fd,
			           mxml_save_cb_t cb);
extern int		mxmlSaveFile(mxml_node_t *node, FILE *fp,
			             mxml_save_cb_t cb);
extern int		mxmlSaveString(mxml_node_t *node, char *buffer,
			               int bufsize, mxml_save_cb_t cb);
extern mxml_node_t	*mxmlSAXLoadFd(mxml_node_t *top, int fd,
			               mxml_type_t (*cb)(mxml_node_t *),
			               mxml_sax_cb_t sax, void *sax_data);
extern mxml_node_t	*mxmlSAXLoadFile(mxml_node_t *top, FILE *fp,
			                 mxml_type_t (*cb)(mxml_node_t *),
			                 mxml_sax_cb_t sax, void *sax_data);
extern mxml_node_t	*mxmlSAXLoadString(mxml_node_t *top, const char *s,
			                   mxml_type_t (*cb)(mxml_node_t *),
			                   mxml_sax_cb_t sax, void *sax_data);
extern int		mxmlSetCDATA(mxml_node_t *node, const char *data);
extern int		mxmlSetCustom(mxml_node_t *node, void *data,
			              mxml_custom_destroy_cb_t destroy);
extern void		mxmlSetCustomHandlers(mxml_custom_load_cb_t load,
			                      mxml_custom_save_cb_t save);
extern int		mxmlSetElement(mxml_node_t *node, const char *name);
extern void		mxmlSetErrorCallback(mxml_error_cb_t cb);
extern int		mxmlSetInteger(mxml_node_t *node, int integer);
extern int		mxmlSetOpaque(mxml_node_t *node, const char *opaque);
extern int		mxmlSetReal(mxml_node_t *node, double real);
extern int		mxmlSetText(mxml_node_t *node, int whitespace,
			            const char *string);
extern int		mxmlSetTextf(mxml_node_t *node, int whitespace,
			             const char *format, ...)
#    ifdef __GNUC__
__attribute__ ((__format__ (__printf__, 3, 4)))
#    endif /*          */
;
extern int		mxmlSetUserData(mxml_node_t *node, void *data);
extern void		mxmlSetWrapMargin(int column);
extern mxml_node_t	*mxmlWalkNext(mxml_node_t *node, mxml_node_t *top,
			              int descend);
extern mxml_node_t	*mxmlWalkPrev(mxml_node_t *node, mxml_node_t *top,
			              int descend);


/*
                            
 */

extern void		mxml_error(const char *format, ...);
extern mxml_type_t	mxml_ignore_cb(mxml_node_t *node);
extern mxml_type_t	mxml_integer_cb(mxml_node_t *node);
extern mxml_type_t	mxml_opaque_cb(mxml_node_t *node);
extern mxml_type_t	mxml_real_cb(mxml_node_t *node);


/*
                 
 */

#  ifdef __cplusplus
}
#  endif /*             */
#endif /*           */


/*
                                                          
 */
