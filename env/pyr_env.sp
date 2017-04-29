-- pyr_env.sp

{ "std.sp" >>> sexpr bpdli !

    st2map { collect ! mkmap ! } <
    env_dir "c:/pyramid/env/" <

    -- stuff to put in the map
    "/pyramid/tag/user_code_table"
        env_dir "user_code_table.sp" .+ include ! !

    "/pyramid/tag/verb_table"
        env_dir "verb_table.sp" .+ include ! !

    "/pyramid/tag/noun_table"
        env_dir "noun_table.sp" .+ include ! !

    "/pyramid/tag/cpu_env"
        env_dir "cpu_env.sp" .+ include ! !

    st2map !
    "/pyramid/tag/pyr_env" chtag !
    repl ! }

