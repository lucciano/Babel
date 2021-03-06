<head>
    <title>Babel Programming Language</title>
</head>

![](sacred_scarab.jpg)

[**Contact Us**](babel_contact.html)  
<a href="https://github.com/claytonkb/Babel" target="_blank">Babel on github</a>  
<a href="http://rosettacode.org/wiki/Category:Babel" target="_blank">Babel at RosettaCode</a>   
<a href="https://thestrangeloop.com/sessions/babel-an-untyped-stack-based-hll" target="_blank">Babel at Strange Loop 2013</a>  

[Babel Internals](babel_guts.html)  
[Babel Operator Reference](babel_op.html)

Contents
========

- <a href="#getting_started">Getting Started</a>
- <a href="#overview">Overview</a>
- <a href="#sparse">Sparse</a>
- <a href="#bipedal">Bipedal</a>

<a name="getting_started"></a>
Getting Started
===============

To get started using Babel, you will need the following prerequisites:

- Windows machine
- Cygwin
- ActivePerl
- Babel repo cloned from <a href="https://github.com/claytonkb/Babel" target="_blank">github</a>

To make Babel, run the following command from the Cygwin prompt:

> `perl make.pl`

To run Babel code in a file named foo.sp, run the following command from the 
Cygwin prompt:

> `perl babel foo.sp`

Babel is written in C specifically to facilitate cross-platform support - it
is in the works. If you want to compile Babel for GCC, simply #undef WINDOWS
and then put together a suitable makefile.

To begin writing your own Babel program, see the section below on <a href="#sparse">Sparse</a>.

<a name="overview"></a>
Overview
========

Babel is an interpreted language.  It is an untyped, stack-based, postfix 
language with support for arrays, lists and hashes (dictionaries). Babel 
1.0 will support built-in crypto-based verification of code in order to 
enable safer remote code execution.

Babel was designed with the TIMTOWTDI  philosophy in mind - there are many 
right ways to solve any particular problem in Babel. However, some 
solutions are a bad fit for Babel. In particular, trying to write Babel 
code to resemble standard function-call code - while possible - will 
result in sub-optimal, hard-to-read Babel code.

Babel is a stack-based language - the stack is the primary means of 
communicating data from one place to another. The ideal situation is to 
reduce your code to a set of sections that can be thought of as 
combinators where each combinator takes a small number or zero operands 
from the stack and returns one or zero results on the stack. In this 
case, you will not have �stack-noise� induced by the need to re-arrange 
arguments on the stack.

The next preferred situation is if your sections take multiple values on 
the stack and each value is used only once or twice in the section, in 
order or nearly so. In this case, you can simply use the up and down 
operators to move cleanly from operand to operand.

But if one or more operands are going to be used in more than a couple 
places in a section, this will create significant stack-noise which is 
not only a performance problem but drastically reduces the readability 
of your Babel code.

In this case, consider using symbolic-variables or lexical variables, 
depending on your needs. Symbolic variables are entries in the symbol 
table and are primarily intended for structural or spatial organization. 
Lexical variables (symbolic variables can also be treated lexically) 
permit you to make a section re-entrant and are primarily intended for
functional or temporal organization.

Many array- and list-processing tasks are most easily done by monopolizing 
the entire stack with an each loop. To facilitate this, Babel provides the 
`nest` operator. Within a nested context, you have a fresh stack � when you 
return from the nested context, that temporary stack will be destroyed, 
except for TOS which is returned. You can read doc/babel_ref.txt for more 
information on the nest operator.

There are two kinds of Babel syntax. The first kind is being used to 
bootstrap the Babel construct, it is called Sparse (S-expression parser).
The second kind is the built-in Babel parser, which is called Bipedal
(Babel Program Description Language).

<a name="sparse"></a>
S-expression parser (sparse.pl)
-------------------------------

The s-expression parser (sparse.pl) is designed as an ultra-lightweight 
front-end for generating native Babel code. It uses S-expressions to encode
the various Babel data structures. To compile a set of .sp files into a
single .bbl file, pass the filenames as arguments to sparse.pl. Evaluation
of the compiled code begins at root:

(root *your Babel code here*)  

A .sp file is a list-of-lists (each list defining a "section"), so you must
place the root list inside another in order for it to be parseable:

((root *code*))  

Perhaps the simplest program you might compile:

((root (val 10)))  

To see a listing of the binary file that was created, open filename.sp.lst.
For example, the above code would result in the following lines in the
.lst file:

> `0000 00000004`  
> `0004 0000000a`   

The first line is at offset 0 and has a value of 4, meaning a leaf-array of
4 bytes in length. The second line is at offset 4 and has the value of 0xa,
which is the hexadecimal equivalent of the value 10 in the example .sp 
program.

The kinds of lists which sparse recognizes are:

        (val  ...)  Creates a leaf-array (values)  
        (ptr  ...)  Creates an interior-array (pointers)  
        (tag  ...)  Creates a tagged-pointer  
        (list ...)  Creates a Lisp-style list  
        (hash ...)  Returns the Pearson16 hash of a string  
        (oper ...)  Inserts a constant value into a code-list as an operator  
        (ref  ...)  Creates a reference  
        (code ...)  Creates a code-list  
       
Examples. Paste these into some filename.sp file and use sparse.pl to compile
and see the results in the .lst file. Note that none of these examples are
runnable as a Babel Virtual Machine, that is, you cannot pass the generated
.bbl file to Babel.

        ((root (val  1 2 3)))  
        ((root (ptr  1 2 3)))  
        ((root (ptr root)))  
        ((root (list 1 2 3)))  
        ((root (tag 'foo' nil)))  
        ((root (hash 'foo')))  
        ((root (oper 0x134)))  
        ((root (ref 'foo')))  
   
In order to generate meaningful code-lists, you will need to pass 
src/opcodes.sp to sparse.pl in addition to your .sp filename.

        ((root (code fnord fnord)))  

In order to create a loadable BVM, you will need to pass both src/opcodes.sp
and src/construct.sp to sparse.pl in addition to your .sp filename. You must
*not* define 'root' in your .sp file (since it is already defined in 
construct.sp) and you must define a 'main' section and a 'symbol' section.

For example, if foo.sp is the following:

        (   (main (code "Hello, world\n" stdout8))  
        (symbol (list nil)))  
  
... then you can compile a loadable BVM out from it with the following command:

        perl sparse.pl foo.sp src/construct.sp src/opcodes.sp   

Now, foo.sp.bbl can be executed by the Babel interpreter directly:

        bin/babel.exe foo.sp.bbl

<a name="bipedal"></a>
Babel Program Description Language (Bipedal)
--------------------------------------------

Support for Bipedal does not yet exist in Babel, but will be available in 
Babel 1.0. The primary design goal of Bipedal has been to make the syntax 
reflect the underlying data structure that is being described as closely as 
possible. 

*Lexicals*

Bipedal is UTF-8 encoded - this means you can give your sections any name 
that can be encoded in UTF-8. You can easily alias the built-in operator 
names, as well.

Like any other postfix language, operands come first, then operators:

        2 3 + 4 *

The above expression, when evaluated, will result in 20.

        "Hello, " "world" . <<

When this expression is evaluated, it will place the two strings on the 
stack, concatenate them and then print them to STDOUT.

Babel differentiates between values and pointers. **Values** are stored in 
leaf-arrays and **pointers** are stored in interior-arrays. In Bipedal, there 
are two syntactic elements that describe values: a number or a string.

A **number** can be a decimal integer, decimal floating-point, hexadecimal, 
or binary. Bipedal differentiates between integer and floating-point based 
on the presence of a decimal-point and the default is integer.

        -42e15  
        32.7152e-12  
        0x13  
        0b1101111  

The limits are dependent on MWORD\_SIZE (use the msize operator to get 
this) except for pnums which can be of arbitrary size.

A string is a set of characters wrapped in quotes:

        "Lorem ipsum dolor sit amet"  
        'Lorem ipsum dolor sit amet'  

... or in a quote-block (see below). The quote character can be escaped 
with a backslash in the usual way. Line comments in Bipedal are specified 
by double-dash:

        -- This is a comment  

*Sections*

Bipedal is organized at the top-level as a set of labeled text blocks 
which can be nested via indentation, like a YAML file. A label is a string 
of characters that begins at the left-edge and is followed by a colon or 
equal-sign:

        this_is\ a\ label\!\@          :  
        this_is\ a\ label_too:  
        this_is\ a\ label           =  
        and\=so\=is\=this=  

Any character can be used in a label, including the colon and equal-sign. 
However, any ASCII character that is not an identifier-character must be 
escaped with the backslash. Non-ASCII Unicode characters can be used 
freely.

A label opens an indented text block which is terminated either by the end 
of file or by a dedent. The indent must be at least one space. Tabs or any 
other whitespace cannot be used as an indent character. A line that is 
one space less indented than the left-edge of the current block, or the 
end-of-file, is a dedent and ends the current text block. A labeled text 
block allows the definition of a section.

        my_section : ( 10 9 8 7 6 5 4 3 2 1 "Blast off!" )  

*Leaf-arrays*

A leaf-array is defined by enclosing a string or one or more numbers in 
square-brackets with the 'val' tag:

        [val 1 1 2 3 5 8 13 0x15]

A single number or string can be enclosed in brackets without the val tag:

        [101]
        ["Let me not to the marriage of true minds admit impediments"]

A section consisting of a single number or string value is encoded as a 
leaf-array:

        foo : 1234    
        bar : "baz"    

An empty section defaults to nil:

        foo :
        bar : 123

... is equivalent to:

        foo : nil  
        bar : 123  

You can alias one section to another:

        foo : bar

Anywhere foo is used, it will be as if bar had been used instead. You 
cannot use sigils (see below) on either side of an alias statement and the 
alias inherits any sigil attached to the aliased section. You cannot alias a 
location label. Aliases have global visibility, so you can alias any 
section by using its full name. You can use this feature to alias the 
built-in operators to a name of your preference:

        suma : +  
        foo : { 2 3 suma }  

If a symbol is not defined by a labeled section in your Bipedal file, it 
is treated as if it defines a symbolic section. That is, use of an 
otherwise undefined symbol implicitly creates an entry in the symbol-table 
for that symbol. This is the case regardless of whether the symbol refers 
to a symbolic variable or a reference variable.

*Interior-array*

An interior-array is defined by enclosing one or more leaf-arrays, labels 
or interior arrays in square-brackets with the 'ptr' tag:

        [ptr ["achilles"] ["tortoise"] ] 

*Lists*

Lists are created with parentheses. A list in Babel means a linked-list 
in the Lisp-sense. The following are equivalent:

        (1 2 3)  

        [ptr 1 [ptr 2 [ptr 3 nil]]]  

You can verify this with the following code:

        { (1 2 3) [ptr 1 [ptr 2 [ptr 3 nil]]] == }  

... which will evaluate to 0 (same, see `arcmp`).

The nil element is distinct from that of Lisp in that Babel has no notion of 
an atomic and nil is just implemented as an entry in the symbol table - 
but you can take its car or cdr and you will always get nil again. In 
keeping with the spirit of Lisp, you can also notate nil as 
empty-parentheses or empty-brackets:

        [] == () == nil

One final difference between Bipedal syntax and Lisp syntax is that 
Bipedal does not use the dot-notation. In Lisp, the following syntax

        (1 2 . 3)

... expresses the same list as the following written in Bipedal:

        (1 [ptr 2 3])

In Babel, operands in the code-stream are differentiated from operators 
by being nested in an interior-array. For example, to add two numbers, 
the following are all equivalent:

        A: ( [42] [23] + )  
        B: ( (42) (23) + )  
        C: ( [ptr 42 nil] [ptr 23 nil] + )  
        D: ( [ptr 42 nil] [ptr 23 nil] 0x38 )  

A-D all produce the same result when executed. A and B do not generate 
exactly the same byte-code but B, C and D do.

A code-list is created with curly-braces. A code-list is just a list but 
it performs the nesting of non-operators automatically, thus reducing 
visual clutter. It also alters the meaning of labeled references. The 
following are identically equivalent:

        ( [2] [3] + )  

        { 2 3 + }  

And:

        ( ["Hello, "] ["world\n"] . << )  

        { "Hello, "  "world\n" . << }  

You can check that this is the case:

        { { 1 2 3 "Hello, world" } ( [1] [2] [3] ["Hello, world"] ) == }

*Tagged-pointers*

The tagged-pointer is the third basic sub-type alongside interior-arrays 
and leaf-arrays that together comprise the bstruct type in which all 
Babel data is stored. A tagged-pointer is created using square-brackets with
the 'tag' ptr:

        [tag <stuff>]

The stuff that follows the tag (if any) is dependent on the tag. Certain 
built-ins, such as nil, are implemented internally using tagged-pointers.

*References*

A reference permits indexing into any portion of a bstruct. There are two
syntaxes for creating references. The first is to use square brackets with
the 'ref' tag:

        [ref 'foo']

The above creates a reference to 'foo'. A reference can index into any number
of hash-tables and lists:

        [ref 'foo' 0 15 'bar']

This will create a reference that looks up foo in the local symbol-table, 
finds the 0th element of the list returned, then finds the 15th element of the 
list returned, then indexes into the returned hash-table with 'bar'. 

The refa tag creates a reference which uses array-indexing. The refg tag
creates a reference that begins lookup in the global symbol-table.

The second syntax for creating refernces is a URI-style syntax:

        foo/0/15/bar

Equivalent:

        ./foo/0/15/bar

Numeric elements in the URI are interpreted as list-indices. To begin with 
the global symbol-table, simply begin the URI with a '/':

        /foo/0/15/bar

Finally, since references can be used to index into any bstruct, use the
`lu` operator to apply a reference to a particular data-structure:

        (1 2 3) ./1 lu --> returns 2

*Sigils*

Bipedal permits the use of a sigil (magic symbol) to specify the meaning of 
a label or symbol. There are six sigils:

<table>
  <tr>
    <th>Sigil</th><th>Magical property</th>
        <th>Bare symbol equivalent in</th><th>Mnemonic</th>
  </tr>
  <tr>
    <td>*</td><td>Lexical substitution</td><td>Leaf-array</td><td>"Contents-of" (C)</td>
  </tr>
  <tr>
    <td>&</td><td>Direct pointer</td><td>Interior-array and list</td><td>Reference (C++)</td>
  </tr>
  <tr>
    <td>!</td><td>Auto-eval reference</td><td>N/A</td><td>Imperative - do it!</td>
  </tr>
  <tr>
    <td>@</td><td>List location</td><td>N/A</td><td>Where it's at</td>
  </tr>
</table>

Appending a sigil to a section label or to a symbol imparts that magical 
property to it and may alter its default behavior. A sigil appended to a 
label overrides the default sense of that label in any context. However, 
you can double-override the symbol by appending a different sigil in a 
particular context. The default behavior of a bare symbol varies depending 
on the context in which it occurs. 

The asterisk (*) specifies that a symbol or section is to be used for 
lexical substitution. Lexical substitution allows a section or string to 
be expanded in a manner similar to a #define in C or string-interpolation 
in Perl. Note that lexical substitutions do not take arguments and are 
not macros.

This:

        foo : (1 2 3)  
        bar : (foo* 4 5)  

... is equivalent to this:

        bar : (1 2 3 4 5)

The order in which you define sections is maintained by Bipedal. In the 
above example, foo has precedence over bar.

Note that you can only use like with like - interior arrays can be 
substituted only into interior arrays, leaf arrays only into leaf arrays, 
lists only into lists, code-lists only into code-lists and strings only 
into strings (a compile-time version of string interpolation).

        hi : "hello"  
        hw : "hi* world"  

`hw` evaluates to "hello world"

Only double-quotes (and interpolating quote blocks) will perform lexical 
substitution of strings. Use single-quotes if you want to use asterisks in 
a string without having to escape each one.

If you plan to use a section primarily for lexical substitution, you can 
append an asterisk to the section-definition label.

        foo* : [ptr bar baz]  
        bop  : [ptr foo doo wop]  

Equivalent to:

        bop : [bar baz doo wop]  

And:

        add2 : { 2 + }  
        bar  : { 1 add2* }  

Equivalent to:

        bar : { 1 2 + }  

In a leaf-array, the only meaning of a symbol is lexical substitution.

        foo : [val 1 2 3 ]  
        bar : [val foo 4 5 ]  

Equivalent to:

        bar : [ 1 2 3 4 5 ]  

Gratuitous use of sigils is permitted:

        foo* : [val 1 2 3]  
        bar  : [val foo* 4 5]  

You cannot use any other sigil than the asterisk on a symbol that occurs 
in a leaf-array. If the symbol cannot be lexically substituted into the 
leaf-array, an error will occur. 

In list context or interior-array context, the default meaning of a bare 
symbol is a direct pointer.

        foo : [ptr 1 2 3]  
        bar : [ptr foo [4] [5]]  

Equivalent to:

        bar : [ptr [ptr 1 2 3] [4] [5]]  

The same is true of lists:

        foo : (1 2 3)  
        bar : (foo 4 5)  

Equivalent to:

        bar : ((1 2 3) 4 5)  

Note that, unlike lexical substitution, a direct-pointer to foo preserves 
the nesting but does not make a copy. If you want to nest and make a copy:

        foo : [1 2 3]  
        bar : [[foo*] [4] [5]]  

In list, interior-array or code-list contexts, the default meaning of a bare
symbol is as a reference.

        foo : [ptr 1 2 3]  
        bar : [ptr foo [4] [5]]  

Note that foo does not need to be defined at compile-time. If you want a 
direct-pointer instead of a reference, use the ampersand sigil (&):

        foo& : 0x38  
        bar  : [ptr foo [4] [5]]  

or:  

        bar  : [ptr foo& [4] [5]]  

Note that, in this case, foo must be defined at compile-time.

In Babel code, a reference can have one of two effects - it can either 
auto-eval a named section or it can auto-lookup data from the symbol table 
and place it on the stack. The default sense is to auto-lookup data. If 
you want to auto-eval it, you can use the auto-eval sigil, bang (!):

        add2 : { 2 + }  
        foo  : { 3 add2! }  

Note that, in this case, the above form has the same effect as:

        foo : { 3 add2 ! }  

Most of the time, you will likely want to simply define the section with 
the auto-eval sigil:

        add2! : { 2 + }  
        foo   : { 3 add2 }  

The auto-eval sigil can only be used in code-list context.

The location sigil (@) can never be used as a section definition label or 
in array context. It allows you to create direct references within a list 
or code-list. For example:

        foo: ( 1 2 three@ 3 )  
        bar: { foo/three car %d << }  

Equivalent effect as:

        { foo cdr cdr car %d << }  

You can use this facility to create a _compile-time hash_ that permits 
your code to reach the elements of a list but without the overhead of 
run-time hashing. For example:

        ( first_name@        "John"  
        last_name@           "Smith"  
        ID@                  "19010378" )  


