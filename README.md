tibru
=====

Tibru is an alternative to the nonsense that is the Urbit project. By nonsense I mean poor implementation, unecessarily obfuscated source code and generally poor design decisions leading to high runtime overheads with little advantage. It also suffers from academic elitism and lack of reference to prior art which is rampant throughout its design and documentation. Unfortunately despite its academic roots the chosen runtime language is arbitrary and poorly selected from hundreds of alternatives.

This project has two aims. The first it too allow anyone to easily access systems of this type by using generally accepted coding standards and techniques. The executable provides an interactive REPL environment with human readable qualities. The second is to go above and beyond what is reasonably capable with Nock (Urbit's runtime language). This tenchnology is open for anyone to use and lends itself quite nicely to app container implementations since it is side-effect free and runs on immutable data structures.

As of writing no further work is being done on the system as it has met its first goal to implement a basic runtime structure.

Build
=====

To build the 2 main executables 'kcon' and 'ohno' run make within the src/cpp directory.

	make

To start KCon in REPL mode use

	./bin/kcon

To run a script

	./bin/kcon <script name>

	./bin/kcon -help

Shows all the other command arguments available.


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

Then type any expression to see the value. It will then be named 'it' for future expressions.

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

KCon accepts multi-line input:

	>>>[1
	... 2]
	[1 2]

KCon provides operators that act on expressions to yield another expression.

	>>> :help

The specification of KCon is the set of operators as defined by the help. The simplest operator is 'quote'

	>>> .21
	21

The select operator '/' traverses a binary tree data expression and selects the relevant element. It has a non-standard path format.
This path list has an even number of elements alternating between the number of tail traverses followed by head traverses.
The tail traverses are multi-byte numbers constructed of lists of bytes. The n'th byte is the coefficient of 256^n starting at n=0.
The head traverses are simple bytes. Since writing out multi-byte numbers is a pain KCon provides a 'reader' which is a special parse




