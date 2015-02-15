Tibru
=====

Tibru is an alternative to the nonsense that is the Urbit project. By nonsense I mean poor implementation, unecessarily obfuscated source code and generally poor design decisions leading to high runtime overheads with little advantage. It also suffers from academic elitism and lack of reference to prior art which is rampant throughout its design and documentation. Unfortunately despite its academic roots the chosen runtime language is arbitrary and poorly selected from hundreds of alternatives.

This project has two aims. The first it too allow anyone to easily access systems of this type by using generally accepted coding standards and techniques. The executable provides an interactive REPL environment with human readable qualities. The second is to go above and beyond what is reasonably capable with Nock (Urbit's runtime language). This tenchnology is open for anyone to use and lends itself quite nicely to app container implementations since it is side-effect free and runs on immutable data structures.

As of writing no further work is being done on the system as it has met its first goal to implement a basic runtime structure.

Build
=====

To build the 2 main executables 'kcon' and 'ohno' run make within the src/cpp directory. G++ 4.9 or higher is required.

	make

To start KCon in REPL mode use

	./bin/kcon

To run a script

	./bin/kcon <script name>

	./bin/kcon -help

Shows all the other command arguments available.
If you run a script rather than from the REPL then you'll only see the last expression evaluated.
To see all expressions use the -noisy command line parameter.


KCon
====

The basic runtime language follows from Nock that takes primitives to be numbers and pairs. Pairs being pairs of numbers or other pairs. One critism of Nock is its use of arbitrarily large integers for a number base. KCon on the other hand uses only bytes (0-255). This enables KCon to run without arithmetic operators (Nock is liberally supplied with infinitely many more arithmetic operators by supporting increment).

We write bytes base 10 and pairs use square bracket notation.

	>>> 0
	0
	>>> [0 [1 2]]
	[0 1 2]

Pairs are written right associatively so [0 [1 2]] is equivalent to [0 1 2]. Within the KCon environemnt we can assign names to elements.

	>>> :def twenty 20
	>>> :def primes [2 3 5 7 11]

Then type any expression to see the value. It will then be named ***it*** for future expressions.

	>>> twenty
	20
	>>> 20
	20
	>>[twenty twenty]
	[20 20]

It would be nice to see the named expression by name. We can do that with:

	>>> :names on
	>>> it
	[twenty twenty]

Several names are already defined in KCon. These can be listed:

	>>> :defs
	EXIT
	F
	GRAFT
	IF
	T
	nil
	qt
	sel
	>>> qt
	0

Since the data is immutable and within KCon it's impossible to decide if two elements are **identical** we rely on the implementation to optimize for shared elements.
This can be seen with the names turned on but it's an artefact of the implementation. Logically the data tree doesn't recombine but KCon can't decide that.

KCon accepts multi-line input:

	>>>[1
	... 2]
	[1 2]

KCon provides operators that act on expressions to yield another expression.

	>>> :help
	Evaluate an expression of the form <name>|byte|[<expr> <expr>+]|<reader><expr>|<expr><macro> and define 'it' as its value
	Or process an operator on an expression of the form <op><expr> where 'it' is the current state and define 'it' as its value
	Or run a command

	Commands:
	:def <name> <expr> - Define a named expression
	:process <filename> - Process all statements in the specified file
	:include <filename> - Silently process all statements in the specified file
	:dec  - Show bytes in decimal notation (default)
	:hex  - Show bytes in hex notation
	:flat - Show cells as flattened right associative lists (default)
	:deep - Show cells as pairs
	:line - Show expressions on a single line (default)
	:list - Show expressions over multiple lines in list format
	:names [on|off] - Show named expressions as defined names (works best in debug modes)
	:defs - Show all defined names
	:sys  - Show information about the system
	:gc   - Run the garbage collector
	:help - Show this help
	:exit - End the shell session
	:quit - End the shell session
	:trace [<limit>|off] - Turn tracing on or off for ! operator

	Operators:
	![[f x] v]        -> ![*[v f] *[v x]]
	![0 x]            -> x
	![1 c [k1 k2] v]  -> ![?[c k1 k2] v]
	![2 v e r k]      -> ![v +[e r k]]
	*[v [[x y] .. z]] -> [@[v z] @[v [x y]]]
	*[v e]            -> @[v e]
	@[v 0 x]          -> .x
	@[v 1 r]          -> /[v r]
	.x                -> x
	/[v [t1 ..] h1]   -> head{h1}( tail{t1 + 256*t2 ...}(v) )
	/[v [t1 ..] h1 r] -> /[head{h1}( tail{t1 + 256*t2 ...}(v) ) r]
	?[[a b] x y]      -> x
	?[a x y]          -> y
	+[v e r]          -> e++v|r where e++v|r := append e onto v at r

	Readers:
	# - Convert unsigned integer into [b1 b2 b3 b3] format e.g. #1000 -> [232 3 0 0]

	Macros:
	' - Convert <expr> into [0 <expr>] e.g. 3' -> [0 3]
	< - Reverse preceeding list elements e.g. [1 2 3 < 4 5] -> [3 2 1 < 4 5]


The specification of KCon is the set of operators as defined by the help. The simplest operator is ***quote*** written as **.**.

	>>> .21
	21

The ***select*** operator **/** traverses a binary tree data expression and selects the relevant element. It has a non-standard path format.
This path list has an even number of elements alternating between the number of tail traverses followed by head traverses.
The tail traverses are multi-byte numbers constructed of lists of bytes. The n'th byte is the coefficient of 256<sup>n</sup> starting at n=0.
The head traverses are simple bytes. Since writing out multi-byte numbers is a pain KCon provides a *reader* which is a special parser that generates data expressions.
For 32-bit multi-byte numbers use the **#** reader:

	>>> #1000
	[232 3 0 0]

We can then easily use the ***select*** operator:

	>>> /[21 #0 0]
	21
	>>> /[[1 2 3] #1 1]
	2
	>>> /[[[1 2] 3 4] #0 1 #1 0]
	2
	>>> :def hd [#0 1]
	>>> :def tl [#1 0]
	>>> /[[1 2 3] hd]
	1
	>>> /[[1 2 3] tl]
	[2 3]

A path of `[#0 0]` acts like an identity function.

The conditional operator ***if*** selects based on whether the test is a byte or a pair.

	>>> ?[[twenty twenty] 0 1]
	0
	>>> ?[twenty 0 1]
	1

These are the 3 basic operators that reduce expressions based on selection. In implementation terms they don't allocate new cells and aren't recursive.
The next operators reduce expressions by building new ones and are interpreters since their behaviour is modified by parameters.
**@** is called the ***reduce*** operator it takes an environment, an instruction code and some parameters:

	>>> :def nums [1 2 3]
	>>> @[nums qt 21]
	21
	>>> @[nums sel tl]
	[2 3]

It's common to need constant expressions using the ***quote*** operator so instead of having to write `[qt 21]` there is a macro:

	>>> 21'
	[qt 21]
	>>> [1 2 3]'
	[qt 1 2 3]

Analogously the ***evaluate*** operator **\*** delegates to **@** but can create pairs as well:

	>>> *[nums [qt 21] [sel tl]]
	[[2 3] 21]

Notice how it evaluates in reverse order. This increases performance as the list of arguments is evaluated from head to tail but the results are built up tail to head.
Also since **\*** delegates to **@** it is not recursive, again, for performance reasons.

This leaves one operator which is recursive to explain but fortunately its tail recursive. The ***execute*** operator **!** takes a statement and an environment.
It then **executes** the statement in that environment resulting in a new environment and a continuation.
Continuations are built into KCon which necessarily uses CPS (continuation passing style).

The simplest form in the **EXIT** statement:

	>>> ![EXIT 21]
	21

Then we have the conditional form that selects 1 of two continuations based on whether the test element is a pair of not:

	>>> ![IF [twenty twenty] [[[sel tl] EXIT'] [[sel hd] EXIT']] nums]
	[2 3]
	>>> ![IF twenty [[[sel tl] EXIT'] [[sel hd] EXIT']] nums]
	1

It's interesting for debugging purposes to see each step being executed:

	>>> :trace 32
	>>> ![IF twenty [[[sel tl] EXIT'] [[sel hd] EXIT']] nums]
	1.	[[[sel hd] qt EXIT] nums]
	2.	[EXIT 1]
	1

That's all that is needed for functional programming.
However, there is one more operator ***GRAFT*** which is redundant but allows tree manipulations without having to formally write out a recursive algorithm.
It takes a data expression (the tree), a path and an element. This result is a new tree with the element attached at the path location:
	
	>>>	 +[nums [3 4] [#2 0]]
	[1 2 3 4]

Inexecute mode it takes a continuation:
	
	>>> :def env [sel #0 0]
	>>> ![GRAFT nums [3 4] [#2 0] [env EXIT']]
	1.	[[env qt nil] 1 2 3 4]
	2.	[nil 1 2 3 4]
	[1 2 3 4]

That's all there is but it's very powerful.

OhNo (Not that language)
========================

All this is very unweildy so the OhNo interprerer defines a new shell command ***fn*** that parses a function definition and outputs an expression implementing it.
The choses runtime structure is to pass a **heap** object into every function call and back out again with the result.
Since KCon requires CPS the order of execution is known so updates to this **heap** structure are well defined. This allows for imperative programming.
We haven't yet seen function recursion but this is possible.
**Currently fn is not implemented but the ideas here show what to do**

The goal of this section is to define a function that calculates fibonacci numbers and threads a **heap** throughout.
We'd like to be able to write continuations at the end of expressions so there is a macro for arranging arguments.

	>>> [1 2 3 4 @]
	[1 4 2 3]

You must be running the ./bin/ohno shell command to have this macro available.
Let's start with arithmetic.

	>>> :def inc_table [#1 #2 #3 #4 #5 #6 #7 #8 #9 #10 nil]

And a lookup function:

	>>> :names on
	>>> :def env [1 #0 0]
	>>> :def stk [1 #0 0]
	>>> :def hd [sel #0 1]
	>>> :def tl [sel #1 0]
	>>> :def heap [sel #0 1]
	>>> :def kont [sel #1 1]
	>>> :def pop1 [sel #3 0]
	>>> :def pop2 [sel #4 0]
	>>> :def pop3 [sel #5 0]
	>>> :def v0 [sel #2 1]
	>>> :def v1 [sel #3 1]
	>>> :def v2 [sel #4 1]
	>>> :def EXITENV [env EXIT']
	>>> :def EXITVAL [[sel #1 1] EXIT']
	>>> :def lookup1 [1' v1 1' #2' sel' env [kont' heap' tl pop2' hd [hd tl]']']
	>>> :def memory [0 0 0 0]

	>>> ![lookup1 [memory EXITENV inc_table #3 nil]]
	[memory [4 0 0 0] nil]

This is what a function call looks like. We call `lookup1` passing in the argument `#3`, the lookup table, the continuation `EXITENV` and the heap.
We get out the heap and the result. The element `nil` is the call stack terminator.
From here we can define `inc` formally:

	>>> :def inc [pop1 v0 inc_table' heap lookup1' kont @]
	>>> ![inc [memory EXITVAL #3 nil]]
	[4 0 0 0]

`EXITVAL` removes the call stack and heap leaving just the return value.
`kont` references the passed in continuation on the stack.
`heap` references the passed in heap on the stack
`pop1` removes the one argument from the stack (ie takes a tail part of the stack).

From here we can alter the heap.

	>>> :def mem2 [#0 1 #2 1]
	>>> :def setheap [[pop2 heap kont]' v1 v0 stk GRAFT']
	>>> ![setheap [memory EXITENV 21 mem2 nil]]
	[[0 0 21 0] nil]

Here we set the heap at index 2 with the value `21`.

Putting this altogether we can define `fib` with a little help from `add` and `sub` defined using lookup tables and `lookup2`.

	>>> :def lookup2 [1' v1 1' v2 1' #2' sel' env [kont' heap' tl pop3' hd [hd tl]']']

This is what `fib` looks like:

	>>> :def r0 [sel #1 1]
	>>> :def r1 [sel #2 1]
	>>> :def r2 [sel #3 1]
	>>> :def call [sel #1 0]
	>>> :def _kont [sel #2 1]
	>>> :def _pop1 [sel #4 0]
	>>> :def _pop2 [sel #5 0]
	>>> :def _v0 [sel #3 1]
	>>> :def _v1 [sel #4 1]
	>>> :def __v0 [sel #4 1]
	>>> :def __v1 [sel #5 1]
	>>> :def ___v0 [sel #5 1]
	>>> :def ___v1 [sel #6 1]
	>>> :def ____v0 [sel #6 1]
	>>> :def ____v1 [sel #7 1]
	>>> :def ______kont [sel #7 1]
	>>> :def ______pop2 [sel #10 0]
	>>> :def fib [call #2' v1 heap lt' [
			stk r0 IF' [
				[_pop2 #1' heap _kont]
				[call #1' _v1 heap sub'
					[call r0 __v0 heap __v0
						[call #2' ___v1 heap sub'
							[call r0 ____v0 heap ____v0
								[call r0 r2 heap add'
									[______pop2 r0 heap ______kont]' @]' @]' @]' @]' @]
			]' @]' @]

	>>> ![fib [memory EXITENV fib #6 nil]]
	[memory [13 0 0 0] nil]

The full source for this is available in the file `test.ohno`.

For the brave without names this becomes:

	>>> fib
	[[1 [1 0 0 0] 0] [0 2 ...
