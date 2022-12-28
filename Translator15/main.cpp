#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <windows.h>
#include <string.h>
#include <string>

using namespace std;

// Коды и список команд
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

// Символ разделения дробной и целой
// части вещественного числа
char sym_point = '.';

// Символьное представление значения переменной
char str_value[100];

// Обозначение переменной
string name_var;

// Комментарий ввода
string comment_var;

// Тип переменной
int type;

// Результат ввода
double value_input;

// Типы лексем и операций
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

// Структура элемента таблицы идентификаторв
struct WORD_
{
  const char *txt;
  int        key;
};

// Структура элемента таблицы разделителей
struct DELIM
{
  char c;
  int  key;
};

// Признак успешной трансляции (если true)
bool ok_compile = false;

// Уровень вложения циклов
int level;

// Максимальный уровень вложения циклов
#define MAX_LEVEL 10

// Максимальное количество break на любом уровне цикла
#define MAX_BREAK 30

// Служебные слова
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

// Разделители (не все, а только односимвольные)
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

// Элемент ПОЛИЗа
struct POLIZ
{
  // 0 - операнд, 1 - операция
  char op;

  // Для операции               - соответствующая операция
  // Для обычной переменной     - IDENT
  // Для целой константы        - NUM
  // Для вещественной константы - NUMD
  int t;

  // Для операнда - индекс в таблице или номер рабочей переменной
  int n;
};

// Максимальный размер ПОЛИЗа
#define MAX_POLIZ 2000

// Собственно сам ПОЛИЗ
POLIZ poliz[MAX_POLIZ];

// Индекс ПОЛИЗа
int ind_poliz;
int ind_poliz2;

// Результирующая строка печати
string result_print;

// Значение и тип элемента стека операндов
struct VT
{
  int    type_elem;
  double value_elem;
};

// Элемент стека операндов
struct STACK_OP
{
  // Для обычной переменной              - IDENT
  // Для целой рабочей переменной        - WORK
  // Для вещественной рабочей переменной - WORKD
  // Для целой константы                 - NUM
  // Для вещественной константы          - NUMD
  int t;

  // Индекс обычной переменной или константы в соответствующей таблице
  int n;

  // Значение рабочей переменной
  double val;
};

// Максимальное количество чисел
#define MAX_NUMBER  100

// Максимальное количество идентификаторов
#define MAX_VAR     100

// Максимальное количество символьных строк
#define MAX_SYM_STR 100

// Максимальный размер стека операндов
#define MAX_STACK_OP 50

// Собственно сам стек операндов
STACK_OP stack_op[MAX_STACK_OP];

// Индекс стека операндов
int ind_stack_op;

// Переменные, извлеченные из стека операндов
VT vt1, vt2;

// Массив индексов начала проверки выхода из цикла
int ind_test_loop[MAX_LEVEL + 1];

// Массив номеров переменных вложенных циклов
int ind_var_for[MAX_LEVEL + 1];

// Уровень вложения номеров переменных for
int level_for;

// Массив индексов операторов break
// На каждом уровне цикла вначале идет количество break,
// а затем их индексы в ПОЛИЗе
int ind_break_loop[MAX_LEVEL][MAX_BREAK + 1];

// Таблица чисел
string number[MAX_NUMBER];
double number_bin[MAX_NUMBER];

// Количество переменных
int cnt_var;

// Таблица переменных
string var[MAX_VAR];

// Значения переменных
double value_var[MAX_VAR];

// Тип переменных
int type_var[MAX_VAR];

// Признаки использования переменных
char yes_var[MAX_VAR];

// Индекс переменной в таблице идентификаторов
int ind_var;

// Таблица символьных строк
string sym_str[MAX_SYM_STR];

// Структура элемента списка с программой
struct RECORD
{
  // Строка программы
  string line;

  // Указатель на следующий элемент или NULL для последнего
  RECORD *next;
};

// Класс списка строк
class List
{
  // Адрес первого элемента списка
  RECORD *first;

  // Адрес последнего элемента списка
  RECORD *last;

  // Указатель текущего элемента списка
  RECORD *cur;

public:

  // Конструктор создает пустой список
  List(void)
  {
    first = last = NULL;
    Count = 0;
  }

  // Деструктор удаляет список
  ~List()
  {
    erase();
  }

  // Удаление списка
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

  // Количество элементов списка
  int Count;

  // Установиться на начало списка
  void Begin(void)
  {
    cur = first;
  }

  // Получить очередной элемент списка
  string Get(void)
  {
    string str = cur->line;
    cur = cur->next;
    return str;
  }

  // Добавить элемент в список
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

// Класс распознавания лексем
class Lex
{

private:

  // Состояния конечного автомата лексического анализатора
  enum
  {
    BeginLex,        // Начало лексемы
    ContinueIdent,   // Продолжение идентификатора
    ContinueNum,     // Продолжение числа
    ContinueComment, // Продолжение многострочного комментария
    BeginEndComment, // Начало завершения многострочного комментария
    EndComment,      // Завершение комментария
    EndString,       // Завершение строки
    ContinueOr,      // Продолжение разбора |
    ContinueAnd,     // Продолжение разбора &
    ContinueSet,     // Продолжение разбора :
    ContinueEq,      // Продолжение разбора =
    ContinueNe,      // Продолжение разбора !
    ContinueLt,      // Продолжение разбора <
    ContinueGt       // Продолжение разбора >
  };

  // Состояние обработки идентификатора
  // true, если идентификатор определяется
  // помощи "dim", false - используется в операторах
  bool yes_dim;

  int        n2;

  // Очередная строка программы
  string str;

  // Ширина номера строки программы
  unsigned width;

  // true - если это новый идентификатор
  bool new_ident;

  // Координаты возможной ошибочной лексемы
  int beg_num_save, beg_ind_save, len_lex_save;

  // Количество переменных в таблице на начало оператора описания переменных
  int beg_cnt_var;

  // Количество чисел
  int cnt_number;

  // Количество символьных строк
  int cnt_sym_str;

  // Текущее состояние автомата
  int state;

  // Текст лексической ошибки
  string txt_error;

  // Индекс символа в исходной строке
  int ind;

  // Номер исходной строки
  int num;

  // Двоичное значение целого числа
  int val;

  // Двоичное значение вещественного числа
  double vald;

  // Текущий сканируемый символ исходной программы
  char c;

  // Признак чтения символа из файла (true - если символ уже прочитан в 'c')
  bool yes_c;

  // Текущий идентификатор
  string ident;

  // Индекс начала лексемы
  int beg_ind;

  // Длина лексемы
  int len_lex;

  // Номер строки с лексемой
  int beg_num;

  // Исходная программа
  List *re;

  // Вернуть -1 или код односимвольного разделителя
  int get_delim (void)
  {
    char q;

    for (n2 = 0; (q = delim[n2].c) != c && q; ++n2)
      ;
    return delim[n2].key;
  }

  // Вернуть -1 или код служебного слова
  int get_word (void)
  {
    const char *q, *s;

    s = ident.c_str();
    for (n2 = 0; (q = word[n2].txt) != NULL && strcmp (s, q); ++n2)
      ;
    return word[n2].key;
  }

  // Сбор шестнадцатиричного числа
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

  // Сбор десятичного числа
  void make10 (void)
  {
    val = atoi (ident.c_str());
  }

  // Сбор вещественного числа
  void make_double (void)
  {
    vald = atof (ident.c_str());
  }

  // Сбор двоичного числа с контролем на ошибку
  int make2 (void)
  {
    const char *s;
    char       c;

    for (val = 0, s = ident.c_str(); (c = *s) != 0; ++s)
    {
      if (c > '1')
        er ("Двоичное число ошибочно");
      c -= '0';
      val = (val << 1) | c;
    }
    return NUM;
  }

  // Сбор восьмеричного числа с контролем на ошибку
  int make8 (void)
  {
    const char *s;
    char       c;

    for (val = 0, s = ident.c_str(); (c = *s) != 0; ++s)
    {
      if (c > '7')
        er ("Восьмеричное число ошибочно");
      c -= '0';
      val = (val << 3) | c;
    }
    return NUM;
  }

  // Поместить идентификатор в таблицу переменных;
  // если это возможно - return индекс таблицы, иначе -1
  int set_var (void)
  {
    // Поиск идентификатора в таблице
    for (ind_var = 0; ind_var < cnt_var && var[ind_var] != ident; ++ind_var)
      ;

    // Поиск удачен?
    if (ind_var < cnt_var)
    {
      // Идентификатор в определении "dim"?
      if (yes_dim)
        er ("Повторное определениие переменной");

      // Переменная использована
      yes_var[ind_var] = 1;
      return ind_var;
    }

    // Идентификатор в операторах?
    if (!yes_dim)
      er ("Неизвестная переменная");

    // Нет места в таблице?
    if (cnt_var == MAX_VAR)
      er ("Переполнение таблицы переменных");

    // Разместим идентификатор в таблице
    var[cnt_var] = ident;
    yes_var[cnt_var] = 0;
    return ind_var = cnt_var++;
  }

  // Поместить число в таблицу чисел;
  // если это возможно - return индекс таблицы, иначе -1
  // t - INTEGER или REAL
  int set_number (int t)
  {
    // Поиск числа в таблице
    for (ind_var = 0; ind_var < cnt_number && number[ind_var] != ident; ++ind_var)
      ;

    // Поиск удачен?
    if (ind_var < cnt_number)
      return ind_var;

    // Нет места в таблице?
    if (cnt_number == MAX_NUMBER)
      return -1;

    // Разместим число в символьной таблице
    number[cnt_number] = ident;

    // Разместим двоичное значение в таблице
    if (t == REAL)
      number_bin[cnt_number] = vald;
    else
      number_bin[cnt_number] = val;

    return ind_var = cnt_number++;
  }

  // Поместить символьную строку в таблицу строк;
  // если это возможно - return индекс таблицы, иначе -1
  int set_sym_str (void)
  {
    // Поиск строки в таблице
    for (ind_var = 0; ind_var < cnt_sym_str && sym_str[ind_var] != ident; ++ind_var)
      ;

    // Поиск удачен?
    if (ind_var < cnt_sym_str)
      return ind_var;

    // Нет места в таблице?
    if (cnt_sym_str == MAX_SYM_STR)
      return -1;

    // Разместим строку в таблице
    sym_str[cnt_sym_str] = ident;

    return ind_var = cnt_sym_str++;
  }

  // Отправить лексему в результат
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

  // Чтение очередного символа программы
  // В случае конца программы возвращается 0
  // Для конца строки возвращается '\n'
  char Input(void)
  {
    // Символ уже прочитан при сканировании предыдущей лексемы?
    if (yes_c)
    {
      if (c)
        yes_c = false;
      return c;
    }

    // Надо читать очередную строку?
    if (ind == -1)
    {
      // Конец исходной программы?
      if (num == re->Count)
        return (char)0;

      // Забираем очередную строку программы
      str = re->Get();

      // Печатаем ее
      printf("[%*d] %s\n", width, ++num, str.c_str());

      // Устанавливаем индекс чтения очередного символа исходной строки
      ind = 0;
    }

    // Прочитали конец строки?
    if ((unsigned)ind == str.length())
    {
      // Устанавливаем признак чтения очередной строки
      ind = -1;

      // Возвращаем признак конца строки
      return '\n';
    }

    // Возвращаем очередной символ строки
    ++len_lex;
    return str[ind++];
  }

public:

  // Остальные таблицы с результатом лексического разбора
  List *memo;

  // Индекс результата
  int ind_result;

  // Строка результата
  string result;

  Lex (void)
  {
    memo = new List;
  }

  ~Lex (void)
  {
    delete memo;
  }

  // Инициализация лексического разбора для программы p
  void InitLex(List *p)
  {
    char txt[10];

    re = p;
    memo->erase();
    ind = -1;
    num = 0;

    // Определяем максимальную ширину номера исходной программы
    width = sprintf(txt, "%d", re->Count);

    // Устанавливаемся на первую строку программы
    re->Begin();

    // Читаем первый символ программы
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

  // Инициализация начала описания переменных
  void init_declare (void)
  {
    beg_cnt_var = cnt_var;
  }

  // Установка типов переменных после трансляции очередного описания
  void set_declare (int type)
  {
    while (beg_cnt_var != cnt_var)
      type_var[beg_cnt_var++] = type;
    yes_dim = false;
  }

  // Опубликовать таблицу лексического разбора
  void show_lex(void)
  {
    int i;

    memo->Begin();
    printf
    (
      "Данные лексического разбора\n"
      "Информация о каждой лексеме заключена в круглые скобки\n"
      "Первое число в скобках - номер таблицы:\n"
      "0 - таблица служебных слов\n"
      "1 - таблица разделителей\n"
      "2 - таблица чисел\n"
      "3 - таблица идентификаторов\n"
      "4 - таблица строк\n"
      "Второе число в скобках - позиция лексемы в таблице\n\n"
    );
    for (i = 0; i < memo->Count; ++i)
      printf("%s\n", memo->Get().c_str());
  }

  // Опубликовать таблицу идентификаторов
  void show_ident(void)
  {
    int i;

    if (cnt_var == 0)
    {
      printf("Нет ни одного идентификатора\n");
      return;
    }

    printf("Таблица идентификаторов [3]\n");
    for (i = 0; i < cnt_var; ++i)
      printf("%2d: %s\n", i, var[i].c_str());
  }

  // Опубликовать таблицу чисел
  void show_num(void)
  {
    int i;

    if (cnt_number == 0)
    {
      printf("Нет ни одного числа\n");
      return;
    }

    printf("Таблица чисел [2]\n");
    for (i = 0; i < cnt_number; ++i)
      printf("%2d: %s\n", i, number[i].c_str());
  }

  // Опубликовать таблицу строк
  void show_str(void)
  {
    int i;

    if (cnt_sym_str == 0)
    {
      printf("Нет ни одной строки\n");
      return;
    }

    printf("Таблица строк [4]\n");
    for (i = 0; i < cnt_sym_str; ++i)
      printf("%2d: %s\n", i, sym_str[i].c_str());
  }

  // Возбудить исключительную ситуацию по ошибке
  void er (const char *s)
  {
    int i;

    txt_error = s;

    // Позиционируем указатель к месту ошибки с учетом возможных табуляций
    for (i = 0; i < int(width + 3); ++i)
      printf(" ");
    for (s = str.c_str(), i = 1; i < beg_ind; ++i, ++s)
    {
      printf(*s == '\t' ? "\t" : " ");
    }
    printf("?\n");

    // Печатаем сообщение об ошибке
    printf("%s\n", txt_error.c_str());

    // Допечатываем остаток программы
    while (num != re->Count)
      printf("[%*d] %s\n", width, ++num, re->Get().c_str());
    throw 0;
  }

  // Чтение числовой константы
  int getNum (void)
  {
    return val;
  }

  // Прочитать очередную лексему
  // Если ошибок нет, тип лексемы содержится в type
  int lex (void)
  {
    int        offs, r;
    char       cc, c2[2];
    const char *s;

    c2[1] = 0;

    // Переход по состояниям автомата
    for (;;)
    {
      switch (state)
      {

      case BeginLex:
        // Игнорируем пустые символы, оставаясь в том же состоянии
        while ((c = Input()) == ' ' || c == '\t' || c == '\n')
          ;

        // Координаты начала лексемы
        beg_ind = ind;
        beg_num = num;
        len_lex = 1;

        // Встретили конец программы?
        if (c == 0)
        {
          yes_c = true;
          return END_FILE;
        }

        // Это вырожденная вещественная константа?
        if (c == '.')
        {
          ident = "0";

          // Заморозим введенный символ
          yes_c = true;

          // Изменяем состояние, продолжаем разбор
          state = ContinueNum;

          continue;
        }

        // Это начало числовой константы?
        if (isdigit(c))
        {
          // Изменяем состояние, продолжаем разбор
          state = ContinueNum;

          // Устанавливаем значение старшего символа числа
          ident = c;
          continue;
        }

        // Это начало идентификатора?
        if (c == '_' || isalpha (c))
        {
          // Изменяем состояние, продолжаем разбор
          state = ContinueIdent;

          // Устанавливаем значение старшего символа идентификатора
          ident = c;
          continue;
        }

        // Распознавание односимвольных разделителей
        state = BeginLex;
        if ((val = get_delim ()) >= 0)
        {
          yes_c = false;
          out_result (1, n2);
          return val;
        }

        // Распознавание строк, двухсимвольных разделителей и комментария
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

        er ("Неизвестный символ");

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

        er ("Ожидалось '|'");

      case ContinueAnd:
        yes_c = false;
        state = BeginLex;
        if((c = Input()) == '&')
        {
          out_result (1, 22);
          return AND;
        }

        er ("Ожидалось '&'");

      case ContinueEq:
        yes_c = false;
        state = BeginLex;
        if((c = Input()) == '=')
        {
          out_result (1, 10);
          return EQ;
        }

        er ("Ожидалось '='");

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
          // Игнорируем строку до конца
          ind = -1;

          // Выполняем подход к следующей лексеме
          state = BeginLex;
          continue;

        case '*':
          // Переходим в состояние поиска конца многострочного комментария
          state = BeginEndComment;
          continue;
        }

        yes_c = false;

        // Операция деления
        state = BeginLex;
        out_result (1, 6);
        return DIV;

      case BeginEndComment:
        // Неоконченный комментарий?
        if ((c = Input()) == 0)
          er ("Неожиданный конец многострочного комментария");

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
        // Неоконченный комментарий?
        if ((c = Input()) == 0)
          er ("Неожиданный конец многострочного комментария");

        if (c == '*')
          continue;

        // Конец комментария?
        if (c == '/')
        {
          // Выполняем подход к следующей лексеме
          state = BeginLex;
          continue;
        }

        // Продолжаем "ловить" конец комментария
        state = BeginEndComment;
        continue;

      case EndString:
        // Строка закончена?
        if ((c = Input ()) == '"')
        {
          // Поместим строку в таблицу
          if ((r = set_sym_str ()) < 0)
            er ("Переполнение таблицы символьных строк");
          out_result (4, r);

          // Возвращаем лексему
          state = BeginLex;
          return STR;
        }

        if (c == '\n' || c == 0)
          er ("Незаконченная строка");

        // Продолжаем собирать строку
        ident += c;
        continue;

      case ContinueIdent:
        // Остаемся в том же состоянии при сканировании допустимых символов идентификатора
        // Продолжаем собирать идентификатор
        if ((c = Input ()) == '_' || isdigit (c) || isalpha (c))
        {
          ident += c;
          continue;
        }

        // Идентификатор закончился, устанавливаем начальное состояние автомата
        state = BeginLex;
        yes_c = true;

        // Может это логическая константа?
        if (ident == "false" || ident == "true")
        {
          // Считаем false значением 0, true значением 1
          // Поместим целое число в таблицу
          val = ident == "true";
          if ((r = set_number (INTEGER)) < 0)
            er ("Переполнение таблицы чисел");
          out_result (3, r);

          // Возвращаем лексему
          return NUM;
        }

        // Это начало определений переменных?
        if (ident == "dim")
          yes_dim = true;

        // Может это служебное слово?
        if ((r = get_word ()) >= 0)
        {
          out_result (0, n2);
          return r;
        }

        // Нет, это обычный идентификатор, поместим его в таблицу
        if ((r = set_var ()) < 0)
          er ("Переполнение таблицы идентификаторов");
        out_result (3, r);
        return IDENT;

      case ContinueNum:
        // Остаемся в том же состоянии при сканировании цифр
        // Продолжаем собирать число
        if (isdigit (c = Input()))
        {
          ident += c;
          continue;
        }

        // Устанавливаем начальное состояние автомата
        state = BeginLex;

        // Это шестнадцатиричное число?
        if (isxdigit (c))
        {
          offs = 0;

          // Может это ситуация 123E+ как вещественное число?
          if (tolower (c) == 'e')
          {
            ident += 'e';

            if ((c = Input ()) == '+' || c == '-')
              goto lab1;

            // Запомним смещение до символа 'e', если это
            // вдруг не шестнадцатиричное число
            offs = strlen (ident.c_str());
          }
          // Добавляем все символы шестнадцатиричного числа
          do
          {
            c2[0] = tolower (c);
            ident += c2;
          } while (isxdigit (c = Input()));

          // Шестнадцатиричное число должно оканчиваться символами H,h
          if (tolower (c) != 'h')
          {
            // Может это была запись двоичного числа?
            if (ident[1] == '0' || ident[1] == '1')
            {
              for (s = ident.c_str(); (cc = *++s) == '0' || cc == '1';)
                ;
              if ((cc == 'b' || cc == 'B') && s[1] == 0)
              {
                // Удаляем символ B
                ident = ident.substr(0, ident.length() - 1);

                // Соберем двоичное число
                make2 ();

                // Поместим целое число в таблицу
                ident += "b";
                if ((r = set_number (INTEGER)) < 0)
                  er ("Переполнение таблицы чисел");
                out_result (3, r);
                yes_c = true;

                // Возвращаем лексему
                return NUM;
              }
            }

            // Может это была запись десятичного числа?
            if (isdigit (ident[1]))
            {
              for (s = ident.c_str(); isdigit (*++s);)
                ;
              if ((*s == 'd' || *s == 'D') && s[1] == 0)
              {
                // Удаляем символ D
                ident = ident.substr(0, ident.length() - 1);

                // Соберем десятичное число
                make10 ();

                // Поместим целое число в таблицу
                ident += "d";
                if ((r = set_number (INTEGER)) < 0)
                  er ("Переполнение таблицы чисел");
                out_result (3, r);
                yes_c = true;

                // Возвращаем лексему
                return NUM;
              }
            }

            // Может это была запись вещественного числа?
            if (offs)
            {
              s = ident.c_str() + offs;
              if (isdigit (*s))
              {
                while (isdigit (*++s))
                  ;
                if (*s == 0)
                {
                  // Действительно, это запись вида 123E17
                  // Соберем вещественное число
                  make_double ();

                  // Поместим вещественное число в таблицу
                  if ((r = set_number (REAL)) < 0)
                    er ("Переполнение таблицы чисел");
                  yes_c = true;
                  out_result (3, r);
                  return NUMD;
                }
              }
            }

            er ("Ошибочная запись шестнадцатиричного числа");
          }

          // Соберем шестнадцатиричное число
          make16 ();

          // Поместим целое число в таблицу
          ident += "h";
          if ((r = set_number (INTEGER)) < 0)
            er ("Переполнение таблицы чисел");
          out_result (3, r);
          return NUM;
        }

        switch (c = tolower (c))
        {

        // Это конец шестнадцатиричного числа?
        case 'h':
          // Соберем шестнадцатиричное число
          make16 ();

          // Поместим целое число в таблицу
          ident += c;
          if ((r = set_number (INTEGER)) < 0)
            er ("Переполнение таблицы чисел");
          out_result (3, r);

          // Возвращаем лексему
          return NUM;

        // Это конец двоичного числа?
        case 'b':
          // Соберем двоичное число с контролем на ошибку
          make2 ();

          // Поместим целое число в таблицу
          ident += c;
          if ((r = set_number (INTEGER)) < 0)
            er ("Переполнение таблицы чисел");
          out_result (3, r);

          // Возвращаем лексему
          return NUM;

        // Это конец восьмеричного числа?
        case 'o':
          // Соберем восьмеричное число с контролем на ошибку
          make8 ();

          // Поместим целое число в таблицу
          ident += c;
          if ((r = set_number (INTEGER)) < 0)
            er ("Переполнение таблицы чисел");
          out_result (3, r);

          // Возвращаем лексему
          return NUM;

        // Это конец десятичного числа?
        case 'd':
          // Соберем десятичное число
          make10 ();

          // Поместим целое число в таблицу
          ident += c;
          if ((r = set_number (INTEGER)) < 0)
            er ("Переполнение таблицы чисел");
          out_result (3, r);

          // Возвращаем лексему
          return NUM;

        // Это начало дробной части вещественного числа?
        case '.':
          // Собираем дробную часть
          ident += '.';
          if (!isdigit (c = Input ()))
            er ("Ошибочная запись вещественного числа");
          do
          {
            ident += c;
          } while (isdigit (c = Input ()));
          if (tolower (c) != 'e')
          {
            // Соберем вещественное число
            make_double ();
            yes_c = true;

            // Поместим вещественное число в таблицу
            if ((r = set_number (REAL)) < 0)
              er ("Переполнение таблицы чисел");
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
            er ("Ошибочная запись вещественного числа");
          do
          {
            ident += c;
          } while (isdigit (c = Input ()));

          // Соберем вещественное число
          make_double ();
          yes_c = true;

          // Поместим вещественное число в таблицу
          if ((r = set_number (REAL)) < 0)
            er ("Переполнение таблицы чисел");
          out_result (3, r);
          return NUMD;
        }

        // Это обычное десятичное число
        make10 ();
        yes_c = true;

        // Поместим целое число в таблицу
        if ((r = set_number (INTEGER)) < 0)
          er ("Переполнение таблицы чисел");
        out_result (3, r);

        // Возвращаем лексему
        return NUM;
      }
    }
  }
};

// Объект лексического разбора
Lex *Scan;

int lex;

// Поместить в ПОЛИЗ очередной элемент
void put_poliz (char op, int t, int n)
{
  // Есть место в ПОЛИЗе?
  if (ind_poliz == MAX_POLIZ)
    Scan->er ("Переполнение ПОЛИЗа");

  poliz[ind_poliz].op = op;
  poliz[ind_poliz].t  = t;
  poliz[ind_poliz].n  = n;
  ++ind_poliz;
}

// Если текущая лексема не t, сообщить об ошибке s
void wait_lex (int t, const char *s)
{
  char txt[100];

  if (lex != t)
  {
    sprintf (txt, "Ожидалось: %s", s);
    Scan->er (txt);
  }
}

// Чтение очередной лексемы
int get_lex (void)
{
  return lex = Scan->lex ();
}

// Чтение очередной лексемы и проверка ее на END
int get_end (void)
{
  if ((lex = Scan->lex ()) == END_FILE)
    Scan->er ("Неожиданный конец программы");
  return lex;
}

// Прочитать лексему, совпадающую с параметром p
// mess - название лексемы
void token (int p, const char *mess)
{
  get_end ();
  wait_lex (p, mess);
}

// Предварительное объявление функций рекурсивного спуска
void expr1 (void);
void expr2 (void);
void expr3 (void);
void expr4 (void);
void expr5 (void);
void atom  (void);

// Компиляция выражения
void expr (void)
{
  get_end ();
  expr1 ();
}

// Операции отношения
void expr1 (void)
{
  int w;

  expr2 ();
  if (lex == NE || lex == EQ || lex == LT || lex == GT || lex == LE || lex == GE)
  {
    w = lex;
    get_end ();
    expr1 ();

    // Поместить в ПОЛИЗ соответствующую операцию
    put_poliz (1, w, 0);
  }
}

// Операции сложения
void expr2 (void)
{
  int w;

  expr3 ();
  while (lex == ADD || lex == SUB || lex == OR)
  {
    w = lex;
    get_end ();
    expr3 ();

    // Поместить в ПОЛИЗ соответствующую операцию
    put_poliz (1, w, 0);
  }
}

// Операции умножения и деления
void expr3 (void)
{
  int w;

  expr4 ();
  while (lex == MUL || lex == DIV || lex == MOD || lex == AND)
  {
    w = lex;
    get_end ();
    expr4 ();

    // Поместить в ПОЛИЗ соответствующую операцию
    put_poliz (1, w, 0);
  }
}

// Унарный +, - или not
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

  // При необходимости поместить в ПОЛИЗ NOT или NEG
  if (w >= 0)
    put_poliz (1, w, 0);
}

// Выражение в скобках
void expr5 (void)
{
  if (lex == LEFT)
  {
    get_end ();
    expr1 ();
    if (lex != RIGHT)
      Scan->er ("Нарушен баланс скобок ()");
    get_end ();
  }
  else
    atom ();
}

// Получение идентификатора или числа
void atom (void)
{
  // Поместить в ПОЛИЗ идентификатор или число
  put_poliz (0, lex, ind_var);

  switch (lex)
  {

  default:
    Scan->er ("Ожидалось: идентификатор, rand, rand_real или число");

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

// Компиляция оператора присваивания
void compile_set (void)
{
  // Поместить в ПОЛИЗ переменную результата
  put_poliz (0, IDENT, ind_var);

  // Вычислить выражение
  expr ();
  wait_lex (SEMICOLON, ";");
  get_end ();

  // Поместить в ПОЛИЗ операцию присваивания
  put_poliz (1, SET, 0);
}

// Оператор if
void oper_if (void)
{
  int ind, ind2;

  token (LEFT, "(");
  expr ();

  // Сохранить текущий индекс ПОЛИЗа
  ind = ind_poliz;

  // Поместить в ПОЛИЗ операцию условного перехода
  // Пока адрес перехода не известен
  put_poliz (1, TEST, 0);
  wait_lex (RIGHT, ")");
  get_end ();
  oper ();
  if (lex == ELSE)
  {
    // Сохранить текущий индекс ПОЛИЗа
    ind2 = ind_poliz;

    // Поместить в ПОЛИЗ операцию безусловного перехода
    // Пока адрес перехода не известен
    put_poliz (1, GOTO, 0);

    // Установим адрес перехода по лжи для начала if
    poliz[ind].n = ind_poliz;
    ind = ind2;
    get_end ();
    oper ();
  }

  // Коррекция адреса перехода
  poliz[ind].n = ind_poliz;
}

// Оператор continue
void oper_continue (void)
{
  if (level == 0)
    Scan->er ("Оператор continue вне цикла");

  // Операция безусловного перехода на переадресацию цикла
  put_poliz (1, GOTO, ind_test_loop[level - 1]);

  token (SEMICOLON, ";");
  get_end ();
}

// Оператор break
void oper_break (void)
{
  int n;

  if (level == 0)
    Scan->er ("Оператор break вне цикла");

  // Операция безусловного перехода выхода из цикла
  // Адрес выхода еще не сформирован, место коррекции запоминается
  n = ++ind_break_loop[level - 1][0];

  // Не слишком ли много операторов break?
  if (n == MAX_BREAK + 1)
    Scan->er ("Слишком много операторов break для текущего цикла");

  ind_break_loop[level - 1][n] = ind_poliz;

  // Операция выхода из цикла (не до конца сформированная)
  put_poliz (1, GOTO, 0);

  token (SEMICOLON, ";");
  get_end ();
}

// Оператор while
void oper_while (void)
{
  int ind_test, ind_break;
  int i, *adr;

  if (level == MAX_LEVEL)
    Scan->er ("Слишком большой уровень вложения циклов");

  // Пока на данном уровне цикла нет операторов break;
  ind_break_loop[level][0] = 0;

  // Увеличиваем уровень вложения циклов
  ++level;

  token (LEFT, "(");

  // Запомним индекс ПОЛИЗа начала условия выполнения цикла
  ind_test_loop[level - 1] = ind_test = ind_poliz;

  expr ();

  // Адрес операции выхода из цикла
  ind_break = ind_poliz;

  // Поместить в ПОЛИЗ операцию условного выхода из цикла
  // Пока адрес перехода не известен
  put_poliz (1, TEST, 0);

  wait_lex (RIGHT, ")");
  get_end();
  oper ();

  // Операция безусловного перехода на начало цикла
  put_poliz (1, GOTO, ind_test);

  // Коррекция адреса выхода из цикла
  poliz[ind_break].n = ind_poliz;

  // Уменьшаем уровень вложения циклов
  --level;

  // Коррекция адресов перехода для break
  adr = ind_break_loop[level];
  for (i = *adr; i; --i)
  {
    poliz[*++adr].n = ind_poliz;
  }
}

// Оператор for
void oper_for (void)
{
  int ind_test, ind_break = -1, ind_next, ind_body;
  int i, *adr;

  if (level == MAX_LEVEL)
    Scan->er ("Слишком большой уровень вложения циклов");

  // Пока на данном уровне цикла нет операторов break;
  ind_break_loop[level][0] = 0;

  // Компилируем оператор for
  token (IDENT, "идентификатор");

  // Переменная цикла должна быть целой
  if (type_var[ind_var] != INTEGER)
    Scan->er ("Ожидалась переменная типа integer");

  // Проверка на дубликат переменных вложенных циклов
  for (i = 0; i < level_for; ++i)
  {
    if (ind_var == ind_var_for[i])
      Scan->er ("Дубликат переменной вложенного цикла for");
  }

  // Сохраним переменную цикла
  ind_var_for[level_for] = ind_var;

  // Ожидаем знак присваивания
  token (SET, ":=");

  // Поместить в ПОЛИЗ переменную результата
  put_poliz (0, IDENT, ind_var);

  // Вычислить выражение
  expr ();

  // Поместить в ПОЛИЗ операцию присваивания
  put_poliz (1, SET, 0);

  wait_lex (TO, "to");

  // Запомним индекс ПОЛИЗа начала условия выполнения цикла
  ind_test = ind_poliz;

  expr ();

  // Поместим в ПОЛИЗ переменную цикла и операцию сравнение >=
  put_poliz (0, IDENT, ind_var_for[level_for]);
  put_poliz (1, GE, 0);

  // Запомним индекс ПОЛИЗа c выходом из цикла
  ind_break = ind_poliz;

  // Поместить в ПОЛИЗ операцию условного выхода из цикла
  // Пока адрес перехода не известен
  put_poliz (1, TEST, 0);

  // Поместить в ПОЛИЗ операцию безусловного перехода на тело цикла
  // Пока адрес перехода не известен
  ind_body = ind_poliz;
  put_poliz (1, GOTO, 0);
  ind_test_loop[level] = ind_next = ind_poliz;

  // Задан шаг переадресации переменной цикла?
  if (lex == STEP)
  {
    expr ();

    // Поместим в ПОЛИЗ переменную цикла и операцию
    // изменения этой переменной на величину выражения
    put_poliz (0, IDENT, ind_var_for[level_for]);
    put_poliz (1, FOR_STEP, 0);
  }
  else
  {
    // Поместим в ПОЛИЗ переменную цикла и операцию
    // изменения этой переменной на 1 при отсутствиее step
    put_poliz (0, IDENT, ind_var_for[level_for]);
    put_poliz (1, FOR_NO_STEP, 0);
  }

  // Поместить в ПОЛИЗ операцию перехода на начало проверки цикла
  put_poliz (1, GOTO, ind_test);

  // Коррекция адреса перехода на тело цикла
  poliz[ind_body].n = ind_poliz;

  // Увеличиваем уровень вложения циклов
  ++level;
  ++level_for;

  do
    oper ();
  while (lex != NEXT);
  get_end();

  // Поместить в ПОЛИЗ операцию перехода на переадресацию цикла
  put_poliz (1, GOTO, ind_next);

  // Для условного цикла скорректировать адрес выхода из цикла
  if (ind_break >= 0)
    poliz[ind_break].n = ind_poliz;

  // Уменьшаем уровень вложения циклов
  --level;
  --level_for;

  // Коррекция адресов перехода для break
  adr = ind_break_loop[level];
  for (i = *adr; i; --i)
  {
    poliz[*++adr].n = ind_poliz;
  }
}

// Оператор write
void oper_write (void)
{
  int n;

  get_end ();
  if (lex == SEMICOLON)
    Scan->er ("Оператор write пустой");
  for (;;)
  {
    if (lex == STR)
    {
      // Поместить в ПОЛИЗ операнд строки
      put_poliz (0, STR, ind_var);

      // Поместить в ПОЛИЗ операцию печати строки
      put_poliz (1, STR, 0);
      get_end ();
    }
    else
    {
      expr1 ();
      if (lex == COLON)
      {
        token (NUM, "число");
        if ((n = Scan->getNum()) < 2 || n > 20)
          Scan->er ("Размер печатаемого поля не в диапазоне 2...20");

        // Поместить в ПОЛИЗ размер поля печати
        put_poliz (0, NUM, ind_var);

        // Поместить в ПОЛИЗ операцию печати с размером поля
        put_poliz (1, OUTN, 0);
        get_end ();
      }
      else
      {
        // Поместить в ПОЛИЗ операцию печати без размера поля
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

// Оператор writeln
void oper_writeln (void)
{
  int n;

  get_end ();
  if (lex == SEMICOLON)
  {
    // Печать только перевода строки
    // Поместить в ПОЛИЗ операцию печати перевода строки
    put_poliz (1, OUTLN, 0);
    get_end ();
    return;
  }
  for (;;)
  {
    if (lex == STR)
    {
      // Поместить в ПОЛИЗ операнд строки
      put_poliz (0, STR, ind_var);

      // Поместить в ПОЛИЗ операцию печати строки
      put_poliz (1, STR, 0);
      get_end ();
    }
    else
    {
      expr1 ();
      if (lex == COLON)
      {
        token (NUM, "число");
        if ((n = Scan->getNum()) < 2 || n > 20)
          Scan->er ("Размер печатаемого поля не в диапазоне 2...20");

        // Поместить в ПОЛИЗ размер поля печати
        put_poliz (0, NUM, ind_var);

        // Поместить в ПОЛИЗ операцию печати с размером поля
        put_poliz (1, OUTN, 0);
        get_end ();
      }
      else
      {
        // Поместить в ПОЛИЗ операцию печати без размера поля
        put_poliz (1, OUT_, 0);
      }
    }
    if (lex == SEMICOLON)
    {
      // Поместить в ПОЛИЗ операцию печати перевода строки
      put_poliz (1, OUTLN, 0);
      get_end ();
      return;
    }
    wait_lex (COMMA, ",");
    get_end ();
  }
}

// Оператор readln
void oper_readln (void)
{
  int  w;

  get_end ();
  if (lex == SEMICOLON)
    Scan->er ("Оператор readln пустой");

  for (;;)
  {
    w = READLN;
    if (lex == STR)
    {
      // Поместить в ПОЛИЗ операнд строки
      put_poliz (0, STR, ind_var);
      get_end ();
      w = READLN_STR;
    }
    wait_lex (IDENT, "идентификатор");

    // Поместить в ПОЛИЗ операнд переменной ввода
    put_poliz (0, IDENT, ind_var);

    // Поместить в ПОЛИЗ соответствующую операцию ввода
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

// Оператор exit (завершить выполнение)
void oper_exit (void)
{
  token (SEMICOLON, ";");
  get_end ();

  // Поместить в ПОЛИЗ операцию завершения выполнения
  put_poliz (1, EXIT, 0);
}

// Оператор erase (очистить консоль)
void oper_erase (void)
{
  token (SEMICOLON, ";");
  get_end ();

  // Поместить в ПОЛИЗ операцию очистки консоли
  put_poliz (1, ERASE, 0);
}

// Оператор variable (отладочная печать всех переменных)
void oper_variable(void)
{
  token (SEMICOLON, ";");
  get_end ();

  // Поместить в ПОЛИЗ операцию отладочной печати
  put_poliz (1, VARIABLE, 0);
}

// Оператор pause (пауза в выполнении программы)
void oper_pause (void)
{
  get_end();
  if (lex == STR)
  {
    // Поместить в ПОЛИЗ операнд строки
    put_poliz (0, STR, ind_var);

    // Поместить в ПОЛИЗ операцию останова
    put_poliz (1, PAUSE_STR, 0);
    get_end();
  }
  else
  {
    // Поместить в ПОЛИЗ операцию останова
    put_poliz (1, PAUSE, 0);
  }
  wait_lex (SEMICOLON, ";");
  get_end ();
}

// Оператор
void oper (void)
{
  switch (lex)
  {

  default:
    Scan->er ("Синтаксическая ошибка");

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

// Оператор объявления переменных
void declare (void)
{
  for (;;)
  {
    token (IDENT, "идентификатор");

    get_end ();
    if (lex == INTEGER || lex == REAL || lex == LOGIC)
    {
        // Устанавливаем типы описанных переменных
        Scan->set_declare (lex);
        token(SEMICOLON, ";");
        get_end();
        return;
    }
    wait_lex(COMMA, ",");
  }
}

// Опубликовать ПОЛИЗ
void info_poliz (void)
{
  int i;

  printf("Содержимое ПОЛИЗа:\n");
  for (i = 0; i < ind_poliz2; ++i)
  {
    printf ("[%3d] ", i);

    // Операнд?
    if (poliz[i].op == 0)
    {
      switch (poliz[i].t)
      {

      // Случайное целое число
      case RAND:
        printf("Случайное целое число\n");
        break;

      // Случайное вещественное число
      case RAND_REAL:
        printf("Случайное вещественное число\n");
        break;

      // Значение идентификатора (может быть целым или вещественным)
      case IDENT:
        printf("%u", poliz[i].n);
        if (type_var[poliz[i].n] == REAL)
          printf(" Вещественный идентификатор\n");
        else
          printf(" Целочисленный идентификатор\n");
        break;

      // Целочисленная константа
      case NUM:
        printf("%u", poliz[i].n);
        printf(" Целочисленная константа\n");
        break;

      // Вещественная константа
      case NUMD:
        printf("%u", poliz[i].n);
        printf(" Вещественная константа\n");
        break;

      // Строка
      default:
        printf("%u", poliz[i].n);
        printf(" Символьная строка\n");
      }
      continue;
    }

    // Операция
    switch (poliz[i].t)
    {

    // Логическое ИЛИ
    case OR:
      printf("Операция OR\n");
      break;

    // Логическое И
    case AND:
      printf("Операция AND\n");
      break;

    // Сложение
    case ADD:
      printf("Операция ADD\n");
      break;

    // Вычитание
    case SUB:
      printf("Операция SUB\n");
      break;

    // Умножение
    case MUL:
      printf("Операция MUL\n");
      break;

    // Деление
    case DIV:
      printf("Операция DIV\n");
      break;

    // Остаток
    case MOD:
      printf("Операция MOD\n");
      break;

    // Присваивание
    case SET:
      printf("Операция SET\n");
      break;

    // Унарный минус
    case NEG:
      printf("Операция NEG\n");
      break;

    // Логическое отрицание
    case NOT:
      printf("Операция NOT\n");
      break;

    // Операции сравнения
    case EQ:
      printf("Операция EQ\n");
      break;

    case NE:
      printf("Операция NE\n");
      break;

    case LE:
      printf("Операция LE\n");
      break;

    case GE:
      printf("Операция GE\n");
      break;

    case LT:
      printf("Операция LT\n");
      break;

    case GT:
      printf("Операция GT\n");
      break;

    case STR:
      printf("Операция Печать строки\n");
      break;

    // Печать одного перевода строки
    case OUTLN:
      printf("Операция Печать перевод строки\n");
      break;

    // Печать числа без размера поля
    case OUT_:
      printf("Операция Печать числа без размера\n");
      break;

    // Печать числа с размером поля
    case OUTN:
      printf("Операция Печать числа с размером\n");
      break;

    // Ввод значения переменной с комментарием
    case READLN_STR:
      printf("Операция READLN с комментарием\n");
      break;

    // Ввод значения переменной без комментария
    case READLN:
      printf("Операция READLN\n");
      break;

    // Безусловный переход
    case GOTO:
      printf ("Операция GOTO %d\n", poliz[i].n);
      break;

    // Условный переход по false
    case TEST:
      printf ("Операция IF_FALSE %d\n", poliz[i].n);
      break;

    // Очистить консоль
    case ERASE:
      printf("Операция ERASE\n");
      break;

    // Операция отладочной печати всех переменных
    case VARIABLE:
      printf("Операция VARIABLE\n");
      break;

    // Пауза в выполнении программы
    case PAUSE:
      printf("Операция PAUSE без сообщения\n");
      break;

    case PAUSE_STR:
      printf("Операция PAUSE с сообщением\n");
      break;

    // Завершение выполнения программы
    default:
      printf("Операция EXIT\n");
    }
  }
}

// Трансляция
void compile (void)
{
  // Читаем первую лексему, она обязательно должна быть
  if (get_lex () == END_FILE)
    Scan->er ("Программа не задана");

  // Программа начинается с "{"
  wait_lex (LEFT_FIG, "{");
  get_end ();

  // Пока нет ни одного цикла
  level = level_for = 0;

  // Пока ни одна переменная не использована
  memset (yes_var, 0, sizeof (yes_var));

  // ПОЛИЗ еще не создан
  ind_poliz = 0;

  // Компиляция описаний и операторов
  for (;;)
  {
    Scan->init_declare ();
    if (lex == RIGHT_FIG)
      break;

    // Попытка распознать оператор объявления переменных
    // или оператор присваивания с опущенным let
    if (lex == DIM)
    {
      Scan->init_declare ();
      declare ();
      continue;
    }

    // Обработка исполнительных операторов
    oper ();
  }

  if (get_lex () != END_FILE)
    Scan->er ("Программа содержит лишние операторы");

  // Поместить в ПОЛИЗ операцию завершения программы
  put_poliz (1, EXIT, 0);
  ind_poliz2 = ind_poliz;
}

// Получить значение из элемента n стека операндов
// Например, для верхнего элемента стека n = 1
VT get_elem (int n)
{
  STACK_OP *p;
  VT       vt;

  p = stack_op + ind_stack_op - n;
  switch (p->t)
  {

  // Случайное целое число
  case RAND:
    vt.type_elem  = INTEGER;
    vt.value_elem = rand ();
    break;

  // Случайное вещественное число
  case RAND_REAL:
    vt.type_elem  = REAL;
    vt.value_elem = (double)rand () / RAND_MAX;
    break;

  // Значение идентификатора (может быть целым или вещественным)
  case IDENT:
    vt.type_elem  = (type_var[p->n] == REAL) ? REAL : INTEGER;
    vt.value_elem = value_var[p->n];
    break;

  // Значение целочисленной константы, сформированной
  // в результате предыдущих операций ПОЛИЗа
  case WORK:
    vt.type_elem  = INTEGER;
    vt.value_elem = (int)p->val;
    break;

  // Значение вещественной константы, сформированной
  // в результате предыдущих операций ПОЛИЗа
  case WORKD:
    vt.type_elem  = REAL;
    vt.value_elem = p->val;
    break;

  // Целочисленная константа
  case NUM:
    vt.type_elem  = INTEGER;
    vt.value_elem = number_bin[p->n];
    break;

  // Вещественная константа
  default:  // NUMD
    vt.type_elem  = REAL;
    vt.value_elem = number_bin[p->n];
  }

  return vt;
}

// Получить два операнда из стека операндов
void get_elem2 (void)
{
  vt1 = get_elem (2);
  vt2 = get_elem (1);
}

// Сброс незначащих нулей дробной части
void delete_zero (void)
{
  char *s;

  // Есть десятичная точка?
  if ((s = strchr (str_value, '.')) != NULL)
  {
    for (s = str_value + strlen (str_value); *--s == '0';)
      ;
    if (*s != '.')
      ++s;
    *s = 0;
  }
}

// Выполнение оператора readln
// return false в случае отказа от ввода переменной
bool run_readln(void)
{
  char txt[1024], *s, *q, c, *end;

  printf("Ввод пустой строки означает отказ от ввода переменной\n");
  printf("Текущее значение: '%s' = %s\n", name_var.c_str(), str_value);
  for (;;)
  {
    printf("Новое значение  : ");
    gets(txt);

    // Игнорируем головные пустые символы
    for (s = txt; (c = *s) == ' ' || c == '\t'; ++s)
      ;
    if (c == 0)
    {
      // Значение не введено
      return false;
    }

    // Игнорируем хвостовые пустые символы
    for (q = s + strlen(s); (c = *--q) == ' ' || c == '\t';)
      ;

    // Сброс хвостовых пустых символов
    q[1] = 0;

    // Контроль корректности типа данных
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
      // Пытаемся собрать число
      value_input = strtod(s, &end);
      if (*end == 0 && (type != LOGIC || value_input == 0.0 || value_input == 1.0))
        return true;
    }
    printf("Значение ошибочно\n");
  }
}

// Отладочная печать всех переменных
void print_variable(void)
{
  unsigned w, len;
  int      i;
  string   line;

  // А есть ли переменные?
  if (cnt_var == 0)
  {
    printf("Программа не содержит переменных\n");
    return;
  }

  // Определяем максимальную ширину колонки
  // с именами переменных
  w = sizeof("Переменные") - 1;
  for (i = 0; i < cnt_var; ++i)
  {
    if ((len = var[i].length()) > w)
      w = len;
  }

  // Получаем разделительную линию
  line = "+";
  for (len = 0; len < w; ++len)
    line += '-';
  line += "+---------------+\n";

  // Печать шапки таблицы
  printf(line.c_str());
  printf("|%-*s|   Значения    |\n", w, "Переменные");
  printf(line.c_str());

  // Печать таблицы
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

    // Сброс незначащих нулей дробной части вещественного числа
    delete_zero ();
    printf("%15s|\n", str_value);
  }
  printf(line.c_str());
}

// Интерпретация ПОЛИЗа
void run_poliz (void)
{
  int        n, int_v;
  char       c;
  double     double_v;
  const char *s;

  // Начинаем интерпретацию с начала ПОЛИЗа
  ind_poliz    = 0;

  // Стек операндов пока пуст
  ind_stack_op = 0;

  // Все переменные программы перед началом выполнения нулевые
  memset (value_var, 0, sizeof (value_var));

  // Пока нет результата output
  result_print = "";

  for (;;)
  {
    // Очередной элемент ПОЛИЗа операнд?
    if (poliz[ind_poliz].op == 0)
    {
      // Стек операндов переполнен?
      if (ind_stack_op == MAX_STACK_OP)
      {
        printf ("\nПереполнение стека операндов\n");
        return;
      }

      // Поместим операнд в стек операндов
      stack_op[ind_stack_op].t = poliz[ind_poliz].t;
      stack_op[ind_stack_op].n = poliz[ind_poliz].n;
      ++ind_stack_op;

      // Переходим к следующему элементу ПОЛИЗа
      ++ind_poliz;
      continue;
    }

    // Признак отсутствия оператора input с комментарием
    comment_var = "";

    // Выполним соответствующую операцию
    switch (poliz[ind_poliz].t)
    {

    // Логическое ИЛИ
    case OR:
      // Получим операнды логической операции
      get_elem2 ();

      // В рабочей переменной сохраняем результат операции
      int_v = vt1.value_elem != 0.0 || vt2.value_elem != 0.0;

      // Вместо двух элементов стека операнда размещаем
      // рабочую переменную целого типа
      --ind_stack_op;
      stack_op[ind_stack_op - 1].t   = WORK;
      stack_op[ind_stack_op - 1].val = int_v;
      break;

    // Логическое И
    case AND:
      // Получим операнды логической операции
      get_elem2 ();

      // В рабочей переменной сохраняем результат операции
      int_v = vt1.value_elem != 0.0 && vt2.value_elem != 0.0;

      // Вместо двух элементов стека операнда размещаем
      // рабочую переменную целого типа
      --ind_stack_op;
      stack_op[ind_stack_op - 1].t   = WORK;
      stack_op[ind_stack_op - 1].val = int_v;
      break;

    // Сложение
    case ADD:
      // Получим операнды операции сложения
      get_elem2 ();

      // В рабочей переменной сохраняем результат операции
      double_v = vt1.value_elem + vt2.value_elem;

      // Вместо двух элементов стека операнда размещаем
      // рабочую переменную целого или вещественного типа
      --ind_stack_op;
      stack_op[ind_stack_op - 1].t   = (vt1.type_elem == INTEGER && vt2.type_elem == INTEGER) ? WORK : WORKD;
      stack_op[ind_stack_op - 1].val = double_v;
      break;

    // Вычитание
    case SUB:
      // Получим операнды операции вычитания
      get_elem2 ();

      // В рабочей переменной сохраняем результат операции
      double_v = vt1.value_elem - vt2.value_elem;

      // Вместо двух элементов стека операнда размещаем
      // рабочую переменную целого или вещественного типа
      --ind_stack_op;
      stack_op[ind_stack_op - 1].t   = (vt1.type_elem == INTEGER && vt2.type_elem == INTEGER) ? WORK : WORKD;
      stack_op[ind_stack_op - 1].val = double_v;
      break;

    // Умножение
    case MUL:
      // Получим операнды операции умножения
      get_elem2 ();

      // В рабочей переменной сохраняем результат операции
      double_v = vt1.value_elem * vt2.value_elem;

      // Вместо двух элементов стека операнда размещаем
      // рабочую переменную целого или вещественного типа
      --ind_stack_op;
      stack_op[ind_stack_op - 1].t   = (vt1.type_elem == INTEGER && vt2.type_elem == INTEGER) ? WORK : WORKD;
      stack_op[ind_stack_op - 1].val = double_v;
      break;

    // Деление
    case DIV:
      // Получим операнды операции деления
      get_elem2 ();

      // Контроль деления на ноль
      if (vt2.value_elem == 0.0)
      {
        printf ("\nДеление на ноль\n");
        return;
      }

      // В рабочей переменной сохраняем результат операции
      // Операция деления зависит от типа операндов
      if (vt1.type_elem == INTEGER && vt2.type_elem == INTEGER)
        double_v = (int)vt1.value_elem / (int)vt2.value_elem;
      else
        double_v = vt1.value_elem / vt2.value_elem;

      // Вместо двух элементов стека операнда размещаем
      // рабочую переменную целого или вещественного типа
      --ind_stack_op;
      stack_op[ind_stack_op - 1].t   = (vt1.type_elem == INTEGER && vt2.type_elem == INTEGER) ? WORK : WORKD;
      stack_op[ind_stack_op - 1].val = double_v;
      break;

    // Остаток
    case MOD:
      // Получим операнды операции получения остатка
      get_elem2 ();

      // Контроль деления на ноль
      if (vt2.value_elem == 0.0)
      {
        printf ("\nДеление на ноль (остаток)\n");
        return;
      }

      // Остаток допустим только для целых чисел
      if (vt1.type_elem != INTEGER || vt2.type_elem != INTEGER)
      {
        printf ("\nОстаток допустим только для целых чисел\n");
        return;
      }
      double_v = (int)vt1.value_elem % (int)vt2.value_elem;

      // Вместо двух элементов стека операнда размещаем
      // рабочую переменную целого типа
      --ind_stack_op;
      stack_op[ind_stack_op - 1].t   = WORK;
      stack_op[ind_stack_op - 1].val = double_v;
      break;

    // Переадресация переменной цикла for на 1
    case FOR_NO_STEP:
      // Получаем позицию переменной
      n = stack_op[ind_stack_op - 1].n;

      // Увеличиваем ее на 1
      ++value_var[n];

      // Из стека операндов удаляется информация переадресации
      --ind_stack_op;

      // Переходим к следующему элементу ПОЛИЗа
      ++ind_poliz;
      continue;

    // Переадресация переменной цикла for на заданное выражение
    case FOR_STEP:
      // Получаем позицию переменной
      n = stack_op[ind_stack_op - 1].n;

      // Получим величину выражения
      vt2 = get_elem (2);

      // Увеличиваем переменную цикла на заданное выражение
      value_var[n] += vt2.value_elem;

      // Из стека операндов удаляется информация переадресации
      ind_stack_op -= 2;

      // Переходим к следующему элементу ПОЛИЗа
      ++ind_poliz;
      continue;

    // Присваивание
    case SET:
      // Получим значение верхнего элемента стека
      vt1 = get_elem (1);

      // В зависимости от типа результирующей переменной выполняем присваивание
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

      // Из стека операндов удаляется информация оператора присваивания
      ind_stack_op -= 2;

      // Переходим к следующему элементу ПОЛИЗа
      ++ind_poliz;
      continue;

    // Унарный минус
    case NEG:
      // Получим значение верхнего элемента стека
      vt1 = get_elem (1);

      // В рабочей переменной сохраняем результат операции
      double_v = -vt1.value_elem;

      // Вместо верхнего элемента стека операнда размещаем
      // рабочую переменную целого или вещественного типа
      stack_op[ind_stack_op - 1].t   = (vt1.type_elem == INTEGER) ? WORK : WORKD;
      stack_op[ind_stack_op - 1].val = double_v;
      break;

    // Логическое отрицание
    case NOT:
      // Получим значение верхнего элемента стека
      vt1 = get_elem (1);

      // В рабочей переменной сохраняем результат операции
      double_v = vt1.value_elem == 0.0 ? 1 : 0;

      // Вместо верхнего элемента стека операнда размещаем
      // рабочую переменную целого типа
      stack_op[ind_stack_op - 1].t   = WORK;
      stack_op[ind_stack_op - 1].val = double_v;
      break;

    // Операции сравнения
    case EQ:
    case NE:
    case LE:
    case GE:
    case LT:
    case GT:
      // Получим операнды операции сравнения
      get_elem2 ();

      // В рабочей переменной сохраняем результат операции
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

      // Вместо двух элементов стека операнда размещаем
      // рабочую переменную целого типа
      --ind_stack_op;
      stack_op[ind_stack_op - 1].t   = WORK;
      stack_op[ind_stack_op - 1].val = double_v;
      break;

    // Печать символьной строки
    case STR:
      // Получить адрес печатаемой строки
      s = sym_str[stack_op[--ind_stack_op].n].c_str();

      // При печати строки символ # заменяется на перевод строки
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

      // Переходим к следующему элементу ПОЛИЗа
      ++ind_poliz;
      continue;

    // Печать одного перевода строки
    case OUTLN:
      result_print += '\n';
      printf ("%s", result_print.c_str());
      result_print = "";

      // Переходим к следующему элементу ПОЛИЗа
      ++ind_poliz;
      continue;

    // Печать числа без размера поля
    case OUT_:
      // Получим значение верхнего элемента стека
      vt1 = get_elem (1);

      // В зависимости от типа выполним печать
      if (vt1.type_elem == REAL)
      {
        sprintf (str_value, "%lf", vt1.value_elem);

        // Сброс незначащих нулей дробной части вещественного числа
        delete_zero ();
      }
      else
      {
        sprintf (str_value, "%d", (int)vt1.value_elem);
      }
      result_print += str_value;
      --ind_stack_op;

      // Переходим к следующему элементу ПОЛИЗа
      ++ind_poliz;
      continue;

    // Печать числа с размером поля
    case OUTN:
      // Получим значение размера поля
      vt1 = get_elem (1);

      // Получим печатаемое значение
      vt2 = get_elem (2);

      // В зависимости от типа выполним печать
      if (vt2.type_elem == REAL)
      {
        sprintf (str_value, "%.*lf", (int)vt1.value_elem, vt2.value_elem);

        // Сброс незначащих нулей дробной части вещественного числа
        delete_zero ();
      }
      else
      {
        sprintf (str_value, "%*d", (int)vt1.value_elem, (int)vt2.value_elem);
      }
      result_print += str_value;
      ind_stack_op -= 2;

      // Переходим к следующему элементу ПОЛИЗа
      ++ind_poliz;
      continue;

    // Ввод значения переменной с комментарием
    case READLN_STR:
      // Получить адрес комментария
      s = sym_str[stack_op[ind_stack_op - 2].n].c_str();

      // При печати строки символ # заменяется на перевод строки
      comment_var = "";
      while ((c = *s++) != 0)
      {
        if (c == '#')
          comment_var += "\r\n";
        else
          comment_var += c;
      }

      // Печатаем комментарий ввода
      printf("%s\n", comment_var.c_str());

      // На место строки запишем элемент идентификатора
      stack_op[ind_stack_op - 2] = stack_op[ind_stack_op - 1];
      --ind_stack_op;

    // Ввод значения переменной без комментария
    case READLN:
      // Получим текущее значение переменной
      vt1 = get_elem (1);

      // В зависимости от типа выполним печать текущего значения
      if (vt1.type_elem == REAL)
      {
        sprintf (str_value, "%lf", vt1.value_elem);

        // Сброс незначащих нулей дробной части
        delete_zero ();
      }
      else
      {
        sprintf (str_value, "%d", (int)vt1.value_elem);
      }
      n        = stack_op[--ind_stack_op].n;
      name_var = var[n];
      type     = type_var[n];

      // Выполнение ввода
      if (run_readln())
        value_var[n] = value_input;

      // Переходим к следующему элементу ПОЛИЗа
      ++ind_poliz;
      continue;

    // Безусловный переход
    case GOTO:
      ind_poliz = poliz[ind_poliz].n;
      continue;

    // Условный переход по false
    case TEST:
      // Получим значение условия перехода
      vt1 = get_elem (1);
      --ind_stack_op;

      // Нет перехода?
      if (vt1.value_elem)
        ++ind_poliz;
      else
        ind_poliz = poliz[ind_poliz].n;
      continue;

    // Очистить консоль
    case ERASE:
      system("cls");
      break;

    // Отладочная печать всех переменных
    case VARIABLE:
      print_variable();
      break;

    // Пауза выполнения программы (с сообщением)
    case PAUSE_STR:
      // Получить адрес комментария
      s = sym_str[stack_op[ind_stack_op - 1].n].c_str();

      // При печати строки символ # заменяется на перевод строки
      comment_var = "";
      while ((c = *s++) != 0)
      {
        if (c == '#')
          comment_var += "\r\n";
        else
          comment_var += c;
      }

      // Печатаем комментарий ввода
      printf("%s\n", comment_var.c_str());
      --ind_stack_op;

    // Пауза выполнения программы
    case PAUSE:
      system("pause");
      break;

    // Завершение выполнения программы
    case EXIT:
      printf ("\nВыполнение программы завершено\n");
      return;
    }

    // Переходим к следующему элементу ПОЛИЗа
    ++ind_poliz;
  }

  printf ("\nВыполнение прекращено, возможно зацикливание\n");
}

// Краткая помощь
char help[] =
"Help      - краткая помощь\n"
"Load name - загрузка программы из файла с именем name\n"
"Print     - печать программы\n"
"Compile   - компиляция программы\n"
"Lex       - печать таблицы лексического разбора\n"
"Poliz     - печать результата компиляции в виде ПОЛИЗа\n"
"Run       - выполнение скомпилированной программы\n"
"Word      - печать таблицы служебных слов\n"
"Delim     - печать таблицы разделителей\n"
"Ident     - печать таблицы идентификаторов\n"
"Number    - печать таблицы чисел\n"
"String    - печать таблицы строк\n"
"Gramm     - печать грамматики языка\n"
"Exit      - завершение работы\n";

// Информация о задании
char info[] =
"            Задание на лабораторную работу (вариант 15)\n"
"Разработать  программу, выполняющую лексический и синтаксический\n"
"разбора исходного  текста  программы, а также его интерпретацию.\n\n";

// Строка ввода
char str[1024];

// Массив адресов параметров строки ввода str (параметры - тексты, разделенные пустыми символами)
// В работе максимальное число параметров равно 10,
// но для контроля превышения этого числа добавляется еще один элемент массива
char *param[11];

// Количество параметров строки ввода str
int cnt;

// Разбор строки str на отдельные параметры
void unixp(void)
{
  char *s, c;

  cnt = 0;
  s = str;

  for (;;)
  {
    // Игнорируем пустые символы перед очередным параметром
    while ((c = *s) == ' ' || c == '\t' || c == '\n')
      ++s;

    // Параметры закончились?
    if (c == 0)
      break;

    // Не слишком ли много параметров?
    if (cnt == 11)
      return;

    // Запоминаем начало параметра
    param[cnt++] = s;

    // Ищем конец параметраw
    while ((c = *s) != ' ' && c != '\t' && c != '\n' && c)
      ++s;

    // Строка параметров закончена?
    if (c == 0)
      break;

    // Нет, записываем двоичный нуль в конце параметра
    *s++ = 0;
  }
}

// Исходная программа
List Prog;

// Загрузка программы
void load(void)
{
  FILE *f;
  char *s;

  if (cnt != 2)
  {
    printf("Ошибочно число параметров\n");
    return;
  }

  if ((f = fopen(param[1], "r")) == NULL)
  {
    printf("Файл не найден\n");
    return;
  }

  ok_compile = false;

  Prog.erase();
  while (fgets(str, sizeof(str) - 1, f))
  {
    // Удаляем возможный перевод строки
    if ((s = strchr(str, '\n')) != NULL)
      *s = 0;
    Prog.Add(str);
  }
  fclose(f);
}

// Печать программы
void print(void)
{
  char     txt[20];
  unsigned len;
  int      i;

  if (cnt != 1)
  {
    printf("Ошибочно число параметров\n");
    return;
  }

  if (Prog.Count == 0)
  {
    printf("Программа не загружена\n");
    return;
  }

  // Определим ширину номера строки
  len = sprintf(txt, "%d", Prog.Count);

  // Печатаем программу
  Prog.Begin();
  for (i = 0; i < Prog.Count; ++i)
    printf("[%*d] %s\n", len, i + 1, Prog.Get().c_str());
}

// Выполнение компиляции
void make_compile(void)
{
  int  i;
  bool b;

  if (cnt != 1)
  {
    printf("Ошибочно число параметров\n");
    return;
  }

  if (Prog.Count == 0)
  {
    printf("Программа не загружена\n");
    return;
  }

  // Выполним трансляцию
  Scan->result = "";
  Scan->ind_result = 0;
  ok_compile = false;
  try
  {
    Scan->InitLex(&Prog);
    compile();

    // Сохраним остаток лексем для показа
    if (Scan->ind_result)
      Scan->memo->Add(Scan->result);

    printf("Программа не содержит ошибок\n");

    // В случае корректной трансляции формируем возможный
    // список неиспользованных переменных
    b = true;
    for (i = 0; i < cnt_var; ++i)
    {
      if (yes_var[i] == 0)
      {
        if (b)
        {
          b = false;
          printf ("\nНеиспользованные переменные:\n");
        }
        printf("%s\n", var[i].c_str());
      }
    }
    ok_compile = true;
  }

  // Перехват ошибочной ситуации
  catch (int)
  {
    printf("Программа ошибочна\n");
  }
}

// Вернуть true, если допустима печать таблиц
bool ok_table(void)
{
  if (cnt != 1)
  {
    printf("Ошибочно число параметров\n");
    return false;
  }

  if (!ok_compile)
  {
    printf("Выполните безошибочную компиляцию\n");
    return false;
  }

  return true;
}

// Печать таблицы лексического разбора
void show_lex(void)
{
  if (ok_table())
    Scan->show_lex();
}

// Печать таблицы идентификаторов
void show_ident(void)
{
  if (ok_table())
    Scan->show_ident();
}

// Печать таблицы чисел
void show_num(void)
{
  if (ok_table())
    Scan->show_num();
}

// Печать таблицы строк
void show_str(void)
{
  if (ok_table())
    Scan->show_str();
}

// Печать таблицы ПОЛИЗа
void show_poliz(void)
{
  if (ok_table())
    info_poliz();
}

// Печать таблицы служебных слов
void show_word(void)
{
  int        i;
  const char *s;

  printf("Таблица служебных слов [0]\n");
  for (i = 0; (s = word[i].txt) != NULL; ++i)
    printf("%2d: %s\n", i, s);
}

// Печать таблицы разделителей
void show_delim(void)
{
  printf("Таблица разделителей [1]\n");
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

// Описание грамматики
const char gr[] =
"\n"
" При описании синтаксиса модельного языка приняты следующие соглашения:\n"
"\n"
"- элементы, взятые в скобки '{_' и '_}' могут повторяться 0 и более раз;\n"
"- элементы, взятые в скобки '{/' и '/}' обязаны повторяться минимум 1 раз;\n"
"- элементы, взятые в скобки '[' и ']' могут быть опущены;\n"
"- элементы, взятые в скобки '(_' и '_)' обязательны и выбираются из заданного списка только как один элемент;\n"
"- альтернативные элементы разделяются символом '|';\n"
"- левая и правая части синтаксического правила разделяются символами '::=';\n"
"- символы {, }, ( и ) являются лексемами для обозначения фигурных и обычных скобок.\n"
"\n"
"                                        Синтаксис модельного языка\n"
"\n"
"<выражение>                  ::= <операнд> {_<операции_группы_отношения> <операнд>_}\n"
"<операнд>                    ::= <слагаемое> {_<операции_группы_сложения> <слагаемое>_}\n"
"<слагаемое>                  ::= <множитель> {_<операции_группы_умножения> <множитель>_}\n"
"<множитель>                  ::= <идентификатор> | <число> | rand | rand_real | <логическая_константа>\n"
"                                 <унарная_операция> <множитель> | (<выражение>)\n"
"<логическая_константа>       ::= true | false\n"
"<строка>                     ::= \"<произвольный набор символов в кавычках, # используется как символ перевода строки>\"\n"
"<число>                      ::= <целое> | <действительное>\n"
"<идентификатор>              ::= <буква> {_ <буква> | <цифра> _}\n"
"<буква>                      ::= A | B | C | D | E | F | G | H | I | J | K | L | M |\n"
"                                 N | O | P | Q | R | S | T | U | V | W | X | Y | Z |\n"
"                                 a | b | c | d | e | f | g | h | i | j | k | l | m |\n"
"                                 n | o | p | q | r | s | t | u | v | w | x | y | z | _\n"
"<цифра>                      ::= 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9\n"
"<целое>                      ::= <двоичное> | <восьмеричное> | <десятичное> | <шестнадцатеричное>\n"
"<двоичное>                   ::= {/ 0 | 1 /} (_ B | b _)\n"
"<восьмеричное>               ::= {/ 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 /} (_ O | o _)\n"
"<десятичное>                 ::= {/ <цифра> /} [_ D | d _]\n"
"<шестнадцатеричное>          ::= <цифра> {_ <цифра> | A | B | C | D | E | F | a | b | c | d | e | f _} (_ H | h _)\n"
"<действительное>             ::= <числовая_строка> <порядок> | [<числовая_строка>].<числовая_строка> [<порядок>]\n"
"<числовая_строка>            ::= {/ <цифра> /}\n"
"<операции_группы_отношения>  ::= != | == | < | <= | > | >=\n"
"<операции_группы_сложениия>  ::= +  | - | ||\n"
"<операции_группы_умножениия> ::= *  | / | && | %\n"
"<унарная_операция>           ::= !  | -\n"
"<программа>                  ::= { {/ (_ <описание> | <оператор> _); /} }\n"
"<описание>                   ::= dim <идентификатор> {_,<идентификатор>_} <тип>\n"
"<тип>                        ::= integer | real | boolean\n"
"<оператор>                   ::= <составной> | <присваивание> | <условный> | <фиксированный_цикл> | <условный_цикл>\n"
"                                 <ввод> | <вывод> | continue | break | exit | erase | variable | pause [<строка>]\n"
"<составной>                  ::= begin <оператор> {_ ; <оператор> _} end\n"
"<присваивание>               ::= <переменная> := <выражение>\n"
"<условный>                   ::= if (<выражение>) <оператор> [ else <оператор> ]\n"
"<фиксированный_цикл>         ::= for <присваивание> to <выражение> [step <выражение>] <оператор> next\n"
"<условный_цикл>              ::= while (<выражение>) <оператор>\n"
"<ввод>                       ::= readln [<строка>] <переменная> {_ ,[<строка>] <переменная>_}\n"
"<вывод>                      ::= writeln [<печать>] | write <печать>\n"
"<печать>                     ::= <печать2> {_ , <печать2> _}\n"
"<печать2>                    ::= <строка> | <выражение>[: <целое>]\n"
"<комментарий>                ::= // или /* ... */\n";

int main()
{
  int i;
  char PI[] = "3.14";

  // Инициализация датчика случайных чисел
  srand (time (NULL));

  // Определяем символ разделения дробной и целой
  // части вещественного числа
  if (atof(PI) <= 3.0)
    sym_point = ',';

  // Установить кодировку печатаемых символов
  // для корректной печати и ввода русского текста
  SetConsoleCP(1251);
  SetConsoleOutputCP(1251);

  // Печать информации о задании
  printf(info);

  // Печать помощи о выполняемых командах
  printf(help);

  Scan = new Lex;

  // Выполнение команд пользователя
  for (;;)
  {
    // Приглашение на ввод команды
    printf("/");

    // Ввод команды
    gets(str);

    // Разбор параметров
    unixp();

    // Пустая строка игнорируется
    if (cnt == 0)
      continue;

    // Распознаем команды
    for (i = 0; i <= _EXIT; ++i)
    {
      if (stricmp(cmd[i].name, param[0]) == 0)
        break;
    }

    if (i > _EXIT)
    {
      printf("Неизвестная команда\n");
      continue;
    }

    // Выполняем команды
    switch (i)
    {

    default:
      if (cnt != 1)
      {
        printf("Ошибочно число параметров\n");
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
