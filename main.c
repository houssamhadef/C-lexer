#include <stdio.h>

#include <stdlib.h>

#include <ctype.h>

#include <string.h>

#define SIZE 100

typedef struct

{

    char lexeme[SIZE];

    char type[20];

    char value[SIZE];

    int line;

} Token;

Token *init_token(char *lexeme, char *type, char *value, int line)

{

    Token *token = malloc(sizeof(Token));

    if (token == NULL)

        return NULL;

    strcpy(token->lexeme, lexeme);

    strcpy(token->type, type);

    strcpy(token->value, value);

    token->line = line;

    return token;

}

int total_lines(FILE *file)

{

    int len = 0;

    int ch;

    int has_content = 0;

    while ((ch = fgetc(file)) != EOF)

    {

        if (ch == '\n')

        {

            len++;

            has_content = 0;

        }

        else

            has_content = 1;

    }

    if (has_content)

        len++;

    rewind(file);

    return len;

}

int isKeyword(const char *word)

{

    const char *keywords[] = {"int", "char", "if", "else", "while", "return"};

    for (int i = 0; i < 6; i++)

    {

        if (strcmp(keywords[i], word) == 0)

            return 1;

    }

    return 0;

}

int isOperatorChar(char c)

{

    char operators[] = "+-*/=<>!";

    for (int i = 0; operators[i] != '\0'; i++)

    {

        if (operators[i] == c)

            return 1;

    }

    return 0;

}

int isDelimiterChar(char c)

{

    char delimiters[] = "(){};,";

    for (int i = 0; delimiters[i] != '\0'; i++)

    {

        if (delimiters[i] == c)

            return 1;

    }

    return 0;

}

int isIdentifierStart(char c)

{

    return isalpha((unsigned char)c) || c == '_';

}

int isIdentifierChar(char c)

{

    return isalnum((unsigned char)c) || c == '_';

}

int skip_preprocess(FILE *fp, int *line)

{

    int ch;

    while ((ch = fgetc(fp)) != EOF)

    {

        if (isspace((unsigned char)ch))

        {

            if (ch == '\n')

                (*line)++;

            continue;

        }

        if (ch == '/')

        {

            int next = fgetc(fp);

            if (next == '/')

            {

                while ((ch = fgetc(fp)) != '\n' && ch != EOF)

                    ;

                if (ch == '\n')

                    (*line)++;

                continue;

            }

            if (next == '*')

            {

                int prev = 0;

                while ((ch = fgetc(fp)) != EOF)

                {

                    if (ch == '\n')

                        (*line)++;

                    if (prev == '*' && ch == '/')

                        break;

                    prev = ch;

                }

                if (ch == EOF)

                    return -2;

                continue;

            }

            if (next != EOF)

                ungetc(next, fp);

            return '/';

        }

        return ch;

    }

    return EOF;

}

char *recognize_identifier_keyword(char *lexeme)

{

    if (isKeyword(lexeme))

        return "KEYWORD";

    return "IDENTIFIER";

}

Token *recognize_integers(char *lexeme, Token *token)

{

    strcpy(token->lexeme, lexeme);

    strcpy(token->type, "NUMBER");

    snprintf(token->value, SIZE, "%d", atoi(lexeme));

    return token;

}

Token getNextToken(FILE *fp)

{

    static int line = 1;

    Token token = {0};

    int ch = skip_preprocess(fp, &line);

    token.line = line;

    if (ch == EOF)

    {

        strcpy(token.type, "EOF");

        return token;

    }

    if (ch == -2)

    {

        strcpy(token.type, "ERROR");

        strcpy(token.value, "Unterminated comment");

        fprintf(stderr, "Error: Unterminated comment at line %d\n", token.line);

        return token;

    }

    if (isIdentifierStart((char)ch))

    {

        int i = 0;

        token.lexeme[i++] = (char)ch;

        while ((ch = fgetc(fp)) != EOF && isIdentifierChar((char)ch))

        {

            if (i < SIZE - 1)

                token.lexeme[i++] = (char)ch;

        }

        if (ch != EOF)

            ungetc(ch, fp);

        token.lexeme[i] = '\0';

        strcpy(token.type, recognize_identifier_keyword(token.lexeme));

        return token;

    }

    if (isdigit((unsigned char)ch))

    {

        int i = 0;

        int next;

        token.lexeme[i++] = (char)ch;

        while ((next = fgetc(fp)) != EOF && isdigit((unsigned char)next))

        {

            if (i < SIZE - 1)

                token.lexeme[i++] = (char)next;

        }

        if (next != EOF && (isalpha((unsigned char)next) || next == '_'))

        {

            if (i < SIZE - 1)

                token.lexeme[i++] = (char)next;

            while ((next = fgetc(fp)) != EOF && isIdentifierChar((char)next))

            {

                if (i < SIZE - 1)

                    token.lexeme[i++] = (char)next;

            }

            if (next != EOF)

                ungetc(next, fp);

            token.lexeme[i] = '\0';

            strcpy(token.type, "ERROR");

            strcpy(token.value, "Invalid identifier");

            fprintf(stderr, "Error: Invalid identifier '%s' at line %d\n", token.lexeme, token.line);

            return token;

        }

        if (next == '.')

        {

            if (i < SIZE - 1)

                token.lexeme[i++] = '.';

            while ((next = fgetc(fp)) != EOF && isdigit((unsigned char)next))

            {

                if (i < SIZE - 1)

                    token.lexeme[i++] = (char)next;

            }

            if (next != EOF)

                ungetc(next, fp);

            token.lexeme[i] = '\0';

            strcpy(token.type, "ERROR");

            strcpy(token.value, "Floating numbers not supported");

            fprintf(stderr, "Error: Unsupported number format '%s' at line %d\n", token.lexeme, token.line);

            return token;

        }

        if (next != EOF)

            ungetc(next, fp);

        token.lexeme[i] = '\0';

        recognize_integers(token.lexeme, &token);

        return token;

    }

    if (isOperatorChar((char)ch))

    {

        int next;

        token.lexeme[0] = (char)ch;

        token.lexeme[1] = '\0';

        if (ch == '=' || ch == '!' || ch == '<' || ch == '>')

        {

            next = fgetc(fp);

            if (next == '=')

            {

                token.lexeme[1] = '=';

                token.lexeme[2] = '\0';

            }

            else if (next != EOF)

                ungetc(next, fp);

        }

        strcpy(token.type, "OPERATOR");

        return token;

    }

    if (isDelimiterChar((char)ch))

    {

        token.lexeme[0] = (char)ch;

        token.lexeme[1] = '\0';

        strcpy(token.type, "DELIMITER");

        return token;

    }

    if (ch == '"')

    {

        int i = 0;

        int j = 0;

        int valid = 1;

        int closed = 0;

        token.lexeme[i++] = '"';

        while ((ch = fgetc(fp)) != EOF)

        {

            if (ch == '"')

            {

                if (i < SIZE - 1)

                    token.lexeme[i++] = '"';

                else

                    valid = 0;

                closed = 1;

                break;

            }

            if (ch == '\n')

            {

                line++;

                break;

            }

            if (ch == '\\')

            {

                int next = fgetc(fp);

                char value;

                if (i < SIZE - 2)

                    token.lexeme[i++] = '\\';

                else

                    valid = 0;

                if (next == EOF || next == '\n')

                {

                    if (next == '\n')

                        line++;

                    break;

                }

                if (i < SIZE - 1)

                    token.lexeme[i++] = (char)next;

                else

                    valid = 0;

                if (next == '"') value = '"';

                else if (next == '\\') value = '\\';

                else if (next == '\'') value = '\'';

                else if (next == 'n') value = '\n';

                else if (next == 't') value = '\t';

                else

                {

                    value = (char)next;

                    valid = 0;

                }

                if (j < SIZE - 1)

                    token.value[j++] = value;

                else

                    valid = 0;

            }

            else

            {

                if (i < SIZE - 1)

                    token.lexeme[i++] = (char)ch;

                else

                    valid = 0;

                if (j < SIZE - 1)

                    token.value[j++] = (char)ch;

                else

                    valid = 0;

            }

        }

        token.lexeme[i] = '\0';

        token.value[j] = '\0';

        if (!closed)

        {

            strcpy(token.type, "ERROR");

            strcpy(token.value, "Unterminated string literal");

            fprintf(stderr, "Error: Unterminated string literal at line %d\n", token.line);

        }

        else if (!valid)

        {

            strcpy(token.type, "ERROR");

            strcpy(token.value, "Invalid escape sequence");

            fprintf(stderr, "Error: Invalid escape sequence in string literal at line %d\n", token.line);

        }

        else

            strcpy(token.type, "STRING_LITERAL");

        return token;

    }

    if (ch == '\'')

    {

        int first = fgetc(fp);

        int next;

        int value = 0;

        int valid = 1;

        int closed = 0;

        token.lexeme[0] = '\'';

        token.lexeme[1] = '\0';

        if (first == EOF || first == '\n')

        {

            if (first == '\n')

                line++;

            strcpy(token.type, "ERROR");

            strcpy(token.value, "Unterminated char literal");

            fprintf(stderr, "Error: Unterminated character literal at line %d\n", token.line);

            return token;

        }

        if (first == '\\')

        {

            int escaped = fgetc(fp);

            if (escaped == EOF || escaped == '\n')

            {

                if (escaped == '\n')

                    line++;

                strcpy(token.type, "ERROR");

                strcpy(token.value, "Unterminated char literal");

                fprintf(stderr, "Error: Unterminated character literal at line %d\n", token.line);

                return token;

            }

            token.lexeme[1] = '\\';

            token.lexeme[2] = (char)escaped;

            token.lexeme[3] = '\0';

            if (escaped == 'n') value = '\n';

            else if (escaped == 't') value = '\t';

            else if (escaped == '\\') value = '\\';

            else if (escaped == '\'') value = '\'';

            else if (escaped == '"') value = '"';

            else

            {

                value = escaped;

                valid = 0;

            }

            next = fgetc(fp);

        }

        else

        {

            token.lexeme[1] = (char)first;

            token.lexeme[2] = '\0';

            value = first;

            next = fgetc(fp);

        }

        if (next == '\'')

        {

            if (first == '\\')

            {

                token.lexeme[3] = '\'';

                token.lexeme[4] = '\0';

            }

            else

            {

                token.lexeme[2] = '\'';

                token.lexeme[3] = '\0';

            }

            closed = 1;

        }

        else

        {

            while (next != EOF && next != '\n')

            {

                if (strlen(token.lexeme) < SIZE - 1)

                {

                    int len = (int)strlen(token.lexeme);

                    token.lexeme[len] = (char)next;

                    token.lexeme[len + 1] = '\0';

                }

                next = fgetc(fp);

            }

            if (next == '\n')

                line++;

            valid = 0;

        }

        if (closed && valid)

        {

            token.value[0] = (char)value;

            token.value[1] = '\0';

            strcpy(token.type, "CHAR_LITERAL");

        }

        else

        {

            strcpy(token.type, "ERROR");

            if (!closed)

            {

                strcpy(token.value, "Unterminated char literal");

                fprintf(stderr, "Error: Unterminated character literal at line %d\n", token.line);

            }

            else

            {

                strcpy(token.value, "Invalid char literal");

                fprintf(stderr, "Error: Invalid character literal at line %d\n", token.line);

            }

        }

        return token;

    }

    token.lexeme[0] = (char)ch;

    token.lexeme[1] = '\0';

    strcpy(token.type, "ERROR");

    strcpy(token.value, "Invalid symbol");

    fprintf(stderr, "Error: Invalid symbol '%c' at line %d\n", ch, token.line);

    return token;

}

int main(int argc, char *argv[])

{

    if (argc < 2)

    {

        printf("Usage: %s <source_file>\n", argv[0]);

        return 1;

    }

    FILE *file = fopen(argv[1], "r");

    if (file == NULL)

    {

        printf("Error while opening this file\n");

        return 1;

    }

    Token token;

    printf("%-6s %-20s %-20s %s\n", "Line", "Lexeme", "Token Type", "Value");

    while (1)

    {

        token = getNextToken(file);

        if (strcmp(token.type, "EOF") == 0)

            break;

        printf("%-6d %-20s %-20s %s\n",

               token.line,

               token.lexeme,

               token.type,

               token.value[0] == '\0' ? "-" : token.value);

    }

    fclose(file);

    return 0;

}
