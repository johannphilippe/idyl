" after/ftplugin/idyl.vim — buffer-local mappings for idyl
"
" Lives in after/ftplugin/ so it runs after all plugin FileType handlers.
" Mappings are (re-)applied via BufEnter autocmd so that plugins loaded
" after this file (e.g. vim-sneak, vim-sandwich) cannot override them.
" This also fixes the classic-Vim quirk where buffer-local q mappings
" can fail to register on the first FileType event.

function! s:ApplyIdylMappings()
  " s/q first — critical for start/stop; no abort so <C-e> failure can't block them
  nnoremap <buffer> <nowait> <silent> s :<C-u>call IdylStartProcess()<CR>
  nnoremap <buffer> <nowait> <silent> q :<C-u>call IdylStopProcess()<CR>

  " t — evaluate construct at cursor (shadows till-char motion)
  nnoremap <buffer> <nowait> <silent> t :<C-u>call IdylEvalAtCursor()<CR>

  " <C-e> — evaluate from insert mode without leaving it
  silent! inoremap <buffer> <nowait> <silent> <C-e> <C-o>:call IdylEvalAtCursor()<CR>
endfunction

call s:ApplyIdylMappings()

augroup IdylMappings
  autocmd! BufEnter <buffer> call s:ApplyIdylMappings()
augroup END

let b:undo_ftplugin = get(b:, 'undo_ftplugin', '')
if !empty(b:undo_ftplugin) | let b:undo_ftplugin .= ' | ' | endif
let b:undo_ftplugin .= 'nunmap <buffer> t | iunmap <buffer> <C-e> | nunmap <buffer> s | nunmap <buffer> q | autocmd! IdylMappings'
