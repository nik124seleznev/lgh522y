/*
 * "$Id: mxml-string.c 454 2014-01-05 03:25:07Z msweet $"
 *
 * String functions for Mini-XML, a small XML-like file parsing library.
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


/*
                                                                           
                                                                            
 */

#ifndef va_copy
#  ifdef __va_copy
#    define va_copy(dst,src) __va_copy(dst,src)
#  else
#    define va_copy(dst,src) memcpy(&dst, src, sizeof(va_list))
#  endif /*           */
#endif /*         */


#ifndef HAVE_SNPRINTF
/*
                                        
 */

int					/*                               */
_mxml_snprintf(char       *buffer,	/*                   */
               size_t     bufsize,	/*                           */
	       const char *format,	/*                                */
	       ...)			/*                                    */
{
  va_list	ap;			/*               */
  int		bytes;			/*                           */


  va_start(ap, format);
  bytes = vsnprintf(buffer, bufsize, format, ap);
  va_end(ap);

  return (bytes);
}
#endif /*                */


/*
                                         
 */

#ifndef HAVE_STRDUP
char *					/*                        */
_mxml_strdup(const char *s)		/*                         */
{
  char	*t;				/*                    */


  if (s == NULL)
    return (NULL);

  if ((t = malloc(strlen(s) + 1)) == NULL)
    return (NULL);

  return (strcpy(t, s));
}
#endif /*              */


/*
                                                     
 */

char *					/*                        */
_mxml_strdupf(const char *format,	/*                                */
              ...)			/*                                    */
{
  va_list	ap;			/*                                 */
  char		*s;			/*                             */


 /*
                                                                 
                    
  */

  va_start(ap, format);
  s = _mxml_vstrdupf(format, ap);
  va_end(ap);

  return (s);
}


#ifndef HAVE_VSNPRINTF
/*
                                                                  
 */

int					/*                               */
_mxml_vsnprintf(char       *buffer,	/*                   */
                size_t     bufsize,	/*                           */
		const char *format,	/*                                */
 		va_list    ap)		/*                                     */
{
  char		*bufptr,		/*                               */
		*bufend,		/*                          */
		sign,			/*                      */
		size,			/*                          */
		type;			/*                       */
  int		width,			/*                */
		prec;			/*                                   */
  char		tformat[100],		/*                                       */
		*tptr,			/*                               */
		temp[1024];		/*                              */
  char		*s;			/*                   */
  int		slen;			/*                  */
  int		bytes;			/*                              */


 /*
                                                           
  */

  bufptr = buffer;
  bufend = buffer + bufsize - 1;
  bytes  = 0;

  while (*format)
  {
    if (*format == '%')
    {
      tptr = tformat;
      *tptr++ = *format++;

      if (*format == '%')
      {
        if (bufptr && bufptr < bufend)
          *bufptr++ = *format;
        bytes ++;
        format ++;
	continue;
      }
      else if (strchr(" -+#\'", *format))
      {
        *tptr++ = *format;
        sign = *format++;
      }
      else
        sign = 0;

      if (*format == '*')
      {
       /*
                                    
 */

	format ++;
	width = va_arg(ap, int);

	snprintf(tptr, sizeof(tformat) - (tptr - tformat), "%d", width);
	tptr += strlen(tptr);
      }
      else
      {
	width = 0;

	while (isdigit(*format & 255))
	{
	  if (tptr < (tformat + sizeof(tformat) - 1))
	    *tptr++ = *format;

	  width = width * 10 + *format++ - '0';
	}
      }

      if (*format == '.')
      {
	if (tptr < (tformat + sizeof(tformat) - 1))
	  *tptr++ = *format;

        format ++;

        if (*format == '*')
	{
         /*
                                   
   */

	  format ++;
	  prec = va_arg(ap, int);

	  snprintf(tptr, sizeof(tformat) - (tptr - tformat), "%d", prec);
	  tptr += strlen(tptr);
	}
	else
	{
	  prec = 0;

	  while (isdigit(*format & 255))
	  {
	    if (tptr < (tformat + sizeof(tformat) - 1))
	      *tptr++ = *format;

	    prec = prec * 10 + *format++ - '0';
	  }
	}
      }
      else
        prec = -1;

      if (*format == 'l' && format[1] == 'l')
      {
        size = 'L';

	if (tptr < (tformat + sizeof(tformat) - 2))
	{
	  *tptr++ = 'l';
	  *tptr++ = 'l';
	}

	format += 2;
      }
      else if (*format == 'h' || *format == 'l' || *format == 'L')
      {
	if (tptr < (tformat + sizeof(tformat) - 1))
	  *tptr++ = *format;

        size = *format++;
      }

      if (!*format)
        break;

      if (tptr < (tformat + sizeof(tformat) - 1))
        *tptr++ = *format;

      type  = *format++;
      *tptr = '\0';

      switch (type)
      {
	case 'E' : /*                        */
	case 'G' :
	case 'e' :
	case 'f' :
	case 'g' :
	    if ((width + 2) > sizeof(temp))
	      break;

	    sprintf(temp, tformat, va_arg(ap, double));

            bytes += strlen(temp);

            if (bufptr)
	    {
	      if ((bufptr + strlen(temp)) > bufend)
	      {
		strncpy(bufptr, temp, (size_t)(bufend - bufptr));
		bufptr = bufend;
	      }
	      else
	      {
		strcpy(bufptr, temp);
		bufptr += strlen(temp);
	      }
	    }
	    break;

        case 'B' : /*                 */
	case 'X' :
	case 'b' :
        case 'd' :
	case 'i' :
	case 'o' :
	case 'u' :
	case 'x' :
	    if ((width + 2) > sizeof(temp))
	      break;

#ifdef HAVE_LONG_LONG
	    if (size == 'L')
	      sprintf(temp, tformat, va_arg(ap, long long));
	    else
#endif /*                */
	    sprintf(temp, tformat, va_arg(ap, int));

            bytes += strlen(temp);

	    if (bufptr)
	    {
	      if ((bufptr + strlen(temp)) > bufend)
	      {
		strncpy(bufptr, temp, (size_t)(bufend - bufptr));
		bufptr = bufend;
	      }
	      else
	      {
		strcpy(bufptr, temp);
		bufptr += strlen(temp);
	      }
	    }
	    break;

	case 'p' : /*               */
	    if ((width + 2) > sizeof(temp))
	      break;

	    sprintf(temp, tformat, va_arg(ap, void *));

            bytes += strlen(temp);

	    if (bufptr)
	    {
	      if ((bufptr + strlen(temp)) > bufend)
	      {
		strncpy(bufptr, temp, (size_t)(bufend - bufptr));
		bufptr = bufend;
	      }
	      else
	      {
		strcpy(bufptr, temp);
		bufptr += strlen(temp);
	      }
	    }
	    break;

        case 'c' : /*                              */
	    bytes += width;

	    if (bufptr)
	    {
	      if (width <= 1)
	        *bufptr++ = va_arg(ap, int);
	      else
	      {
		if ((bufptr + width) > bufend)
		  width = bufend - bufptr;

		memcpy(bufptr, va_arg(ap, char *), (size_t)width);
		bufptr += width;
	      }
	    }
	    break;

	case 's' : /*        */
	    if ((s = va_arg(ap, char *)) == NULL)
	      s = "(null)";

	    slen = strlen(s);
	    if (slen > width && prec != width)
	      width = slen;

            bytes += width;

	    if (bufptr)
	    {
	      if ((bufptr + width) > bufend)
	        width = bufend - bufptr;

              if (slen > width)
	        slen = width;

	      if (sign == '-')
	      {
		strncpy(bufptr, s, (size_t)slen);
		memset(bufptr + slen, ' ', (size_t)(width - slen));
	      }
	      else
	      {
		memset(bufptr, ' ', (size_t)(width - slen));
		strncpy(bufptr + width - slen, s, (size_t)slen);
	      }

	      bufptr += width;
	    }
	    break;

	case 'n' : /*                               */
	    *(va_arg(ap, int *)) = bytes;
	    break;
      }
    }
    else
    {
      bytes ++;

      if (bufptr && bufptr < bufend)
        *bufptr++ = *format;

      format ++;
    }
  }

 /*
                                                                        
  */

  *bufptr = '\0';

  return (bytes);
}
#endif /*                 */


/*
                                                      
 */

char *					/*                        */
_mxml_vstrdupf(const char *format,	/*                                */
               va_list    ap)		/*                                     */
{
  int		bytes;			/*                          */
  char		*buffer,		/*               */
		temp[256];		/*                                  */
  va_list	apcopy;			/*                       */


 /*
                                                                         
             
  */

  va_copy(apcopy, ap);
  bytes = vsnprintf(temp, sizeof(temp), format, apcopy);

  if (bytes < sizeof(temp))
  {
   /*
                                                                            
    */

    return (strdup(temp));
  }

 /*
                                                                       
             
  */

  if ((buffer = calloc(1, bytes + 1)) != NULL)
    vsnprintf(buffer, bytes + 1, format, ap);

 /*
                            
  */

  return (buffer);
}


/*
                                                                 
 */
