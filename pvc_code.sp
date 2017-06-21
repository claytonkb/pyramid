-- [append [list 1 2 3] [append [list 4 5 6] [list 7 8 9]]]

{ 'std.sp' >>> sexpr bpdli !

        [ptr 1 2 3] "/pyramid/tag/list" newtag

        [ptr [ptr 4 5] [ptr 5 6] ] "/pyramid/tag/cat" newtag
            "/pyramid/tag/list" newtag
    2 takeptr ! "/pyramid/tag/append" newtag

    [ptr 7 8 9] "/pyramid/tag/list" newtag

    2 takeptr ! "/pyramid/tag/append" newtag

    "pvc_code.bbl"
    savefs ! }

