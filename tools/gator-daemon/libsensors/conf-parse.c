/*                                         */

/* Bison implementation for Yacc-like parsers in C

      Copyright (C) 1984, 1989-1990, 2000-2011 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/*                                                          
                                                          */

/*                                                               
                                                                     
                                                                 
                                                                
                                                                 
                            */

/*                         */
#define YYBISON 1

/*                 */
#define YYBISON_VERSION "2.5"

/*                 */
#define YYSKELETON_NAME "yacc.c"

/*                */
#define YYPURE 0

/*                */
#define YYPUSH 0

/*                */
#define YYPULL 1

/*                   */
#define YYLSP_NEEDED 0

/*                                              */
#define yyparse         sensors_yyparse
#define yylex           sensors_yylex
#define yyerror         sensors_yyerror
#define yylval          sensors_yylval
#define yychar          sensors_yychar
#define yydebug         sensors_yydebug
#define yynerrs         sensors_yynerrs


/*                                            */

/*                     */
#line 1 "lib/conf-parse.y"

/*
    conf-parse.y - Part of libsensors, a Linux library for reading sensor data.
    Copyright (c) 1998, 1999  Frodo Looijaard <frodol@dds.nl>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
    MA 02110-1301 USA.
*/

#define YYERROR_VERBOSE

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "data.h"
#include "general.h"
#include "error.h"
#include "conf.h"
#include "access.h"
#include "init.h"

static void sensors_yyerror(const char *err);
static sensors_expr *malloc_expr(void);

static sensors_chip *current_chip = NULL;

#define bus_add_el(el) sensors_add_array_el(el,\
                                      &sensors_config_busses,\
                                      &sensors_config_busses_count,\
                                      &sensors_config_busses_max,\
                                      sizeof(sensors_bus))
#define label_add_el(el) sensors_add_array_el(el,\
                                        &current_chip->labels,\
                                        &current_chip->labels_count,\
                                        &current_chip->labels_max,\
                                        sizeof(sensors_label));
#define set_add_el(el) sensors_add_array_el(el,\
                                      &current_chip->sets,\
                                      &current_chip->sets_count,\
                                      &current_chip->sets_max,\
                                      sizeof(sensors_set));
#define compute_add_el(el) sensors_add_array_el(el,\
                                          &current_chip->computes,\
                                          &current_chip->computes_count,\
                                          &current_chip->computes_max,\
                                          sizeof(sensors_compute));
#define ignore_add_el(el) sensors_add_array_el(el,\
                                          &current_chip->ignores,\
                                          &current_chip->ignores_count,\
                                          &current_chip->ignores_max,\
                                          sizeof(sensors_ignore));
#define chip_add_el(el) sensors_add_array_el(el,\
                                       &sensors_config_chips,\
                                       &sensors_config_chips_count,\
                                       &sensors_config_chips_max,\
                                       sizeof(sensors_chip));

#define fits_add_el(el,list) sensors_add_array_el(el,\
                                                  &(list).fits,\
                                                  &(list).fits_count,\
                                                  &(list).fits_max, \
		                                  sizeof(sensors_chip_name));



/*                     */
#line 158 "lib/conf-parse.c"

/*                   */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/*                                   */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/*                            */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif


/*          */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /*                                                                      
                        */
   enum yytokentype {
     NEG = 258,
     EOL = 259,
     BUS = 260,
     LABEL = 261,
     SET = 262,
     CHIP = 263,
     COMPUTE = 264,
     IGNORE = 265,
     FLOAT = 266,
     NAME = 267,
     ERROR = 268
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/*                     */
#line 79 "lib/conf-parse.y"

  double value;
  char *name;
  void *nothing;
  sensors_chip_name_list chips;
  sensors_expr *expr;
  sensors_bus_id bus;
  sensors_chip_name chip;
  sensors_config_line line;



/*                     */
#line 220 "lib/conf-parse.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /*                                */
# define YYSTYPE_IS_DECLARED 1
#endif


/*                                             */


/*                     */
#line 232 "lib/conf-parse.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /*                              */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /*                              */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/*                                                  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /*       */
#endif

/*                                                                          */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/*                                                                     */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /*                              */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /*                              */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /*                              */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /*                                        */
#  define YYSTACK_FREE(Ptr) do { /*       */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /*                                                                       
                                                                          
                                                                          
                                                                     */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /*                       */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /*                              */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /*                              */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /*                              */
#   endif
#  endif
# endif
#endif /*                                         */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/*                                                        */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/*                                                                      */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/*                                                                   
                */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/*                                                          
                                                                        
                                                                 
                                                                    
           */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/*                                                                   
                 */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif
#endif /*                */

/*                                                    */
#define YYFINAL  2
/*                                   */
#define YYLAST   58

/*                                    */
#define YYNTOKENS  24
/*                                    */
#define YYNNTS  16
/*                               */
#define YYNRULES  34
/*                                */
#define YYNSTATES  63

/*                                                                    */
#define YYUNDEFTOK  2
#define YYMAXUTOK   268

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/*                                                                    */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      22,    23,     5,     4,    10,     3,     2,     6,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,    21,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     8,     2,     9,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     7,    11,
      12,    13,    14,    15,    16,    17,    18,    19,    20
};

#if YYDEBUG
/*                                                                   
           */
static const yytype_uint8 yyprhs[] =
{
       0,     0,     3,     4,     7,    10,    13,    16,    19,    22,
      25,    28,    32,    36,    40,    46,    49,    52,    54,    57,
      59,    61,    63,    67,    71,    75,    79,    82,    86,    89,
      92,    94,    96,    98,   100
};

/*                                                    */
static const yytype_int8 yyrhs[] =
{
      25,     0,    -1,    -1,    25,    26,    -1,    27,    11,    -1,
      28,    11,    -1,    29,    11,    -1,    32,    11,    -1,    30,
      11,    -1,    31,    11,    -1,     1,    11,    -1,    12,    35,
      36,    -1,    13,    37,    38,    -1,    14,    37,    34,    -1,
      16,    37,    34,    10,    34,    -1,    17,    37,    -1,    15,
      33,    -1,    39,    -1,    33,    39,    -1,    18,    -1,    19,
      -1,    21,    -1,    34,     4,    34,    -1,    34,     3,    34,
      -1,    34,     5,    34,    -1,    34,     6,    34,    -1,     3,
      34,    -1,    22,    34,    23,    -1,     8,    34,    -1,     9,
      34,    -1,    19,    -1,    19,    -1,    19,    -1,    19,    -1,
      19,    -1
};

/*                                                                 */
static const yytype_uint16 yyrline[] =
{
       0,   119,   119,   120,   123,   124,   125,   126,   127,   128,
     129,   132,   141,   156,   171,   188,   201,   219,   225,   231,
     236,   241,   245,   252,   259,   266,   273,   280,   282,   289,
     298,   308,   312,   316,   320
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/*                                                             
                                                                     */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "'-'", "'+'", "'*'", "'/'", "NEG", "'^'",
  "'`'", "','", "EOL", "BUS", "LABEL", "SET", "CHIP", "COMPUTE", "IGNORE",
  "FLOAT", "NAME", "ERROR", "'@'", "'('", "')'", "$accept", "input",
  "line", "bus_statement", "label_statement", "set_statement",
  "compute_statement", "ignore_statement", "chip_statement",
  "chip_name_list", "expression", "bus_id", "adapter_name",
  "function_name", "string", "chip_name", 0
};
#endif

# ifdef YYPRINT
/*                                                              
                     */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,    45,    43,    42,    47,   258,    94,    96,
      44,   259,   260,   261,   262,   263,   264,   265,   266,   267,
     268,    64,    40,    41
};
# endif

/*                                                              */
static const yytype_uint8 yyr1[] =
{
       0,    24,    25,    25,    26,    26,    26,    26,    26,    26,
      26,    27,    28,    29,    30,    31,    32,    33,    33,    34,
      34,    34,    34,    34,    34,    34,    34,    34,    34,    34,
      35,    36,    37,    38,    39
};

/*                                                                        */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     2,     2,     2,     2,     2,     2,     2,
       2,     3,     3,     3,     5,     2,     2,     1,     2,     1,
       1,     1,     3,     3,     3,     3,     2,     3,     2,     2,
       1,     1,     1,     1,     1
};

/*                                                                     
                                                                     
                                   */
static const yytype_uint8 yydefact[] =
{
       2,     0,     1,     0,     0,     0,     0,     0,     0,     0,
       3,     0,     0,     0,     0,     0,     0,    10,    30,     0,
      32,     0,     0,    34,    16,    17,     0,    15,     4,     5,
       6,     8,     9,     7,    31,    11,    33,    12,     0,     0,
       0,    19,    20,    21,     0,    13,    18,     0,    26,    28,
      29,     0,     0,     0,     0,     0,     0,    27,    23,    22,
      24,    25,    14
};

/*                        */
static const yytype_int8 yydefgoto[] =
{
      -1,     1,    10,    11,    12,    13,    14,    15,    16,    24,
      45,    19,    35,    21,    37,    25
};

/*                                                                
               */
#define YYPACT_NINF -27
static const yytype_int8 yypact[] =
{
     -27,    37,   -27,    -4,    -3,     1,     1,     6,     1,     1,
     -27,     8,    13,    20,    23,    32,    34,   -27,   -27,    29,
     -27,    39,    14,   -27,     6,   -27,    14,   -27,   -27,   -27,
     -27,   -27,   -27,   -27,   -27,   -27,   -27,   -27,    14,    14,
      14,   -27,   -27,   -27,    14,    36,   -27,     5,   -27,   -27,
     -27,    -2,    14,    14,    14,    14,    14,   -27,     0,     0,
     -27,   -27,    36
};

/*                      */
static const yytype_int8 yypgoto[] =
{
     -27,   -27,   -27,   -27,   -27,   -27,   -27,   -27,   -27,   -27,
     -26,   -27,   -27,    38,   -27,    31
};

/*                                                                
                                                                  
                                                            */
#define YYTABLE_NINF -1
static const yytype_uint8 yytable[] =
{
      47,    52,    53,    54,    55,    54,    55,    17,    52,    53,
      54,    55,    48,    49,    50,    56,    18,    38,    51,    28,
      20,    57,    39,    40,    29,    23,    58,    59,    60,    61,
      62,    30,    41,    42,    31,    43,    44,     2,     3,    52,
      53,    54,    55,    32,    22,    33,    26,    27,    34,     4,
       5,     6,     7,     8,     9,    46,     0,     0,    36
};

#define yypact_value_is_default(yystate) \
  ((yystate) == (-27))

#define yytable_value_is_error(yytable_value) \
  YYID (0)

static const yytype_int8 yycheck[] =
{
      26,     3,     4,     5,     6,     5,     6,    11,     3,     4,
       5,     6,    38,    39,    40,    10,    19,     3,    44,    11,
      19,    23,     8,     9,    11,    19,    52,    53,    54,    55,
      56,    11,    18,    19,    11,    21,    22,     0,     1,     3,
       4,     5,     6,    11,     6,    11,     8,     9,    19,    12,
      13,    14,    15,    16,    17,    24,    -1,    -1,    19
};

/*                                                            
                               */
static const yytype_uint8 yystos[] =
{
       0,    25,     0,     1,    12,    13,    14,    15,    16,    17,
      26,    27,    28,    29,    30,    31,    32,    11,    19,    35,
      19,    37,    37,    19,    33,    39,    37,    37,    11,    11,
      11,    11,    11,    11,    19,    36,    19,    38,     3,     8,
       9,    18,    19,    21,    22,    34,    39,    34,    34,    34,
      34,    34,     3,     4,     5,     6,    10,    23,    34,    34,
      34,    34,    34
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/*                                                                    
                                                                 
                                                                      
                                                                        
                                                               
               */

#define YYFAIL		goto yyerrlab
#if defined YYFAIL
  /*                                                     
                                                                      
                                                                   
                                                               */
#endif

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/*                                                             
                                                               
                                                  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/*                                                    */

#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


/*                                                     */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/*                                 */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /*                              */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*                                 
                                   
                                 */

/*        */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*                                 
                                   
                                 */

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*                                                                   
                                                                     
                                                                     
                                                                   */

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*                                                 
                                                   
                                                 */

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /*                             */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/*                                                                   
                                  */
int yydebug;
#else /*          */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /*          */


/*                                                      */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/*                                                                  
                                                   

                                                                 
                                                     
                                                          */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/*                              */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/*                                                                       
            */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/*                                                                     
                                                                  
                                                                    
                                                            
                                                                   
                                                                   
                     */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /*                */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/*                                                                  
                                                                      
         

                                                                      
                                                                
                                                                 
                                                    */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (0, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  YYSIZE_T yysize1;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /*                                  */
  const char *yyformat = 0;
  /*                        */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /*                                                             
                  */
  int yycount = 0;

  /*                                               
                                                                    
                                                                              
                                                               
                
                                                                      
                                                                      
                                                                  
                                     
                                                                       
                                                                     
                                                                      
                                                                        
                                                 
                                                                        
                                                                       
                                                                       
                                                               
                                                                   
                                                                      
                                                                      
                                                                        
                                                                      
                                                                      
                                                                      
                                                        
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /*                                                           
                                                                      
                                                           */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /*                                                  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                yysize1 = yysize + yytnamerr (0, yytname[yyx]);
                if (! (yysize <= yysize1
                       && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                  return 2;
                yysize = yysize1;
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  yysize1 = yysize + yystrlen (yyformat);
  if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
    return 2;
  yysize = yysize1;

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /*                                                           
                                                          
                                                        */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /*                 */

/*                                                
                                                  
                                                */

/*        */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}


/*                                              */
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /*                 */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /*                 */


/*                        */
int yychar;

/*                                              */
YYSTYPE yylval;

/*                                  */
int yynerrs;


/*           
             
           */

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /*                 */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
    int yystate;
    /*                                                           */
    int yyerrstatus;

    /*                            
                                 
                                          

                                                                      
                                      */

    /*                   */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /*                            */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /*                                                            */
  int yytoken;
  /*                                                                  
                       */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /*                                                     */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /*                                                      
                                                    */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /*                            */

  /*                           
                                                  
                                                            
                                                 */
  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*                                                             
                                                               
                                                             */
 yynewstate:
  /*                                                               
                                                                        */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /*                                                              */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/*                                                           
                                                        
             */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;

	/*                                                          
                                                           
                                                              
                                            */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /*               */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /*                                */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /*               */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*            
              
            */
yybackup:

  /*                                                           
                                                                 */

  /*                                                                       */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /*                                                                */

  /*                                                                 */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /*                                                                
                                         */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /*                                                              
              */
  if (yyerrstatus)
    yyerrstatus--;

  /*                             */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /*                             */
  yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

  goto yynewstate;


/*                                                            
                                                              
                                                            */
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*                              
                                
                              */
yyreduce:
  /*                                              */
  yylen = yyr2[yyn];

  /*                                                                
               

                                                         
                                            
                                                       
                                                                    
                                                        */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 11:

/*                      */
#line 133 "lib/conf-parse.y"
    { sensors_bus new_el;
		    new_el.line = (yyvsp[(1) - (3)].line);
		    new_el.bus = (yyvsp[(2) - (3)].bus);
                    new_el.adapter = (yyvsp[(3) - (3)].name);
		    bus_add_el(&new_el);
		  }
    break;

  case 12:

/*                      */
#line 142 "lib/conf-parse.y"
    { sensors_label new_el;
			    if (!current_chip) {
			      sensors_yyerror("Label statement before first chip statement");
			      free((yyvsp[(2) - (3)].name));
			      free((yyvsp[(3) - (3)].name));
			      YYERROR;
			    }
			    new_el.line = (yyvsp[(1) - (3)].line);
			    new_el.name = (yyvsp[(2) - (3)].name);
			    new_el.value = (yyvsp[(3) - (3)].name);
			    label_add_el(&new_el);
			  }
    break;

  case 13:

/*                      */
#line 157 "lib/conf-parse.y"
    { sensors_set new_el;
		    if (!current_chip) {
		      sensors_yyerror("Set statement before first chip statement");
		      free((yyvsp[(2) - (3)].name));
		      sensors_free_expr((yyvsp[(3) - (3)].expr));
		      YYERROR;
		    }
		    new_el.line = (yyvsp[(1) - (3)].line);
		    new_el.name = (yyvsp[(2) - (3)].name);
		    new_el.value = (yyvsp[(3) - (3)].expr);
		    set_add_el(&new_el);
		  }
    break;

  case 14:

/*                      */
#line 172 "lib/conf-parse.y"
    { sensors_compute new_el;
			    if (!current_chip) {
			      sensors_yyerror("Compute statement before first chip statement");
			      free((yyvsp[(2) - (5)].name));
			      sensors_free_expr((yyvsp[(3) - (5)].expr));
			      sensors_free_expr((yyvsp[(5) - (5)].expr));
			      YYERROR;
			    }
			    new_el.line = (yyvsp[(1) - (5)].line);
			    new_el.name = (yyvsp[(2) - (5)].name);
			    new_el.from_proc = (yyvsp[(3) - (5)].expr);
			    new_el.to_proc = (yyvsp[(5) - (5)].expr);
			    compute_add_el(&new_el);
			  }
    break;

  case 15:

/*                      */
#line 189 "lib/conf-parse.y"
    { sensors_ignore new_el;
			  if (!current_chip) {
			    sensors_yyerror("Ignore statement before first chip statement");
			    free((yyvsp[(2) - (2)].name));
			    YYERROR;
			  }
			  new_el.line = (yyvsp[(1) - (2)].line);
			  new_el.name = (yyvsp[(2) - (2)].name);
			  ignore_add_el(&new_el);
			}
    break;

  case 16:

/*                      */
#line 202 "lib/conf-parse.y"
    { sensors_chip new_el;
		    new_el.line = (yyvsp[(1) - (2)].line);
		    new_el.labels = NULL;
		    new_el.sets = NULL;
		    new_el.computes = NULL;
		    new_el.ignores = NULL;
		    new_el.labels_count = new_el.labels_max = 0;
		    new_el.sets_count = new_el.sets_max = 0;
		    new_el.computes_count = new_el.computes_max = 0;
		    new_el.ignores_count = new_el.ignores_max = 0;
		    new_el.chips = (yyvsp[(2) - (2)].chips);
		    chip_add_el(&new_el);
		    current_chip = sensors_config_chips +
		                   sensors_config_chips_count - 1;
		  }
    break;

  case 17:

/*                      */
#line 220 "lib/conf-parse.y"
    {
		    (yyval.chips).fits = NULL;
		    (yyval.chips).fits_count = (yyval.chips).fits_max = 0;
		    fits_add_el(&(yyvsp[(1) - (1)].chip),(yyval.chips));
		  }
    break;

  case 18:

/*                      */
#line 226 "lib/conf-parse.y"
    { (yyval.chips) = (yyvsp[(1) - (2)].chips);
		    fits_add_el(&(yyvsp[(2) - (2)].chip),(yyval.chips));
		  }
    break;

  case 19:

/*                      */
#line 232 "lib/conf-parse.y"
    { (yyval.expr) = malloc_expr();
		    (yyval.expr)->data.val = (yyvsp[(1) - (1)].value);
		    (yyval.expr)->kind = sensors_kind_val;
		  }
    break;

  case 20:

/*                      */
#line 237 "lib/conf-parse.y"
    { (yyval.expr) = malloc_expr();
		    (yyval.expr)->data.var = (yyvsp[(1) - (1)].name);
		    (yyval.expr)->kind = sensors_kind_var;
		  }
    break;

  case 21:

/*                      */
#line 242 "lib/conf-parse.y"
    { (yyval.expr) = malloc_expr();
		    (yyval.expr)->kind = sensors_kind_source;
		  }
    break;

  case 22:

/*                      */
#line 246 "lib/conf-parse.y"
    { (yyval.expr) = malloc_expr();
		    (yyval.expr)->kind = sensors_kind_sub;
		    (yyval.expr)->data.subexpr.op = sensors_add;
		    (yyval.expr)->data.subexpr.sub1 = (yyvsp[(1) - (3)].expr);
		    (yyval.expr)->data.subexpr.sub2 = (yyvsp[(3) - (3)].expr);
		  }
    break;

  case 23:

/*                      */
#line 253 "lib/conf-parse.y"
    { (yyval.expr) = malloc_expr();
		    (yyval.expr)->kind = sensors_kind_sub;
		    (yyval.expr)->data.subexpr.op = sensors_sub;
		    (yyval.expr)->data.subexpr.sub1 = (yyvsp[(1) - (3)].expr);
		    (yyval.expr)->data.subexpr.sub2 = (yyvsp[(3) - (3)].expr);
		  }
    break;

  case 24:

/*                      */
#line 260 "lib/conf-parse.y"
    { (yyval.expr) = malloc_expr();
		    (yyval.expr)->kind = sensors_kind_sub;
		    (yyval.expr)->data.subexpr.op = sensors_multiply;
		    (yyval.expr)->data.subexpr.sub1 = (yyvsp[(1) - (3)].expr);
		    (yyval.expr)->data.subexpr.sub2 = (yyvsp[(3) - (3)].expr);
		  }
    break;

  case 25:

/*                      */
#line 267 "lib/conf-parse.y"
    { (yyval.expr) = malloc_expr();
		    (yyval.expr)->kind = sensors_kind_sub;
		    (yyval.expr)->data.subexpr.op = sensors_divide;
		    (yyval.expr)->data.subexpr.sub1 = (yyvsp[(1) - (3)].expr);
		    (yyval.expr)->data.subexpr.sub2 = (yyvsp[(3) - (3)].expr);
		  }
    break;

  case 26:

/*                      */
#line 274 "lib/conf-parse.y"
    { (yyval.expr) = malloc_expr();
		    (yyval.expr)->kind = sensors_kind_sub;
		    (yyval.expr)->data.subexpr.op = sensors_negate;
		    (yyval.expr)->data.subexpr.sub1 = (yyvsp[(2) - (2)].expr);
		    (yyval.expr)->data.subexpr.sub2 = NULL;
		  }
    break;

  case 27:

/*                      */
#line 281 "lib/conf-parse.y"
    { (yyval.expr) = (yyvsp[(2) - (3)].expr); }
    break;

  case 28:

/*                      */
#line 283 "lib/conf-parse.y"
    { (yyval.expr) = malloc_expr();
		    (yyval.expr)->kind = sensors_kind_sub;
		    (yyval.expr)->data.subexpr.op = sensors_exp;
		    (yyval.expr)->data.subexpr.sub1 = (yyvsp[(2) - (2)].expr);
		    (yyval.expr)->data.subexpr.sub2 = NULL;
		  }
    break;

  case 29:

/*                      */
#line 290 "lib/conf-parse.y"
    { (yyval.expr) = malloc_expr();
		    (yyval.expr)->kind = sensors_kind_sub;
		    (yyval.expr)->data.subexpr.op = sensors_log;
		    (yyval.expr)->data.subexpr.sub1 = (yyvsp[(2) - (2)].expr);
		    (yyval.expr)->data.subexpr.sub2 = NULL;
		  }
    break;

  case 30:

/*                      */
#line 299 "lib/conf-parse.y"
    { int res = sensors_parse_bus_id((yyvsp[(1) - (1)].name),&(yyval.bus));
		    free((yyvsp[(1) - (1)].name));
		    if (res) {
                      sensors_yyerror("Parse error in bus id");
		      YYERROR;
                    }
		  }
    break;

  case 31:

/*                      */
#line 309 "lib/conf-parse.y"
    { (yyval.name) = (yyvsp[(1) - (1)].name); }
    break;

  case 32:

/*                      */
#line 313 "lib/conf-parse.y"
    { (yyval.name) = (yyvsp[(1) - (1)].name); }
    break;

  case 33:

/*                      */
#line 317 "lib/conf-parse.y"
    { (yyval.name) = (yyvsp[(1) - (1)].name); }
    break;

  case 34:

/*                      */
#line 321 "lib/conf-parse.y"
    { int res = sensors_parse_chip_name((yyvsp[(1) - (1)].name),&(yyval.chip));
		    free((yyvsp[(1) - (1)].name));
		    if (res) {
		      sensors_yyerror("Parse error in chip name");
		      YYERROR;
		    }
		  }
    break;



/*                      */
#line 1793 "lib/conf-parse.c"
      default: break;
    }
  /*                                                                
                                                                   
                                                                     
                                                                     
                                                                        
                                                                       
                                                                    
                                                                    
                                                                      
                                                                    
                                          */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /*                                                               
                                                                    
                         */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*                                     
                                       
                                     */
yyerrlab:
  /*                                                                 
                                                       */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /*                                                              */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



  if (yyerrstatus == 3)
    {
      /*                                                           
                      */

      if (yychar <= YYEOF)
	{
	  /*                                     */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /*                                                                
             */
  goto yyerrlab1;


/*                                                    
                                                      
                                                    */
yyerrorlab:

  /*                                                           
                                                                     
            */
  if (/*         */ 0)
     goto yyerrorlab;

  /*                                                              
                    */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*                                                              
                                                                
                                                              */
yyerrlab1:
  yyerrstatus = 3;	/*                                           */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /*                                                                  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;


  /*                         */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*                                      
                                        
                                      */
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*                                    
                                      
                                    */
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*                                                  
                                                    
                                                  */
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /*                */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /*                                                                 
                                                           */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /*                                                              
                                */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /*                          */
  return YYID (yyresult);
}



/*                      */
#line 330 "lib/conf-parse.y"


void sensors_yyerror(const char *err)
{
  if (sensors_lex_error[0]) {
    sensors_parse_error_wfn(sensors_lex_error, sensors_yyfilename, sensors_yylineno);
    sensors_lex_error[0] = '\0';
  } else
    sensors_parse_error_wfn(err, sensors_yyfilename, sensors_yylineno);
}

sensors_expr *malloc_expr(void)
{
  sensors_expr *res = malloc(sizeof(sensors_expr));
  if (! res)
    sensors_fatal_error(__func__, "Allocating a new expression");
  return res;
}

