" ftplugin/idyl.vim — Live-coding integration for idyl
"
" Auto-sourced by Vim/Neovim for every *.idyl buffer (via ftdetect).
" No FileType autocmd needed here.
"
" Sends the top-level construct enclosing the cursor to a running idyl
" instance via an OSC /idyl/eval UDP message.
"
" Key mappings (idyl files only):
"   t          (normal mode)  — evaluate construct at cursor
"   <C-e>      (insert mode)  — evaluate without leaving insert mode
"
" Configuration (set in vimrc / init.vim before any idyl file is opened):
"   let g:idyl_osc_host = '127.0.0.1'   " default
"   let g:idyl_osc_port = 9000           " default

" ── User-visible configuration ───────────────────────────────────────────────
if !exists('g:idyl_osc_host') | let g:idyl_osc_host = '127.0.0.1' | endif
if !exists('g:idyl_osc_port') | let g:idyl_osc_port = 9000         | endif

" ── Path to Python OSC sender ────────────────────────────────────────────────
" <sfile> is captured at script level (before any function call) so that
" it always refers to this file's path, even inside a function.
"
" Directory layout (both manual install and plugin manager):
"   <runtime_root>/ftplugin/idyl.vim   ← this file
"   <runtime_root>/python/idyl_send.py ← the sender
"
" fnamemodify(..., ':h')  → ftplugin dir
" fnamemodify(..., ':h:h') → runtime root
let s:this_file   = resolve(expand('<sfile>:p'))
let s:runtime_root = fnamemodify(s:this_file, ':h:h')
let s:py_sender    = s:runtime_root . '/python/idyl_send.py'

" ── Script-level functions (defined once, guard with exists()) ────────────────
" ftplugin files are re-sourced for every new idyl buffer; guard all
" function definitions so we don't redefine them on every buffer open.

if !exists('*s:SendOsc')

" ── Highlight group ───────────────────────────────────────────────────────────
highlight default IdylEval ctermbg=58 guibg=#5f5f00

function! s:SendOsc(code) abort
  if !executable('python3')
    echohl WarningMsg
    echom '[idyl] python3 not found — install Python 3 to use live-coding'
    echohl None
    return
  endif

  let l:cmd = 'python3 ' . shellescape(s:py_sender)
            \ . ' ' . shellescape(g:idyl_osc_host)
            \ . ' ' . shellescape(string(g:idyl_osc_port))
  call system(l:cmd, a:code)

  if v:shell_error
    echohl WarningMsg
    echom '[idyl] OSC send failed (is idyl running with --listen?)'
    echohl None
  endif
endfunction

" ── Block-start detection ─────────────────────────────────────────────────────
" Walk backward from lnum to find the first top-level construct start line.
" A top-level start is any line that:
"   • starts at column 0 (^\S)
"   • is not a lone closing brace  (^}\s*$)
"   • is not a |> continuation     (^|>)
function! s:BlockStart(lnum) abort
  let l = a:lnum
  while l >= 1
    let ln = getline(l)
    if ln =~# '^\S' && ln !~# '^}\s*$' && ln !~# '^|>'
      return l
    endif
    let l -= 1
  endwhile
  return 1
endfunction

" ── Block-end detection ───────────────────────────────────────────────────────
" Walk forward from start counting brace depth.
function! s:BlockEnd(start) abort
  let depth = 0
  let l     = a:start
  let last  = line('$')

  while l <= last
    let ln     = getline(l)
    let opens  = len(substitute(ln, '[^{]', '', 'g'))
    let closes = len(substitute(ln, '[^}]', '', 'g'))
    let depth += opens - closes

    " Brace-delimited block: depth returns to 0 after going positive
    if depth <= 0 && l > a:start && (opens > 0 || closes > 0)
      return l
    endif

    " Single-line construct: check whether next line is a |> or opening {
    if l == a:start && depth == 0 && l < last
      let nx = getline(l + 1)
      if nx =~# '^|>' || nx =~# '^\s*{'
        let l += 1
        continue
      endif
      return l
    endif

    let l += 1
  endwhile

  return last
endfunction

" ── Highlight evaluated range ─────────────────────────────────────────────────
" Flash the evaluated block using matchadd(), then clear after 250 ms.
function! s:FlashRange(start, end) abort
  let l:win = win_getid()
  let l:pat = '\%>' . (a:start - 1) . 'l\%<' . (a:end + 1) . 'l'
  " matchadd() adds to the current window (we are always called from it).
  let l:mid = matchadd('IdylEval', l:pat)
  " win_execute runs matchdelete in the right window even if focus moved away.
  call timer_start(250, {_ -> win_execute(l:win, 'silent! call matchdelete(' . l:mid . ')')})
endfunction

" ── Evaluate the construct at the cursor ─────────────────────────────────────
function! s:EvalAtCursor() abort
  let s    = s:BlockStart(line('.'))
  let e    = s:BlockEnd(s)
  let code = join(getline(s, e), "\n")

  echo '[idyl] ' . s . ':' . e . '  ' . trim(getline(s))

  call s:FlashRange(s, e)
  call s:SendOsc(code)
endfunction

endif  " !exists('*s:SendOsc')

" ── Buffer-local key mappings ─────────────────────────────────────────────────
" These are set per-buffer every time an idyl file is opened (correct).

" t in normal mode: evaluate construct at cursor.
" (Shadows Vim's built-in `t` motion; use `;`/`,` to repeat f/t searches.)
nnoremap <buffer> <silent> t :<C-u>call <SID>EvalAtCursor()<CR>

" Ctrl+e in insert mode: evaluate without leaving insert mode.
" (Shadows Vim's built-in <C-e> which inserts the char from the line below.)
inoremap <buffer> <silent> <C-e> <Cmd>call <SID>EvalAtCursor()<CR>

" Allow :e or plugin managers to clean up buffer-local settings
let b:undo_ftplugin = get(b:, 'undo_ftplugin', '')
if !empty(b:undo_ftplugin) | let b:undo_ftplugin .= ' | ' | endif
let b:undo_ftplugin .= 'nunmap <buffer> t | iunmap <buffer> <C-e>'
