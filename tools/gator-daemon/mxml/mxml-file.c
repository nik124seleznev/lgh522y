/*
 * "$Id: mxml-file.c 455 2014-01-05 03:28:03Z msweet $"
 *
 * File loading code for Mini-XML, a small XML-like file parsing library.
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

/*                                                                                                                                                                                   */

/*
                               
 */

#ifndef WIN32
#  include <unistd.h>
#endif /*        */
#include "mxml-private.h"


/*
                        
 */

#define ENCODE_UTF8	0		/*       */
#define ENCODE_UTF16BE	1		/*                   */
#define ENCODE_UTF16LE	2		/*                      */


/*
                                           
 */

#define mxml_bad_char(ch) ((ch) < ' ' && (ch) != '\n' && (ch) != '\r' && (ch) != '\t')


/*
                          
 */

typedef int (*_mxml_getc_cb_t)(void *, int *);
typedef int (*_mxml_putc_cb_t)(int, void *);

typedef struct _mxml_fdbuf_s		/*                              */
{
  int		fd;			/*                 */
  unsigned char	*current,		/*                            */
		*end,			/*               */
		buffer[8192];		/*                  */
} _mxml_fdbuf_t;


/*
                     
 */

static int		mxml_add_char(int ch, char **ptr, char **buffer,
			              int *bufsize);
static int		mxml_fd_getc(void *p, int *encoding);
static int		mxml_fd_putc(int ch, void *p);
static int		mxml_fd_read(_mxml_fdbuf_t *buf);
static int		mxml_fd_write(_mxml_fdbuf_t *buf);
static int		mxml_file_getc(void *p, int *encoding);
static int		mxml_file_putc(int ch, void *p);
static int		mxml_get_entity(mxml_node_t *parent, void *p,
			                int *encoding,
					_mxml_getc_cb_t getc_cb);
static inline int	mxml_isspace(int ch)
			{
			  return (ch == ' ' || ch == '\t' || ch == '\r' ||
			          ch == '\n');
			}
static mxml_node_t	*mxml_load_data(mxml_node_t *top, void *p,
			                mxml_load_cb_t cb,
			                _mxml_getc_cb_t getc_cb,
                                        mxml_sax_cb_t sax_cb, void *sax_data);
static int		mxml_parse_element(mxml_node_t *node, void *p,
			                   int *encoding,
					   _mxml_getc_cb_t getc_cb);
static int		mxml_string_getc(void *p, int *encoding);
static int		mxml_string_putc(int ch, void *p);
static int		mxml_write_name(const char *s, void *p,
					_mxml_putc_cb_t putc_cb);
static int		mxml_write_node(mxml_node_t *node, void *p,
			                mxml_save_cb_t cb, int col,
					_mxml_putc_cb_t putc_cb,
					_mxml_global_t *global);
static int		mxml_write_string(const char *s, void *p,
					  _mxml_putc_cb_t putc_cb);
static int		mxml_write_ws(mxml_node_t *node, void *p,
			              mxml_save_cb_t cb, int ws,
				      int col, _mxml_putc_cb_t putc_cb);


/*
                                                                 
  
                                                                       
                                                                      
                                                                   
                                                                       
                                                                       
                                   
  
                                                             
                                                                     
                                     
 */

mxml_node_t *				/*                                                       */
mxmlLoadFd(mxml_node_t    *top,		/*              */
           int            fd,		/*                                  */
           mxml_load_cb_t cb)		/*                                           */
{
  _mxml_fdbuf_t	buf;			/*                        */


 /*
                                            
  */

  buf.fd      = fd;
  buf.current = buf.buffer;
  buf.end     = buf.buffer;

 /*
                        
  */

  return (mxml_load_data(top, &buf, cb, mxml_fd_getc, MXML_NO_CALLBACK, NULL));
}


/*
                                                        
  
                                                                       
                                                                      
                                                                   
                                                                       
                                                                       
                                   
  
                                                             
                                                                     
                                     
 */

mxml_node_t *				/*                                                       */
mxmlLoadFile(mxml_node_t    *top,	/*              */
             FILE           *fp,	/*                       */
             mxml_load_cb_t cb)		/*                                           */
{
 /*
                        
  */

  return (mxml_load_data(top, fp, cb, mxml_file_getc, MXML_NO_CALLBACK, NULL));
}


/*
                                                            
  
                                                                         
                                                                        
                                                                     
                                                                       
                                                                       
                                   
  
                                                             
                                                                     
                                     
 */

mxml_node_t *				/*                                                  */
mxmlLoadString(mxml_node_t    *top,	/*              */
               const char     *s,	/*                    */
               mxml_load_cb_t cb)	/*                                           */
{
 /*
                        
  */

  return (mxml_load_data(top, (void *)&s, cb, mxml_string_getc, MXML_NO_CALLBACK,
                         NULL));
}


/*
                                                                     
  
                                                                     
                                                                   
                                                                         
                                                                       
             
  
                                                                       
                                                                    
                                                                     
                                                                    
                
 */

char *					/*                              */
mxmlSaveAllocString(
    mxml_node_t    *node,		/*                   */
    mxml_save_cb_t cb)			/*                                             */
{
  int	bytes;				/*                */
  char	buffer[8192];			/*                  */
  char	*s;				/*                  */


 /*
                                             
  */

  bytes = mxmlSaveString(node, buffer, sizeof(buffer), cb);

  if (bytes <= 0)
    return (NULL);

  if (bytes < (int)(sizeof(buffer) - 1))
  {
   /*
                                                                   
               
    */

    return (strdup(buffer));
  }

 /*
                                                                   
                 
  */

  if ((s = malloc(bytes + 1)) == NULL)
    return (NULL);

  mxmlSaveString(node, s, bytes + 1, cb);

 /*
                                  
  */

  return (s);
}


/*
                                                          
  
                                                                       
                                                                    
                                                                     
                                                                    
                
 */

int					/*                                */
mxmlSaveFd(mxml_node_t    *node,	/*                   */
           int            fd,		/*                                 */
	   mxml_save_cb_t cb)		/*                                             */
{
  int		col;			/*              */
  _mxml_fdbuf_t	buf;			/*                        */
  _mxml_global_t *global = _mxml_global();
					/*             */


 /*
                                            
  */

  buf.fd      = fd;
  buf.current = buf.buffer;
  buf.end     = buf.buffer + sizeof(buf.buffer);

 /*
                     
  */

  if ((col = mxml_write_node(node, &buf, cb, 0, mxml_fd_putc, global)) < 0)
    return (-1);

  if (col > 0)
    if (mxml_fd_putc('\n', &buf) < 0)
      return (-1);

 /*
                       
  */

  return (mxml_fd_write(&buf));
}


/*
                                                 
  
                                                                       
                                                                    
                                                                     
                                                                    
                
 */

int					/*                                */
mxmlSaveFile(mxml_node_t    *node,	/*                   */
             FILE           *fp,	/*                      */
	     mxml_save_cb_t cb)		/*                                             */
{
  int	col;				/*              */
  _mxml_global_t *global = _mxml_global();
					/*             */


 /*
                     
  */

  if ((col = mxml_write_node(node, fp, cb, 0, mxml_file_putc, global)) < 0)
    return (-1);

  if (col > 0)
    if (putc('\n', fp) < 0)
      return (-1);

 /*
                         
  */

  return (0);
}


/*
                                                          
  
                                                                
                                                                   
                             
  
                                                                       
                                                                    
                                                                     
                                                                    
                
 */

int					/*                    */
mxmlSaveString(mxml_node_t    *node,	/*                   */
               char           *buffer,	/*                   */
               int            bufsize,	/*                           */
               mxml_save_cb_t cb)	/*                                             */
{
  int	col;				/*              */
  char	*ptr[3];			/*                      */
  _mxml_global_t *global = _mxml_global();
					/*             */


 /*
                     
  */

  ptr[0] = buffer;
  ptr[1] = buffer + bufsize;
  ptr[2] = 0;

  if ((col = mxml_write_node(node, ptr, cb, 0, mxml_string_putc, global)) < 0)
    return (-1);

  if (col > 0)
    mxml_string_putc('\n', ptr);

 /*
                               
  */

  if (ptr[2] != 0)
    buffer[bufsize - 1] = '\0';
  else
    ptr[0][0] = '\0';

 /*
                                      
  */

  return (ptr[0] - buffer);
}


/*
                                                                   
                                            
  
                                                                       
                                                                      
                                                                   
                                                                       
                                                                       
                                   
  
                                                             
                                                                     
                                     
  
                                                                     
                                                                      
                                                                        
  
                       
 */

mxml_node_t *				/*                                                       */
mxmlSAXLoadFd(mxml_node_t    *top,	/*              */
              int            fd,	/*                                  */
              mxml_load_cb_t cb,	/*                                           */
              mxml_sax_cb_t  sax_cb,	/*                                      */
              void           *sax_data)	/*                   */
{
  _mxml_fdbuf_t	buf;			/*                        */


 /*
                                            
  */

  buf.fd      = fd;
  buf.current = buf.buffer;
  buf.end     = buf.buffer;

 /*
                        
  */

  return (mxml_load_data(top, &buf, cb, mxml_fd_getc, sax_cb, sax_data));
}


/*
                                                          
                                              
  
                                                                       
                                                                      
                                                                   
                                                                       
                                                                       
                                   
  
                                                             
                                                                     
                                     
  
                                                                     
                                                                      
                                                                        
  
                       
 */

mxml_node_t *				/*                                                       */
mxmlSAXLoadFile(
    mxml_node_t    *top,		/*              */
    FILE           *fp,			/*                       */
    mxml_load_cb_t cb,			/*                                           */
    mxml_sax_cb_t  sax_cb,		/*                                      */
    void           *sax_data)		/*                   */
{
 /*
                        
  */

  return (mxml_load_data(top, fp, cb, mxml_file_getc, sax_cb, sax_data));
}


/*
                                                              
                                                
  
                                                                         
                                                                        
                                                                     
                                                                       
                                                                       
                                   
  
                                                             
                                                                     
                                     
  
                                                                     
                                                                      
                                                                        
  
                       
 */

mxml_node_t *				/*                                                  */
mxmlSAXLoadString(
    mxml_node_t    *top,		/*              */
    const char     *s,			/*                    */
    mxml_load_cb_t cb,			/*                                           */
    mxml_sax_cb_t  sax_cb,		/*                                      */
    void           *sax_data)		/*                   */
{
 /*
                        
  */

  return (mxml_load_data(top, (void *)&s, cb, mxml_string_getc, sax_cb, sax_data));
}


/*
                                                                          
  
                                                                      
                                             
  
                                                                      
                                       
  
 */

void
mxmlSetCustomHandlers(
    mxml_custom_load_cb_t load,		/*                   */
    mxml_custom_save_cb_t save)		/*                   */
{
  _mxml_global_t *global = _mxml_global();
					/*             */


  global->custom_load_cb = load;
  global->custom_save_cb = save;
}


/*
                                                             
 */

void
mxmlSetErrorCallback(mxml_error_cb_t cb)/*                             */
{
  _mxml_global_t *global = _mxml_global();
					/*             */


  global->error_cb = cb;
}


/*
                                                                    
  
                                           
  
                       
 */

void
mxmlSetWrapMargin(int column)		/*                                                */
{
  _mxml_global_t *global = _mxml_global();
					/*             */


  global->wrap = column;
}


/*
                                                                        
 */

static int				/*                                */
mxml_add_char(int  ch,			/*                       */
              char **bufptr,		/*                                 */
	      char **buffer,		/*                     */
	      int  *bufsize)		/*                          */
{
  char	*newbuffer;			/*                  */


  if (*bufptr >= (*buffer + *bufsize - 4))
  {
   /*
                                        
    */

    if (*bufsize < 1024)
      (*bufsize) *= 2;
    else
      (*bufsize) += 1024;

    if ((newbuffer = realloc(*buffer, *bufsize)) == NULL)
    {
      free(*buffer);

      mxml_error("Unable to expand string buffer to %d bytes!", *bufsize);

      return (-1);
    }

    *bufptr = newbuffer + (*bufptr - *buffer);
    *buffer = newbuffer;
  }

  if (ch < 0x80)
  {
   /*
                          
    */

    *(*bufptr)++ = ch;
  }
  else if (ch < 0x800)
  {
   /*
                       
    */

    *(*bufptr)++ = 0xc0 | (ch >> 6);
    *(*bufptr)++ = 0x80 | (ch & 0x3f);
  }
  else if (ch < 0x10000)
  {
   /*
                         
    */

    *(*bufptr)++ = 0xe0 | (ch >> 12);
    *(*bufptr)++ = 0x80 | ((ch >> 6) & 0x3f);
    *(*bufptr)++ = 0x80 | (ch & 0x3f);
  }
  else
  {
   /*
                        
    */

    *(*bufptr)++ = 0xf0 | (ch >> 18);
    *(*bufptr)++ = 0x80 | ((ch >> 12) & 0x3f);
    *(*bufptr)++ = 0x80 | ((ch >> 6) & 0x3f);
    *(*bufptr)++ = 0x80 | (ch & 0x3f);
  }

  return (0);
}


/*
                                                              
 */

static int				/*                       */
mxml_fd_getc(void *p,			/*                             */
             int  *encoding)		/*               */
{
  _mxml_fdbuf_t	*buf;			/*                        */
  int		ch,			/*                   */
		temp;			/*                     */


 /*
                                            
  */

  buf = (_mxml_fdbuf_t *)p;

  if (buf->current >= buf->end)
    if (mxml_fd_read(buf) < 0)
      return (EOF);

  ch = *(buf->current)++;

  switch (*encoding)
  {
    case ENCODE_UTF8 :
       /*
                                                                
 */

	if (!(ch & 0x80))
	{
#if DEBUG > 1
          printf("mxml_fd_getc: %c (0x%04x)\n", ch < ' ' ? '.' : ch, ch);
#endif /*           */

	  if (mxml_bad_char(ch))
	  {
	    mxml_error("Bad control character 0x%02x not allowed by XML standard!",
        	       ch);
	    return (EOF);
	  }

	  return (ch);
        }
	else if (ch == 0xfe)
	{
	 /*
                           
   */

	  if (buf->current >= buf->end)
	    if (mxml_fd_read(buf) < 0)
	      return (EOF);

	  ch = *(buf->current)++;

	  if (ch != 0xff)
	    return (EOF);

	  *encoding = ENCODE_UTF16BE;

	  return (mxml_fd_getc(p, encoding));
	}
	else if (ch == 0xff)
	{
	 /*
                              
   */

	  if (buf->current >= buf->end)
	    if (mxml_fd_read(buf) < 0)
	      return (EOF);

	  ch = *(buf->current)++;

	  if (ch != 0xfe)
	    return (EOF);

	  *encoding = ENCODE_UTF16LE;

	  return (mxml_fd_getc(p, encoding));
	}
	else if ((ch & 0xe0) == 0xc0)
	{
	 /*
                      
   */

	  if (buf->current >= buf->end)
	    if (mxml_fd_read(buf) < 0)
	      return (EOF);

	  temp = *(buf->current)++;

	  if ((temp & 0xc0) != 0x80)
	    return (EOF);

	  ch = ((ch & 0x1f) << 6) | (temp & 0x3f);

	  if (ch < 0x80)
	  {
	    mxml_error("Invalid UTF-8 sequence for character 0x%04x!", ch);
	    return (EOF);
	  }
	}
	else if ((ch & 0xf0) == 0xe0)
	{
	 /*
                        
   */

	  if (buf->current >= buf->end)
	    if (mxml_fd_read(buf) < 0)
	      return (EOF);

	  temp = *(buf->current)++;

	  if ((temp & 0xc0) != 0x80)
	    return (EOF);

	  ch = ((ch & 0x0f) << 6) | (temp & 0x3f);

	  if (buf->current >= buf->end)
	    if (mxml_fd_read(buf) < 0)
	      return (EOF);

	  temp = *(buf->current)++;

	  if ((temp & 0xc0) != 0x80)
	    return (EOF);

	  ch = (ch << 6) | (temp & 0x3f);

	  if (ch < 0x800)
	  {
	    mxml_error("Invalid UTF-8 sequence for character 0x%04x!", ch);
	    return (EOF);
	  }

         /*
                                            
   */

	  if (ch == 0xfeff)
	    return (mxml_fd_getc(p, encoding));
	}
	else if ((ch & 0xf8) == 0xf0)
	{
	 /*
                       
   */

	  if (buf->current >= buf->end)
	    if (mxml_fd_read(buf) < 0)
	      return (EOF);

	  temp = *(buf->current)++;

	  if ((temp & 0xc0) != 0x80)
	    return (EOF);

	  ch = ((ch & 0x07) << 6) | (temp & 0x3f);

	  if (buf->current >= buf->end)
	    if (mxml_fd_read(buf) < 0)
	      return (EOF);

	  temp = *(buf->current)++;

	  if ((temp & 0xc0) != 0x80)
	    return (EOF);

	  ch = (ch << 6) | (temp & 0x3f);

	  if (buf->current >= buf->end)
	    if (mxml_fd_read(buf) < 0)
	      return (EOF);

	  temp = *(buf->current)++;

	  if ((temp & 0xc0) != 0x80)
	    return (EOF);

	  ch = (ch << 6) | (temp & 0x3f);

	  if (ch < 0x10000)
	  {
	    mxml_error("Invalid UTF-8 sequence for character 0x%04x!", ch);
	    return (EOF);
	  }
	}
	else
	  return (EOF);
	break;

    case ENCODE_UTF16BE :
       /*
                                        
 */

	if (buf->current >= buf->end)
	  if (mxml_fd_read(buf) < 0)
	    return (EOF);

	temp = *(buf->current)++;

	ch = (ch << 8) | temp;

	if (mxml_bad_char(ch))
	{
	  mxml_error("Bad control character 0x%02x not allowed by XML standard!",
        	     ch);
	  return (EOF);
	}
        else if (ch >= 0xd800 && ch <= 0xdbff)
	{
	 /*
                              
   */

          int lch;

	  if (buf->current >= buf->end)
	    if (mxml_fd_read(buf) < 0)
	      return (EOF);

	  lch = *(buf->current)++;

	  if (buf->current >= buf->end)
	    if (mxml_fd_read(buf) < 0)
	      return (EOF);

	  temp = *(buf->current)++;

	  lch = (lch << 8) | temp;

          if (lch < 0xdc00 || lch >= 0xdfff)
	    return (EOF);

          ch = (((ch & 0x3ff) << 10) | (lch & 0x3ff)) + 0x10000;
	}
	break;

    case ENCODE_UTF16LE :
       /*
                                           
 */

	if (buf->current >= buf->end)
	  if (mxml_fd_read(buf) < 0)
	    return (EOF);

	temp = *(buf->current)++;

	ch |= (temp << 8);

        if (mxml_bad_char(ch))
	{
	  mxml_error("Bad control character 0x%02x not allowed by XML standard!",
        	     ch);
	  return (EOF);
	}
        else if (ch >= 0xd800 && ch <= 0xdbff)
	{
	 /*
                              
   */

          int lch;

	  if (buf->current >= buf->end)
	    if (mxml_fd_read(buf) < 0)
	      return (EOF);

	  lch = *(buf->current)++;

	  if (buf->current >= buf->end)
	    if (mxml_fd_read(buf) < 0)
	      return (EOF);

	  temp = *(buf->current)++;

	  lch |= (temp << 8);

          if (lch < 0xdc00 || lch >= 0xdfff)
	    return (EOF);

          ch = (((ch & 0x3ff) << 10) | (lch & 0x3ff)) + 0x10000;
	}
	break;
  }

#if DEBUG > 1
  printf("mxml_fd_getc: %c (0x%04x)\n", ch < ' ' ? '.' : ch, ch);
#endif /*           */

  return (ch);
}


/*
                                                             
 */

static int				/*                               */
mxml_fd_putc(int  ch,			/*               */
             void *p)			/*                            */
{
  _mxml_fdbuf_t	*buf;			/*                        */


 /*
                                       
  */

  buf = (_mxml_fdbuf_t *)p;

  if (buf->current >= buf->end)
    if (mxml_fd_write(buf) < 0)
      return (-1);

  *(buf->current)++ = ch;

 /*
                          
  */

  return (0);
}


/*
                                                                   
 */

static int				/*                               */
mxml_fd_read(_mxml_fdbuf_t *buf)		/*                            */
{
  int	bytes;				/*               */


 /*
                        
  */

  if (!buf)
    return (-1);

 /*
                                    
  */

  while ((bytes = read(buf->fd, buf->buffer, sizeof(buf->buffer))) < 0)
#ifdef EINTR
    if (errno != EAGAIN && errno != EINTR)
#else
    if (errno != EAGAIN)
#endif /*       */
      return (-1);

  if (bytes == 0)
    return (-1);

 /*
                                             
  */

  buf->current = buf->buffer;
  buf->end     = buf->buffer + bytes;

  return (0);
}


/*
                                                                   
 */

static int				/*                               */
mxml_fd_write(_mxml_fdbuf_t *buf)	/*                            */
{
  int		bytes;			/*               */
  unsigned char	*ptr;			/*                     */


 /*
                  
  */

  if (!buf)
    return (-1);

 /*
                                            
  */

  if (buf->current == buf->buffer)
    return (0);

 /*
                                            
  */

  for (ptr = buf->buffer; ptr < buf->current; ptr += bytes)
    if ((bytes = write(buf->fd, ptr, buf->current - ptr)) < 0)
      return (-1);

 /*
                                                  
  */

  buf->current = buf->buffer;

  return (0);
}


/*
                                                    
 */

static int				/*                       */
mxml_file_getc(void *p,			/*                      */
               int  *encoding)		/*               */
{
  int	ch,				/*                     */
	temp;				/*                     */
  FILE	*fp;				/*                 */


 /*
                                                                   
  */

  fp = (FILE *)p;
  ch = getc(fp);

  if (ch == EOF)
    return (EOF);

  switch (*encoding)
  {
    case ENCODE_UTF8 :
       /*
                                                                
 */

	if (!(ch & 0x80))
	{
	  if (mxml_bad_char(ch))
	  {
	    mxml_error("Bad control character 0x%02x not allowed by XML standard!",
        	       ch);
	    return (EOF);
	  }

#if DEBUG > 1
          printf("mxml_file_getc: %c (0x%04x)\n", ch < ' ' ? '.' : ch, ch);
#endif /*           */

	  return (ch);
        }
	else if (ch == 0xfe)
	{
	 /*
                           
   */

          ch = getc(fp);
	  if (ch != 0xff)
	    return (EOF);

	  *encoding = ENCODE_UTF16BE;

	  return (mxml_file_getc(p, encoding));
	}
	else if (ch == 0xff)
	{
	 /*
                              
   */

          ch = getc(fp);
	  if (ch != 0xfe)
	    return (EOF);

	  *encoding = ENCODE_UTF16LE;

	  return (mxml_file_getc(p, encoding));
	}
	else if ((ch & 0xe0) == 0xc0)
	{
	 /*
                      
   */

	  if ((temp = getc(fp)) == EOF || (temp & 0xc0) != 0x80)
	    return (EOF);

	  ch = ((ch & 0x1f) << 6) | (temp & 0x3f);

	  if (ch < 0x80)
	  {
	    mxml_error("Invalid UTF-8 sequence for character 0x%04x!", ch);
	    return (EOF);
	  }
	}
	else if ((ch & 0xf0) == 0xe0)
	{
	 /*
                        
   */

	  if ((temp = getc(fp)) == EOF || (temp & 0xc0) != 0x80)
	    return (EOF);

	  ch = ((ch & 0x0f) << 6) | (temp & 0x3f);

	  if ((temp = getc(fp)) == EOF || (temp & 0xc0) != 0x80)
	    return (EOF);

	  ch = (ch << 6) | (temp & 0x3f);

	  if (ch < 0x800)
	  {
	    mxml_error("Invalid UTF-8 sequence for character 0x%04x!", ch);
	    return (EOF);
	  }

         /*
                                            
   */

	  if (ch == 0xfeff)
	    return (mxml_file_getc(p, encoding));
	}
	else if ((ch & 0xf8) == 0xf0)
	{
	 /*
                       
   */

	  if ((temp = getc(fp)) == EOF || (temp & 0xc0) != 0x80)
	    return (EOF);

	  ch = ((ch & 0x07) << 6) | (temp & 0x3f);

	  if ((temp = getc(fp)) == EOF || (temp & 0xc0) != 0x80)
	    return (EOF);

	  ch = (ch << 6) | (temp & 0x3f);

	  if ((temp = getc(fp)) == EOF || (temp & 0xc0) != 0x80)
	    return (EOF);

	  ch = (ch << 6) | (temp & 0x3f);

	  if (ch < 0x10000)
	  {
	    mxml_error("Invalid UTF-8 sequence for character 0x%04x!", ch);
	    return (EOF);
	  }
	}
	else
	  return (EOF);
	break;

    case ENCODE_UTF16BE :
       /*
                                        
 */

	ch = (ch << 8) | getc(fp);

	if (mxml_bad_char(ch))
	{
	  mxml_error("Bad control character 0x%02x not allowed by XML standard!",
        	     ch);
	  return (EOF);
	}
        else if (ch >= 0xd800 && ch <= 0xdbff)
	{
	 /*
                              
   */

          int lch = getc(fp);
          lch = (lch << 8) | getc(fp);

          if (lch < 0xdc00 || lch >= 0xdfff)
	    return (EOF);

          ch = (((ch & 0x3ff) << 10) | (lch & 0x3ff)) + 0x10000;
	}
	break;

    case ENCODE_UTF16LE :
       /*
                                           
 */

	ch |= (getc(fp) << 8);

        if (mxml_bad_char(ch))
	{
	  mxml_error("Bad control character 0x%02x not allowed by XML standard!",
        	     ch);
	  return (EOF);
	}
        else if (ch >= 0xd800 && ch <= 0xdbff)
	{
	 /*
                              
   */

          int lch = getc(fp);
          lch |= (getc(fp) << 8);

          if (lch < 0xdc00 || lch >= 0xdfff)
	    return (EOF);

          ch = (((ch & 0x3ff) << 10) | (lch & 0x3ff)) + 0x10000;
	}
	break;
  }

#if DEBUG > 1
  printf("mxml_file_getc: %c (0x%04x)\n", ch < ' ' ? '.' : ch, ch);
#endif /*           */

  return (ch);
}


/*
                                                    
 */

static int				/*                                 */
mxml_file_putc(int  ch,			/*                        */
               void *p)			/*                     */
{
  return (putc(ch, (FILE *)p) == EOF ? -1 : 0);
}


/*
                                                                        
 */

static int				/*                                      */
mxml_get_entity(mxml_node_t *parent,	/*                  */
		void        *p,		/*                        */
		int         *encoding,	/*                         */
                int         (*getc_cb)(void *, int *))
					/*                             */
{
  int	ch;				/*                   */
  char	entity[64],			/*               */
	*entptr;			/*                     */


  entptr = entity;

  while ((ch = (*getc_cb)(p, encoding)) != EOF)
    if (ch > 126 || (!isalnum(ch) && ch != '#'))
      break;
    else if (entptr < (entity + sizeof(entity) - 1))
      *entptr++ = ch;
    else
    {
      mxml_error("Entity name too long under parent <%s>!",
	         parent ? parent->value.element.name : "null");
      break;
    }

  *entptr = '\0';

  if (ch != ';')
  {
    mxml_error("Character entity \"%s\" not terminated under parent <%s>!",
	       entity, parent ? parent->value.element.name : "null");
    return (EOF);
  }

  if (entity[0] == '#')
  {
    if (entity[1] == 'x')
      ch = strtol(entity + 2, NULL, 16);
    else
      ch = strtol(entity + 1, NULL, 10);
  }
  else if ((ch = mxmlEntityGetValue(entity)) < 0)
    mxml_error("Entity name \"%s;\" not supported under parent <%s>!",
	       entity, parent ? parent->value.element.name : "null");

  if (mxml_bad_char(ch))
  {
    mxml_error("Bad control character 0x%02x under parent <%s> not allowed by XML standard!",
               ch, parent ? parent->value.element.name : "null");
    return (EOF);
  }

  return (ch);
}


/*
                                                        
 */

static mxml_node_t *			/*                                                       */
mxml_load_data(
    mxml_node_t     *top,		/*              */
    void            *p,			/*                     */
    mxml_load_cb_t  cb,			/*                                           */
    _mxml_getc_cb_t getc_cb,		/*                   */
    mxml_sax_cb_t   sax_cb,		/*                                      */
    void            *sax_data)		/*                   */
{
  mxml_node_t	*node,			/*              */
		*first,			/*                  */
		*parent;		/*                     */
  int		ch,			/*                     */
		whitespace;		/*                             */
  char		*buffer,		/*               */
		*bufptr;		/*                     */
  int		bufsize;		/*                */
  mxml_type_t	type;			/*                   */
  int		encoding;		/*                    */
  _mxml_global_t *global = _mxml_global();
					/*             */
  static const char * const types[] =	/*                 */
		{
		  "MXML_ELEMENT",	/*                             */
		  "MXML_INTEGER",	/*               */
		  "MXML_OPAQUE",	/*               */
		  "MXML_REAL",		/*            */
		  "MXML_TEXT",		/*               */
		  "MXML_CUSTOM"		/*             */
		};


 /*
                                                  
  */

  if ((buffer = malloc(64)) == NULL)
  {
    mxml_error("Unable to allocate string buffer!");
    return (NULL);
  }

  bufsize    = 64;
  bufptr     = buffer;
  parent     = top;
  first      = NULL;
  whitespace = 0;
  encoding   = ENCODE_UTF8;

  if (cb && parent)
    type = (*cb)(parent);
  else if (parent)
    type = MXML_TEXT;
  else
    type = MXML_IGNORE;

  while ((ch = (*getc_cb)(p, &encoding)) != EOF)
  {
    if ((ch == '<' ||
         (mxml_isspace(ch) && type != MXML_OPAQUE && type != MXML_CUSTOM)) &&
        bufptr > buffer)
    {
     /*
                               
      */

      *bufptr = '\0';

      switch (type)
      {
	case MXML_INTEGER :
            node = mxmlNewInteger(parent, strtol(buffer, &bufptr, 0));
	    break;

	case MXML_OPAQUE :
            node = mxmlNewOpaque(parent, buffer);
	    break;

	case MXML_REAL :
            node = mxmlNewReal(parent, strtod(buffer, &bufptr));
	    break;

	case MXML_TEXT :
            node = mxmlNewText(parent, whitespace, buffer);
	    break;

	case MXML_CUSTOM :
	    if (global->custom_load_cb)
	    {
	     /*
                                                       
       */

              node = mxmlNewCustom(parent, NULL, NULL);

	      if ((*global->custom_load_cb)(node, buffer))
	      {
	        mxml_error("Bad custom value '%s' in parent <%s>!",
		           buffer, parent ? parent->value.element.name : "null");
		mxmlDelete(node);
		node = NULL;
	      }
	      break;
	    }

        default : /*           */
	    node = NULL;
	    break;
      }

      if (*bufptr)
      {
       /*
                                          
 */

        mxml_error("Bad %s value '%s' in parent <%s>!",
	           type == MXML_INTEGER ? "integer" : "real", buffer,
		   parent ? parent->value.element.name : "null");
	break;
      }

      bufptr     = buffer;
      whitespace = mxml_isspace(ch) && type == MXML_TEXT;

      if (!node && type != MXML_IGNORE)
      {
       /*
                            
 */

	mxml_error("Unable to add value node of type %s to parent <%s>!",
	           types[type], parent ? parent->value.element.name : "null");
	goto error;
      }

      if (sax_cb)
      {
        (*sax_cb)(node, MXML_SAX_DATA, sax_data);

        if (!mxmlRelease(node))
          node = NULL;
      }

      if (!first && node)
        first = node;
    }
    else if (mxml_isspace(ch) && type == MXML_TEXT)
      whitespace = 1;

   /*
                                                                 
                   
    */

    if (ch == '<' && whitespace && type == MXML_TEXT)
    {
      if (parent)
      {
	node = mxmlNewText(parent, whitespace, "");

	if (sax_cb)
	{
	  (*sax_cb)(node, MXML_SAX_DATA, sax_data);

	  if (!mxmlRelease(node))
	    node = NULL;
	}

	if (!first && node)
	  first = node;
      }

      whitespace = 0;
    }

    if (ch == '<')
    {
     /*
                                  
      */

      bufptr = buffer;

      while ((ch = (*getc_cb)(p, &encoding)) != EOF)
        if (mxml_isspace(ch) || ch == '>' || (ch == '/' && bufptr > buffer))
	  break;
	else if (ch == '<')
	{
	  mxml_error("Bare < in element!");
	  goto error;
	}
	else if (ch == '&')
	{
	  if ((ch = mxml_get_entity(parent, p, &encoding, getc_cb)) == EOF)
	    goto error;

	  if (mxml_add_char(ch, &bufptr, &buffer, &bufsize))
	    goto error;
	}
	else if (mxml_add_char(ch, &bufptr, &buffer, &bufsize))
	  goto error;
	else if (((bufptr - buffer) == 1 && buffer[0] == '?') ||
	         ((bufptr - buffer) == 3 && !strncmp(buffer, "!--", 3)) ||
	         ((bufptr - buffer) == 8 && !strncmp(buffer, "![CDATA[", 8)))
	  break;

      *bufptr = '\0';

      if (!strcmp(buffer, "!--"))
      {
       /*
                                   
 */

	while ((ch = (*getc_cb)(p, &encoding)) != EOF)
	{
	  if (ch == '>' && bufptr > (buffer + 4) &&
	      bufptr[-3] != '-' && bufptr[-2] == '-' && bufptr[-1] == '-')
	    break;
	  else if (mxml_add_char(ch, &bufptr, &buffer, &bufsize))
	    goto error;
	}

       /*
                                                         
 */

        if (ch != '>')
	{
	 /*
                              
   */

	  mxml_error("Early EOF in comment node!");
	  goto error;
	}


       /*
                                                                      
 */

	*bufptr = '\0';

        if (!parent && first)
	{
	 /*
                                        
   */

	  mxml_error("<%s> cannot be a second root node after <%s>",
	             buffer, first->value.element.name);
          goto error;
	}

	if ((node = mxmlNewElement(parent, buffer)) == NULL)
	{
	 /*
                                
   */

	  mxml_error("Unable to add comment node to parent <%s>!",
	             parent ? parent->value.element.name : "null");
	  break;
	}

        if (sax_cb)
        {
          (*sax_cb)(node, MXML_SAX_COMMENT, sax_data);

          if (!mxmlRelease(node))
            node = NULL;
        }

	if (node && !first)
	  first = node;
      }
      else if (!strcmp(buffer, "![CDATA["))
      {
       /*
                                 
 */

	while ((ch = (*getc_cb)(p, &encoding)) != EOF)
	{
	  if (ch == '>' && !strncmp(bufptr - 2, "]]", 2))
	    break;
	  else if (mxml_add_char(ch, &bufptr, &buffer, &bufsize))
	    goto error;
	}

       /*
                                                         
 */

        if (ch != '>')
	{
	 /*
                              
   */

	  mxml_error("Early EOF in CDATA node!");
	  goto error;
	}


       /*
                                                                      
 */

	*bufptr = '\0';

        if (!parent && first)
	{
	 /*
                                        
   */

	  mxml_error("<%s> cannot be a second root node after <%s>",
	             buffer, first->value.element.name);
          goto error;
	}

	if ((node = mxmlNewElement(parent, buffer)) == NULL)
	{
	 /*
                              
   */

	  mxml_error("Unable to add CDATA node to parent <%s>!",
	             parent ? parent->value.element.name : "null");
	  goto error;
	}

        if (sax_cb)
        {
          (*sax_cb)(node, MXML_SAX_CDATA, sax_data);

          if (!mxmlRelease(node))
            node = NULL;
        }

	if (node && !first)
	  first = node;
      }
      else if (buffer[0] == '?')
      {
       /*
                                                  
 */

	while ((ch = (*getc_cb)(p, &encoding)) != EOF)
	{
	  if (ch == '>' && bufptr > buffer && bufptr[-1] == '?')
	    break;
	  else if (mxml_add_char(ch, &bufptr, &buffer, &bufsize))
	    goto error;
	}

       /*
                                                                        
 */

        if (ch != '>')
	{
	 /*
                              
   */

	  mxml_error("Early EOF in processing instruction node!");
	  goto error;
	}

       /*
                                                                      
 */

	*bufptr = '\0';

        if (!parent && first)
	{
	 /*
                                        
   */

	  mxml_error("<%s> cannot be a second root node after <%s>",
	             buffer, first->value.element.name);
          goto error;
	}

	if ((node = mxmlNewElement(parent, buffer)) == NULL)
	{
	 /*
                              
   */

	  mxml_error("Unable to add processing instruction node to parent <%s>!",
	             parent ? parent->value.element.name : "null");
	  goto error;
	}

        if (sax_cb)
        {
          (*sax_cb)(node, MXML_SAX_DIRECTIVE, sax_data);

          if (!mxmlRelease(node))
            node = NULL;
        }

        if (node)
	{
	  if (!first)
            first = node;

	  if (!parent)
	  {
	    parent = node;

	    if (cb)
	      type = (*cb)(parent);
	  }
	}
      }
      else if (buffer[0] == '!')
      {
       /*
                                       
 */

	do
	{
	  if (ch == '>')
	    break;
	  else
	  {
            if (ch == '&')
	      if ((ch = mxml_get_entity(parent, p, &encoding, getc_cb)) == EOF)
		goto error;

	    if (mxml_add_char(ch, &bufptr, &buffer, &bufsize))
	      goto error;
	  }
	}
        while ((ch = (*getc_cb)(p, &encoding)) != EOF);

       /*
                                                             
 */

        if (ch != '>')
	{
	 /*
                              
   */

	  mxml_error("Early EOF in declaration node!");
	  goto error;
	}

       /*
                                                                      
 */

	*bufptr = '\0';

        if (!parent && first)
	{
	 /*
                                        
   */

	  mxml_error("<%s> cannot be a second root node after <%s>",
	             buffer, first->value.element.name);
          goto error;
	}

	if ((node = mxmlNewElement(parent, buffer)) == NULL)
	{
	 /*
                              
   */

	  mxml_error("Unable to add declaration node to parent <%s>!",
	             parent ? parent->value.element.name : "null");
	  goto error;
	}

        if (sax_cb)
        {
          (*sax_cb)(node, MXML_SAX_DIRECTIVE, sax_data);

          if (!mxmlRelease(node))
            node = NULL;
        }

        if (node)
	{
	  if (!first)
            first = node;

	  if (!parent)
	  {
	    parent = node;

	    if (cb)
	      type = (*cb)(parent);
	  }
	}
      }
      else if (buffer[0] == '/')
      {
       /*
                             
 */

        if (!parent || strcmp(buffer + 1, parent->value.element.name))
	{
	 /*
                                                            
   */

	  mxml_error("Mismatched close tag <%s> under parent <%s>!",
	             buffer, parent ? parent->value.element.name : "(null)");
          goto error;
	}

       /*
                                        
 */

        while (ch != '>' && ch != EOF)
	  ch = (*getc_cb)(p, &encoding);

        node   = parent;
        parent = parent->parent;

        if (sax_cb)
        {
          (*sax_cb)(node, MXML_SAX_ELEMENT_CLOSE, sax_data);

          if (!mxmlRelease(node) && first == node)
	    first = NULL;
        }

       /*
                                                             
 */

	if (cb && parent)
	  type = (*cb)(parent);
      }
      else
      {
       /*
                            
 */

        if (!parent && first)
	{
	 /*
                                        
   */

	  mxml_error("<%s> cannot be a second root node after <%s>",
	             buffer, first->value.element.name);
          goto error;
	}

        if ((node = mxmlNewElement(parent, buffer)) == NULL)
	{
	 /*
                                
   */

	  mxml_error("Unable to add element node to parent <%s>!",
	             parent ? parent->value.element.name : "null");
	  goto error;
	}

        if (mxml_isspace(ch))
        {
	  if ((ch = mxml_parse_element(node, p, &encoding, getc_cb)) == EOF)
	    goto error;
        }
        else if (ch == '/')
	{
	  if ((ch = (*getc_cb)(p, &encoding)) != '>')
	  {
	    mxml_error("Expected > but got '%c' instead for element <%s/>!",
	               ch, buffer);
            mxmlDelete(node);
            goto error;
	  }

	  ch = '/';
	}

        if (sax_cb)
          (*sax_cb)(node, MXML_SAX_ELEMENT_OPEN, sax_data);

        if (!first)
	  first = node;

	if (ch == EOF)
	  break;

        if (ch != '/')
	{
	 /*
                                                                
   */

	  parent = node;

	  if (cb && parent)
	    type = (*cb)(parent);
	}
        else if (sax_cb)
        {
          (*sax_cb)(node, MXML_SAX_ELEMENT_CLOSE, sax_data);

          if (!mxmlRelease(node) && first == node)
            first = NULL;
        }
      }

      bufptr  = buffer;
    }
    else if (ch == '&')
    {
     /*
                                                 
      */

      if ((ch = mxml_get_entity(parent, p, &encoding, getc_cb)) == EOF)
	goto error;

      if (mxml_add_char(ch, &bufptr, &buffer, &bufsize))
	goto error;
    }
    else if (type == MXML_OPAQUE || type == MXML_CUSTOM || !mxml_isspace(ch))
    {
     /*
                                          
      */

      if (mxml_add_char(ch, &bufptr, &buffer, &bufsize))
	goto error;
    }
  }

 /*
                                                        
  */

  free(buffer);

 /*
                                         
  */

  if (parent)
  {
    node = parent;

    while (parent != top && parent->parent)
      parent = parent->parent;

    if (node != parent)
    {
      mxml_error("Missing close tag </%s> under parent <%s>!",
	         node->value.element.name,
		 node->parent ? node->parent->value.element.name : "(null)");

      mxmlDelete(first);

      return (NULL);
    }
  }

  if (parent)
    return (parent);
  else
    return (first);

 /*
                          
  */

error:

  mxmlDelete(first);

  free(buffer);

  return (NULL);
}


/*
                                                                  
 */

static int				/*                            */
mxml_parse_element(
    mxml_node_t     *node,		/*                   */
    void            *p,			/*                        */
    int             *encoding,		/*               */
    _mxml_getc_cb_t getc_cb)		/*                    */
{
  int	ch,				/*                           */
	quote;				/*                   */
  char	*name,				/*                */
	*value,				/*                 */
	*ptr;				/*                         */
  int	namesize,			/*                     */
	valsize;			/*                      */


 /*
                                            
  */

  if ((name = malloc(64)) == NULL)
  {
    mxml_error("Unable to allocate memory for name!");
    return (EOF);
  }

  namesize = 64;

  if ((value = malloc(64)) == NULL)
  {
    free(name);
    mxml_error("Unable to allocate memory for value!");
    return (EOF);
  }

  valsize = 64;

 /*
                                          
  */

  while ((ch = (*getc_cb)(p, encoding)) != EOF)
  {
#if DEBUG > 1
    fprintf(stderr, "parse_element: ch='%c'\n", ch);
#endif /*           */

   /*
                                
    */

    if (mxml_isspace(ch))
      continue;

   /*
                           
    */

    if (ch == '/' || ch == '?')
    {
     /*
                                                                    
      */

      quote = (*getc_cb)(p, encoding);

      if (quote != '>')
      {
        mxml_error("Expected '>' after '%c' for element %s, but got '%c'!",
	           ch, node->value.element.name, quote);
        goto error;
      }

      break;
    }
    else if (ch == '<')
    {
      mxml_error("Bare < in element %s!", node->value.element.name);
      goto error;
    }
    else if (ch == '>')
      break;

   /*
                                
    */

    name[0] = ch;
    ptr     = name + 1;

    if (ch == '\"' || ch == '\'')
    {
     /*
                                                    
      */

      quote = ch;

      while ((ch = (*getc_cb)(p, encoding)) != EOF)
      {
        if (ch == '&')
	  if ((ch = mxml_get_entity(node, p, encoding, getc_cb)) == EOF)
	    goto error;

	if (mxml_add_char(ch, &ptr, &name, &namesize))
	  goto error;

	if (ch == quote)
          break;
      }
    }
    else
    {
     /*
                                          
      */

      while ((ch = (*getc_cb)(p, encoding)) != EOF)
	if (mxml_isspace(ch) || ch == '=' || ch == '/' || ch == '>' ||
	    ch == '?')
          break;
	else
	{
          if (ch == '&')
	    if ((ch = mxml_get_entity(node, p, encoding, getc_cb)) == EOF)
	      goto error;

	  if (mxml_add_char(ch, &ptr, &name, &namesize))
	    goto error;
	}
    }

    *ptr = '\0';

    if (mxmlElementGetAttr(node, name))
      goto error;

    while (ch != EOF && mxml_isspace(ch))
      ch = (*getc_cb)(p, encoding);

    if (ch == '=')
    {
     /*
                                   
      */

      while ((ch = (*getc_cb)(p, encoding)) != EOF && mxml_isspace(ch));

      if (ch == EOF)
      {
        mxml_error("Missing value for attribute '%s' in element %s!",
	           name, node->value.element.name);
        goto error;
      }

      if (ch == '\'' || ch == '\"')
      {
       /*
                              
 */

        quote = ch;
	ptr   = value;

        while ((ch = (*getc_cb)(p, encoding)) != EOF)
	  if (ch == quote)
	    break;
	  else
	  {
	    if (ch == '&')
	      if ((ch = mxml_get_entity(node, p, encoding, getc_cb)) == EOF)
	        goto error;

	    if (mxml_add_char(ch, &ptr, &value, &valsize))
	      goto error;
	  }

        *ptr = '\0';
      }
      else
      {
       /*
                                
 */

	value[0] = ch;
	ptr      = value + 1;

	while ((ch = (*getc_cb)(p, encoding)) != EOF)
	  if (mxml_isspace(ch) || ch == '=' || ch == '/' || ch == '>')
            break;
	  else
	  {
	    if (ch == '&')
	      if ((ch = mxml_get_entity(node, p, encoding, getc_cb)) == EOF)
	        goto error;

	    if (mxml_add_char(ch, &ptr, &value, &valsize))
	      goto error;
	  }

        *ptr = '\0';
      }

     /*
                                                        
      */

      mxmlElementSetAttr(node, name, value);
    }
    else
    {
      mxml_error("Missing value for attribute '%s' in element %s!",
	         name, node->value.element.name);
      goto error;
    }

   /*
                                
    */

    if (ch == '/' || ch == '?')
    {
     /*
                                                                    
      */

      quote = (*getc_cb)(p, encoding);

      if (quote != '>')
      {
        mxml_error("Expected '>' after '%c' for element %s, but got '%c'!",
	           ch, node->value.element.name, quote);
        ch = EOF;
      }

      break;
    }
    else if (ch == '>')
      break;
  }

 /*
                                                 
  */

  free(name);
  free(value);

  return (ch);

 /*
                                
  */

error:

  free(name);
  free(value);

  return (EOF);
}


/*
                                                        
 */

static int				/*                       */
mxml_string_getc(void *p,		/*                      */
                 int  *encoding)	/*               */
{
  int		ch;			/*           */
  const char	**s;			/*                           */


  s = (const char **)p;

  if ((ch = (*s)[0] & 255) != 0 || *encoding == ENCODE_UTF16LE)
  {
   /*
                                                           
    */

    (*s)++;

    switch (*encoding)
    {
      case ENCODE_UTF8 :
	  if (!(ch & 0x80))
	  {
#if DEBUG > 1
            printf("mxml_string_getc: %c (0x%04x)\n", ch < ' ' ? '.' : ch, ch);
#endif /*           */

	    if (mxml_bad_char(ch))
	    {
	      mxml_error("Bad control character 0x%02x not allowed by XML standard!",
        		 ch);
	      return (EOF);
	    }

	    return (ch);
          }
	  else if (ch == 0xfe)
	  {
	   /*
                             
     */

            if (((*s)[0] & 255) != 0xff)
	      return (EOF);

	    *encoding = ENCODE_UTF16BE;
	    (*s)++;

	    return (mxml_string_getc(p, encoding));
	  }
	  else if (ch == 0xff)
	  {
	   /*
                                
     */

            if (((*s)[0] & 255) != 0xfe)
	      return (EOF);

	    *encoding = ENCODE_UTF16LE;
	    (*s)++;

	    return (mxml_string_getc(p, encoding));
	  }
	  else if ((ch & 0xe0) == 0xc0)
	  {
	   /*
                        
     */

	    if (((*s)[0] & 0xc0) != 0x80)
              return (EOF);

	    ch = ((ch & 0x1f) << 6) | ((*s)[0] & 0x3f);

	    (*s)++;

	    if (ch < 0x80)
	    {
	      mxml_error("Invalid UTF-8 sequence for character 0x%04x!", ch);
	      return (EOF);
	    }

#if DEBUG > 1
            printf("mxml_string_getc: %c (0x%04x)\n", ch < ' ' ? '.' : ch, ch);
#endif /*           */

	    return (ch);
	  }
	  else if ((ch & 0xf0) == 0xe0)
	  {
	   /*
                          
     */

	    if (((*s)[0] & 0xc0) != 0x80 ||
        	((*s)[1] & 0xc0) != 0x80)
              return (EOF);

	    ch = ((((ch & 0x0f) << 6) | ((*s)[0] & 0x3f)) << 6) | ((*s)[1] & 0x3f);

	    (*s) += 2;

	    if (ch < 0x800)
	    {
	      mxml_error("Invalid UTF-8 sequence for character 0x%04x!", ch);
	      return (EOF);
	    }

	   /*
                                              
     */

	    if (ch == 0xfeff)
	      return (mxml_string_getc(p, encoding));

#if DEBUG > 1
            printf("mxml_string_getc: %c (0x%04x)\n", ch < ' ' ? '.' : ch, ch);
#endif /*           */

	    return (ch);
	  }
	  else if ((ch & 0xf8) == 0xf0)
	  {
	   /*
                         
     */

	    if (((*s)[0] & 0xc0) != 0x80 ||
        	((*s)[1] & 0xc0) != 0x80 ||
        	((*s)[2] & 0xc0) != 0x80)
              return (EOF);

	    ch = ((((((ch & 0x07) << 6) | ((*s)[0] & 0x3f)) << 6) |
        	   ((*s)[1] & 0x3f)) << 6) | ((*s)[2] & 0x3f);

	    (*s) += 3;

	    if (ch < 0x10000)
	    {
	      mxml_error("Invalid UTF-8 sequence for character 0x%04x!", ch);
	      return (EOF);
	    }

#if DEBUG > 1
            printf("mxml_string_getc: %c (0x%04x)\n", ch < ' ' ? '.' : ch, ch);
#endif /*           */

	    return (ch);
	  }
	  else
	    return (EOF);

      case ENCODE_UTF16BE :
	 /*
                                          
   */

	  ch = (ch << 8) | ((*s)[0] & 255);
	  (*s) ++;

          if (mxml_bad_char(ch))
	  {
	    mxml_error("Bad control character 0x%02x not allowed by XML standard!",
        	       ch);
	    return (EOF);
	  }
          else if (ch >= 0xd800 && ch <= 0xdbff)
	  {
	   /*
                                
     */

            int lch;			/*            */


            if (!(*s)[0])
	      return (EOF);

            lch = (((*s)[0] & 255) << 8) | ((*s)[1] & 255);
	    (*s) += 2;

            if (lch < 0xdc00 || lch >= 0xdfff)
	      return (EOF);

            ch = (((ch & 0x3ff) << 10) | (lch & 0x3ff)) + 0x10000;
	  }

#if DEBUG > 1
          printf("mxml_string_getc: %c (0x%04x)\n", ch < ' ' ? '.' : ch, ch);
#endif /*           */

	  return (ch);

      case ENCODE_UTF16LE :
	 /*
                                             
   */

	  ch = ch | (((*s)[0] & 255) << 8);

	  if (!ch)
	  {
	    (*s) --;
	    return (EOF);
	  }

	  (*s) ++;

          if (mxml_bad_char(ch))
	  {
	    mxml_error("Bad control character 0x%02x not allowed by XML standard!",
        	       ch);
	    return (EOF);
	  }
          else if (ch >= 0xd800 && ch <= 0xdbff)
	  {
	   /*
                                
     */

            int lch;			/*            */


            if (!(*s)[1])
	      return (EOF);

            lch = (((*s)[1] & 255) << 8) | ((*s)[0] & 255);
	    (*s) += 2;

            if (lch < 0xdc00 || lch >= 0xdfff)
	      return (EOF);

            ch = (((ch & 0x3ff) << 10) | (lch & 0x3ff)) + 0x10000;
	  }

#if DEBUG > 1
          printf("mxml_string_getc: %c (0x%04x)\n", ch < ' ' ? '.' : ch, ch);
#endif /*           */

	  return (ch);
    }
  }

  return (EOF);
}


/*
                                                        
 */

static int				/*                                 */
mxml_string_putc(int  ch,		/*                        */
                 void *p)		/*                                */
{
  char	**pp;				/*                            */


  pp = (char **)p;

  if (pp[2] == 0) {
    if (pp[0] < pp[1])
      pp[0][0] = ch;
    else
      pp[2] = (char *)1;
  }

  pp[0] ++;

  return (0);
}


/*
                                             
 */

static int				/*                                 */
mxml_write_name(const char *s,		/*                   */
                void       *p,		/*                   */
		int        (*putc_cb)(int, void *))
					/*                    */
{
  char		quote;			/*                 */
  const char	*name;			/*             */


  if (*s == '\"' || *s == '\'')
  {
   /*
                                   
    */

    if ((*putc_cb)(*s, p) < 0)
      return (-1);

    quote = *s++;

    while (*s && *s != quote)
    {
      if ((name = mxmlEntityGetName(*s)) != NULL)
      {
	if ((*putc_cb)('&', p) < 0)
          return (-1);

        while (*name)
	{
	  if ((*putc_cb)(*name, p) < 0)
            return (-1);

          name ++;
	}

	if ((*putc_cb)(';', p) < 0)
          return (-1);
      }
      else if ((*putc_cb)(*s, p) < 0)
	return (-1);

      s ++;
    }

   /*
                            
    */

    if ((*putc_cb)(quote, p) < 0)
      return (-1);
  }
  else
  {
   /*
                                       
    */

    while (*s)
    {
      if ((*putc_cb)(*s, p) < 0)
	return (-1);

      s ++;
    }
  }

  return (0);
}


/*
                                                    
 */

static int				/*                           */
mxml_write_node(mxml_node_t     *node,	/*                   */
                void            *p,	/*                      */
	        mxml_save_cb_t  cb,	/*                         */
		int             col,	/*                    */
		_mxml_putc_cb_t putc_cb,/*                     */
		_mxml_global_t  *global)/*                 */
{
  int		i,			/*             */
		width;			/*                       */
  mxml_attr_t	*attr;			/*                   */
  char		s[255];			/*                  */


 /*
                           
  */

  switch (node->type)
  {
    case MXML_ELEMENT :
	col = mxml_write_ws(node, p, cb, MXML_WS_BEFORE_OPEN, col, putc_cb);

	if ((*putc_cb)('<', p) < 0)
	  return (-1);
	if (node->value.element.name[0] == '?' ||
	    !strncmp(node->value.element.name, "!--", 3) ||
	    !strncmp(node->value.element.name, "![CDATA[", 8))
	{
	 /*
                                                        
                            
   */

	  const char	*ptr;		/*                   */


	  for (ptr = node->value.element.name; *ptr; ptr ++)
	    if ((*putc_cb)(*ptr, p) < 0)
	      return (-1);
	}
	else if (mxml_write_name(node->value.element.name, p, putc_cb) < 0)
	  return (-1);

	col += strlen(node->value.element.name) + 1;

	for (i = node->value.element.num_attrs, attr = node->value.element.attrs;
	     i > 0;
	     i --, attr ++)
	{
	  width = strlen(attr->name);

	  if (attr->value)
	    width += strlen(attr->value) + 3;

	  if (global->wrap > 0 && (col + width) > global->wrap)
	  {
	    if ((*putc_cb)('\n', p) < 0)
	      return (-1);

	    col = 0;
	  }
	  else
	  {
	    if ((*putc_cb)(' ', p) < 0)
	      return (-1);

	    col ++;
	  }

	  if (mxml_write_name(attr->name, p, putc_cb) < 0)
	    return (-1);

	  if (attr->value)
	  {
	    if ((*putc_cb)('=', p) < 0)
	      return (-1);
	    if ((*putc_cb)('\"', p) < 0)
	      return (-1);
	    if (mxml_write_string(attr->value, p, putc_cb) < 0)
	      return (-1);
	    if ((*putc_cb)('\"', p) < 0)
	      return (-1);
	  }

	  col += width;
	}

	if (node->child)
	{
	 /*
                      
   */

	  mxml_node_t *child;		/*               */


	  if ((*putc_cb)('>', p) < 0)
	    return (-1);
	  else
	    col ++;

	  col = mxml_write_ws(node, p, cb, MXML_WS_AFTER_OPEN, col, putc_cb);

          for (child = node->child; child; child = child->next)
	  {
	    if ((col = mxml_write_node(child, p, cb, col, putc_cb, global)) < 0)
	      return (-1);
	  }

	 /*
                                                                   
   */

	  if (node->value.element.name[0] != '!' &&
	      node->value.element.name[0] != '?')
	  {
	    col = mxml_write_ws(node, p, cb, MXML_WS_BEFORE_CLOSE, col, putc_cb);

	    if ((*putc_cb)('<', p) < 0)
	      return (-1);
	    if ((*putc_cb)('/', p) < 0)
	      return (-1);
	    if (mxml_write_string(node->value.element.name, p, putc_cb) < 0)
	      return (-1);
	    if ((*putc_cb)('>', p) < 0)
	      return (-1);

	    col += strlen(node->value.element.name) + 3;

	    col = mxml_write_ws(node, p, cb, MXML_WS_AFTER_CLOSE, col, putc_cb);
	  }
	}
	else if (node->value.element.name[0] == '!' ||
		 node->value.element.name[0] == '?')
	{
	 /*
                                              
   */

	  if ((*putc_cb)('>', p) < 0)
	    return (-1);
	  else
	    col ++;

	  col = mxml_write_ws(node, p, cb, MXML_WS_AFTER_OPEN, col, putc_cb);
	}
	else
	{
	  if ((*putc_cb)(' ', p) < 0)
	    return (-1);
	  if ((*putc_cb)('/', p) < 0)
	    return (-1);
	  if ((*putc_cb)('>', p) < 0)
	    return (-1);

	  col += 3;

	  col = mxml_write_ws(node, p, cb, MXML_WS_AFTER_OPEN, col, putc_cb);
	}
	break;

    case MXML_INTEGER :
	if (node->prev)
	{
	  if (global->wrap > 0 && col > global->wrap)
	  {
	    if ((*putc_cb)('\n', p) < 0)
	      return (-1);

	    col = 0;
	  }
	  else if ((*putc_cb)(' ', p) < 0)
	    return (-1);
	  else
	    col ++;
	}

	sprintf(s, "%d", node->value.integer);
	if (mxml_write_string(s, p, putc_cb) < 0)
	  return (-1);

	col += strlen(s);
	break;

    case MXML_OPAQUE :
	if (mxml_write_string(node->value.opaque, p, putc_cb) < 0)
	  return (-1);

	col += strlen(node->value.opaque);
	break;

    case MXML_REAL :
	if (node->prev)
	{
	  if (global->wrap > 0 && col > global->wrap)
	  {
	    if ((*putc_cb)('\n', p) < 0)
	      return (-1);

	    col = 0;
	  }
	  else if ((*putc_cb)(' ', p) < 0)
	    return (-1);
	  else
	    col ++;
	}

	sprintf(s, "%f", node->value.real);
	if (mxml_write_string(s, p, putc_cb) < 0)
	  return (-1);

	col += strlen(s);
	break;

    case MXML_TEXT :
	if (node->value.text.whitespace && col > 0)
	{
	  if (global->wrap > 0 && col > global->wrap)
	  {
	    if ((*putc_cb)('\n', p) < 0)
	      return (-1);

	    col = 0;
	  }
	  else if ((*putc_cb)(' ', p) < 0)
	    return (-1);
	  else
	    col ++;
	}

	if (mxml_write_string(node->value.text.string, p, putc_cb) < 0)
	  return (-1);

	col += strlen(node->value.text.string);
	break;

    case MXML_CUSTOM :
	if (global->custom_save_cb)
	{
	  char	*data;		/*                    */
	  const char	*newline;	/*                        */


	  if ((data = (*global->custom_save_cb)(node)) == NULL)
	    return (-1);

	  if (mxml_write_string(data, p, putc_cb) < 0)
	    return (-1);

	  if ((newline = strrchr(data, '\n')) == NULL)
	    col += strlen(data);
	  else
	    col = strlen(newline);

	  free(data);
	  break;
	}

    default : /*                     */
	return (-1);
  }

  return (col);
}


/*
                                                                      
 */

static int				/*                                 */
mxml_write_string(
    const char      *s,			/*                     */
    void            *p,			/*                   */
    _mxml_putc_cb_t putc_cb)		/*                    */
{
  const char	*name;			/*                     */


  while (*s)
  {
    if ((name = mxmlEntityGetName(*s)) != NULL)
    {
      if ((*putc_cb)('&', p) < 0)
        return (-1);

      while (*name)
      {
	if ((*putc_cb)(*name, p) < 0)
          return (-1);
        name ++;
      }

      if ((*putc_cb)(';', p) < 0)
        return (-1);
    }
    else if ((*putc_cb)(*s, p) < 0)
      return (-1);

    s ++;
  }

  return (0);
}


/*
                                                
 */

static int				/*                */
mxml_write_ws(mxml_node_t     *node,	/*                  */
              void            *p,	/*                   */
              mxml_save_cb_t  cb,	/*                       */
	      int             ws,	/*                 */
	      int             col,	/*                    */
              _mxml_putc_cb_t putc_cb)	/*                    */
{
  const char	*s;			/*                   */


  if (cb && (s = (*cb)(node, ws)) != NULL)
  {
    while (*s)
    {
      if ((*putc_cb)(*s, p) < 0)
	return (-1);
      else if (*s == '\n')
	col = 0;
      else if (*s == '\t')
      {
	col += MXML_TAB;
	col = col - (col % MXML_TAB);
      }
      else
	col ++;

      s ++;
    }
  }

  return (col);
}


/*
                                                               
 */
