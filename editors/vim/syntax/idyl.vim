" Vim syntax file
" Language:    Idyl
" Maintainer:  Idyl contributors
" Filenames:   *.idl
" URL:         https://github.com/...  (update as needed)

if exists('b:current_syntax')
  finish
endif

let s:cpo_save = &cpo
set cpo&vim

" ── Comments ────────────────────────────────────────────────────
syn match   idylComment       '//.*$'           contains=idylTodo
syn region  idylComment       start='/\*' end='\*/' contains=idylTodo
syn keyword idylTodo          TODO FIXME XXX NOTE HACK contained

" ── Strings ─────────────────────────────────────────────────────
syn region  idylString        start='"' skip='\\"' end='"'
                              \ contains=idylEscape
syn match   idylEscape        '\\.'             contained

" ── Numbers ─────────────────────────────────────────────────────
" Time literals must come first (more specific)
syn match   idylTimeLiteral   '\<\d\+\(\.\d\+\)\?\(ms\|hz\|s\|b\|bpm\)\>'
syn match   idylFloat         '\<\d\+\.\d\+\>'
syn match   idylInteger       '\<\d\+\>'

" ── Trigger & Rest literals ─────────────────────────────────────
syn match   idylTrigger       '\(^\|[=\s,\[;(]\)\zs!\ze\([\s,\];})\n]\|$\)'
syn match   idylRest          '\(^\|[=\s,\[;(]\)\zs_\ze\([\s,\];})\n]\|$\)'

" ── Operators ───────────────────────────────────────────────────
syn match   idylPipeArrow     '|>'
syn match   idylEmitAccess    '::'
syn match   idylRange         '\.\.'
syn match   idylRestart       '||'
syn match   idylOperator      '[+\-*/%=<>!&|^~@?]'
syn match   idylOperator      '=='
syn match   idylOperator      '!='
syn match   idylOperator      '<='
syn match   idylOperator      '>='
syn match   idylOperator      '<<'
syn match   idylOperator      '>>'

" ── Keywords ────────────────────────────────────────────────────
syn keyword idylKeyword       process init catch emit end
syn keyword idylKeyword       flow lib module dt dur as parent

" ── Constants ───────────────────────────────────────────────────
syn keyword idylConstant      pi tau

" ── Built-in functions ──────────────────────────────────────────
syn keyword idylBuiltin       sin cos tan log exp pow sqrt abs
syn keyword idylBuiltin       floor ceil fmod min max
syn keyword idylBuiltin       int float rint bit rnd
syn keyword idylBuiltin       len print now
syn keyword idylBuiltin       clock tempo bpm
syn keyword idylBuiltin       osc_out osc_send osc_close osc_in

" ── Function definitions & calls ────────────────────────────────
syn match   idylFuncDef       '\<[a-zA-Z_][a-zA-Z0-9_]*\>\ze\s*('
                              \ contains=idylKeyword,idylBuiltin

" ── Braces / brackets ──────────────────────────────────────────
syn match   idylBrace         '[{}()\[\]]'

" ── Highlight links ─────────────────────────────────────────────
hi def link idylComment       Comment
hi def link idylTodo          Todo
hi def link idylString        String
hi def link idylEscape        SpecialChar
hi def link idylTimeLiteral   Special
hi def link idylFloat         Float
hi def link idylInteger       Number
hi def link idylTrigger       WarningMsg
hi def link idylRest          SpecialChar
hi def link idylPipeArrow     Operator
hi def link idylEmitAccess    Operator
hi def link idylRange         Operator
hi def link idylRestart       Operator
hi def link idylOperator      Operator
hi def link idylKeyword       Keyword
hi def link idylConstant      Constant
hi def link idylBuiltin       Function
hi def link idylFuncDef       Identifier
hi def link idylBrace         Delimiter

let b:current_syntax = 'idyl'
let &cpo = s:cpo_save
unlet s:cpo_save
