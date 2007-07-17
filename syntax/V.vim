" Vim syntax file
" Language:	V
" Maintaner:	Rahul
" Version:	0.1
"
if version < 600
    syntax clear
endif
syntax case match

"setlocal iskeyword+=-

syntax match vTodo /TODO/ contained
syn match vComment /#.*$/ contains=vTodo
"syn match vWord /[a-zA-Z_:&!@$%^-]\+/
"syn match vMWord /\$[a-zA-Z_:&!@$%^-]\+/
syn match vOpen /\[/
syn match vClose /\]/


syntax match vNumber /\<-*[0-9]*\.*[0-9]\+\>/
syntax region vQComment start="(" end=")"
syntax match vChar /\<\~[a-zA-Z0-9]\>/
syntax match vString1 /"[^"]*"/
syntax match vString2 /'[^']*'/
syntax match vTrue /\<true\>/
syntax match vFalse /\<false\>/

if !exists("g:sh_fold_enabled")
    let g:sh_fold_enabled= 0
endif

hi link vComment Comment
hi link vQComment Comment
"hi link vWord Identifier
hi link vString1 String
hi link vString2 String
hi link vOpen Statement
hi link vClose Statement
hi link vNumber Number
hi link vTrue Boolean
hi link vFalse Boolean

let b:current_syntax = "V"

" vim: ts=8 ft=vim
