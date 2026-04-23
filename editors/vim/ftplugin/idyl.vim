" ftplugin/idyl.vim — function definitions for idyl live-coding
"
" Mappings are in after/ftplugin/idyl.vim so they survive any plugin
" FileType autocmds that run after this file.
"
" Configuration (set in vimrc / init.vim):
"   let g:idyl_osc_host = '127.0.0.1'   " default
"   let g:idyl_osc_port = 9000           " default

if !exists('g:idyl_osc_host') | let g:idyl_osc_host = '127.0.0.1' | endif
if !exists('g:idyl_osc_port') | let g:idyl_osc_port = 9000         | endif

let s:this_file    = resolve(expand('<sfile>:p'))
let s:runtime_root = fnamemodify(s:this_file, ':h:h')
let s:py_sender    = s:runtime_root . '/python/idyl_send.py'

highlight default IdylEval ctermbg=58 guibg=#5f5f00

function! s:SendOsc(payload, address) abort
  if !executable('python3')
    echohl WarningMsg | echom '[idyl] python3 not found' | echohl None
    return
  endif
  let l:cmd = 'python3 ' . shellescape(s:py_sender)
            \ . ' ' . shellescape(g:idyl_osc_host)
            \ . ' ' . shellescape(string(g:idyl_osc_port))
            \ . ' ' . shellescape(a:address)
  call system(l:cmd, a:payload)
  if v:shell_error
    echohl WarningMsg
    echom '[idyl] OSC send failed (is idyl running with --listen?)'
    echohl None
  endif
endfunction

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

function! s:BlockEnd(start) abort
  let depth = 0
  let l     = a:start
  let last  = line('$')
  while l <= last
    let ln     = getline(l)
    let opens  = len(substitute(ln, '[^{]', '', 'g'))
    let closes = len(substitute(ln, '[^}]', '', 'g'))
    let depth += opens - closes
    if depth <= 0 && l > a:start && (opens > 0 || closes > 0)
      return l
    endif
    if l == a:start && depth == 0 && l < last
      let nx = getline(l + 1)
      if nx =~# '^|>' || nx =~# '^\s*{'
        let l += 1 | continue
      endif
      return l
    endif
    let l += 1
  endwhile
  return last
endfunction

function! s:FlashRange(start, end) abort
  let l:win = win_getid()
  let l:pat = '\%>' . (a:start - 1) . 'l\%<' . (a:end + 1) . 'l'
  let l:mid = matchadd('IdylEval', l:pat)
  call timer_start(250, {_ -> win_execute(l:win, 'silent! call matchdelete(' . l:mid . ')')})
endfunction

function! IdylEvalAtCursor() abort
  let s    = s:BlockStart(line('.'))
  let e    = s:BlockEnd(s)
  let code = join(getline(s, e), "\n")
  echo '[idyl] eval ' . s . ':' . e . '  ' . trim(getline(s))
  call s:FlashRange(s, e)
  call s:SendOsc(code, '/idyl/eval')
endfunction

function! IdylProcessNameAtCursor() abort
  let l:top = getline(s:BlockStart(line('.')))
  let l:m   = matchlist(l:top, '^process\s\+\(\w\+\)')
  if !empty(l:m)
    return l:m[1]
  endif
  return input('[idyl] process name: ')
endfunction

function! IdylStartProcess() abort
  let l:name = IdylProcessNameAtCursor()
  if empty(l:name) | return | endif
  echo '[idyl] start ' . l:name
  call s:SendOsc(l:name, '/idyl/process/start')
endfunction

function! IdylStopProcess() abort
  let l:name = IdylProcessNameAtCursor()
  if empty(l:name) | return | endif
  echo '[idyl] stop ' . l:name
  call s:SendOsc(l:name, '/idyl/process/stop')
endfunction
