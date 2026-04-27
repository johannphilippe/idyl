" Vim syntax file
" Language:    Idyl
" Maintainer:  Idyl contributors
" Filenames:   *.idyl

if exists('b:current_syntax')
  finish
endif

let s:cpo_save = &cpo
set cpo&vim

" ── Comments ────────────────────────────────────────────────────────────────
syn match   idylComment       '//.*$'            contains=idylTodo
syn region  idylComment       start='/\*' end='\*/' contains=idylTodo
syn keyword idylTodo          TODO FIXME XXX NOTE HACK contained

" ── Strings ─────────────────────────────────────────────────────────────────
syn region  idylString        start='"' skip='\\"' end='"' contains=idylEscape
syn match   idylEscape        '\\.' contained

" ── Time literals (must come before plain numbers) ──────────────────────────
" Matches: 100ms  1s  440hz  120bpm  2b  0.5b
syn match   idylTimeLiteral   '\<\d\+\(\.\d\+\)\?\(bpm\|ms\|hz\|s\|b\)\>'

" ── Plain numbers ────────────────────────────────────────────────────────────
syn match   idylFloat         '\<\d\+\.\d\+\>'
syn match   idylInteger       '\<\d\+\>'

" ── Trigger & Rest literals ──────────────────────────────────────────────────
" Standalone ! or _ (not part of an identifier or trigger-param suffix)
syn match   idylTrigger       '\(^\|[=\s,\[;(]\)\zs!\ze\([\s,\];})\n]\|$\)'
syn match   idylRest          '\(^\|[=\s,\[;(]\)\zs_\ze\([\s,\];})\n]\|$\)'

" ── Trigger parameter suffix: name! ─────────────────────────────────────────
" Highlights the ! in a function parameter like  spike!  or  trig!
syn match   idylTriggerParam  '\<[a-zA-Z_][a-zA-Z0-9_]*\zs!'

" ── Delay / sample operator: '( ─────────────────────────────────────────────
syn match   idylDelayOp       "'\ze\s*("

" ── Deferred block operator: @( ─────────────────────────────────────────────
syn match   idylDeferOp       '@\ze\s*('

" ── Namespace / emit accessor: :: ───────────────────────────────────────────
syn match   idylNamespace     '::'

" ── Range operator: .. ──────────────────────────────────────────────────────
syn match   idylRange         '\.\.'

" ── Lambda operator: |> ─────────────────────────────────────────────────────
syn match   idylPipeArrow     '|>'

" ── Compound operators (must precede single-char matches) ───────────────────
syn match   idylOperator      '==\|!=\|<=\|>='

" ── Single-char operators ───────────────────────────────────────────────────
syn match   idylOperator      '[+\-*/%<>&|^~?]'
syn match   idylAssign        '=\ze[^>=]'

" ── Control keywords ────────────────────────────────────────────────────────
syn keyword idylControl       process catch emit init start stop on

" ── Declaration keywords ────────────────────────────────────────────────────
syn keyword idylDecl          flow import module

" ── Named parameter keywords (dt=, dur=, parent=) ───────────────────────────
syn match   idylParam         '\<\(dt\|dur\|parent\)\ze\s*='

" ── Language constants ───────────────────────────────────────────────────────
syn keyword idylConstant      pi tau
" ── Temporal built-in variables (available inside lambda blocks) ─────────────
syn keyword idylTemporal      age

" ── Built-in functions ───────────────────────────────────────────────────────
" Math
syn keyword idylBuiltin       sin cos tan log exp pow sqrt abs
syn keyword idylBuiltin       floor ceil round rint fmod min max
" Conversion
syn keyword idylBuiltin       int float string trigger
syn keyword idylBuiltin       as_bpm as_ms as_hz as_s
syn keyword idylBuiltin       mtof ftom
" Clock / tempo
syn keyword idylBuiltin       clock tempo bpm
" Random
syn keyword idylBuiltin       rnd seed
" Utility
syn keyword idylBuiltin       len bit print printf now
" Stdlib temporal
syn keyword idylBuiltin       metro sine lfo
" OSC module
syn keyword idylBuiltin       osc_out osc_in osc_send osc_recv osc_close
" Csound module
syn keyword idylBuiltin       cs_open cs_note cs_chnset cs_chnget

" ── Process block name ───────────────────────────────────────────────────────
" Highlights the name after the 'process' keyword
syn match   idylProcessName   '\<process\>\s\+\zs\<[a-zA-Z_][a-zA-Z0-9_]*\>'

" ── Flow definition name ─────────────────────────────────────────────────────
syn match   idylFlowName      '\<flow\>\s\+\zs\<[a-zA-Z_][a-zA-Z0-9_]*\>'

" ── Function definitions & calls ─────────────────────────────────────────────
" Any identifier immediately followed by ( is a function call/definition
syn match   idylFuncCall      '\<[a-zA-Z_][a-zA-Z0-9_]*\>\ze\s*('
                              \ contains=idylControl,idylDecl,idylBuiltin

" ── Member access: .name ─────────────────────────────────────────────────────
syn match   idylMember        '\.[a-zA-Z_][a-zA-Z0-9_]*'

" ── Braces / brackets / delimiters ──────────────────────────────────────────
syn match   idylBrace         '[{}()\[\]]'

" ── Highlight links ──────────────────────────────────────────────────────────
hi def link idylComment       Comment
hi def link idylTodo          Todo
hi def link idylString        String
hi def link idylEscape        SpecialChar
hi def link idylTimeLiteral   Special
hi def link idylFloat         Float
hi def link idylInteger       Number
hi def link idylTrigger       WarningMsg
hi def link idylTriggerParam  WarningMsg
hi def link idylRest          Comment
hi def link idylDelayOp       Operator
hi def link idylDeferOp       Operator
hi def link idylPipeArrow     Operator
hi def link idylNamespace     Operator
hi def link idylRange         Operator
hi def link idylOperator      Operator
hi def link idylAssign        Operator
hi def link idylControl       Keyword
hi def link idylDecl          Keyword
hi def link idylParam         Type
hi def link idylConstant      Constant
hi def link idylTemporal      Special
hi def link idylBuiltin       Function
hi def link idylProcessName   Title
hi def link idylFlowName      Type
hi def link idylFuncCall      Identifier
hi def link idylMember        Identifier
hi def link idylBrace         Delimiter

let b:current_syntax = 'idyl'
let &cpo = s:cpo_save
unlet s:cpo_save
