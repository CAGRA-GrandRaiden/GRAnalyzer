
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.4.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
#line 1 "src/freadin.y"

/*
  feradin.y --- yacc program for fread
  Copyright (C) 2000  A. Tamii
  Author:   A. Tamii
  Facility: Department of Physics, University of Tokyo
  Version 1.00 04-JUN-2000 by A. Tamii
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>

#include "freadin.h"
#include "histogram.h"

extern vrec_p slist;
vrec_p add_node(int ope, vrec_p arg1, vrec_p arg2);


/* Line 189 of yacc.c  */
#line 95 "freadin.tab.c"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     NAME = 258,
     NAME2 = 259,
     NUM = 260,
     TDEF = 261,
     TH1 = 262,
     TH2 = 263,
     TCOM = 264,
     TSEP = 265,
     STR = 266,
     TUNDEF = 267,
     TNODE = 268,
     TUMIN = 269,
     TFUNC = 270,
     TARRAY = 271,
     TELM = 272,
     TLOR = 273,
     TLAND = 274,
     TEQ = 275,
     TNEQ = 276,
     TLE = 277,
     TGE = 278,
     TSR = 279,
     TSL = 280,
     UPLUS = 281,
     UMINUS = 282
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 214 of yacc.c  */
#line 22 "src/freadin.y"

	struct  vrec   *vrecp;



/* Line 214 of yacc.c  */
#line 164 "freadin.tab.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 176 "freadin.tab.c"

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
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
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

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
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

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
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

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  10
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   346

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  49
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  7
/* YYNRULES -- Number of rules.  */
#define YYNRULES  50
/* YYNRULES -- Number of states.  */
#define YYNSTATES  135

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   282

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      42,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    40,     2,     2,     2,    37,    28,     2,
      47,    48,    35,    33,    25,    34,     2,    36,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      29,    24,    30,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    43,     2,    44,    27,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    45,    26,    46,    41,     2,     2,     2,
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
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    31,
      32,    38,    39
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     8,    11,    15,    21,    27,    36,
      44,    55,    73,    92,   112,   138,   165,   193,   196,   198,
     202,   204,   208,   212,   216,   220,   224,   228,   232,   236,
     240,   244,   248,   252,   256,   260,   264,   268,   272,   276,
     279,   282,   285,   288,   292,   297,   302,   304,   306,   308,
     311
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      50,     0,    -1,    42,    -1,    51,    42,    -1,    50,    42,
      -1,    50,    51,    42,    -1,    55,     6,     3,    24,    54,
      -1,    55,     6,     3,    24,    11,    -1,    55,     6,     3,
      43,    54,    44,    24,    54,    -1,    55,     6,     3,    24,
      45,    53,    46,    -1,    55,     6,     3,    43,    54,    44,
      24,    45,    53,    46,    -1,    55,     7,     3,    10,    11,
      10,    52,    10,    52,    10,    52,    10,    52,    10,    11,
      10,    11,    -1,    55,     7,     3,    10,    11,    10,    52,
      10,    52,    10,    52,    10,    52,    10,    11,    10,    11,
      10,    -1,    55,     7,     3,    10,    11,    10,    52,    10,
      52,    10,    52,    10,    52,    10,    11,    10,    11,    10,
      52,    -1,    55,     8,     3,    10,    11,    10,    52,    10,
      52,    10,    52,    10,    52,    10,    11,    10,    52,    10,
      52,    10,    52,    10,    52,    10,    11,    -1,    55,     8,
       3,    10,    11,    10,    52,    10,    52,    10,    52,    10,
      52,    10,    11,    10,    52,    10,    52,    10,    52,    10,
      52,    10,    11,    10,    -1,    55,     8,     3,    10,    11,
      10,    52,    10,    52,    10,    52,    10,    52,    10,    11,
      10,    52,    10,    52,    10,    52,    10,    52,    10,    11,
      10,    52,    -1,    55,     9,    -1,    54,    -1,    54,    25,
      53,    -1,    54,    -1,    54,    18,    54,    -1,    54,    19,
      54,    -1,    54,    26,    54,    -1,    54,    27,    54,    -1,
      54,    28,    54,    -1,    54,    20,    54,    -1,    54,    21,
      54,    -1,    54,    22,    54,    -1,    54,    23,    54,    -1,
      54,    29,    54,    -1,    54,    30,    54,    -1,    54,    32,
      54,    -1,    54,    31,    54,    -1,    54,    33,    54,    -1,
      54,    34,    54,    -1,    54,    35,    54,    -1,    54,    36,
      54,    -1,    54,    37,    54,    -1,    34,    54,    -1,    33,
      54,    -1,    40,    54,    -1,    41,    54,    -1,    47,    54,
      48,    -1,     3,    43,    54,    44,    -1,     3,    47,    53,
      48,    -1,     3,    -1,     5,    -1,     5,    -1,    33,     5,
      -1,    34,     5,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint8 yyrline[] =
{
       0,    51,    51,    52,    53,    54,    57,    60,    63,    66,
      69,    72,    77,    82,    87,    92,    97,   102,   120,   122,
     123,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,   139,   140,   141,   142,   143,   144,
     145,   146,   147,   148,   149,   151,   153,   154,   157,   158,
     159
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "NAME", "NAME2", "NUM", "TDEF", "TH1",
  "TH2", "TCOM", "TSEP", "STR", "TUNDEF", "TNODE", "TUMIN", "TFUNC",
  "TARRAY", "TELM", "TLOR", "TLAND", "TEQ", "TNEQ", "TLE", "TGE", "'='",
  "','", "'|'", "'^'", "'&'", "'<'", "'>'", "TSR", "TSL", "'+'", "'-'",
  "'*'", "'/'", "'%'", "UPLUS", "UMINUS", "'!'", "'~'", "'\\n'", "'['",
  "']'", "'{'", "'}'", "'('", "')'", "$accept", "statement_list",
  "statement", "var", "elmnts", "expr", "number", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,    61,    44,   124,    94,    38,    60,
      62,   279,   280,    43,    45,    42,    47,    37,   281,   282,
      33,   126,    10,    91,    93,   123,   125,    40,    41
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    49,    50,    50,    50,    50,    51,    51,    51,    51,
      51,    51,    51,    51,    51,    51,    51,    51,    52,    53,
      53,    54,    54,    54,    54,    54,    54,    54,    54,    54,
      54,    54,    54,    54,    54,    54,    54,    54,    54,    54,
      54,    54,    54,    54,    54,    54,    54,    54,    55,    55,
      55
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     2,     2,     3,     5,     5,     8,     7,
      10,    17,    18,    19,    25,    26,    27,     2,     1,     3,
       1,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     2,
       2,     2,     2,     3,     4,     4,     1,     1,     1,     2,
       2
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,    48,     0,     0,     2,     0,     0,     0,    49,    50,
       1,     4,     0,     3,     0,     0,     0,    17,     5,     0,
       0,     0,     0,     0,     0,     0,    46,    47,     7,     0,
       0,     0,     0,     0,     0,     6,     0,     0,     0,     0,
       0,    40,    39,    41,    42,     0,    20,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     9,     0,    43,    21,    22,    26,    27,    28,    29,
      23,    24,    25,    30,    31,    33,    32,    34,    35,    36,
      37,    38,     0,     0,    18,     0,    44,    45,    19,     0,
       8,     0,     0,     0,     0,     0,    10,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    11,     0,    12,     0,    13,     0,     0,     0,     0,
       0,     0,    14,    15,    16
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,     5,     6,    93,    45,    94,     7
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -56
static const yytype_int16 yypact[] =
{
      87,   -56,    -1,     0,   -56,    71,   -28,   108,   -56,   -56,
     -56,   -56,   -27,   -56,    20,    21,    22,   -56,   -56,   -21,
      34,    35,    55,    78,    43,    44,   -41,   -56,   -56,    78,
      78,    78,    78,    78,    78,   195,   135,    49,    51,    78,
      78,   -56,   -56,   -56,   -56,    16,   175,   112,    78,    78,
      78,    78,    78,    78,    78,    78,    78,    78,    78,    78,
      78,    78,    78,    78,    78,    78,    39,    78,    78,   155,
      19,   -56,    78,   -56,   214,   232,   302,   302,   309,   309,
     250,   268,   286,   309,   309,    73,    73,   -16,   -16,   -56,
     -56,   -56,    46,    54,   195,    63,   -56,   -56,   -56,    78,
     195,    78,    78,    36,    74,    75,   -56,    78,    78,    80,
      84,    78,    78,    88,    89,    86,    90,    93,   113,   111,
      78,   114,   116,    78,    78,   -56,   117,    78,   118,    78,
     126,   139,   127,    78,   -56
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
     -56,   -56,   146,   -55,   -24,   -22,   -56
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint8 yytable[] =
{
      35,    36,    39,    22,     8,     9,    40,    41,    42,    43,
      44,    46,    47,    95,    13,    18,    70,    69,    46,    63,
      64,    65,    23,    19,    20,    21,    74,    75,    76,    77,
      78,    79,    80,    81,    82,    83,    84,    85,    86,    87,
      88,    89,    90,    91,    24,    25,   104,   105,    98,    26,
      46,    27,   109,   110,    37,    38,   113,   114,    26,    67,
      27,    68,    71,    92,   101,   122,    28,    97,   125,   126,
     100,    10,   128,   102,   130,   103,     1,    46,   134,    29,
      30,    26,   106,    27,   107,   108,    31,    32,    29,    30,
     111,    99,     1,    34,   112,    31,    32,   117,   115,   116,
      33,   118,    34,   119,     2,     3,    61,    62,    63,    64,
      65,    29,    30,    11,    14,    15,    16,    17,    31,    32,
       2,     3,   121,   120,   123,    34,   124,   127,   129,     4,
      48,    49,    50,    51,    52,    53,   131,   133,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
     132,    12,     0,    48,    49,    50,    51,    52,    53,     0,
      73,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    48,    49,    50,    51,    52,    53,    66,
       0,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    48,    49,    50,    51,    52,    53,    96,
      72,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    48,    49,    50,    51,    52,    53,     0,
       0,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    49,    50,    51,    52,    53,     0,     0,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    50,    51,    52,    53,     0,     0,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      50,    51,    52,    53,     0,     0,     0,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    50,    51,
      52,    53,     0,     0,     0,     0,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    50,    51,    52,    53,
       0,     0,     0,     0,     0,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    52,    53,     0,     0,     0,     0,
       0,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      59,    60,    61,    62,    63,    64,    65
};

static const yytype_int16 yycheck[] =
{
      22,    23,    43,    24,     5,     5,    47,    29,    30,    31,
      32,    33,    34,    68,    42,    42,    40,    39,    40,    35,
      36,    37,    43,     3,     3,     3,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    10,    10,   101,   102,    72,     3,
      72,     5,   107,   108,    11,    11,   111,   112,     3,    10,
       5,    10,    46,    24,    10,   120,    11,    48,   123,   124,
      92,     0,   127,    10,   129,    99,     5,    99,   133,    33,
      34,     3,    46,     5,    10,    10,    40,    41,    33,    34,
      10,    45,     5,    47,    10,    40,    41,    11,    10,    10,
      45,    11,    47,    10,    33,    34,    33,    34,    35,    36,
      37,    33,    34,    42,     6,     7,     8,     9,    40,    41,
      33,    34,    11,    10,    10,    47,    10,    10,    10,    42,
      18,    19,    20,    21,    22,    23,    10,    10,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      11,     5,    -1,    18,    19,    20,    21,    22,    23,    -1,
      48,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    18,    19,    20,    21,    22,    23,    44,
      -1,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    18,    19,    20,    21,    22,    23,    44,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    18,    19,    20,    21,    22,    23,    -1,
      -1,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    19,    20,    21,    22,    23,    -1,    -1,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    20,    21,    22,    23,    -1,    -1,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      20,    21,    22,    23,    -1,    -1,    -1,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    20,    21,
      22,    23,    -1,    -1,    -1,    -1,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    20,    21,    22,    23,
      -1,    -1,    -1,    -1,    -1,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    22,    23,    -1,    -1,    -1,    -1,
      -1,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      31,    32,    33,    34,    35,    36,    37
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     5,    33,    34,    42,    50,    51,    55,     5,     5,
       0,    42,    51,    42,     6,     7,     8,     9,    42,     3,
       3,     3,    24,    43,    10,    10,     3,     5,    11,    33,
      34,    40,    41,    45,    47,    54,    54,    11,    11,    43,
      47,    54,    54,    54,    54,    53,    54,    54,    18,    19,
      20,    21,    22,    23,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    44,    10,    10,    54,
      53,    46,    25,    48,    54,    54,    54,    54,    54,    54,
      54,    54,    54,    54,    54,    54,    54,    54,    54,    54,
      54,    54,    24,    52,    54,    52,    44,    48,    53,    45,
      54,    10,    10,    53,    52,    52,    46,    10,    10,    52,
      52,    10,    10,    52,    52,    10,    10,    11,    11,    10,
      10,    11,    52,    10,    10,    52,    52,    10,    52,    10,
      52,    10,    11,    10,    52
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
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


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

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


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
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


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
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


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

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

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

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


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

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
  /* The symbols being reduced.  */
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

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
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
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
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
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
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
	    /* Fall through.  */
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

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
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

/* Prevent warnings from -Wmissing-prototypes.  */
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */


/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*-------------------------.
| yyparse or yypush_parse.  |
`-------------------------*/

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
#else /* ! YYPARSE_PARAM */
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
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */
  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
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
#endif /* no yyoverflow */

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

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
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

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:

/* Line 1455 of yacc.c  */
#line 51 "src/freadin.y"
    {;}
    break;

  case 3:

/* Line 1455 of yacc.c  */
#line 52 "src/freadin.y"
    {;}
    break;

  case 4:

/* Line 1455 of yacc.c  */
#line 53 "src/freadin.y"
    {;}
    break;

  case 5:

/* Line 1455 of yacc.c  */
#line 54 "src/freadin.y"
    {;}
    break;

  case 6:

/* Line 1455 of yacc.c  */
#line 57 "src/freadin.y"
    {
						 def((yyvsp[(1) - (5)].vrecp),(yyvsp[(3) - (5)].vrecp),0,(yyvsp[(5) - (5)].vrecp));
             ;}
    break;

  case 7:

/* Line 1455 of yacc.c  */
#line 60 "src/freadin.y"
    {
						 def((yyvsp[(1) - (5)].vrecp),(yyvsp[(3) - (5)].vrecp),0,(yyvsp[(5) - (5)].vrecp));
             ;}
    break;

  case 8:

/* Line 1455 of yacc.c  */
#line 63 "src/freadin.y"
    {
						 def((yyvsp[(1) - (8)].vrecp),(yyvsp[(3) - (8)].vrecp),(yyvsp[(5) - (8)].vrecp),(yyvsp[(8) - (8)].vrecp));
             ;}
    break;

  case 9:

/* Line 1455 of yacc.c  */
#line 66 "src/freadin.y"
    {
						 def((yyvsp[(1) - (7)].vrecp),(yyvsp[(3) - (7)].vrecp),0,(yyvsp[(6) - (7)].vrecp));
             ;}
    break;

  case 10:

/* Line 1455 of yacc.c  */
#line 69 "src/freadin.y"
    {
						 def((yyvsp[(1) - (10)].vrecp),(yyvsp[(3) - (10)].vrecp),(yyvsp[(5) - (10)].vrecp),(yyvsp[(9) - (10)].vrecp));
             ;}
    break;

  case 11:

/* Line 1455 of yacc.c  */
#line 74 "src/freadin.y"
    {
						 hist1((yyvsp[(1) - (17)].vrecp),(yyvsp[(3) - (17)].vrecp),(yyvsp[(5) - (17)].vrecp),(yyvsp[(7) - (17)].vrecp),(yyvsp[(9) - (17)].vrecp),(yyvsp[(11) - (17)].vrecp),(yyvsp[(13) - (17)].vrecp),(yyvsp[(15) - (17)].vrecp),(yyvsp[(17) - (17)].vrecp),0);
             ;}
    break;

  case 12:

/* Line 1455 of yacc.c  */
#line 79 "src/freadin.y"
    {
						 hist1((yyvsp[(1) - (18)].vrecp),(yyvsp[(3) - (18)].vrecp),(yyvsp[(5) - (18)].vrecp),(yyvsp[(7) - (18)].vrecp),(yyvsp[(9) - (18)].vrecp),(yyvsp[(11) - (18)].vrecp),(yyvsp[(13) - (18)].vrecp),(yyvsp[(15) - (18)].vrecp),(yyvsp[(17) - (18)].vrecp),0);
             ;}
    break;

  case 13:

/* Line 1455 of yacc.c  */
#line 84 "src/freadin.y"
    {
						 hist1((yyvsp[(1) - (19)].vrecp),(yyvsp[(3) - (19)].vrecp),(yyvsp[(5) - (19)].vrecp),(yyvsp[(7) - (19)].vrecp),(yyvsp[(9) - (19)].vrecp),(yyvsp[(11) - (19)].vrecp),(yyvsp[(13) - (19)].vrecp),(yyvsp[(15) - (19)].vrecp),(yyvsp[(17) - (19)].vrecp),(yyvsp[(19) - (19)].vrecp));
             ;}
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 89 "src/freadin.y"
    {
						 hist2((yyvsp[(1) - (25)].vrecp),(yyvsp[(3) - (25)].vrecp),(yyvsp[(5) - (25)].vrecp),(yyvsp[(7) - (25)].vrecp),(yyvsp[(9) - (25)].vrecp),(yyvsp[(11) - (25)].vrecp),(yyvsp[(13) - (25)].vrecp),(yyvsp[(15) - (25)].vrecp),(yyvsp[(17) - (25)].vrecp),(yyvsp[(19) - (25)].vrecp),(yyvsp[(21) - (25)].vrecp),(yyvsp[(23) - (25)].vrecp),(yyvsp[(25) - (25)].vrecp),0);
             ;}
    break;

  case 15:

/* Line 1455 of yacc.c  */
#line 94 "src/freadin.y"
    {
						 hist2((yyvsp[(1) - (26)].vrecp),(yyvsp[(3) - (26)].vrecp),(yyvsp[(5) - (26)].vrecp),(yyvsp[(7) - (26)].vrecp),(yyvsp[(9) - (26)].vrecp),(yyvsp[(11) - (26)].vrecp),(yyvsp[(13) - (26)].vrecp),(yyvsp[(15) - (26)].vrecp),(yyvsp[(17) - (26)].vrecp),(yyvsp[(19) - (26)].vrecp),(yyvsp[(21) - (26)].vrecp),(yyvsp[(23) - (26)].vrecp),(yyvsp[(25) - (26)].vrecp),0);
             ;}
    break;

  case 16:

/* Line 1455 of yacc.c  */
#line 99 "src/freadin.y"
    {
						 hist2((yyvsp[(1) - (27)].vrecp),(yyvsp[(3) - (27)].vrecp),(yyvsp[(5) - (27)].vrecp),(yyvsp[(7) - (27)].vrecp),(yyvsp[(9) - (27)].vrecp),(yyvsp[(11) - (27)].vrecp),(yyvsp[(13) - (27)].vrecp),(yyvsp[(15) - (27)].vrecp),(yyvsp[(17) - (27)].vrecp),(yyvsp[(19) - (27)].vrecp),(yyvsp[(21) - (27)].vrecp),(yyvsp[(23) - (27)].vrecp),(yyvsp[(25) - (27)].vrecp),(yyvsp[(27) - (27)].vrecp));
             ;}
    break;

  case 17:

/* Line 1455 of yacc.c  */
#line 102 "src/freadin.y"
    {com((yyvsp[(1) - (2)].vrecp));;}
    break;

  case 18:

/* Line 1455 of yacc.c  */
#line 120 "src/freadin.y"
    { (yyval.vrecp) = (yyvsp[(1) - (1)].vrecp); ;}
    break;

  case 19:

/* Line 1455 of yacc.c  */
#line 122 "src/freadin.y"
    { (yyval.vrecp) = add_node(TELM,(yyvsp[(1) - (3)].vrecp),(yyvsp[(3) - (3)].vrecp)); ;}
    break;

  case 20:

/* Line 1455 of yacc.c  */
#line 123 "src/freadin.y"
    { (yyval.vrecp) = (yyvsp[(1) - (1)].vrecp); ;}
    break;

  case 21:

/* Line 1455 of yacc.c  */
#line 126 "src/freadin.y"
    { (yyval.vrecp) = add_node(TLOR,(yyvsp[(1) - (3)].vrecp),(yyvsp[(3) - (3)].vrecp)); ;}
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 127 "src/freadin.y"
    { (yyval.vrecp) = add_node(TLAND,(yyvsp[(1) - (3)].vrecp),(yyvsp[(3) - (3)].vrecp)); ;}
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 128 "src/freadin.y"
    { (yyval.vrecp) = add_node('|',(yyvsp[(1) - (3)].vrecp),(yyvsp[(3) - (3)].vrecp)); ;}
    break;

  case 24:

/* Line 1455 of yacc.c  */
#line 129 "src/freadin.y"
    { (yyval.vrecp) = add_node('^',(yyvsp[(1) - (3)].vrecp),(yyvsp[(3) - (3)].vrecp)); ;}
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 130 "src/freadin.y"
    { (yyval.vrecp) = add_node('&',(yyvsp[(1) - (3)].vrecp),(yyvsp[(3) - (3)].vrecp)); ;}
    break;

  case 26:

/* Line 1455 of yacc.c  */
#line 131 "src/freadin.y"
    { (yyval.vrecp) = add_node(TEQ,(yyvsp[(1) - (3)].vrecp),(yyvsp[(3) - (3)].vrecp)); ;}
    break;

  case 27:

/* Line 1455 of yacc.c  */
#line 132 "src/freadin.y"
    { (yyval.vrecp) = add_node(TNEQ,(yyvsp[(1) - (3)].vrecp),(yyvsp[(3) - (3)].vrecp)); ;}
    break;

  case 28:

/* Line 1455 of yacc.c  */
#line 133 "src/freadin.y"
    { (yyval.vrecp) = add_node(TLE,(yyvsp[(1) - (3)].vrecp),(yyvsp[(3) - (3)].vrecp)); ;}
    break;

  case 29:

/* Line 1455 of yacc.c  */
#line 134 "src/freadin.y"
    { (yyval.vrecp) = add_node(TGE,(yyvsp[(1) - (3)].vrecp),(yyvsp[(3) - (3)].vrecp)); ;}
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 135 "src/freadin.y"
    { (yyval.vrecp) = add_node('<',(yyvsp[(1) - (3)].vrecp),(yyvsp[(3) - (3)].vrecp)); ;}
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 136 "src/freadin.y"
    { (yyval.vrecp) = add_node('>',(yyvsp[(1) - (3)].vrecp),(yyvsp[(3) - (3)].vrecp)); ;}
    break;

  case 32:

/* Line 1455 of yacc.c  */
#line 137 "src/freadin.y"
    { (yyval.vrecp) = add_node(TSL,(yyvsp[(1) - (3)].vrecp),(yyvsp[(3) - (3)].vrecp)); ;}
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 138 "src/freadin.y"
    { (yyval.vrecp) = add_node(TSR,(yyvsp[(1) - (3)].vrecp),(yyvsp[(3) - (3)].vrecp)); ;}
    break;

  case 34:

/* Line 1455 of yacc.c  */
#line 139 "src/freadin.y"
    { (yyval.vrecp) = add_node('+',(yyvsp[(1) - (3)].vrecp),(yyvsp[(3) - (3)].vrecp)); ;}
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 140 "src/freadin.y"
    { (yyval.vrecp) = add_node('-',(yyvsp[(1) - (3)].vrecp),(yyvsp[(3) - (3)].vrecp)); ;}
    break;

  case 36:

/* Line 1455 of yacc.c  */
#line 141 "src/freadin.y"
    { (yyval.vrecp) = add_node('*',(yyvsp[(1) - (3)].vrecp),(yyvsp[(3) - (3)].vrecp)); ;}
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 142 "src/freadin.y"
    { (yyval.vrecp) = add_node('/',(yyvsp[(1) - (3)].vrecp),(yyvsp[(3) - (3)].vrecp)); ;}
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 143 "src/freadin.y"
    { (yyval.vrecp) = add_node('%',(yyvsp[(1) - (3)].vrecp),(yyvsp[(3) - (3)].vrecp)); ;}
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 144 "src/freadin.y"
    { (yyval.vrecp) = add_node(TUMIN,(yyvsp[(2) - (2)].vrecp),0); ;}
    break;

  case 40:

/* Line 1455 of yacc.c  */
#line 145 "src/freadin.y"
    { (yyval.vrecp) = (yyvsp[(2) - (2)].vrecp); ;}
    break;

  case 41:

/* Line 1455 of yacc.c  */
#line 146 "src/freadin.y"
    { (yyval.vrecp) = add_node('!',(yyvsp[(2) - (2)].vrecp),0); ;}
    break;

  case 42:

/* Line 1455 of yacc.c  */
#line 147 "src/freadin.y"
    { (yyval.vrecp) = add_node('~',(yyvsp[(2) - (2)].vrecp),0); ;}
    break;

  case 43:

/* Line 1455 of yacc.c  */
#line 148 "src/freadin.y"
    { (yyval.vrecp) = (yyvsp[(2) - (3)].vrecp); ;}
    break;

  case 44:

/* Line 1455 of yacc.c  */
#line 149 "src/freadin.y"
    { (yyval.vrecp) = add_node(TARRAY,(yyvsp[(1) - (4)].vrecp),(yyvsp[(3) - (4)].vrecp)); ;}
    break;

  case 45:

/* Line 1455 of yacc.c  */
#line 151 "src/freadin.y"
    { (yyval.vrecp) = add_node(TFUNC,(yyvsp[(1) - (4)].vrecp),(yyvsp[(3) - (4)].vrecp)); ;}
    break;

  case 46:

/* Line 1455 of yacc.c  */
#line 153 "src/freadin.y"
    { (yyval.vrecp) = (yyvsp[(1) - (1)].vrecp); (yyvsp[(1) - (1)].vrecp)->vtype = NAME2; ;}
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 154 "src/freadin.y"
    { (yyval.vrecp) = (yyvsp[(1) - (1)].vrecp); ;}
    break;

  case 48:

/* Line 1455 of yacc.c  */
#line 157 "src/freadin.y"
    { (yyval.vrecp) = (yyvsp[(1) - (1)].vrecp); ;}
    break;

  case 49:

/* Line 1455 of yacc.c  */
#line 158 "src/freadin.y"
    { (yyval.vrecp) = (yyvsp[(2) - (2)].vrecp); ;}
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 159 "src/freadin.y"
    { (yyval.vrecp) = (yyvsp[(2) - (2)].vrecp); (yyval.vrecp)->dval = -(yyval.vrecp)->dval; ;}
    break;



/* Line 1455 of yacc.c  */
#line 1888 "freadin.tab.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
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

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
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
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}



/* Line 1675 of yacc.c  */
#line 161 "src/freadin.y"

extern int   line;
extern char  *yytext;

/* yyerror */
int yyerror(s)
		 char  *s;
{
	extern int parse_error;
	showerr("line %d: parse error at '%s'.\n", line, yytext);
	parse_error++;
  return 0;
}

/* new_vrecp --- create new variable table */
vrec_p new_vrecp()
{
	vrec_p  vp;
	vp = (vrec_p)malloc(sizeof(struct vrec));
	if(vp==(vrec_p)NULL){
		showerr("new_vrecp: malloc: %s", strerror(errno));
		exit(1);
	}
	vp->vtype = TUNDEF;
	vp->vref = 0;
	dr_set_nothing(&vp->dval);
	vp->sval = (char*)NULL;
	vp->arg1 = vp->arg2 = (vrec_p)NULL;
	vp->eval = EVAL_NONE;
	vp->neval = 0;
	return(vp);
}
		
/* free_vrecp --- free variable table */
void free_vrecp(v)
		 vrec_p v;
{
	vrec_p  v2;
	if(!v) return;
	if((v2=v->arg1)){
		v->arg1 = (vrec_p)NULL;
		free_vrecp(v2);
	}
	if((v2=v->arg2)){
		v->arg2 = (vrec_p)NULL;
		free_vrecp(v2);
	}
	if(v->sval){
		free(v->sval);
		v->sval = (char*)NULL;
	}
	free(v);
}
		
/* str_token --- get string of the token */
char *str_token(vtype)
		 int vtype;
{
	static char str[256];
	switch(vtype){
	case NUM:     return("NUM");
	case STR:     return("STR");
	case NAME:   	return("NAME");
	case NAME2:  	return("NAME2");
	case '!':     return("!");
	case '~':     return("~");
	case '|':     return("|");
	case '^':     return("^");
	case '&':     return("&");
	case '<':     return("<");
	case '>':     return(">");
	case TSL:     return("<<");
	case TSR:     return(">>");
	case '+':     return("+");
	case '-':     return("-");
	case '*':     return("*");
	case '/':     return("/");
	case '%':     return("%");
	case '=':     return("=");
	case TUMIN:   return("MINUS");
	case TLOR:    return("||");
	case TLAND:   return("&&");
	case TEQ:     return("==");
	case TNEQ:    return("!=");
	case TLE:     return("<=");
	case TGE:     return(">=");
	case TFUNC:   return("FUNCTION");
	case TNODE:   return("NODE");
	case TUNDEF:  return("UNDEFINED");
	case TARRAY:  return("ARRAY");
	case TELM:    return("ELEMENT");
	default:      return("UNKOWN");
	}
	return((char*)str);
}

/* str_vtype --- get string of the vrecp */
char *str_vtype(v)
		 vrec_p  v;
{
	static char str[256];
	switch(v->vtype){
	case NAME:
	case NAME2:
		if(v->sval){
			sprintf(str,"(%s)",v->sval);
			return(str);
		}
		else
			return(str_token(v->vtype));
	case TARRAY:
	case TFUNC:
		if(v->arg1->sval){
			sprintf(str,"(%s)",v->arg1->sval);
			return(str);
		}
		else
			return(str_token(v->vtype));
	case NUM:
		sprintf(str,"%5.3f",v->dval);
		return(str);
	default:
		return(str_token(v->vtype));
	}
}

/* add_node --- add a node to variable tree */
vrec_p add_node(ope, arg1, arg2)
		 int      ope;
		 vrec_p   arg1, arg2;
{
	vrec_p  vp;
#if DEBUG_YACC
	fprintf(stderr, "add_node: token: %s\n", str_token(ope));
#endif
	vp = new_vrecp();
	vp->vtype = TNODE;
	switch(ope){
	case TUMIN:
	case '!':
	case '~':
		if(arg1==(vrec_p)NULL){
			showerr("add_node: illegal argument for token %s\n", str_token(ope));
			break;
		}
		vp->vtype = ope;
		vp->arg1 = arg1;
		break;
	case '|':
	case '^':
	case '&':
	case '<':
	case '>':
	case TSL:
	case TSR:
	case '+':
	case '-':
	case '*':
	case '/':
	case '%':
	case TLOR:
	case TLAND:
	case TEQ:
	case TNEQ:
	case TLE:
	case TGE:
	case '=':
	case TELM:
		if(arg1==(vrec_p)NULL || arg2==(vrec_p)NULL){
			showerr("add_node: illegal argument for token %s\n", str_token(ope));
			break;
		}
		vp->vtype = ope;
		vp->arg1 = arg1;
		vp->arg2 = arg2;
		break;
	case TNODE:
		showerr("add_node: illegal token %s.", str_token(ope));
		break;
	case TUNDEF:
		showerr("add_node: illegal token %s.", str_token(ope));
		break;
	case TFUNC:
		if(arg1==(vrec_p)NULL || arg2==(vrec_p)NULL){
			showerr("add_node: illegal argument for token %s.", str_token(ope));
		  break;
		}
		vp->vtype = ope;
		vp->arg1 = arg1;
		vp->arg2 = arg2;
		break;
	case TARRAY:
		if(arg1==(vrec_p)NULL || arg2==(vrec_p)NULL){
			showerr("add_node: illegal argument for token %s.", str_token(ope));
		  break;
		}
		vp->vtype = ope;
		vp->arg1 = arg1;
		vp->arg2 = arg2;
		break;
	default:
		showerr("add_node: unknown token %d.\n", ope);
		break;
	}
	return(vp);
}
/*
Local Variables:
mode: C
tab-width: 2
End:
*/


