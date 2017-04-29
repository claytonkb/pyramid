{ "std.sp" >>> sexpr bpdli !

entries
   ("code_array" nil
    "operand"    nil
    "operator"   nil) <

    entries
    lsby2 !
    mattrans !
    dup
        --
        0 ith
        <- labels -> <
        --
        1 ith
        <- values -> <

    labels {<- "/pyramid/tag/" -> .+} over !
    values

    2 take
    mattrans !

    ls2map !
    
    repl ! }

