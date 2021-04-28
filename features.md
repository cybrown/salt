Parser:
    expression
        identifiers
        literal values
            double
            strings
            booleans
            null
            arrays
            dicts
        operators
            binary operator
                binary operator with precedence
                binary operator with associativity
            unary prefix operators
            index []
            function call
            assignement
            shortcut circuit evaluation for && and ||
        parenthesis
    statements
        expression
        if
        for
        while
        break
        continue
        switch / case
    function definition
    block of statements
    parse errors

Runtime:
    variables
        load variable
        set variable
        global variables
        function scoped variables
        arguments
        block scoped variables
        variable declaration
    hash tables of current variables
    hash table of functions
    var container
    stack of vars
    operations
        booleans
            and
            or
            xor
            not
        doubles
            add
            sub
            mul
            div
            mod
            pow
            neg
            b_and
            b_or
            b_xor
            inv
        strings
            concat
            substring
            char at index
        arrays
            create new
            append
            get at index
            set at index
            range
        dicts
            create new
            get at key
            set at key
            exists at key
        functions
            define function
            call
        intrisics
