/* def.h */

typedef struct {
   char *name;
   unsigned int value;
} KWORD;

KWORD k_tab[]= {	/* keyword table */
   {	"auto", 	TYPEMOD	},
   {	"break", 	BREAK	},
   {	"case", 	CASE	},
   {	"char", 	TYPESPEC	},
   {	"const",	TYPEMOD	},
   {    "continue", 	CONTINUE},
   {	"default", 	DEFAULT_TOKEN	},
   {    "do",       	DO      },
   {	"double", 	TYPESPEC	},
   {	"else", 	ELSE	},
   {	"enum", 	ENUM	},
   {	"extern", 	TYPEMOD	},
   {	"float", 	TYPESPEC	},
   {    "for",      	FOR     },
   {	"goto", 	GOTO	},
   {    "if",       	IF      },
   {	"int", 		TYPESPEC	},
   {	"long", 	TYPEMOD	},
   {    "register", 	TYPESPEC  },
   {	"return", 	RETURN	},
   {	"short", 	TYPESPEC	},
   {	"signed", 	TYPEMOD	},
   {	"sizeof", 	SIZEOF	},
   {	"static", 	TYPEMOD	},
   {	"struct", 	STRUCT	},
   {	"switch", 	SWITCH	},
   {	"typedef", 	TYPEMOD	},
   {	"union", 	STRUCT	},
   {    "unsigned", 	TYPEMOD },
   {	"void", 	TYPESPEC	},
   {	"volatile",	TYPESPEC  },
   {	"while", 	WHILE	}
};   

int lineno = 0;

#define FALSE		0
#define TRUE		1

typedef struct {
   char         *lexptr;
   unsigned int token;
   union {
      int   ival;
      float fval;
      char  *sval;
   } attrib;
} sym_type;

sym_type sym_tbl[100];
int sym_tbl_index = 0;

