#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <windows.h>
#include <string.h>
#include <string>

using namespace std;

// ���� � ������ ������
enum
{
  _HELP,
  _LOAD,
  _PRINT,
  _POLIZ,
  _LEX,
  _WORD,
  _RUN,
  _DELIM,
  _COMPILE,
  _IDENT,
  _NUMBER,
  _STRING,
  _GRAMM,
  _EXIT
};

struct CMD
{
  int        code;
  const char *name;
};

CMD cmd[] =
{
  { _HELP,    "Help"    },
  { _LOAD,    "Load"    },
  { _PRINT,   "Print"   },
  { _POLIZ,   "Poliz"   },
  { _LEX,     "Lex"     },
  { _WORD,    "Word"    },
  { _RUN,     "Run"     },
  { _DELIM,   "Delim"   },
  { _COMPILE, "Compile" },
  { _IDENT,   "Ident"   },
  { _NUMBER,  "Number"  },
  { _STRING,  "String"  },
  { _GRAMM,   "Gramm"   },
  { _EXIT,    "Exit"    }
};

// ������ ���������� ������� � �����
// ����� ������������� �����
char sym_point = '.';

// ���������� ������������� �������� ����������
char str_value[100];

// ����������� ����������
string name_var;

// ����������� �����
string comment_var;

// ��� ����������
int type;

// ��������� �����
double value_input;

// ���� ������ � ��������
enum
{
  INTEGER,   REAL,      LOGIC,     SEMICOLON,
  OR,        AND,       NOT,       SET,
  IF,        ELSE,      FOR,       TO,
  NEXT,      WHILE,     READLN,    WRITE,
  WRITELN,   NE,        EQ,        LT,
  LE,        GT,        GE,        ADD,
  SUB,       MUL,       DIV,       MOD,
  LEFT,      RIGHT,     LEFT_FIG,  RIGHT_FIG,
  COMMA,     COLON,     END_FILE,  NUM,
  NUMD,      IDENT,     STR,       EXIT,
  BREAK,     CONTINUE,  BEGIN,     END,
  WORK,      WORKD,     NEG,       PAUSE_STR,
  OUTN,      OUTLN,     GOTO,      READLN_STR,
  TEST,      ERASE,     DIM,       FOR_NO_STEP,
  FOR_STEP,  RAND,      RAND_REAL, STEP,
  OUT_,      OP,        PAUSE,     VARIABLE
};

// ��������� �������� ������� ��������������
struct WORD_
{
  const char *txt;
  int        key;
};

// ��������� �������� ������� ������������
struct DELIM
{
  char c;
  int  key;
};

// ������� �������� ���������� (���� true)
bool ok_compile = false;

// ������� �������� ������
int level;

// ������������ ������� �������� ������
#define MAX_LEVEL 10

// ������������ ���������� break �� ����� ������ �����
#define MAX_BREAK 30

// ��������� �����
WORD_ word[] =
{
  "variable",  VARIABLE,
  "dim",       DIM,
  "if",        IF,
  "else",      ELSE,
  "for",       FOR,
  "to",        TO,
  "step",      STEP,
  "next",      NEXT,
  "while",     WHILE,
  "begin",     BEGIN,
  "end",       END,
  "readln",    READLN,
  "write",     WRITE,
  "writeln",   WRITELN,
  "exit",      EXIT,
  "erase",     ERASE,
  "pause",     PAUSE,
  "break",     BREAK,
  "continue",  CONTINUE,
  "rand",      RAND,
  "rand_real", RAND_REAL,
  "integer",   INTEGER,
  "real",      REAL,
  "boolean",   LOGIC,
  NULL,        -1
};

// ����������� (�� ���, � ������ ��������������)
DELIM delim[] =
{
  '(', LEFT,
  ')', RIGHT,
  '{', LEFT_FIG,
  '}', RIGHT_FIG,
  ';', SEMICOLON,
  '+', ADD,
  '-', SUB,
  '*', MUL,
  '%', MOD,
  ',', COMMA,
  0,   -1
};

// ������� ������
struct POLIZ
{
  // 0 - �������, 1 - ��������
  char op;

  // ��� ��������               - ��������������� ��������
  // ��� ������� ����������     - IDENT
  // ��� ����� ���������        - NUM
  // ��� ������������ ��������� - NUMD
  int t;

  // ��� �������� - ������ � ������� ��� ����� ������� ����������
  int n;
};

// ������������ ������ ������
#define MAX_POLIZ 2000

// ���������� ��� �����
POLIZ poliz[MAX_POLIZ];

// ������ ������
int ind_poliz;
int ind_poliz2;

// �������������� ������ ������
string result_print;

// �������� � ��� �������� ����� ���������
struct VT
{
  int    type_elem;
  double value_elem;
};

// ������� ����� ���������
struct STACK_OP
{
  // ��� ������� ����������              - IDENT
  // ��� ����� ������� ����������        - WORK
  // ��� ������������ ������� ���������� - WORKD
  // ��� ����� ���������                 - NUM
  // ��� ������������ ���������          - NUMD
  int t;

  // ������ ������� ���������� ��� ��������� � ��������������� �������
  int n;

  // �������� ������� ����������
  double val;
};

// ������������ ���������� �����
#define MAX_NUMBER  100

// ������������ ���������� ���������������
#define MAX_VAR     100

// ������������ ���������� ���������� �����
#define MAX_SYM_STR 100

// ������������ ������ ����� ���������
#define MAX_STACK_OP 50

// ���������� ��� ���� ���������
STACK_OP stack_op[MAX_STACK_OP];

// ������ ����� ���������
int ind_stack_op;

// ����������, ����������� �� ����� ���������
VT vt1, vt2;

// ������ �������� ������ �������� ������ �� �����
int ind_test_loop[MAX_LEVEL + 1];

// ������ ������� ���������� ��������� ������
int ind_var_for[MAX_LEVEL + 1];

// ������� �������� ������� ���������� for
int level_for;

// ������ �������� ���������� break
// �� ������ ������ ����� ������� ���� ���������� break,
// � ����� �� ������� � ������
int ind_break_loop[MAX_LEVEL][MAX_BREAK + 1];

// ������� �����
string number[MAX_NUMBER];
double number_bin[MAX_NUMBER];

// ���������� ����������
int cnt_var;

// ������� ����������
string var[MAX_VAR];

// �������� ����������
double value_var[MAX_VAR];

// ��� ����������
int type_var[MAX_VAR];

// �������� ������������� ����������
char yes_var[MAX_VAR];

// ������ ���������� � ������� ���������������
int ind_var;

// ������� ���������� �����
string sym_str[MAX_SYM_STR];

// ��������� �������� ������ � ����������
struct RECORD
{
  // ������ ���������
  string line;

  // ��������� �� ��������� ������� ��� NULL ��� ����������
  RECORD *next;
};

// ����� ������ �����
class List
{
  // ����� ������� �������� ������
  RECORD *first;

  // ����� ���������� �������� ������
  RECORD *last;

  // ��������� �������� �������� ������
  RECORD *cur;

public:

  // ����������� ������� ������ ������
  List(void)
  {
    first = last = NULL;
    Count = 0;
  }

  // ���������� ������� ������
  ~List()
  {
    erase();
  }

  // �������� ������
  void erase(void)
  {
    while (first)
    {
        last = first->next;
        delete first;
        first = last;
    }
    Count = 0;
  }

  // ���������� ��������� ������
  int Count;

  // ������������ �� ������ ������
  void Begin(void)
  {
    cur = first;
  }

  // �������� ��������� ������� ������
  string Get(void)
  {
    string str = cur->line;
    cur = cur->next;
    return str;
  }

  // �������� ������� � ������
  void Add(string str)
  {
    if (Count == 0)
    {
      first = last = new RECORD;
    }
    else
    {
      last->next = new RECORD;
      last = last->next;
    }
    last->line = str;
    last->next = NULL;
    ++Count;
  }
};

// ����� ������������� ������
class Lex
{

private:

  // ��������� ��������� �������� ������������ �����������
  enum
  {
    BeginLex,        // ������ �������
    ContinueIdent,   // ����������� ��������������
    ContinueNum,     // ����������� �����
    ContinueComment, // ����������� �������������� �����������
    BeginEndComment, // ������ ���������� �������������� �����������
    EndComment,      // ���������� �����������
    EndString,       // ���������� ������
    ContinueOr,      // ����������� ������� |
    ContinueAnd,     // ����������� ������� &
    ContinueSet,     // ����������� ������� :
    ContinueEq,      // ����������� ������� =
    ContinueNe,      // ����������� ������� !
    ContinueLt,      // ����������� ������� <
    ContinueGt       // ����������� ������� >
  };

  // ��������� ��������� ��������������
  // true, ���� ������������� ������������
  // ������ "dim", false - ������������ � ����������
  bool yes_dim;

  int        n2;

  // ��������� ������ ���������
  string str;

  // ������ ������ ������ ���������
  unsigned width;

  // true - ���� ��� ����� �������������
  bool new_ident;

  // ���������� ��������� ��������� �������
  int beg_num_save, beg_ind_save, len_lex_save;

  // ���������� ���������� � ������� �� ������ ��������� �������� ����������
  int beg_cnt_var;

  // ���������� �����
  int cnt_number;

  // ���������� ���������� �����
  int cnt_sym_str;

  // ������� ��������� ��������
  int state;

  // ����� ����������� ������
  string txt_error;

  // ������ ������� � �������� ������
  int ind;

  // ����� �������� ������
  int num;

  // �������� �������� ������ �����
  int val;

  // �������� �������� ������������� �����
  double vald;

  // ������� ����������� ������ �������� ���������
  char c;

  // ������� ������ ������� �� ����� (true - ���� ������ ��� �������� � 'c')
  bool yes_c;

  // ������� �������������
  string ident;

  // ������ ������ �������
  int beg_ind;

  // ����� �������
  int len_lex;

  // ����� ������ � ��������
  int beg_num;

  // �������� ���������
  List *re;

  // ������� -1 ��� ��� ��������������� �����������
  int get_delim (void)
  {
    char q;

    for (n2 = 0; (q = delim[n2].c) != c && q; ++n2)
      ;
    return delim[n2].key;
  }

  // ������� -1 ��� ��� ���������� �����
  int get_word (void)
  {
    const char *q, *s;

    s = ident.c_str();
    for (n2 = 0; (q = word[n2].txt) != NULL && strcmp (s, q); ++n2)
      ;
    return word[n2].key;
  }

  // ���� ������������������ �����
  void make16 (void)
  {
    const char *s;
    char       c;

    for (val = 0, s = ident.c_str(); (c = *s) != 0; ++s)
    {
      c = c <= '9' ? c - '0' : c - 'A' + 10;
      val = (val << 4) | c;
    }
  }

  // ���� ����������� �����
  void make10 (void)
  {
    val = atoi (ident.c_str());
  }

  // ���� ������������� �����
  void make_double (void)
  {
    vald = atof (ident.c_str());
  }

  // ���� ��������� ����� � ��������� �� ������
  int make2 (void)
  {
    const char *s;
    char       c;

    for (val = 0, s = ident.c_str(); (c = *s) != 0; ++s)
    {
      if (c > '1')
        er ("�������� ����� ��������");
      c -= '0';
      val = (val << 1) | c;
    }
    return NUM;
  }

  // ���� ������������� ����� � ��������� �� ������
  int make8 (void)
  {
    const char *s;
    char       c;

    for (val = 0, s = ident.c_str(); (c = *s) != 0; ++s)
    {
      if (c > '7')
        er ("������������ ����� ��������");
      c -= '0';
      val = (val << 3) | c;
    }
    return NUM;
  }

  // ��������� ������������� � ������� ����������;
  // ���� ��� �������� - return ������ �������, ����� -1
  int set_var (void)
  {
    // ����� �������������� � �������
    for (ind_var = 0; ind_var < cnt_var && var[ind_var] != ident; ++ind_var)
      ;

    // ����� ������?
    if (ind_var < cnt_var)
    {
      // ������������� � ����������� "dim"?
      if (yes_dim)
        er ("��������� ������������ ����������");

      // ���������� ������������
      yes_var[ind_var] = 1;
      return ind_var;
    }

    // ������������� � ����������?
    if (!yes_dim)
      er ("����������� ����������");

    // ��� ����� � �������?
    if (cnt_var == MAX_VAR)
      er ("������������ ������� ����������");

    // ��������� ������������� � �������
    var[cnt_var] = ident;
    yes_var[cnt_var] = 0;
    return ind_var = cnt_var++;
  }

  // ��������� ����� � ������� �����;
  // ���� ��� �������� - return ������ �������, ����� -1
  // t - INTEGER ��� REAL
  int set_number (int t)
  {
    // ����� ����� � �������
    for (ind_var = 0; ind_var < cnt_number && number[ind_var] != ident; ++ind_var)
      ;

    // ����� ������?
    if (ind_var < cnt_number)
      return ind_var;

    // ��� ����� � �������?
    if (cnt_number == MAX_NUMBER)
      return -1;

    // ��������� ����� � ���������� �������
    number[cnt_number] = ident;

    // ��������� �������� �������� � �������
    if (t == REAL)
      number_bin[cnt_number] = vald;
    else
      number_bin[cnt_number] = val;

    return ind_var = cnt_number++;
  }

  // ��������� ���������� ������ � ������� �����;
  // ���� ��� �������� - return ������ �������, ����� -1
  int set_sym_str (void)
  {
    // ����� ������ � �������
    for (ind_var = 0; ind_var < cnt_sym_str && sym_str[ind_var] != ident; ++ind_var)
      ;

    // ����� ������?
    if (ind_var < cnt_sym_str)
      return ind_var;

    // ��� ����� � �������?
    if (cnt_sym_str == MAX_SYM_STR)
      return -1;

    // ��������� ������ � �������
    sym_str[cnt_sym_str] = ident;

    return ind_var = cnt_sym_str++;
  }

  // ��������� ������� � ���������
  void out_result(int n1, int n2)
  {
    char str[100];

    sprintf(str, "(%d,%2d)", n1, n2);
    result += str;
    if (++ind_result == 6)
    {
      memo->Add(result);
      result = "";
      ind_result = 0;
      return;
    }
    result += " ";
  }

  // ������ ���������� ������� ���������
  // � ������ ����� ��������� ������������ 0
  // ��� ����� ������ ������������ '\n'
  char Input(void)
  {
    // ������ ��� �������� ��� ������������ ���������� �������?
    if (yes_c)
    {
      if (c)
        yes_c = false;
      return c;
    }

    // ���� ������ ��������� ������?
    if (ind == -1)
    {
      // ����� �������� ���������?
      if (num == re->Count)
        return (char)0;

      // �������� ��������� ������ ���������
      str = re->Get();

      // �������� ��
      printf("[%*d] %s\n", width, ++num, str.c_str());

      // ������������� ������ ������ ���������� ������� �������� ������
      ind = 0;
    }

    // ��������� ����� ������?
    if ((unsigned)ind == str.length())
    {
      // ������������� ������� ������ ��������� ������
      ind = -1;

      // ���������� ������� ����� ������
      return '\n';
    }

    // ���������� ��������� ������ ������
    ++len_lex;
    return str[ind++];
  }

public:

  // ��������� ������� � ����������� ������������ �������
  List *memo;

  // ������ ����������
  int ind_result;

  // ������ ����������
  string result;

  Lex (void)
  {
    memo = new List;
  }

  ~Lex (void)
  {
    delete memo;
  }

  // ������������� ������������ ������� ��� ��������� p
  void InitLex(List *p)
  {
    char txt[10];

    re = p;
    memo->erase();
    ind = -1;
    num = 0;

    // ���������� ������������ ������ ������ �������� ���������
    width = sprintf(txt, "%d", re->Count);

    // ��������������� �� ������ ������ ���������
    re->Begin();

    // ������ ������ ������ ���������
    yes_c       = false;
    c           = Input();
    yes_c       = true;

    state       = BeginLex;
    cnt_number  = 0;
    cnt_var     = 0;
    cnt_sym_str = 0;
    ind_result  = 0;
    yes_dim     = false;
  }

  // ������������� ������ �������� ����������
  void init_declare (void)
  {
    beg_cnt_var = cnt_var;
  }

  // ��������� ����� ���������� ����� ���������� ���������� ��������
  void set_declare (int type)
  {
    while (beg_cnt_var != cnt_var)
      type_var[beg_cnt_var++] = type;
    yes_dim = false;
  }

  // ������������ ������� ������������ �������
  void show_lex(void)
  {
    int i;

    memo->Begin();
    printf
    (
      "������ ������������ �������\n"
      "���������� � ������ ������� ��������� � ������� ������\n"
      "������ ����� � ������� - ����� �������:\n"
      "0 - ������� ��������� ����\n"
      "1 - ������� ������������\n"
      "2 - ������� �����\n"
      "3 - ������� ���������������\n"
      "4 - ������� �����\n"
      "������ ����� � ������� - ������� ������� � �������\n\n"
    );
    for (i = 0; i < memo->Count; ++i)
      printf("%s\n", memo->Get().c_str());
  }

  // ������������ ������� ���������������
  void show_ident(void)
  {
    int i;

    if (cnt_var == 0)
    {
      printf("��� �� ������ ��������������\n");
      return;
    }

    printf("������� ��������������� [3]\n");
    for (i = 0; i < cnt_var; ++i)
      printf("%2d: %s\n", i, var[i].c_str());
  }

  // ������������ ������� �����
  void show_num(void)
  {
    int i;

    if (cnt_number == 0)
    {
      printf("��� �� ������ �����\n");
      return;
    }

    printf("������� ����� [2]\n");
    for (i = 0; i < cnt_number; ++i)
      printf("%2d: %s\n", i, number[i].c_str());
  }

  // ������������ ������� �����
  void show_str(void)
  {
    int i;

    if (cnt_sym_str == 0)
    {
      printf("��� �� ����� ������\n");
      return;
    }

    printf("������� ����� [4]\n");
    for (i = 0; i < cnt_sym_str; ++i)
      printf("%2d: %s\n", i, sym_str[i].c_str());
  }

  // ��������� �������������� �������� �� ������
  void er (const char *s)
  {
    int i;

    txt_error = s;

    // ������������� ��������� � ����� ������ � ������ ��������� ���������
    for (i = 0; i < int(width + 3); ++i)
      printf(" ");
    for (s = str.c_str(), i = 1; i < beg_ind; ++i, ++s)
    {
      printf(*s == '\t' ? "\t" : " ");
    }
    printf("?\n");

    // �������� ��������� �� ������
    printf("%s\n", txt_error.c_str());

    // ������������ ������� ���������
    while (num != re->Count)
      printf("[%*d] %s\n", width, ++num, re->Get().c_str());
    throw 0;
  }

  // ������ �������� ���������
  int getNum (void)
  {
    return val;
  }

  // ��������� ��������� �������
  // ���� ������ ���, ��� ������� ���������� � type
  int lex (void)
  {
    int        offs, r;
    char       cc, c2[2];
    const char *s;

    c2[1] = 0;

    // ������� �� ���������� ��������
    for (;;)
    {
      switch (state)
      {

      case BeginLex:
        // ���������� ������ �������, ��������� � ��� �� ���������
        while ((c = Input()) == ' ' || c == '\t' || c == '\n')
          ;

        // ���������� ������ �������
        beg_ind = ind;
        beg_num = num;
        len_lex = 1;

        // ��������� ����� ���������?
        if (c == 0)
        {
          yes_c = true;
          return END_FILE;
        }

        // ��� ����������� ������������ ���������?
        if (c == '.')
        {
          ident = "0";

          // ��������� ��������� ������
          yes_c = true;

          // �������� ���������, ���������� ������
          state = ContinueNum;

          continue;
        }

        // ��� ������ �������� ���������?
        if (isdigit(c))
        {
          // �������� ���������, ���������� ������
          state = ContinueNum;

          // ������������� �������� �������� ������� �����
          ident = c;
          continue;
        }

        // ��� ������ ��������������?
        if (c == '_' || isalpha (c))
        {
          // �������� ���������, ���������� ������
          state = ContinueIdent;

          // ������������� �������� �������� ������� ��������������
          ident = c;
          continue;
        }

        // ������������� �������������� ������������
        state = BeginLex;
        if ((val = get_delim ()) >= 0)
        {
          yes_c = false;
          out_result (1, n2);
          return val;
        }

        // ������������� �����, �������������� ������������ � �����������
        switch (c)
        {

        case '"':
          ident = "";
          state = EndString;
          continue;

        case '/':
          state = ContinueComment;
          continue;

        case '<':
          state = ContinueLt;
          continue;

        case '>':
          state = ContinueGt;
          continue;

        case '=':
          state = ContinueEq;
          continue;

        case '!':
          state = ContinueNe;
          continue;

        case ':':
          state = ContinueSet;
          continue;

        case '|':
          state = ContinueOr;
          continue;

        case '&':
          state = ContinueAnd;
          continue;
        }

        er ("����������� ������");

      case ContinueSet:
        yes_c = false;
        state = BeginLex;
        if((c = Input()) == '=')
        {
          out_result (1, 17);
          return SET;
        }
        yes_c = true;
        out_result (1, 18);
        return COLON;

      case ContinueOr:
        yes_c = false;
        state = BeginLex;
        if((c = Input()) == '|')
        {
          out_result (1, 20);
          return OR;
        }

        er ("��������� '|'");

      case ContinueAnd:
        yes_c = false;
        state = BeginLex;
        if((c = Input()) == '&')
        {
          out_result (1, 22);
          return AND;
        }

        er ("��������� '&'");

      case ContinueEq:
        yes_c = false;
        state = BeginLex;
        if((c = Input()) == '=')
        {
          out_result (1, 10);
          return EQ;
        }

        er ("��������� '='");

      case ContinueLt:
        yes_c = false;
        state = BeginLex;
        if((c = Input()) == '=')
        {
          out_result (1, 15);
          return LE;
        }
        yes_c = true;
        out_result (1, 13);
        return LT;

      case ContinueGt:
        state = BeginLex;
        if ((c = Input()) == '=')
        {
          yes_c = false;
          out_result (1, 14);
          return GE;
        }
        yes_c = true;
        out_result (1, 12);
        return GT;

      case ContinueNe:
        state = BeginLex;
        if ((c = Input()) == '=')
        {
          yes_c = false;
          out_result (1, 11);
          return NE;
        }
        yes_c = true;
        out_result (1, 16);
        return NOT;

      case ContinueComment:
        switch (Input())
        {

        case '/':
          // ���������� ������ �� �����
          ind = -1;

          // ��������� ������ � ��������� �������
          state = BeginLex;
          continue;

        case '*':
          // ��������� � ��������� ������ ����� �������������� �����������
          state = BeginEndComment;
          continue;
        }

        yes_c = false;

        // �������� �������
        state = BeginLex;
        out_result (1, 6);
        return DIV;

      case BeginEndComment:
        // ������������ �����������?
        if ((c = Input()) == 0)
          er ("����������� ����� �������������� �����������");

        switch (c)
        {

        case '*':
          state = EndComment;
          break;

        case '\n':
          out_result (1, 0);
        }
        continue;

      case EndComment:
        // ������������ �����������?
        if ((c = Input()) == 0)
          er ("����������� ����� �������������� �����������");

        if (c == '*')
          continue;

        // ����� �����������?
        if (c == '/')
        {
          // ��������� ������ � ��������� �������
          state = BeginLex;
          continue;
        }

        // ���������� "������" ����� �����������
        state = BeginEndComment;
        continue;

      case EndString:
        // ������ ���������?
        if ((c = Input ()) == '"')
        {
          // �������� ������ � �������
          if ((r = set_sym_str ()) < 0)
            er ("������������ ������� ���������� �����");
          out_result (4, r);

          // ���������� �������
          state = BeginLex;
          return STR;
        }

        if (c == '\n' || c == 0)
          er ("������������� ������");

        // ���������� �������� ������
        ident += c;
        continue;

      case ContinueIdent:
        // �������� � ��� �� ��������� ��� ������������ ���������� �������� ��������������
        // ���������� �������� �������������
        if ((c = Input ()) == '_' || isdigit (c) || isalpha (c))
        {
          ident += c;
          continue;
        }

        // ������������� ����������, ������������� ��������� ��������� ��������
        state = BeginLex;
        yes_c = true;

        // ����� ��� ���������� ���������?
        if (ident == "false" || ident == "true")
        {
          // ������� false ��������� 0, true ��������� 1
          // �������� ����� ����� � �������
          val = ident == "true";
          if ((r = set_number (INTEGER)) < 0)
            er ("������������ ������� �����");
          out_result (3, r);

          // ���������� �������
          return NUM;
        }

        // ��� ������ ����������� ����������?
        if (ident == "dim")
          yes_dim = true;

        // ����� ��� ��������� �����?
        if ((r = get_word ()) >= 0)
        {
          out_result (0, n2);
          return r;
        }

        // ���, ��� ������� �������������, �������� ��� � �������
        if ((r = set_var ()) < 0)
          er ("������������ ������� ���������������");
        out_result (3, r);
        return IDENT;

      case ContinueNum:
        // �������� � ��� �� ��������� ��� ������������ ����
        // ���������� �������� �����
        if (isdigit (c = Input()))
        {
          ident += c;
          continue;
        }

        // ������������� ��������� ��������� ��������
        state = BeginLex;

        // ��� ����������������� �����?
        if (isxdigit (c))
        {
          offs = 0;

          // ����� ��� �������� 123E+ ��� ������������ �����?
          if (tolower (c) == 'e')
          {
            ident += 'e';

            if ((c = Input ()) == '+' || c == '-')
              goto lab1;

            // �������� �������� �� ������� 'e', ���� ���
            // ����� �� ����������������� �����
            offs = strlen (ident.c_str());
          }
          // ��������� ��� ������� ������������������ �����
          do
          {
            c2[0] = tolower (c);
            ident += c2;
          } while (isxdigit (c = Input()));

          // ����������������� ����� ������ ������������ ��������� H,h
          if (tolower (c) != 'h')
          {
            // ����� ��� ���� ������ ��������� �����?
            if (ident[1] == '0' || ident[1] == '1')
            {
              for (s = ident.c_str(); (cc = *++s) == '0' || cc == '1';)
                ;
              if ((cc == 'b' || cc == 'B') && s[1] == 0)
              {
                // ������� ������ B
                ident = ident.substr(0, ident.length() - 1);

                // ������� �������� �����
                make2 ();

                // �������� ����� ����� � �������
                ident += "b";
                if ((r = set_number (INTEGER)) < 0)
                  er ("������������ ������� �����");
                out_result (3, r);
                yes_c = true;

                // ���������� �������
                return NUM;
              }
            }

            // ����� ��� ���� ������ ����������� �����?
            if (isdigit (ident[1]))
            {
              for (s = ident.c_str(); isdigit (*++s);)
                ;
              if ((*s == 'd' || *s == 'D') && s[1] == 0)
              {
                // ������� ������ D
                ident = ident.substr(0, ident.length() - 1);

                // ������� ���������� �����
                make10 ();

                // �������� ����� ����� � �������
                ident += "d";
                if ((r = set_number (INTEGER)) < 0)
                  er ("������������ ������� �����");
                out_result (3, r);
                yes_c = true;

                // ���������� �������
                return NUM;
              }
            }

            // ����� ��� ���� ������ ������������� �����?
            if (offs)
            {
              s = ident.c_str() + offs;
              if (isdigit (*s))
              {
                while (isdigit (*++s))
                  ;
                if (*s == 0)
                {
                  // �������������, ��� ������ ���� 123E17
                  // ������� ������������ �����
                  make_double ();

                  // �������� ������������ ����� � �������
                  if ((r = set_number (REAL)) < 0)
                    er ("������������ ������� �����");
                  yes_c = true;
                  out_result (3, r);
                  return NUMD;
                }
              }
            }

            er ("��������� ������ ������������������ �����");
          }

          // ������� ����������������� �����
          make16 ();

          // �������� ����� ����� � �������
          ident += "h";
          if ((r = set_number (INTEGER)) < 0)
            er ("������������ ������� �����");
          out_result (3, r);
          return NUM;
        }

        switch (c = tolower (c))
        {

        // ��� ����� ������������������ �����?
        case 'h':
          // ������� ����������������� �����
          make16 ();

          // �������� ����� ����� � �������
          ident += c;
          if ((r = set_number (INTEGER)) < 0)
            er ("������������ ������� �����");
          out_result (3, r);

          // ���������� �������
          return NUM;

        // ��� ����� ��������� �����?
        case 'b':
          // ������� �������� ����� � ��������� �� ������
          make2 ();

          // �������� ����� ����� � �������
          ident += c;
          if ((r = set_number (INTEGER)) < 0)
            er ("������������ ������� �����");
          out_result (3, r);

          // ���������� �������
          return NUM;

        // ��� ����� ������������� �����?
        case 'o':
          // ������� ������������ ����� � ��������� �� ������
          make8 ();

          // �������� ����� ����� � �������
          ident += c;
          if ((r = set_number (INTEGER)) < 0)
            er ("������������ ������� �����");
          out_result (3, r);

          // ���������� �������
          return NUM;

        // ��� ����� ����������� �����?
        case 'd':
          // ������� ���������� �����
          make10 ();

          // �������� ����� ����� � �������
          ident += c;
          if ((r = set_number (INTEGER)) < 0)
            er ("������������ ������� �����");
          out_result (3, r);

          // ���������� �������
          return NUM;

        // ��� ������ ������� ����� ������������� �����?
        case '.':
          // �������� ������� �����
          ident += '.';
          if (!isdigit (c = Input ()))
            er ("��������� ������ ������������� �����");
          do
          {
            ident += c;
          } while (isdigit (c = Input ()));
          if (tolower (c) != 'e')
          {
            // ������� ������������ �����
            make_double ();
            yes_c = true;

            // �������� ������������ ����� � �������
            if ((r = set_number (REAL)) < 0)
              er ("������������ ������� �����");
            out_result (3, r);
            return NUMD;
          }

        case 'e':
          ident += 'e';
          if ((c = Input ()) == '+' || c == '-')
          {

lab1:

            ident += c;
            c = Input ();
          }
          if (!isdigit (c))
            er ("��������� ������ ������������� �����");
          do
          {
            ident += c;
          } while (isdigit (c = Input ()));

          // ������� ������������ �����
          make_double ();
          yes_c = true;

          // �������� ������������ ����� � �������
          if ((r = set_number (REAL)) < 0)
            er ("������������ ������� �����");
          out_result (3, r);
          return NUMD;
        }

        // ��� ������� ���������� �����
        make10 ();
        yes_c = true;

        // �������� ����� ����� � �������
        if ((r = set_number (INTEGER)) < 0)
          er ("������������ ������� �����");
        out_result (3, r);

        // ���������� �������
        return NUM;
      }
    }
  }
};

// ������ ������������ �������
Lex *Scan;

int lex;

// ��������� � ����� ��������� �������
void put_poliz (char op, int t, int n)
{
  // ���� ����� � ������?
  if (ind_poliz == MAX_POLIZ)
    Scan->er ("������������ ������");

  poliz[ind_poliz].op = op;
  poliz[ind_poliz].t  = t;
  poliz[ind_poliz].n  = n;
  ++ind_poliz;
}

// ���� ������� ������� �� t, �������� �� ������ s
void wait_lex (int t, const char *s)
{
  char txt[100];

  if (lex != t)
  {
    sprintf (txt, "���������: %s", s);
    Scan->er (txt);
  }
}

// ������ ��������� �������
int get_lex (void)
{
  return lex = Scan->lex ();
}

// ������ ��������� ������� � �������� �� �� END
int get_end (void)
{
  if ((lex = Scan->lex ()) == END_FILE)
    Scan->er ("����������� ����� ���������");
  return lex;
}

// ��������� �������, ����������� � ���������� p
// mess - �������� �������
void token (int p, const char *mess)
{
  get_end ();
  wait_lex (p, mess);
}

// ��������������� ���������� ������� ������������ ������
void expr1 (void);
void expr2 (void);
void expr3 (void);
void expr4 (void);
void expr5 (void);
void atom  (void);

// ���������� ���������
void expr (void)
{
  get_end ();
  expr1 ();
}

// �������� ���������
void expr1 (void)
{
  int w;

  expr2 ();
  if (lex == NE || lex == EQ || lex == LT || lex == GT || lex == LE || lex == GE)
  {
    w = lex;
    get_end ();
    expr1 ();

    // ��������� � ����� ��������������� ��������
    put_poliz (1, w, 0);
  }
}

// �������� ��������
void expr2 (void)
{
  int w;

  expr3 ();
  while (lex == ADD || lex == SUB || lex == OR)
  {
    w = lex;
    get_end ();
    expr3 ();

    // ��������� � ����� ��������������� ��������
    put_poliz (1, w, 0);
  }
}

// �������� ��������� � �������
void expr3 (void)
{
  int w;

  expr4 ();
  while (lex == MUL || lex == DIV || lex == MOD || lex == AND)
  {
    w = lex;
    get_end ();
    expr4 ();

    // ��������� � ����� ��������������� ��������
    put_poliz (1, w, 0);
  }
}

// ������� +, - ��� not
void expr4 (void)
{
  int w = -1;

  switch (lex)
  {

  case NOT:
    w = NOT;
    goto lab;

  case SUB:
    w = NEG;

  case ADD:

lab:

    get_end ();
  }

  expr5 ();

  // ��� ������������� ��������� � ����� NOT ��� NEG
  if (w >= 0)
    put_poliz (1, w, 0);
}

// ��������� � �������
void expr5 (void)
{
  if (lex == LEFT)
  {
    get_end ();
    expr1 ();
    if (lex != RIGHT)
      Scan->er ("������� ������ ������ ()");
    get_end ();
  }
  else
    atom ();
}

// ��������� �������������� ��� �����
void atom (void)
{
  // ��������� � ����� ������������� ��� �����
  put_poliz (0, lex, ind_var);

  switch (lex)
  {

  default:
    Scan->er ("���������: �������������, rand, rand_real ��� �����");

  case RAND:
  case RAND_REAL:
  case IDENT:
  case NUM:
  case NUMD:
    ;
  }

  get_end ();
}

void oper(void);

// ���������� ��������� ������������
void compile_set (void)
{
  // ��������� � ����� ���������� ����������
  put_poliz (0, IDENT, ind_var);

  // ��������� ���������
  expr ();
  wait_lex (SEMICOLON, ";");
  get_end ();

  // ��������� � ����� �������� ������������
  put_poliz (1, SET, 0);
}

// �������� if
void oper_if (void)
{
  int ind, ind2;

  token (LEFT, "(");
  expr ();

  // ��������� ������� ������ ������
  ind = ind_poliz;

  // ��������� � ����� �������� ��������� ��������
  // ���� ����� �������� �� ��������
  put_poliz (1, TEST, 0);
  wait_lex (RIGHT, ")");
  get_end ();
  oper ();
  if (lex == ELSE)
  {
    // ��������� ������� ������ ������
    ind2 = ind_poliz;

    // ��������� � ����� �������� ������������ ��������
    // ���� ����� �������� �� ��������
    put_poliz (1, GOTO, 0);

    // ��������� ����� �������� �� ��� ��� ������ if
    poliz[ind].n = ind_poliz;
    ind = ind2;
    get_end ();
    oper ();
  }

  // ��������� ������ ��������
  poliz[ind].n = ind_poliz;
}

// �������� continue
void oper_continue (void)
{
  if (level == 0)
    Scan->er ("�������� continue ��� �����");

  // �������� ������������ �������� �� ������������� �����
  put_poliz (1, GOTO, ind_test_loop[level - 1]);

  token (SEMICOLON, ";");
  get_end ();
}

// �������� break
void oper_break (void)
{
  int n;

  if (level == 0)
    Scan->er ("�������� break ��� �����");

  // �������� ������������ �������� ������ �� �����
  // ����� ������ ��� �� �����������, ����� ��������� ������������
  n = ++ind_break_loop[level - 1][0];

  // �� ������� �� ����� ���������� break?
  if (n == MAX_BREAK + 1)
    Scan->er ("������� ����� ���������� break ��� �������� �����");

  ind_break_loop[level - 1][n] = ind_poliz;

  // �������� ������ �� ����� (�� �� ����� ��������������)
  put_poliz (1, GOTO, 0);

  token (SEMICOLON, ";");
  get_end ();
}

// �������� while
void oper_while (void)
{
  int ind_test, ind_break;
  int i, *adr;

  if (level == MAX_LEVEL)
    Scan->er ("������� ������� ������� �������� ������");

  // ���� �� ������ ������ ����� ��� ���������� break;
  ind_break_loop[level][0] = 0;

  // ����������� ������� �������� ������
  ++level;

  token (LEFT, "(");

  // �������� ������ ������ ������ ������� ���������� �����
  ind_test_loop[level - 1] = ind_test = ind_poliz;

  expr ();

  // ����� �������� ������ �� �����
  ind_break = ind_poliz;

  // ��������� � ����� �������� ��������� ������ �� �����
  // ���� ����� �������� �� ��������
  put_poliz (1, TEST, 0);

  wait_lex (RIGHT, ")");
  get_end();
  oper ();

  // �������� ������������ �������� �� ������ �����
  put_poliz (1, GOTO, ind_test);

  // ��������� ������ ������ �� �����
  poliz[ind_break].n = ind_poliz;

  // ��������� ������� �������� ������
  --level;

  // ��������� ������� �������� ��� break
  adr = ind_break_loop[level];
  for (i = *adr; i; --i)
  {
    poliz[*++adr].n = ind_poliz;
  }
}

// �������� for
void oper_for (void)
{
  int ind_test, ind_break = -1, ind_next, ind_body;
  int i, *adr;

  if (level == MAX_LEVEL)
    Scan->er ("������� ������� ������� �������� ������");

  // ���� �� ������ ������ ����� ��� ���������� break;
  ind_break_loop[level][0] = 0;

  // ����������� �������� for
  token (IDENT, "�������������");

  // ���������� ����� ������ ���� �����
  if (type_var[ind_var] != INTEGER)
    Scan->er ("��������� ���������� ���� integer");

  // �������� �� �������� ���������� ��������� ������
  for (i = 0; i < level_for; ++i)
  {
    if (ind_var == ind_var_for[i])
      Scan->er ("�������� ���������� ���������� ����� for");
  }

  // �������� ���������� �����
  ind_var_for[level_for] = ind_var;

  // ������� ���� ������������
  token (SET, ":=");

  // ��������� � ����� ���������� ����������
  put_poliz (0, IDENT, ind_var);

  // ��������� ���������
  expr ();

  // ��������� � ����� �������� ������������
  put_poliz (1, SET, 0);

  wait_lex (TO, "to");

  // �������� ������ ������ ������ ������� ���������� �����
  ind_test = ind_poliz;

  expr ();

  // �������� � ����� ���������� ����� � �������� ��������� >=
  put_poliz (0, IDENT, ind_var_for[level_for]);
  put_poliz (1, GE, 0);

  // �������� ������ ������ c ������� �� �����
  ind_break = ind_poliz;

  // ��������� � ����� �������� ��������� ������ �� �����
  // ���� ����� �������� �� ��������
  put_poliz (1, TEST, 0);

  // ��������� � ����� �������� ������������ �������� �� ���� �����
  // ���� ����� �������� �� ��������
  ind_body = ind_poliz;
  put_poliz (1, GOTO, 0);
  ind_test_loop[level] = ind_next = ind_poliz;

  // ����� ��� ������������� ���������� �����?
  if (lex == STEP)
  {
    expr ();

    // �������� � ����� ���������� ����� � ��������
    // ��������� ���� ���������� �� �������� ���������
    put_poliz (0, IDENT, ind_var_for[level_for]);
    put_poliz (1, FOR_STEP, 0);
  }
  else
  {
    // �������� � ����� ���������� ����� � ��������
    // ��������� ���� ���������� �� 1 ��� ����������� step
    put_poliz (0, IDENT, ind_var_for[level_for]);
    put_poliz (1, FOR_NO_STEP, 0);
  }

  // ��������� � ����� �������� �������� �� ������ �������� �����
  put_poliz (1, GOTO, ind_test);

  // ��������� ������ �������� �� ���� �����
  poliz[ind_body].n = ind_poliz;

  // ����������� ������� �������� ������
  ++level;
  ++level_for;

  do
    oper ();
  while (lex != NEXT);
  get_end();

  // ��������� � ����� �������� �������� �� ������������� �����
  put_poliz (1, GOTO, ind_next);

  // ��� ��������� ����� ��������������� ����� ������ �� �����
  if (ind_break >= 0)
    poliz[ind_break].n = ind_poliz;

  // ��������� ������� �������� ������
  --level;
  --level_for;

  // ��������� ������� �������� ��� break
  adr = ind_break_loop[level];
  for (i = *adr; i; --i)
  {
    poliz[*++adr].n = ind_poliz;
  }
}

// �������� write
void oper_write (void)
{
  int n;

  get_end ();
  if (lex == SEMICOLON)
    Scan->er ("�������� write ������");
  for (;;)
  {
    if (lex == STR)
    {
      // ��������� � ����� ������� ������
      put_poliz (0, STR, ind_var);

      // ��������� � ����� �������� ������ ������
      put_poliz (1, STR, 0);
      get_end ();
    }
    else
    {
      expr1 ();
      if (lex == COLON)
      {
        token (NUM, "�����");
        if ((n = Scan->getNum()) < 2 || n > 20)
          Scan->er ("������ ����������� ���� �� � ��������� 2...20");

        // ��������� � ����� ������ ���� ������
        put_poliz (0, NUM, ind_var);

        // ��������� � ����� �������� ������ � �������� ����
        put_poliz (1, OUTN, 0);
        get_end ();
      }
      else
      {
        // ��������� � ����� �������� ������ ��� ������� ����
        put_poliz (1, OUT_, 0);
      }
    }
    if (lex == SEMICOLON)
    {
      get_end ();
      return;
    }
    wait_lex (COMMA, ",");
    get_end ();
  }
}

// �������� writeln
void oper_writeln (void)
{
  int n;

  get_end ();
  if (lex == SEMICOLON)
  {
    // ������ ������ �������� ������
    // ��������� � ����� �������� ������ �������� ������
    put_poliz (1, OUTLN, 0);
    get_end ();
    return;
  }
  for (;;)
  {
    if (lex == STR)
    {
      // ��������� � ����� ������� ������
      put_poliz (0, STR, ind_var);

      // ��������� � ����� �������� ������ ������
      put_poliz (1, STR, 0);
      get_end ();
    }
    else
    {
      expr1 ();
      if (lex == COLON)
      {
        token (NUM, "�����");
        if ((n = Scan->getNum()) < 2 || n > 20)
          Scan->er ("������ ����������� ���� �� � ��������� 2...20");

        // ��������� � ����� ������ ���� ������
        put_poliz (0, NUM, ind_var);

        // ��������� � ����� �������� ������ � �������� ����
        put_poliz (1, OUTN, 0);
        get_end ();
      }
      else
      {
        // ��������� � ����� �������� ������ ��� ������� ����
        put_poliz (1, OUT_, 0);
      }
    }
    if (lex == SEMICOLON)
    {
      // ��������� � ����� �������� ������ �������� ������
      put_poliz (1, OUTLN, 0);
      get_end ();
      return;
    }
    wait_lex (COMMA, ",");
    get_end ();
  }
}

// �������� readln
void oper_readln (void)
{
  int  w;

  get_end ();
  if (lex == SEMICOLON)
    Scan->er ("�������� readln ������");

  for (;;)
  {
    w = READLN;
    if (lex == STR)
    {
      // ��������� � ����� ������� ������
      put_poliz (0, STR, ind_var);
      get_end ();
      w = READLN_STR;
    }
    wait_lex (IDENT, "�������������");

    // ��������� � ����� ������� ���������� �����
    put_poliz (0, IDENT, ind_var);

    // ��������� � ����� ��������������� �������� �����
    put_poliz (1, w, 0);
    get_end();
    if (lex == SEMICOLON)
    {
      get_end ();
      return;
    }
    wait_lex (COMMA, ",");
    get_end ();
  }
}

// �������� exit (��������� ����������)
void oper_exit (void)
{
  token (SEMICOLON, ";");
  get_end ();

  // ��������� � ����� �������� ���������� ����������
  put_poliz (1, EXIT, 0);
}

// �������� erase (�������� �������)
void oper_erase (void)
{
  token (SEMICOLON, ";");
  get_end ();

  // ��������� � ����� �������� ������� �������
  put_poliz (1, ERASE, 0);
}

// �������� variable (���������� ������ ���� ����������)
void oper_variable(void)
{
  token (SEMICOLON, ";");
  get_end ();

  // ��������� � ����� �������� ���������� ������
  put_poliz (1, VARIABLE, 0);
}

// �������� pause (����� � ���������� ���������)
void oper_pause (void)
{
  get_end();
  if (lex == STR)
  {
    // ��������� � ����� ������� ������
    put_poliz (0, STR, ind_var);

    // ��������� � ����� �������� ��������
    put_poliz (1, PAUSE_STR, 0);
    get_end();
  }
  else
  {
    // ��������� � ����� �������� ��������
    put_poliz (1, PAUSE, 0);
  }
  wait_lex (SEMICOLON, ";");
  get_end ();
}

// ��������
void oper (void)
{
  switch (lex)
  {

  default:
    Scan->er ("�������������� ������");

  case BEGIN:
    get_end ();
    do
    {
      oper ();
    } while (lex != END);
    get_end ();
    return;

  case IDENT:
    token (SET, ":=");
    compile_set ();
    return;

  case IF:
    oper_if ();
    return;

  case FOR:
    oper_for ();
    return;

  case WHILE:
    oper_while ();
    return;

  case WRITE:
    oper_write ();
    return;

  case WRITELN:
    oper_writeln ();
    return;

  case READLN:
    oper_readln ();
    return;

  case EXIT:
    oper_exit ();
    return;

  case ERASE:
    oper_erase ();
    return;

  case VARIABLE:
    oper_variable ();
    return;

  case PAUSE:
    oper_pause ();
    return;

  case CONTINUE:
    oper_continue ();
    return;

  case BREAK:
    oper_break ();
    return;
  }
}

// �������� ���������� ����������
void declare (void)
{
  for (;;)
  {
    token (IDENT, "�������������");

    get_end ();
    if (lex == INTEGER || lex == REAL || lex == LOGIC)
    {
        // ������������� ���� ��������� ����������
        Scan->set_declare (lex);
        token(SEMICOLON, ";");
        get_end();
        return;
    }
    wait_lex(COMMA, ",");
  }
}

// ������������ �����
void info_poliz (void)
{
  int i;

  printf("���������� ������:\n");
  for (i = 0; i < ind_poliz2; ++i)
  {
    printf ("[%3d] ", i);

    // �������?
    if (poliz[i].op == 0)
    {
      switch (poliz[i].t)
      {

      // ��������� ����� �����
      case RAND:
        printf("��������� ����� �����\n");
        break;

      // ��������� ������������ �����
      case RAND_REAL:
        printf("��������� ������������ �����\n");
        break;

      // �������� �������������� (����� ���� ����� ��� ������������)
      case IDENT:
        printf("%u", poliz[i].n);
        if (type_var[poliz[i].n] == REAL)
          printf(" ������������ �������������\n");
        else
          printf(" ������������� �������������\n");
        break;

      // ������������� ���������
      case NUM:
        printf("%u", poliz[i].n);
        printf(" ������������� ���������\n");
        break;

      // ������������ ���������
      case NUMD:
        printf("%u", poliz[i].n);
        printf(" ������������ ���������\n");
        break;

      // ������
      default:
        printf("%u", poliz[i].n);
        printf(" ���������� ������\n");
      }
      continue;
    }

    // ��������
    switch (poliz[i].t)
    {

    // ���������� ���
    case OR:
      printf("�������� OR\n");
      break;

    // ���������� �
    case AND:
      printf("�������� AND\n");
      break;

    // ��������
    case ADD:
      printf("�������� ADD\n");
      break;

    // ���������
    case SUB:
      printf("�������� SUB\n");
      break;

    // ���������
    case MUL:
      printf("�������� MUL\n");
      break;

    // �������
    case DIV:
      printf("�������� DIV\n");
      break;

    // �������
    case MOD:
      printf("�������� MOD\n");
      break;

    // ������������
    case SET:
      printf("�������� SET\n");
      break;

    // ������� �����
    case NEG:
      printf("�������� NEG\n");
      break;

    // ���������� ���������
    case NOT:
      printf("�������� NOT\n");
      break;

    // �������� ���������
    case EQ:
      printf("�������� EQ\n");
      break;

    case NE:
      printf("�������� NE\n");
      break;

    case LE:
      printf("�������� LE\n");
      break;

    case GE:
      printf("�������� GE\n");
      break;

    case LT:
      printf("�������� LT\n");
      break;

    case GT:
      printf("�������� GT\n");
      break;

    case STR:
      printf("�������� ������ ������\n");
      break;

    // ������ ������ �������� ������
    case OUTLN:
      printf("�������� ������ ������� ������\n");
      break;

    // ������ ����� ��� ������� ����
    case OUT_:
      printf("�������� ������ ����� ��� �������\n");
      break;

    // ������ ����� � �������� ����
    case OUTN:
      printf("�������� ������ ����� � ��������\n");
      break;

    // ���� �������� ���������� � ������������
    case READLN_STR:
      printf("�������� READLN � ������������\n");
      break;

    // ���� �������� ���������� ��� �����������
    case READLN:
      printf("�������� READLN\n");
      break;

    // ����������� �������
    case GOTO:
      printf ("�������� GOTO %d\n", poliz[i].n);
      break;

    // �������� ������� �� false
    case TEST:
      printf ("�������� IF_FALSE %d\n", poliz[i].n);
      break;

    // �������� �������
    case ERASE:
      printf("�������� ERASE\n");
      break;

    // �������� ���������� ������ ���� ����������
    case VARIABLE:
      printf("�������� VARIABLE\n");
      break;

    // ����� � ���������� ���������
    case PAUSE:
      printf("�������� PAUSE ��� ���������\n");
      break;

    case PAUSE_STR:
      printf("�������� PAUSE � ����������\n");
      break;

    // ���������� ���������� ���������
    default:
      printf("�������� EXIT\n");
    }
  }
}

// ����������
void compile (void)
{
  // ������ ������ �������, ��� ����������� ������ ����
  if (get_lex () == END_FILE)
    Scan->er ("��������� �� ������");

  // ��������� ���������� � "{"
  wait_lex (LEFT_FIG, "{");
  get_end ();

  // ���� ��� �� ������ �����
  level = level_for = 0;

  // ���� �� ���� ���������� �� ������������
  memset (yes_var, 0, sizeof (yes_var));

  // ����� ��� �� ������
  ind_poliz = 0;

  // ���������� �������� � ����������
  for (;;)
  {
    Scan->init_declare ();
    if (lex == RIGHT_FIG)
      break;

    // ������� ���������� �������� ���������� ����������
    // ��� �������� ������������ � ��������� let
    if (lex == DIM)
    {
      Scan->init_declare ();
      declare ();
      continue;
    }

    // ��������� �������������� ����������
    oper ();
  }

  if (get_lex () != END_FILE)
    Scan->er ("��������� �������� ������ ���������");

  // ��������� � ����� �������� ���������� ���������
  put_poliz (1, EXIT, 0);
  ind_poliz2 = ind_poliz;
}

// �������� �������� �� �������� n ����� ���������
// ��������, ��� �������� �������� ����� n = 1
VT get_elem (int n)
{
  STACK_OP *p;
  VT       vt;

  p = stack_op + ind_stack_op - n;
  switch (p->t)
  {

  // ��������� ����� �����
  case RAND:
    vt.type_elem  = INTEGER;
    vt.value_elem = rand ();
    break;

  // ��������� ������������ �����
  case RAND_REAL:
    vt.type_elem  = REAL;
    vt.value_elem = (double)rand () / RAND_MAX;
    break;

  // �������� �������������� (����� ���� ����� ��� ������������)
  case IDENT:
    vt.type_elem  = (type_var[p->n] == REAL) ? REAL : INTEGER;
    vt.value_elem = value_var[p->n];
    break;

  // �������� ������������� ���������, ��������������
  // � ���������� ���������� �������� ������
  case WORK:
    vt.type_elem  = INTEGER;
    vt.value_elem = (int)p->val;
    break;

  // �������� ������������ ���������, ��������������
  // � ���������� ���������� �������� ������
  case WORKD:
    vt.type_elem  = REAL;
    vt.value_elem = p->val;
    break;

  // ������������� ���������
  case NUM:
    vt.type_elem  = INTEGER;
    vt.value_elem = number_bin[p->n];
    break;

  // ������������ ���������
  default:  // NUMD
    vt.type_elem  = REAL;
    vt.value_elem = number_bin[p->n];
  }

  return vt;
}

// �������� ��� �������� �� ����� ���������
void get_elem2 (void)
{
  vt1 = get_elem (2);
  vt2 = get_elem (1);
}

// ����� ���������� ����� ������� �����
void delete_zero (void)
{
  char *s;

  // ���� ���������� �����?
  if ((s = strchr (str_value, '.')) != NULL)
  {
    for (s = str_value + strlen (str_value); *--s == '0';)
      ;
    if (*s != '.')
      ++s;
    *s = 0;
  }
}

// ���������� ��������� readln
// return false � ������ ������ �� ����� ����������
bool run_readln(void)
{
  char txt[1024], *s, *q, c, *end;

  printf("���� ������ ������ �������� ����� �� ����� ����������\n");
  printf("������� ��������: '%s' = %s\n", name_var.c_str(), str_value);
  for (;;)
  {
    printf("����� ��������  : ");
    gets(txt);

    // ���������� �������� ������ �������
    for (s = txt; (c = *s) == ' ' || c == '\t'; ++s)
      ;
    if (c == 0)
    {
      // �������� �� �������
      return false;
    }

    // ���������� ��������� ������ �������
    for (q = s + strlen(s); (c = *--q) == ' ' || c == '\t';)
      ;

    // ����� ��������� ������ ��������
    q[1] = 0;

    // �������� ������������ ���� ������
    for (q = s; (c = *q) != 0; ++q)
    {
      if (c == '.' || c == ',')
      {
        if (type != REAL)
          break;
        *q = sym_point;
      }
    }
    if (c == 0)
    {
      // �������� ������� �����
      value_input = strtod(s, &end);
      if (*end == 0 && (type != LOGIC || value_input == 0.0 || value_input == 1.0))
        return true;
    }
    printf("�������� ��������\n");
  }
}

// ���������� ������ ���� ����������
void print_variable(void)
{
  unsigned w, len;
  int      i;
  string   line;

  // � ���� �� ����������?
  if (cnt_var == 0)
  {
    printf("��������� �� �������� ����������\n");
    return;
  }

  // ���������� ������������ ������ �������
  // � ������� ����������
  w = sizeof("����������") - 1;
  for (i = 0; i < cnt_var; ++i)
  {
    if ((len = var[i].length()) > w)
      w = len;
  }

  // �������� �������������� �����
  line = "+";
  for (len = 0; len < w; ++len)
    line += '-';
  line += "+---------------+\n";

  // ������ ����� �������
  printf(line.c_str());
  printf("|%-*s|   ��������    |\n", w, "����������");
  printf(line.c_str());

  // ������ �������
  for (i = 0; i < cnt_var; ++i)
  {
    printf("|%-*s|", w, var[i].c_str());
    switch(type_var[i])
    {

    case LOGIC:
      printf("%s|\n", value_var[i] == 0.0 ? "          false" : "          true");
      continue;

    case INTEGER:
      printf("%15d|\n", (int)value_var[i]);
      continue;
    }
    sprintf (str_value, "%lf", value_var[i]);

    // ����� ���������� ����� ������� ����� ������������� �����
    delete_zero ();
    printf("%15s|\n", str_value);
  }
  printf(line.c_str());
}

// ������������� ������
void run_poliz (void)
{
  int        n, int_v;
  char       c;
  double     double_v;
  const char *s;

  // �������� ������������� � ������ ������
  ind_poliz    = 0;

  // ���� ��������� ���� ����
  ind_stack_op = 0;

  // ��� ���������� ��������� ����� ������� ���������� �������
  memset (value_var, 0, sizeof (value_var));

  // ���� ��� ���������� output
  result_print = "";

  for (;;)
  {
    // ��������� ������� ������ �������?
    if (poliz[ind_poliz].op == 0)
    {
      // ���� ��������� ����������?
      if (ind_stack_op == MAX_STACK_OP)
      {
        printf ("\n������������ ����� ���������\n");
        return;
      }

      // �������� ������� � ���� ���������
      stack_op[ind_stack_op].t = poliz[ind_poliz].t;
      stack_op[ind_stack_op].n = poliz[ind_poliz].n;
      ++ind_stack_op;

      // ��������� � ���������� �������� ������
      ++ind_poliz;
      continue;
    }

    // ������� ���������� ��������� input � ������������
    comment_var = "";

    // �������� ��������������� ��������
    switch (poliz[ind_poliz].t)
    {

    // ���������� ���
    case OR:
      // ������� �������� ���������� ��������
      get_elem2 ();

      // � ������� ���������� ��������� ��������� ��������
      int_v = vt1.value_elem != 0.0 || vt2.value_elem != 0.0;

      // ������ ���� ��������� ����� �������� ���������
      // ������� ���������� ������ ����
      --ind_stack_op;
      stack_op[ind_stack_op - 1].t   = WORK;
      stack_op[ind_stack_op - 1].val = int_v;
      break;

    // ���������� �
    case AND:
      // ������� �������� ���������� ��������
      get_elem2 ();

      // � ������� ���������� ��������� ��������� ��������
      int_v = vt1.value_elem != 0.0 && vt2.value_elem != 0.0;

      // ������ ���� ��������� ����� �������� ���������
      // ������� ���������� ������ ����
      --ind_stack_op;
      stack_op[ind_stack_op - 1].t   = WORK;
      stack_op[ind_stack_op - 1].val = int_v;
      break;

    // ��������
    case ADD:
      // ������� �������� �������� ��������
      get_elem2 ();

      // � ������� ���������� ��������� ��������� ��������
      double_v = vt1.value_elem + vt2.value_elem;

      // ������ ���� ��������� ����� �������� ���������
      // ������� ���������� ������ ��� ������������� ����
      --ind_stack_op;
      stack_op[ind_stack_op - 1].t   = (vt1.type_elem == INTEGER && vt2.type_elem == INTEGER) ? WORK : WORKD;
      stack_op[ind_stack_op - 1].val = double_v;
      break;

    // ���������
    case SUB:
      // ������� �������� �������� ���������
      get_elem2 ();

      // � ������� ���������� ��������� ��������� ��������
      double_v = vt1.value_elem - vt2.value_elem;

      // ������ ���� ��������� ����� �������� ���������
      // ������� ���������� ������ ��� ������������� ����
      --ind_stack_op;
      stack_op[ind_stack_op - 1].t   = (vt1.type_elem == INTEGER && vt2.type_elem == INTEGER) ? WORK : WORKD;
      stack_op[ind_stack_op - 1].val = double_v;
      break;

    // ���������
    case MUL:
      // ������� �������� �������� ���������
      get_elem2 ();

      // � ������� ���������� ��������� ��������� ��������
      double_v = vt1.value_elem * vt2.value_elem;

      // ������ ���� ��������� ����� �������� ���������
      // ������� ���������� ������ ��� ������������� ����
      --ind_stack_op;
      stack_op[ind_stack_op - 1].t   = (vt1.type_elem == INTEGER && vt2.type_elem == INTEGER) ? WORK : WORKD;
      stack_op[ind_stack_op - 1].val = double_v;
      break;

    // �������
    case DIV:
      // ������� �������� �������� �������
      get_elem2 ();

      // �������� ������� �� ����
      if (vt2.value_elem == 0.0)
      {
        printf ("\n������� �� ����\n");
        return;
      }

      // � ������� ���������� ��������� ��������� ��������
      // �������� ������� ������� �� ���� ���������
      if (vt1.type_elem == INTEGER && vt2.type_elem == INTEGER)
        double_v = (int)vt1.value_elem / (int)vt2.value_elem;
      else
        double_v = vt1.value_elem / vt2.value_elem;

      // ������ ���� ��������� ����� �������� ���������
      // ������� ���������� ������ ��� ������������� ����
      --ind_stack_op;
      stack_op[ind_stack_op - 1].t   = (vt1.type_elem == INTEGER && vt2.type_elem == INTEGER) ? WORK : WORKD;
      stack_op[ind_stack_op - 1].val = double_v;
      break;

    // �������
    case MOD:
      // ������� �������� �������� ��������� �������
      get_elem2 ();

      // �������� ������� �� ����
      if (vt2.value_elem == 0.0)
      {
        printf ("\n������� �� ���� (�������)\n");
        return;
      }

      // ������� �������� ������ ��� ����� �����
      if (vt1.type_elem != INTEGER || vt2.type_elem != INTEGER)
      {
        printf ("\n������� �������� ������ ��� ����� �����\n");
        return;
      }
      double_v = (int)vt1.value_elem % (int)vt2.value_elem;

      // ������ ���� ��������� ����� �������� ���������
      // ������� ���������� ������ ����
      --ind_stack_op;
      stack_op[ind_stack_op - 1].t   = WORK;
      stack_op[ind_stack_op - 1].val = double_v;
      break;

    // ������������� ���������� ����� for �� 1
    case FOR_NO_STEP:
      // �������� ������� ����������
      n = stack_op[ind_stack_op - 1].n;

      // ����������� �� �� 1
      ++value_var[n];

      // �� ����� ��������� ��������� ���������� �������������
      --ind_stack_op;

      // ��������� � ���������� �������� ������
      ++ind_poliz;
      continue;

    // ������������� ���������� ����� for �� �������� ���������
    case FOR_STEP:
      // �������� ������� ����������
      n = stack_op[ind_stack_op - 1].n;

      // ������� �������� ���������
      vt2 = get_elem (2);

      // ����������� ���������� ����� �� �������� ���������
      value_var[n] += vt2.value_elem;

      // �� ����� ��������� ��������� ���������� �������������
      ind_stack_op -= 2;

      // ��������� � ���������� �������� ������
      ++ind_poliz;
      continue;

    // ������������
    case SET:
      // ������� �������� �������� �������� �����
      vt1 = get_elem (1);

      // � ����������� �� ���� �������������� ���������� ��������� ������������
      n = stack_op[ind_stack_op - 2].n;
      switch (type_var[n])
      {

      case INTEGER:
        value_var[n] = (int)vt1.value_elem;
        break;

      case REAL:
        value_var[n] = vt1.value_elem;
        break;

      default: // LOGIC
        value_var[n] = vt1.value_elem != 0.0;
      }

      // �� ����� ��������� ��������� ���������� ��������� ������������
      ind_stack_op -= 2;

      // ��������� � ���������� �������� ������
      ++ind_poliz;
      continue;

    // ������� �����
    case NEG:
      // ������� �������� �������� �������� �����
      vt1 = get_elem (1);

      // � ������� ���������� ��������� ��������� ��������
      double_v = -vt1.value_elem;

      // ������ �������� �������� ����� �������� ���������
      // ������� ���������� ������ ��� ������������� ����
      stack_op[ind_stack_op - 1].t   = (vt1.type_elem == INTEGER) ? WORK : WORKD;
      stack_op[ind_stack_op - 1].val = double_v;
      break;

    // ���������� ���������
    case NOT:
      // ������� �������� �������� �������� �����
      vt1 = get_elem (1);

      // � ������� ���������� ��������� ��������� ��������
      double_v = vt1.value_elem == 0.0 ? 1 : 0;

      // ������ �������� �������� ����� �������� ���������
      // ������� ���������� ������ ����
      stack_op[ind_stack_op - 1].t   = WORK;
      stack_op[ind_stack_op - 1].val = double_v;
      break;

    // �������� ���������
    case EQ:
    case NE:
    case LE:
    case GE:
    case LT:
    case GT:
      // ������� �������� �������� ���������
      get_elem2 ();

      // � ������� ���������� ��������� ��������� ��������
      switch (poliz[ind_poliz].t)
      {

      case EQ:
        double_v = vt1.value_elem == vt2.value_elem;
        break;

      case NE:
        double_v = vt1.value_elem != vt2.value_elem;
        break;

      case LE:
        double_v = vt1.value_elem;
        double_v = vt2.value_elem;
        double_v = vt1.value_elem <= vt2.value_elem;
        break;

      case GE:
        double_v = vt1.value_elem >= vt2.value_elem;
        break;

      case LT:
        double_v = vt1.value_elem < vt2.value_elem;
        break;

      default: // GT
        double_v = vt1.value_elem > vt2.value_elem;
      }

      // ������ ���� ��������� ����� �������� ���������
      // ������� ���������� ������ ����
      --ind_stack_op;
      stack_op[ind_stack_op - 1].t   = WORK;
      stack_op[ind_stack_op - 1].val = double_v;
      break;

    // ������ ���������� ������
    case STR:
      // �������� ����� ���������� ������
      s = sym_str[stack_op[--ind_stack_op].n].c_str();

      // ��� ������ ������ ������ # ���������� �� ������� ������
      while ((c = *s++) != 0)
      {
        if (c == '#')
        {
          result_print += '\n';
          printf ("%s", result_print.c_str());
          result_print = "";
        }
        else
        {
          result_print += c;
        }
      }

      // ��������� � ���������� �������� ������
      ++ind_poliz;
      continue;

    // ������ ������ �������� ������
    case OUTLN:
      result_print += '\n';
      printf ("%s", result_print.c_str());
      result_print = "";

      // ��������� � ���������� �������� ������
      ++ind_poliz;
      continue;

    // ������ ����� ��� ������� ����
    case OUT_:
      // ������� �������� �������� �������� �����
      vt1 = get_elem (1);

      // � ����������� �� ���� �������� ������
      if (vt1.type_elem == REAL)
      {
        sprintf (str_value, "%lf", vt1.value_elem);

        // ����� ���������� ����� ������� ����� ������������� �����
        delete_zero ();
      }
      else
      {
        sprintf (str_value, "%d", (int)vt1.value_elem);
      }
      result_print += str_value;
      --ind_stack_op;

      // ��������� � ���������� �������� ������
      ++ind_poliz;
      continue;

    // ������ ����� � �������� ����
    case OUTN:
      // ������� �������� ������� ����
      vt1 = get_elem (1);

      // ������� ���������� ��������
      vt2 = get_elem (2);

      // � ����������� �� ���� �������� ������
      if (vt2.type_elem == REAL)
      {
        sprintf (str_value, "%.*lf", (int)vt1.value_elem, vt2.value_elem);

        // ����� ���������� ����� ������� ����� ������������� �����
        delete_zero ();
      }
      else
      {
        sprintf (str_value, "%*d", (int)vt1.value_elem, (int)vt2.value_elem);
      }
      result_print += str_value;
      ind_stack_op -= 2;

      // ��������� � ���������� �������� ������
      ++ind_poliz;
      continue;

    // ���� �������� ���������� � ������������
    case READLN_STR:
      // �������� ����� �����������
      s = sym_str[stack_op[ind_stack_op - 2].n].c_str();

      // ��� ������ ������ ������ # ���������� �� ������� ������
      comment_var = "";
      while ((c = *s++) != 0)
      {
        if (c == '#')
          comment_var += "\r\n";
        else
          comment_var += c;
      }

      // �������� ����������� �����
      printf("%s\n", comment_var.c_str());

      // �� ����� ������ ������� ������� ��������������
      stack_op[ind_stack_op - 2] = stack_op[ind_stack_op - 1];
      --ind_stack_op;

    // ���� �������� ���������� ��� �����������
    case READLN:
      // ������� ������� �������� ����������
      vt1 = get_elem (1);

      // � ����������� �� ���� �������� ������ �������� ��������
      if (vt1.type_elem == REAL)
      {
        sprintf (str_value, "%lf", vt1.value_elem);

        // ����� ���������� ����� ������� �����
        delete_zero ();
      }
      else
      {
        sprintf (str_value, "%d", (int)vt1.value_elem);
      }
      n        = stack_op[--ind_stack_op].n;
      name_var = var[n];
      type     = type_var[n];

      // ���������� �����
      if (run_readln())
        value_var[n] = value_input;

      // ��������� � ���������� �������� ������
      ++ind_poliz;
      continue;

    // ����������� �������
    case GOTO:
      ind_poliz = poliz[ind_poliz].n;
      continue;

    // �������� ������� �� false
    case TEST:
      // ������� �������� ������� ��������
      vt1 = get_elem (1);
      --ind_stack_op;

      // ��� ��������?
      if (vt1.value_elem)
        ++ind_poliz;
      else
        ind_poliz = poliz[ind_poliz].n;
      continue;

    // �������� �������
    case ERASE:
      system("cls");
      break;

    // ���������� ������ ���� ����������
    case VARIABLE:
      print_variable();
      break;

    // ����� ���������� ��������� (� ����������)
    case PAUSE_STR:
      // �������� ����� �����������
      s = sym_str[stack_op[ind_stack_op - 1].n].c_str();

      // ��� ������ ������ ������ # ���������� �� ������� ������
      comment_var = "";
      while ((c = *s++) != 0)
      {
        if (c == '#')
          comment_var += "\r\n";
        else
          comment_var += c;
      }

      // �������� ����������� �����
      printf("%s\n", comment_var.c_str());
      --ind_stack_op;

    // ����� ���������� ���������
    case PAUSE:
      system("pause");
      break;

    // ���������� ���������� ���������
    case EXIT:
      printf ("\n���������� ��������� ���������\n");
      return;
    }

    // ��������� � ���������� �������� ������
    ++ind_poliz;
  }

  printf ("\n���������� ����������, �������� ������������\n");
}

// ������� ������
char help[] =
"Help      - ������� ������\n"
"Load name - �������� ��������� �� ����� � ������ name\n"
"Print     - ������ ���������\n"
"Compile   - ���������� ���������\n"
"Lex       - ������ ������� ������������ �������\n"
"Poliz     - ������ ���������� ���������� � ���� ������\n"
"Run       - ���������� ���������������� ���������\n"
"Word      - ������ ������� ��������� ����\n"
"Delim     - ������ ������� ������������\n"
"Ident     - ������ ������� ���������������\n"
"Number    - ������ ������� �����\n"
"String    - ������ ������� �����\n"
"Gramm     - ������ ���������� �����\n"
"Exit      - ���������� ������\n";

// ���������� � �������
char info[] =
"            ������� �� ������������ ������ (������� 15)\n"
"�����������  ���������, ����������� ����������� � ��������������\n"
"������� ���������  ������  ���������, � ����� ��� �������������.\n\n";

// ������ �����
char str[1024];

// ������ ������� ���������� ������ ����� str (��������� - ������, ����������� ������� ���������)
// � ������ ������������ ����� ���������� ����� 10,
// �� ��� �������� ���������� ����� ����� ����������� ��� ���� ������� �������
char *param[11];

// ���������� ���������� ������ ����� str
int cnt;

// ������ ������ str �� ��������� ���������
void unixp(void)
{
  char *s, c;

  cnt = 0;
  s = str;

  for (;;)
  {
    // ���������� ������ ������� ����� ��������� ����������
    while ((c = *s) == ' ' || c == '\t' || c == '\n')
      ++s;

    // ��������� �����������?
    if (c == 0)
      break;

    // �� ������� �� ����� ����������?
    if (cnt == 11)
      return;

    // ���������� ������ ���������
    param[cnt++] = s;

    // ���� ����� ���������w
    while ((c = *s) != ' ' && c != '\t' && c != '\n' && c)
      ++s;

    // ������ ���������� ���������?
    if (c == 0)
      break;

    // ���, ���������� �������� ���� � ����� ���������
    *s++ = 0;
  }
}

// �������� ���������
List Prog;

// �������� ���������
void load(void)
{
  FILE *f;
  char *s;

  if (cnt != 2)
  {
    printf("�������� ����� ����������\n");
    return;
  }

  if ((f = fopen(param[1], "r")) == NULL)
  {
    printf("���� �� ������\n");
    return;
  }

  ok_compile = false;

  Prog.erase();
  while (fgets(str, sizeof(str) - 1, f))
  {
    // ������� ��������� ������� ������
    if ((s = strchr(str, '\n')) != NULL)
      *s = 0;
    Prog.Add(str);
  }
  fclose(f);
}

// ������ ���������
void print(void)
{
  char     txt[20];
  unsigned len;
  int      i;

  if (cnt != 1)
  {
    printf("�������� ����� ����������\n");
    return;
  }

  if (Prog.Count == 0)
  {
    printf("��������� �� ���������\n");
    return;
  }

  // ��������� ������ ������ ������
  len = sprintf(txt, "%d", Prog.Count);

  // �������� ���������
  Prog.Begin();
  for (i = 0; i < Prog.Count; ++i)
    printf("[%*d] %s\n", len, i + 1, Prog.Get().c_str());
}

// ���������� ����������
void make_compile(void)
{
  int  i;
  bool b;

  if (cnt != 1)
  {
    printf("�������� ����� ����������\n");
    return;
  }

  if (Prog.Count == 0)
  {
    printf("��������� �� ���������\n");
    return;
  }

  // �������� ����������
  Scan->result = "";
  Scan->ind_result = 0;
  ok_compile = false;
  try
  {
    Scan->InitLex(&Prog);
    compile();

    // �������� ������� ������ ��� ������
    if (Scan->ind_result)
      Scan->memo->Add(Scan->result);

    printf("��������� �� �������� ������\n");

    // � ������ ���������� ���������� ��������� ���������
    // ������ ���������������� ����������
    b = true;
    for (i = 0; i < cnt_var; ++i)
    {
      if (yes_var[i] == 0)
      {
        if (b)
        {
          b = false;
          printf ("\n���������������� ����������:\n");
        }
        printf("%s\n", var[i].c_str());
      }
    }
    ok_compile = true;
  }

  // �������� ��������� ��������
  catch (int)
  {
    printf("��������� ��������\n");
  }
}

// ������� true, ���� ��������� ������ ������
bool ok_table(void)
{
  if (cnt != 1)
  {
    printf("�������� ����� ����������\n");
    return false;
  }

  if (!ok_compile)
  {
    printf("��������� ������������ ����������\n");
    return false;
  }

  return true;
}

// ������ ������� ������������ �������
void show_lex(void)
{
  if (ok_table())
    Scan->show_lex();
}

// ������ ������� ���������������
void show_ident(void)
{
  if (ok_table())
    Scan->show_ident();
}

// ������ ������� �����
void show_num(void)
{
  if (ok_table())
    Scan->show_num();
}

// ������ ������� �����
void show_str(void)
{
  if (ok_table())
    Scan->show_str();
}

// ������ ������� ������
void show_poliz(void)
{
  if (ok_table())
    info_poliz();
}

// ������ ������� ��������� ����
void show_word(void)
{
  int        i;
  const char *s;

  printf("������� ��������� ���� [0]\n");
  for (i = 0; (s = word[i].txt) != NULL; ++i)
    printf("%2d: %s\n", i, s);
}

// ������ ������� ������������
void show_delim(void)
{
  printf("������� ������������ [1]\n");
  printf
  (
    "%s",
    " 0: (\n"
    " 1: )\n"
    " 2: {\n"
    " 3: }\n"
    " 4: ;\n"
    " 5: +\n"
    " 6: -\n"
    " 7: *\n"
    " 8: %\n"
    " 9: ,\n"
    "10: ==\n"
    "11: !=\n"
    "12: >\n"
    "13: <\n"
    "14: >=\n"
    "15: <=\n"
    "16: !\n"
    "17: :=\n"
    "18: :\n"
    "19: /\n"
    "20: ||\n"
    "22: &&\n"
  );
}

// �������� ����������
const char gr[] =
"\n"
" ��� �������� ���������� ���������� ����� ������� ��������� ����������:\n"
"\n"
"- ��������, ������ � ������ '{_' � '_}' ����� ����������� 0 � ����� ���;\n"
"- ��������, ������ � ������ '{/' � '/}' ������� ����������� ������� 1 ���;\n"
"- ��������, ������ � ������ '[' � ']' ����� ���� �������;\n"
"- ��������, ������ � ������ '(_' � '_)' ����������� � ���������� �� ��������� ������ ������ ��� ���� �������;\n"
"- �������������� �������� ����������� �������� '|';\n"
"- ����� � ������ ����� ��������������� ������� ����������� ��������� '::=';\n"
"- ������� {, }, ( � ) �������� ��������� ��� ����������� �������� � ������� ������.\n"
"\n"
"                                        ��������� ���������� �����\n"
"\n"
"<���������>                  ::= <�������> {_<��������_������_���������> <�������>_}\n"
"<�������>                    ::= <���������> {_<��������_������_��������> <���������>_}\n"
"<���������>                  ::= <���������> {_<��������_������_���������> <���������>_}\n"
"<���������>                  ::= <�������������> | <�����> | rand | rand_real | <����������_���������>\n"
"                                 <�������_��������> <���������> | (<���������>)\n"
"<����������_���������>       ::= true | false\n"
"<������>                     ::= \"<������������ ����� �������� � ��������, # ������������ ��� ������ �������� ������>\"\n"
"<�����>                      ::= <�����> | <��������������>\n"
"<�������������>              ::= <�����> {_ <�����> | <�����> _}\n"
"<�����>                      ::= A | B | C | D | E | F | G | H | I | J | K | L | M |\n"
"                                 N | O | P | Q | R | S | T | U | V | W | X | Y | Z |\n"
"                                 a | b | c | d | e | f | g | h | i | j | k | l | m |\n"
"                                 n | o | p | q | r | s | t | u | v | w | x | y | z | _\n"
"<�����>                      ::= 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9\n"
"<�����>                      ::= <��������> | <������������> | <����������> | <�����������������>\n"
"<��������>                   ::= {/ 0 | 1 /} (_ B | b _)\n"
"<������������>               ::= {/ 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 /} (_ O | o _)\n"
"<����������>                 ::= {/ <�����> /} [_ D | d _]\n"
"<�����������������>          ::= <�����> {_ <�����> | A | B | C | D | E | F | a | b | c | d | e | f _} (_ H | h _)\n"
"<��������������>             ::= <��������_������> <�������> | [<��������_������>].<��������_������> [<�������>]\n"
"<��������_������>            ::= {/ <�����> /}\n"
"<��������_������_���������>  ::= != | == | < | <= | > | >=\n"
"<��������_������_���������>  ::= +  | - | ||\n"
"<��������_������_����������> ::= *  | / | && | %\n"
"<�������_��������>           ::= !  | -\n"
"<���������>                  ::= { {/ (_ <��������> | <��������> _); /} }\n"
"<��������>                   ::= dim <�������������> {_,<�������������>_} <���>\n"
"<���>                        ::= integer | real | boolean\n"
"<��������>                   ::= <���������> | <������������> | <��������> | <�������������_����> | <��������_����>\n"
"                                 <����> | <�����> | continue | break | exit | erase | variable | pause [<������>]\n"
"<���������>                  ::= begin <��������> {_ ; <��������> _} end\n"
"<������������>               ::= <����������> := <���������>\n"
"<��������>                   ::= if (<���������>) <��������> [ else <��������> ]\n"
"<�������������_����>         ::= for <������������> to <���������> [step <���������>] <��������> next\n"
"<��������_����>              ::= while (<���������>) <��������>\n"
"<����>                       ::= readln [<������>] <����������> {_ ,[<������>] <����������>_}\n"
"<�����>                      ::= writeln [<������>] | write <������>\n"
"<������>                     ::= <������2> {_ , <������2> _}\n"
"<������2>                    ::= <������> | <���������>[: <�����>]\n"
"<�����������>                ::= // ��� /* ... */\n";

int main()
{
  int i;
  char PI[] = "3.14";

  // ������������� ������� ��������� �����
  srand (time (NULL));

  // ���������� ������ ���������� ������� � �����
  // ����� ������������� �����
  if (atof(PI) <= 3.0)
    sym_point = ',';

  // ���������� ��������� ���������� ��������
  // ��� ���������� ������ � ����� �������� ������
  SetConsoleCP(1251);
  SetConsoleOutputCP(1251);

  // ������ ���������� � �������
  printf(info);

  // ������ ������ � ����������� ��������
  printf(help);

  Scan = new Lex;

  // ���������� ������ ������������
  for (;;)
  {
    // ����������� �� ���� �������
    printf("/");

    // ���� �������
    gets(str);

    // ������ ����������
    unixp();

    // ������ ������ ������������
    if (cnt == 0)
      continue;

    // ���������� �������
    for (i = 0; i <= _EXIT; ++i)
    {
      if (stricmp(cmd[i].name, param[0]) == 0)
        break;
    }

    if (i > _EXIT)
    {
      printf("����������� �������\n");
      continue;
    }

    // ��������� �������
    switch (i)
    {

    default:
      if (cnt != 1)
      {
        printf("�������� ����� ����������\n");
        continue;
      }

      delete Scan;
      return 0;

    case _HELP:
      printf(help);
      continue;

    case _LOAD:
      load();
      continue;

    case _PRINT:
      print();
      continue;

    case _RUN:
      run_poliz();
      continue;

    case _COMPILE:
      make_compile();
      continue;

    case _LEX:
      show_lex();
      continue;

    case _WORD:
      show_word();
      continue;

    case _DELIM:
      show_delim();
      continue;

    case _IDENT:
      show_ident();
      continue;

    case _POLIZ:
      show_poliz();
      continue;

    case _NUMBER:
      show_num();
      continue;

    case _STRING:
      show_str();
      continue;

    case _GRAMM:
      printf("%s", gr);
    }
  }
}
