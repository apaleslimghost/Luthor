#include <stdio.h>
#include <string.h>
#include <pcre.h>

#include "deps/vec/vec.h"

char* substr(char* str, int i, int j) {
	char* sub = malloc(j - i + 1);
	strncpy(sub, str + i, j - i);
	sub[j - i] = '\0';
	return sub;
}

enum LexResultType { LexToken, LexError };

typedef struct {
	enum LexResultType type;
	char* name;
	char* content;
	char* rest;
} LexResult;

typedef struct {
	char* name;
	pcre* regex;
} LexTokenSpec;

typedef struct {
	LexTokenSpec* tokens;
	int l;
} LexTokens;

LexResult lexOne(char* str, LexTokens* tokens) {
	int i, rc;
	LexTokenSpec token;
	for(i = 0; i < tokens->l; i++) {
		token = tokens->tokens[i];
		int ovector[3];

		rc = pcre_exec(token.regex, NULL, str, strlen(str), 0, 0, ovector, 3);
		if(rc >= 0) {
			LexResult result = {
				.type = LexToken,
				.name = token.name,
				.content = substr(str, ovector[0], ovector[1]),
				.rest = str + (ovector[1] - ovector[0])
			};
			return result;
		}
	}

	char message[50];
	sprintf(message, "No tokens matched %.*s", 30, str);

	LexResult err = {
		.type = LexError,
		.name = "No match",
		.content = message
	};
	return err;
}

char* resultType(enum LexResultType type) {
	switch(type) {
		case LexError: return "Error";
		case LexToken: return "Token";
	}
}

void printLexResult(LexResult* result) {
	printf("%s: %s %s (rest %s)\n", resultType(result->type), result->name, result->content, result->rest);
}

pcre* r(char* reg) {
	const char* err;
	int erroff;
	return pcre_compile(reg, 0, &err, &erroff, NULL);
}

typedef vec_t(LexResult) vec_result_t;

vec_result_t lex(char* str, LexTokens* tokens) {
	char* rest = str;

	vec_result_t results;
	vec_init(&results);

	LexResult result;

	while(strlen(rest)/* && ((LexResult*) results.content)->type != LexError*/) {
		result = lexOne(rest, tokens);
		rest = result.rest;
		vec_push(&results, result);
	}
	return results;
}

int main(int argc, char** argv) {
	LexTokenSpec t[2] = {
		{"FOO", r("foo")},
		{"BA",  r("ba\\w")} 
	};
	LexTokens tokens = {t, 2};

	int i; LexResult result;
	vec_result_t results = lex("foobarbaz", &tokens);
	vec_foreach(&results, result, i) {
		printLexResult(&result);
	}
}
