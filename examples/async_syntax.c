/* `async def` / `await` as a token filter -- no compiler changes.
 *
 *      async def f():  ->  @coroutine <newline> def f():
 *      await x         ->  yield from x
 *
 * Build:
 *   clang -std=c11 -O1 -Iinclude -o main examples/async_syntax.c $(find src/ -name "*.c") -lm -ldl
 */

#include "pocketpy/compiler/compiler.h"
#include <stdio.h>
#include <string.h>

static bool is_id(const Token* t, const char* name) {
    return t->type == TK_ID && (size_t)t->length == strlen(name) &&
           memcmp(t->start, name, t->length) == 0;
}

/// A token on the same line as `like`. `text` must outlive the compilation.
static Token synth(TokenIndex type, const Token* like, const char* text) {
    Token t = {0};
    t.type = type;
    t.start = text;
    t.length = (int)strlen(text);
    t.line = like->line;  // keeps tracebacks pointing at the original source
    t.brackets_level = like->brackets_level;
    return t;
}

static Error* async_filter(SourceData_ src, Token** p_tokens, int* p_length) {
    Token* in = *p_tokens;
    int n = *p_length;
    c11_vector out;
    c11_vector__ctor(&out, sizeof(Token));

    for(int i = 0; i < n; i++) {
        Token* t = &in[i];
        if(is_id(t, "async") && i + 1 < n && in[i + 1].type == TK_DEF) {
            c11_vector__push(Token, &out, synth(TK_DECORATOR, t, "@"));
            c11_vector__push(Token, &out, synth(TK_ID, t, "coroutine"));
            c11_vector__push(Token, &out, synth(TK_EOL, t, ""));
            continue;  // drop `async`
        }
        if(is_id(t, "await")) {
            c11_vector__push(Token, &out, synth(TK_YIELD_FROM, t, "yield from"));
            continue;
        }
        c11_vector__push(Token, &out, *t);  // move: never copy an owning token twice
    }

    PK_FREE(in);
    *p_tokens = c11_vector__submit(&out, p_length);
    return NULL;
}

/* --- runner, just to try it out --- */

int main(int argc, char** argv) {
    if(argc < 2) return printf("usage: %s <script.py>\n", argv[0]), 1;
    FILE* f = fopen(argv[1], "rb");
    if(f == NULL) return printf("cannot open %s\n", argv[1]), 1;
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    char* source = PK_MALLOC(size + 1);
    source[fread(source, 1, size, f)] = 0;
    fclose(f);

    py_initialize();
    pk_token_filter = async_filter;
    bool ok = py_exec(source, argv[1], EXEC_MODE, NULL);
    if(!ok) py_printexc();
    PK_FREE(source);
    py_finalize();
    return ok ? 0 : 1;
}
