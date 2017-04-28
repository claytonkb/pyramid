{ "std.sp" >>> sexpr bpdli !
   [tag "/pyramid/tag/code_array"
    ptr
        [tag "/pyramid/tag/operand" val 2]
        [tag "/pyramid/tag/operand" val 3]
        [tag "/pyramid/tag/operator" 
            hash "/pyramid/tag/+"]]
repl !
dup --
    unload "code_block.bbl" spitm 
    --
    bs2gv "test.dot" <<< }
