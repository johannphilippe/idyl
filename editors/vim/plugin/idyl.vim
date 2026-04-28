" plugin/idyl.vim — Idyl live-coding integration
"
" Loaded unconditionally by Vim (no 'filetype plugin on' required).
" Uses a FileType autocmd to apply buffer-local mappings for *.idyl files.
"
" Configuration (set in vimrc before this loads):
"   let g:idyl_osc_host = '127.0.0.1'   " default
"   let g:idyl_osc_port = 9000           " default

if exists('g:loaded_idyl_plugin') | finish | endif
let g:loaded_idyl_plugin = 1

if !exists('g:idyl_osc_host') | let g:idyl_osc_host = '127.0.0.1' | endif
if !exists('g:idyl_osc_port') | let g:idyl_osc_port = 9000         | endif

let s:plugin_dir   = fnamemodify(resolve(expand('<sfile>:p')), ':h')
let s:runtime_root = fnamemodify(s:plugin_dir, ':h')
let s:py_sender    = s:runtime_root . '/python/idyl_send.py'

highlight default IdylEval ctermbg=58 guibg=#5f5f00

" ── OSC send ─────────────────────────────────────────────────────────────────
function! s:SendOsc(payload, address) abort
  if !executable('python3')
    echohl WarningMsg | echom '[idyl] python3 not found' | echohl None
    return
  endif
  let l:cmd = 'python3 ' . shellescape(s:py_sender)
            \ . ' ' . shellescape(g:idyl_osc_host)
            \ . ' ' . shellescape(string(g:idyl_osc_port))
            \ . ' ' . shellescape(a:address)
  if get(g:, 'idyl_debug', 0)
    echom '[idyl] cmd: ' . l:cmd
    echom '[idyl] payload: ' . a:payload
  endif
  call system(l:cmd, a:payload)
  if get(g:, 'idyl_debug', 0)
    echom '[idyl] shell_error=' . v:shell_error
  endif
  if v:shell_error
    echohl WarningMsg
    echom '[idyl] OSC send failed (is idyl running with --listen?)'
    echohl None
  endif
endfunction

" ── Block detection ───────────────────────────────────────────────────────────
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

" ── Flash highlight ───────────────────────────────────────────────────────────
function! s:FlashRange(start, end) abort
  let l:pat = '\%>' . (a:start - 1) . 'l\%<' . (a:end + 1) . 'l'
  let l:mid = matchadd('IdylEval', l:pat)
  if exists('*win_execute')
    let l:win = win_getid()
    call timer_start(250, {_ -> win_execute(l:win, 'silent! call matchdelete(' . l:mid . ')')})
  else
    call timer_start(250, {_ -> execute('silent! call matchdelete(' . l:mid . ')')})
  endif
endfunction

" ── Public functions ──────────────────────────────────────────────────────────
function! IdylEvalAtCursor() abort
  let s    = s:BlockStart(line('.'))
  let e    = s:BlockEnd(s)
  let code = join(getline(s, e), "\n")
  echom '[idyl] eval ' . s . ':' . e . '  ' . trim(getline(s))
  call s:FlashRange(s, e)
  call s:SendOsc(code, '/idyl/eval')
endfunction

function! IdylProcessNameAtCursor() abort
  let l:lnum  = line('.')
  let l:start = s:BlockStart(l:lnum)
  let l:top   = getline(l:start)
  if get(g:, 'idyl_debug', 0)
    echom '[idyl] BlockStart(' . l:lnum . ')=' . l:start . ' line="' . l:top . '"'
  endif
  let l:m = matchlist(l:top, '^process\s\+\(\w\+\)')
  if !empty(l:m)
    return l:m[1]
  endif
  call inputsave()
  let l:name = input('[idyl] process name: ')
  call inputrestore()
  return l:name
endfunction

function! s:CleanName(n) abort
  return substitute(a:n, '[^A-Za-z0-9_]', '', 'g')
endfunction

function! IdylStartProcess() abort
  let l:name = s:CleanName(IdylProcessNameAtCursor())
  if empty(l:name)
    echom '[idyl] no process name found — is cursor inside a process block?'
    return
  endif
  echom '[idyl] start "' . l:name . '"'
  call s:SendOsc(l:name, '/idyl/process/start')
endfunction

function! IdylStopProcess() abort
  let l:name = s:CleanName(IdylProcessNameAtCursor())
  if empty(l:name)
    echom '[idyl] no process name found — is cursor inside a process block?'
    return
  endif
  echom '[idyl] stop "' . l:name . '"'
  call s:SendOsc(l:name, '/idyl/process/stop')
endfunction

function! IdylDiag(name) abort
  let l:name = s:CleanName(a:name)
  echom '[idyl-diag] sending start "' . l:name . '"'
  call s:SendOsc(l:name, '/idyl/process/start')
  echom '[idyl-diag] shell_error=' . v:shell_error
endfunction
command! -nargs=1 IdylDiag call IdylDiag(<q-args>)

" ── Buffer-local mappings ─────────────────────────────────────────────────────
function! s:ApplyMappings() abort
  nnoremap <buffer> <nowait> <silent> s :<C-u>call IdylStartProcess()<CR>
  nnoremap <buffer> <nowait> <silent> q :<C-u>call IdylStopProcess()<CR>
  nnoremap <buffer> <nowait> <silent> t :<C-u>call IdylEvalAtCursor()<CR>
  silent! inoremap <buffer> <nowait> <silent> <C-e> <C-o>:call IdylEvalAtCursor()<CR>
endfunction

augroup idyl_plugin
  autocmd!
  autocmd FileType idyl call s:ApplyMappings()
augroup END
