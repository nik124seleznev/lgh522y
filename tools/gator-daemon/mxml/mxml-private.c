/*
 * "$Id: mxml-private.c 451 2014-01-04 21:50:06Z msweet $"
 *
 * Private functions for Mini-XML, a small XML-like file parsing library.
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

#include "mxml-private.h"


/*
 * Some crazy people think that unloading a shared object is a good or safe
 * thing to do.  Unfortunately, most objects are simply *not* safe to unload
 * and bad things *will* happen.
 *
 * The following mess of conditional code allows us to provide a destructor
 * function in Mini-XML for our thread-global storage so that it can possibly
 * be unloaded safely, although since there is no standard way to do so I
 * can't even provide any guarantees that you can do it safely on all platforms.
 *
 * This code currently supports AIX, HP-UX, Linux, Mac OS X, Solaris, and
 * Windows.  It might work on the BSDs and IRIX, but I haven't tested that.
 */

#if defined(__sun) || defined(_AIX)
#  pragma fini(_mxml_fini)
#  define _MXML_FINI _mxml_fini
#elif defined(__hpux)
#  pragma FINI _mxml_fini
#  define _MXML_FINI _mxml_fini
#elif defined(__GNUC__) /*                    */
#  define _MXML_FINI __attribute((destructor)) _mxml_fini
#else
#  define _MXML_FINI _fini
#endif /*       */


/*
                                             
 */

void
mxml_error(const char *format,		/*                                */
           ...)				/*                                    */
{
  va_list	ap;			/*                      */
  char		s[1024];		/*                */
  _mxml_global_t *global = _mxml_global();
					/*             */


 /*
                        
  */

  if (!format)
    return;

 /*
                                      
  */

  va_start(ap, format);

  vsnprintf(s, sizeof(s), format, ap);

  va_end(ap);

 /*
                                         
  */

  if (global->error_cb)
    (*global->error_cb)(s);
  else
    fprintf(stderr, "mxml: %s\n", s);
}


/*
                                                            
 */

mxml_type_t				/*               */
mxml_ignore_cb(mxml_node_t *node)	/*                  */
{
  (void)node;

  return (MXML_IGNORE);
}


/*
                                                             
 */

mxml_type_t				/*               */
mxml_integer_cb(mxml_node_t *node)	/*                  */
{
  (void)node;

  return (MXML_INTEGER);
}


/*
                                                           
 */

mxml_type_t				/*               */
mxml_opaque_cb(mxml_node_t *node)	/*                  */
{
  (void)node;

  return (MXML_OPAQUE);
}


/*
                                                              
 */

mxml_type_t				/*               */
mxml_real_cb(mxml_node_t *node)		/*                  */
{
  (void)node;

  return (MXML_REAL);
}


#ifdef HAVE_PTHREAD_H			/*                       */
#  include <pthread.h>

static pthread_key_t	_mxml_key = -1;	/*                          */
static pthread_once_t	_mxml_key_once = PTHREAD_ONCE_INIT;
					/*                                */
static void		_mxml_init(void);
static void		_mxml_destructor(void *g);


/*
                                                         
 */

static void
_mxml_destructor(void *g)		/*                 */
{
  free(g);
}


/*
                                           
 */

static void
_MXML_FINI(void)
{
  _mxml_global_t	*global;	/*             */


  if (_mxml_key != -1)
  {
    if ((global = (_mxml_global_t *)pthread_getspecific(_mxml_key)) != NULL)
      _mxml_destructor(global);

    pthread_key_delete(_mxml_key);
    _mxml_key = -1;
  }
}


/*
                                      
 */

_mxml_global_t *			/*                 */
_mxml_global(void)
{
  _mxml_global_t	*global;	/*             */


  pthread_once(&_mxml_key_once, _mxml_init);

  if ((global = (_mxml_global_t *)pthread_getspecific(_mxml_key)) == NULL)
  {
    global = (_mxml_global_t *)calloc(1, sizeof(_mxml_global_t));
    pthread_setspecific(_mxml_key, global);

    global->num_entity_cbs = 1;
    global->entity_cbs[0]  = _mxml_entity_cb;
    global->wrap           = 72;
  }

  return (global);
}


/*
                                             
 */

static void
_mxml_init(void)
{
  pthread_key_create(&_mxml_key, _mxml_destructor);
}


#elif defined(WIN32) && defined(MXML1_EXPORTS) /*                       */
#  include <windows.h>

static DWORD _mxml_tls_index;		/*                          */


/*
                                        
 */

BOOL WINAPI				/*                     */
DllMain(HINSTANCE hinst,		/*                       */
        DWORD     reason,		/*            */
        LPVOID    reserved)		/*            */
{
  _mxml_global_t	*global;	/*             */


  (void)hinst;
  (void)reserved;

  switch (reason)
  {
    case DLL_PROCESS_ATTACH :		/*                                  */
        if ((_mxml_tls_index = TlsAlloc()) == TLS_OUT_OF_INDEXES)
          return (FALSE);
        break;

    case DLL_THREAD_DETACH :		/*                                 */
        if ((global = (_mxml_global_t *)TlsGetValue(_mxml_tls_index)) != NULL)
          free(global);
        break;

    case DLL_PROCESS_DETACH :		/*                                 */
        if ((global = (_mxml_global_t *)TlsGetValue(_mxml_tls_index)) != NULL)
          free(global);

        TlsFree(_mxml_tls_index);
        break;

    default:
        break;
  }

  return (TRUE);
}


/*
                                      
 */

_mxml_global_t *			/*                 */
_mxml_global(void)
{
  _mxml_global_t	*global;	/*             */


  if ((global = (_mxml_global_t *)TlsGetValue(_mxml_tls_index)) == NULL)
  {
    global = (_mxml_global_t *)calloc(1, sizeof(_mxml_global_t));

    global->num_entity_cbs = 1;
    global->entity_cbs[0]  = _mxml_entity_cb;
    global->wrap           = 72;

    TlsSetValue(_mxml_tls_index, (LPVOID)global);
  }

  return (global);
}


#else					/*                    */
/*
                                      
 */

_mxml_global_t *			/*                 */
_mxml_global(void)
{
  static _mxml_global_t	global =	/*             */
  {
    NULL,				/*          */
    1,					/*                */
    { _mxml_entity_cb },		/*            */
    72,					/*      */
    NULL,				/*                */
    NULL				/*                */
  };


  return (&global);
}
#endif /*                */


/*
                                                                  
 */
