-- noun_table.sp

{ "std.sp" >>> sexpr bpdli !

entries
   ("code_array" nil
    "operand"    nil
    "operator"   nil) 
    lsby2 !
    <

--    <- labels -> <
--
--    <- values -> <

--    lsby2 !
--    mattrans !
--    dup
--        --
--        0 ith
--        <- labels -> <
--        --
--        1 ith
--        <- values -> <

    entries
    0 matcol !
    {<- "/pyramid/tag/" -> .+} over !

    entries
    1 matcol !

    2 take
    mattrans !

    ls2map ! }



