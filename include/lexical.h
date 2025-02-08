#pragma once
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define IDENTIFIER 0
#define BOOL 1
#define BREAK 2
#define BYTE 3
#define CASE 4
#define CATCH 5
#define CLASS 6
#define CONST 7
#define CONTINUE 8
#define DO 9
#define ELSE 10
#define ENUM 11
#define FINALLY 12
#define FLOAT 13
#define FOR 14
#define IF 15
#define IMPORT 16
#define INSTANCEOF 17
#define INT 18
#define LONG 19 
#define PRIVATE 20
#define RETURN 21
#define SIMPLE 22
#define SHORT 23
#define STATIC 24
#define STRING 25
#define SWITCH 26
#define THROW 27
#define UNION 28
#define TRY 29
#define UNSIGNED 30
#define VOID 31
#define VOLATILE 32
#define WHILE 33
#define SEMICOLON 34
#define COLON 35
#define ARITHMETIC 36
#define COMPARISON 37
#define LPAREN 38
#define RPAREN 39
#define LBRACK 40
#define RBRACK 41
#define SQUOTE 42
#define DQUOTE 43
#define BTRICK 44
#define TOTAL_TOKEN_SIZE 45

typedef struct 
{
	uint8_t token;
	char *lexemen;
} Token;

Token* getValidTokens();

size_t lexicalAnalyze(FILE* input, Token *output);