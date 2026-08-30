#pragma once

#include "pocketpy/common/vector.h"
#include "pocketpy/compiler/lexer.h"
#include "pocketpy/objects/sourcedata.h"
#include "pocketpy/objects/codeobject.h"

Error* pk_compile(SourceData_ src, CodeObject* out);

/// Rewrites the token stream after lexing, before parsing. `*p_tokens` is a
/// `PK_MALLOC`-ed array owned by the compiler; a filter that changes its length
/// must `PK_FREE` the old one and store a new one. Return `NULL`, or an `Error*`
/// to abort compilation.
typedef Error* (*pk_TokenFilter)(SourceData_ src, Token** p_tokens, int* p_length);

extern pk_TokenFilter pk_token_filter;
