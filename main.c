#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>

typedef struct
{
    char lexeme[100];
    char type[20]; // i used TokenType but i reverted it to follow the document structur
    char value[100];
    int line;
} Token;

// typedef enum TokenType {
//     KEYWORD,
//     IDENTIFIER,
//     OPERATOR,
//     NUMBER,
//     SEPARATOR
// } TokenType;

Token *init_token(char *lexeme, char *type, char *value, int line)
{
    Token *token = (Token *)malloc(sizeof(Token));
    strcpy(token->lexeme, lexeme);
    strcpy(token->value, value);
    strcpy(token->type, type);
    token->line = line;
    return token;
}
int total_lines(FILE *file)
{
    int len = 0;
    char *line = (char *)malloc(sizeof(char) * 200);
    while (fscanf(file, "%[^\n]\n", line) != EOF)
    {
        len++;
    }
    free(line);
    return len;
}

int isKeyword(const char *word)
{
    const char *keywords[] = {"int", "if", "else", "while", "return"};

    for (int i = 0; i < sizeof(keywords) / 8; ++i)
    {
        if (strcmp(keywords[i], word) == 0)
            return 1;
    }
    return 0;
}

int isOpratorChar(char c)
{
    const char *operators[] = {"+", "-", "*", "/", "=", "<", ">", "!"};

    for (int i = 0; i < sizeof(operators) / 8; ++i)
    {
        if (*operators[i] == c)
            return 1;
    }

    return 0;
}

int isIdentifierStart(char c)
{
    if (c == '_' || isalpha(c))
        return 1;
    return 0;
}

int isIdentifierChar(char c)
{
    if (isalnum(c) || c == '_' || c == '-')
        return 1;
    return 0;
}

char * skip_preprocess(char *raw)
{
    int line = 1;

        while(isspace(*raw) != 0 && *raw != '\0'){
            printf("Runned .... \n");
        raw++;
    }
    printf("value of text is : %c\n", *raw);
    while(*raw == '\n' || *raw == '\t' && *raw != '\0'){
        line++;
        raw++;
    }
     if (*raw == '/' && *(raw++) == '/' && *raw != '\0'){
        while (*raw != '\n'){
            raw++;
        }
        line++;
     }

     if (*raw == '/' && *(raw++) == '*' && *raw != '\0'){
        while(*raw != '*' && *(raw++) != '/'){
            if (*raw == '\n') line++;
            raw++;
        }
     }
    
    return raw;
}

int main(int argc, char *argv[])
{
    FILE *file;
    Token *token = init_token("int", "KEYWORD", "int", 1);
    printf("Token Lexeme: %s\n", token->lexeme);
    printf("Token Type: %s\n", token->type);
    printf("Token Value: %s\n", token->value);
    printf("Token Line: %d\n", token->line);
    // printf("Is keyword : %d \n", isKeyword("float"));
    // printf("Is operator : %d \n", isOpratorChar('='));
    char *rawText =
    "   int age = 21; // User age\n"
    "\n"
    "   /* This is a\n"
    "      multi-line comment\n"
    "      with multiple lines */\n"
    "\n"
    "   int height = 180;\t// Height in cm\n"
    "\n"
    "   // Another comment\n"
    "   return age;";
    printf("Result is :\n%c", (*skip_preprocess(rawText)));

    return 0;
   
}