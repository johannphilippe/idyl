" after/ftplugin/idyl.vim — buffer-local mappings for idyl
"
" Lives in after/ftplugin/ so it runs after all plugin FileType handlers,
" preventing any plugin from overriding these mappings.

" t — evaluate construct at cursor (shadows till-char motion)
nnoremap <buffer> <nowait> <silent> t :<C-u>call IdylEvalAtCursor()<CR>

" <C-e> — evaluate from insert mode without leaving it
inoremap <buffer> <nowait> <silent> <C-e> <C-o>:call IdylEvalAtCursor()<CR>

" s — start the process block the cursor is in (shadows substitute-char)
nnoremap <buffer> <nowait> <silent> s :<C-u>call IdylStartProcess()<CR>

" q — stop the process block the cursor is in (shadows macro-record)
nnoremap <buffer> <nowait> <silent> q :<C-u>call IdylStopProcess()<CR>

let b:undo_ftplugin = get(b:, 'undo_ftplugin', '')
if !empty(b:undo_ftplugin) | let b:undo_ftplugin .= ' | ' | endif
let b:undo_ftplugin .= 'nunmap <buffer> t | iunmap <buffer> <C-e> | nunmap <buffer> s | nunmap <buffer> q'
