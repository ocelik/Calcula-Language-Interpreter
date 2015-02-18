#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <locale.h> //türkçe karakter yazdırmak için.
#include <stdbool.h> // to use bool

/* Charachter classes */
#define LETTER 0
#define DIGIT 1
#define OTHER 2

/* Token codes */
#define KEYWORD 10
#define IDENT 11
#define INT_LIT 12
#define STR_LIT 13
#define ASSIGN_OP 14
#define LEFT_PAREN 15
#define RIGHT_PAREN 16
#define ADD_OP 17
#define SUB_OP 18
#define DIV_OP 19
#define MULT_OP 20
#define EOL_SYM 21

/*Exception Codes*/
#define EX_LONG_ID 100
#define EX_STRING_UNCLOSED 101
#define EX_ID_DIGIT 102
#define EX_UNKNOWN 103
#define EX_UNCLOSED_PAR 104
#define EX_OUT_UNR 105
#define EX_EXPR_UNR 106

#define MAX 50



typedef char *string;
struct lexx
{
    char str_lit[100];
    int token;
    int deger;
};

struct lexx expr_list[20][100];
struct lexx id_list[20];
int expr_no=0;
int tok_no=0;
int out_no=0;
struct lexx outs[100];
/* Global Declarations */
struct lexx matrice[20][100]= {{}};
int charClass;
char lexeme[100];
char nextChar;
int lexLen;
int nextToken;
int lineNo = 1;
int columnNo = 0;
long int position;
FILE *x_cal, *fopen();

/*Function Declarations */
void addChar();
void getChar();
void getNonBlank();
int lookUp(char ch);
int lex();
int isKeyword();
void exceptionHandler(int exception);
void parser();
void expr();
void term();
void fact();
void outputSt();
void addOut();

string keyword_list[1]= {"OUTPUT"};
string token_list[12]= {"KEYWORD","IDENT","INT_LIT","STR_LIT","ASSIGN_OP","LEFT_PAREN","RIGHT_PAREN","ADD_OP",
                        "SUB_OP","DIV_OP","MULT_OP","EOL_SYM"
                       };

void push(int *yigin, int *tepe, int yeni);
int pop(int *yigin, int *tepe);
int tepe_eleman(int *yigin, int tepe);
int bos_mu(int tepe);
int oncelik_bul(char karakter);
void ortaek_sonek_donustur(struct lexx infix[20][100], char *sonek, int rw);
int hesapla(char *sonek);


int main()
{
    setlocale(LC_ALL,"Turkish");

    char dosya_adi[20];
    printf("Lutfen '.cal' dosyasinin ismini uzantisiz olarak giriniz: ");
    gets(dosya_adi);

    strcat(dosya_adi,".cal");

    if((x_cal=fopen(dosya_adi,"r"))== NULL)
        printf("ERROR - cannot open fort.txt \n");
    else
    {
        parser();

        fclose(x_cal);
    }
    int i;
    for(i=0; i<expr_no; i++)
    {
        char sonek[50];
        ortaek_sonek_donustur(expr_list,sonek,i);
        id_list[i].deger=hesapla(sonek);
    }

    for(i=0; i<out_no; i++)
    {
        if(outs[i].token==STR_LIT)
            printf("%s ",outs[i].str_lit);
        else if(outs[i].token==IDENT)
        {
            int j = 0;
            for(j=0; j<expr_no; j++)
            {
                if(strcmp(outs[i].str_lit,id_list[j].str_lit)==0)
                {
                    printf(" %d ",id_list[j].deger);
                }
            }
        }
    }
    return 0;
}

void push(int *yigin, int *tepe, int yeni)
{
    //hem donusturme hem hesaplamada ayni push fonksiyonu (tamsayi ekleyen) kullaniliyor
    //donusturme isleminde yigina ekleme yapilirken karakterin ascii kodu eklenmektedir
    if(*tepe==MAX-1)
        printf("yigin dolu!\n");
    else
    {
        *tepe=*tepe+1;
        yigin[*tepe]=yeni;
    }

    return;
}


int pop(int *yigin, int *tepe)
{
    //hem donusturme hem hesaplamada ayni pop fonksiyonu (tamsayi donduren) kullaniliyor
    //donusturme isleminde yigindan cikarilan sayi karaktere cevrilerek kullanilmaktadir.
    int gecici;

    gecici=yigin[*tepe];
    *tepe=*tepe-1;

    return gecici;
}
int tepe_eleman(int *yigin, int tepe)//tepedeki elemani dondur ama yigindan cikarma
{
    return yigin[tepe];
}

int bos_mu(int tepe)
{
    return tepe==-1;//yigin bossa dogru(1) bos degilse yanlis(0) dondur
}

int oncelik_bul(char karakter)
{
    int oncelik=3;// */+- disindaki tum karakterler icin

    switch (karakter)
    {
    case '*':
    case '/':
        oncelik=2;
        break;
    case '+':
    case '-':
        oncelik=1;
        break;
    }

    return oncelik;
}

void ortaek_sonek_donustur(struct lexx infix[20][100], char *sonek,int rw)
{
    int yigin[MAX];//yiginda karakterlerin ascii kodu saklanacak...
    int tepe=-1;
    int i,j=0;

    push(yigin,&tepe,(infix[rw][0].str_lit)[0]);//ilk karakter direk yigina ekleniyor
    for(i=1; (infix[rw][i].str_lit)[0]!='\0'; i++) //ortaek ifadenin sonuna kadar...
    {
        //yigin bos degilse ve tepedeki elemanin onceligi eldekinin onceligine esit
        //ya da daha buyuk ise onu yigindan cikar ve sonek ifadenin sonuna ekle
        while(!bos_mu(tepe) && oncelik_bul(tepe_eleman(yigin,tepe))>=oncelik_bul((infix[rw][i].str_lit)[0]))
        {
            sonek[j]=pop(yigin,&tepe);
            j++;
        }
        push(yigin,&tepe,(infix[rw][i].str_lit)[0]);//eldekini yigina ekle
    }

    //yiginda kalanlari sonek ifadeye ekle
    while(!bos_mu(tepe))
    {
        sonek[j]=pop(yigin,&tepe);
        j++;
    }

    sonek[j]='\0';//sonek ifade, string haline getiriliyor

    return;
}

int hesapla(char *sonek)
{
    int yigin[MAX];
    int tepe=-1;
    int i,sayi,operand1,operand2;

    for(i=0; sonek[i]!='\0'; ++i)
    {
        if(isdigit(sonek[i]))//karakter, rakam ise...
        {
            sayi=sonek[i]-48;//sayiya donustur(0-9 arasi rakamlarin ascii kodlari 48-57 arasinda
            push(yigin,&tepe,sayi);//ve yigina ekle
        }
        else if(isalpha(sonek[i])) //karakter, harf ise...
        {
            int j;
            for(j=0; j<expr_no; j++)
            {
                if((id_list[j].str_lit)[0]==sonek[i])
                {
                    sayi=id_list[j].deger;
                    break;
                }
            }
            push(yigin,&tepe,sayi);//ve yigina ekle
        }
        else//karakter, bir islem operatoru ise...
        {
            operand2=pop(yigin,&tepe);//ikinci operandi yigindan cikar
            operand1=pop(yigin,&tepe);//ilk operandi yigindan cikar

            switch(sonek[i])//islemi yap ve sonucunu yigina ekle
            {
            case '*':
                push(yigin,&tepe,operand1*operand2);
                break;
            case '/':
                push(yigin,&tepe,operand1/operand2);
                break;//tam bolme yapiliyor!
            case '+':
                push(yigin,&tepe,operand1+operand2);
                break;
            case '-':
                push(yigin,&tepe,operand1-operand2);
                break;
            }
        }
    }

    return pop(yigin,&tepe);//yiginda kalan son eleman sonek ifadenin sonucudur
}








void addChar() //lexeme'e o karakteri ekliyor.
{
    if(lexLen<=98)
    {
        lexeme[lexLen++] = nextChar;
        lexeme[lexLen]=0;
    }
    else printf("Error - too long");
}

void getChar() //Bir sonraki karakteri alýyor.
{
    if ((nextChar=getc(x_cal))!=EOF)
    {
        if(isalpha(nextChar))
        {
            charClass = LETTER;
            nextChar=toupper(nextChar);
        }
        else if (isdigit(nextChar))
            charClass = DIGIT;
        else
            charClass = OTHER;
        columnNo++;
    }
    else charClass = EOF;

    if(nextChar=='\n')
    {
        lineNo++;
        columnNo=0;
    }
}

void getNonBlank() //Boþluklarý geçiyor.
{
    while(isspace(nextChar))
        getChar();
}

void exceptionHandler(int exception) //Hatalarý belirtiyor.
{
    switch(exception)
    {
    case EX_LONG_ID:
        printf("Error > Line %d, Column: %d: Identifier is too long! (%s)\n",lineNo,columnNo,lexeme);
        break;

    case EX_ID_DIGIT:
        printf("Error > Line %d, Column %d: Identifiers MUST start with a letter!\n",lineNo,columnNo);
        break;

    case EX_STRING_UNCLOSED:
        printf("Error > Line %d, Column %d: Unclosed String Literal!\n",lineNo,columnNo);
        nextToken = 0;
        break;

    case EX_UNKNOWN:
        printf("Error > Line %d, Column %d: Unknown charachter! (%s)\n",lineNo,columnNo,lexeme);
        nextToken = 0;
        break;

    case EX_UNCLOSED_PAR:
        printf("Error > Line %d, Column %d: Unclosed Parenthesis!\n",lineNo,columnNo);
        break;

    case EX_OUT_UNR:
        printf("Error > Line %d, Column %d: Unrecognized token at Output Clause!\n",lineNo,columnNo);
        break;

    case EX_EXPR_UNR:
        printf("Error > Line %d, Column %d: Unrecognized token at Arithmetic Expresion!\n",lineNo,columnNo);
        break;
    }
}

int isKeyword() //Keyword olup olmadýðýna bakýyor ve ona göre atama yapýyor.
{
    int i,flag=0;

    for(i=0; i<1; i++)
    {
        if(strcmp(lexeme,keyword_list[i])==0)
        {
            flag=1;
            return KEYWORD;
        }
    }
    if(flag==0 && strlen(lexeme)>25)
    {
        exceptionHandler(EX_LONG_ID);
        return 0;
    }
    return IDENT;
}

int lookUp(char ch) //Sembol,Operatör türünü belirliyor.
{
    switch (ch)
    {
    case '(':
        nextToken=LEFT_PAREN;
        break;

    case ')':
        nextToken=RIGHT_PAREN;
        break;

    case ';':
        nextToken=EOL_SYM;
        break;

    case '*':
        position=ftell(x_cal);
        getChar();

        if (isdigit(nextChar) || isalpha(nextChar) || nextChar=='(')
        {
            nextToken=MULT_OP;
            fseek(x_cal,position,SEEK_SET);
        }
        break;

    case ':':
        position=ftell(x_cal);
        getChar();

        if(nextChar=='=')
        {
            addChar();
            nextToken=ASSIGN_OP;
        }

        else
        {
            exceptionHandler(EX_UNKNOWN);
            fseek(x_cal,position,SEEK_SET);
        }
        break;

    case '+':
        nextToken=ADD_OP;
        break;

    case '-':
        nextToken=SUB_OP;
        break;

    case '/':
        nextToken=DIV_OP;
        break;

    case '\"':
        position=ftell(x_cal);
        do
        {
            getChar();
            if(nextChar!='\"')
                addChar();
        }
        while(nextChar!=';' && nextChar!='\"');

        if(nextChar=='\"')
        {
            nextToken=STR_LIT;
        }
        else
        {
            exceptionHandler(EX_STRING_UNCLOSED);
            position=ftell(x_cal);
        }
        break;

    default:
        exceptionHandler(EX_UNKNOWN);
        break;
    }
    return nextToken;
}

int lex () //Tokenlarý belirliyor,dosyaya ve ekrana yazýyor.
{

    lexLen=0;
    getNonBlank();

    switch(charClass)
    {
    case LETTER:
        addChar();
        getChar();
        while(charClass==LETTER || charClass == DIGIT || nextChar=='_')
        {
            addChar();
            getChar();
        }
        nextToken=isKeyword();
        break;

    case DIGIT:
        addChar();
        getChar();
        while(charClass == DIGIT)
        {
            addChar();
            getChar();
        }
        nextToken=INT_LIT;
        break;

    case OTHER:
        if(nextChar!='\"')
            addChar();
        lookUp(nextChar);
        getChar();
        break;

    case EOF:
        nextToken = EOF;
        lexeme[0] = 'E';
        lexeme[1] = 'O';
        lexeme[2] = 'F';
        lexeme[3] = 0;
        break;
    }
    return nextToken;
}

void parser()
{
    getChar();
    lex();
    do
    {
        if(nextToken==ASSIGN_OP)
        {
            lex();

            expr_list[expr_no][tok_no].token=nextToken;
            strcpy(expr_list[expr_no][tok_no].str_lit,lexeme);
            tok_no++;
            expr();
            expr_no++;
            tok_no=0;
        }

        else if (nextToken==KEYWORD)
            outputSt();
        else
        {
            if(nextToken==IDENT)
            {
                id_list[expr_no].token=nextToken;
                strcpy(id_list[expr_no].str_lit,lexeme);
            }
            lex();
        }
    }
    while(nextToken!=EOF);
}

void expr()
{
    term();

    while (nextToken == ADD_OP || nextToken == SUB_OP)
    {
        lex();
        expr_list[expr_no][tok_no].token=nextToken;
        strcpy(expr_list[expr_no][tok_no].str_lit,lexeme);
        tok_no++;
        term();
    }
}

void term()
{
    fact();
    /* As long as the next token is * or /, get the
    next token and parse the next factor */

    while (nextToken == MULT_OP || nextToken == DIV_OP)
    {
        lex();
        expr_list[expr_no][tok_no].token=nextToken;
        strcpy(expr_list[expr_no][tok_no].str_lit,lexeme);
        tok_no++;
        fact();
    }
}

void fact()
{
    if (nextToken == IDENT || nextToken == INT_LIT)
    {
        /* Get the next token */
        lex();
        if(nextToken!=EOL_SYM)
        {
            expr_list[expr_no][tok_no].token=nextToken;
            strcpy(expr_list[expr_no][tok_no].str_lit,lexeme);
            tok_no++;
        }
    }

    else
    {
        if (nextToken == LEFT_PAREN)
        {
            lex();
            expr_list[expr_no][tok_no].token=nextToken;
            strcpy(expr_list[expr_no][tok_no].str_lit,lexeme);
            tok_no++;
            expr();

            if (nextToken == RIGHT_PAREN)
            {
                lex();
                expr_list[expr_no][tok_no].token=nextToken;
                strcpy(expr_list[expr_no][tok_no].str_lit,lexeme);
                tok_no++;
            }
            else
                exceptionHandler(EX_UNCLOSED_PAR);
        }
        else
            exceptionHandler(EX_EXPR_UNR);
    }
}

void outputSt()
{
    do
    {
        lex();

        if(nextToken==IDENT || nextToken==STR_LIT)
        {
            strcpy(outs[out_no].str_lit,lexeme);
            outs[out_no].token=nextToken;
            out_no++;
        }
        //mainde string sabitine en son yazdırmada kullanılack bişey yapılcak

        else if(nextToken!=EOL_SYM)
        {
            exceptionHandler(EX_OUT_UNR);
        }

    }
    while(nextToken!=EOL_SYM);
}
