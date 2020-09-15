# RESPEG: Rewriting Semantics for PEGs

## General Description
We give a rewriting logic semantics for Paring Expression Grammars
([PEGs](https://en.wikipedia.org/wiki/Parsing_expression_grammar)) and implement such specification in 
[Maude](http://maude.cs.illinois.edu/w/index.php/The_Maude_System) (a rewriting
engine). This repository contains the needed Maude files as well as some
examples of grammars and benchmarks. 

We start with a one-to-one encoding of the PEG semantics as rewrite rules. This
theory helped us to understand the meaning and behavior of PEG expressions. Although rewrite theories are executable specifications, the implementation of this theory in Maude is not efficient even for toy examples. We propose another rewrite theory that introduces more structure on the
representation of PEG's rules, thus obtaining more efficient decision and verification procedures. The rewrite theories included here formalize also the notion of local and global cuts as described in our paper "A semantic framework for PEGs" (see a pre-camera-ready version of the paper [here](paper.pdf)). Cuts help the designer of the grammar to control the backtracks during parsing. The directory `examples/` contains the examples of grammars (with and without cuts) discussed in the paper and the data collected on some benchmarks. 


## Getting Started
This project was tested with [Maude
3.0](http://maude.cs.illinois.edu/w/index.php/Maude_download_and_installation).
In order to reproduce the experiments and data in the paper, besides the
interpreter of Maude, no extra libraries or tools are needed. There is a simple
[script](./examples/mutation) written in
[Python3](https://python.org/) for automatically generating new benchmarks.

The binary distribution of  [Core Maude
3.0](http://maude.cs.illinois.edu/w/index.php/Maude_download_and_installation)
includes the Maude's interpreter and also the `prelude` files. Binary files are
available for MacOS and Linux. Please download the following zip files
according to your architecture:

 - [Maude 3.0 (with CVC 4) for Linux64](http://maude.cs.illinois.edu/w/images/7/73/Maude-3.0%2Bcvc4-linux.zip)
 - [Maude 3.0 (with CVC 4) for Mac OS X](http://maude.cs.illinois.edu/w/images/6/69/Maude-3.0%2Bcvc4-osx.zip)


After extracting the zip file, a folder with the following files will be created:

```
linear.maude
machine-int.maude
maude-CVC4.[linux64|darwin64]
metaInterpreter.maude
model-checker.maude
prelude.maude
socket.maude
term-order.maude
smt.maude
```

By executing `maude-CVC4.[linux64|darwin64]` you should observe the following:

```
$> ./maude-CVC4.linux64

		     \||||||||||||||||||/
		   --- Welcome to Maude ---
		     /||||||||||||||||||\
	     Maude 3.0 built: Dec 17 2019 12:08:10
	     Copyright 1997-2019 SRI International
		   Sun Sep 13 14:15:36 2020

```

To exit the Maude interpreter, type `quit . ` (mind the space and the `.`  after `quit`). 

From now on, we shall assume that the executable above is included in your
current `PATH` environment variable:

```
$> export PATH=$PATH:path-to-maude-directory
```

and we shall simply write `maude` instead of `maude-CVC4.[linux64|darwin64]` to refer to the Maude's interpreter. 

`.maude` files are directly executed with the Maude engine. Hence, the command `$> maude file.maude` will execute the specification written in `file.maude`. 

For the reviewing process, we have also created a Virtual Machine available [here](gdrive) (login=password=test). Maude is already installed there (just type `maude`) and this repository is cloned in `/home/test/RESPEG`. Please, do a `svn pull` to bring the last version of the files. 


## Structure of the project

In the following we describe the files of the project that correspond to the rewrite theories explained in our paper. In each case, we provide some simple examples. Some other examples are also included in the end of the files (after the command `eof`). More interesting, the directory `examples/` contains concrete grammars of different programming languages (e.g., c89 and json) and sets of benchmarks showing how the introduction of cuts in the grammar may save some (unnecessary) backtracks. 


### peg-syntax.maude
This functional theory defines the syntax for building parsing expression, grammars and strings. 

Parsing expressions are built from: 

```
p := emp | t | NT | p . p | p \ p | ! p | p *
```

Terminals are Maude's ```Char```s and non-terminals are Maude's ```Qid```s. 

Some derived constructors are also provided:

```
p := ... | p ? | & p 
```

The usual character classes are also defined here, e.g., 

```
op [.] : -> TExp [ctor] . --- Any character
op [0-9] : -> TExp [ctor] .
op [a-z] : -> TExp [ctor] .
```

Following the mechanism for throwing and capturing errors in PEGs for avoiding (unnecessary) backtracks proposed in our paper, valid expressions include also:

```
p := [e]e | [e]c
```

### peg-rules.maude
This file specifies a naive interpreter for PEGs: operational rules are directly translated as rewrite rules.

States for the parser are terms of the form

```
G[e] x
```
where G is a grammar, e is a parsing expressions and x the input. 

This rewrite theory is not quite efficient (some tests in the end of the file). The main reason is that it extensively uses
conditional rules for given semantics to choices, sequential composition and negation. 

The example below shows:

 1. that the expression ``a *`` on input `aabc` returns `bc`. 

 2. that the expression `A B` where 

 ```
 A <- a / emp
 B <- b
 ```
 on input `abc` and `bc` outputs `c`.

 3. that the grammar for recognizing the language `a^n b^n c^n` defined as 

```
S  <- ( &(R1 c) ) (a +) R2 (! .)
R1 <- a  (R1 ?) b
R2 <- b (R2 ?) c
 
```

succeeds on some valid inputs (and fails on others). 

```
$> maude peg-rules

		     \||||||||||||||||||/
		   --- Welcome to Maude ---
		     /||||||||||||||||||\
	     Maude 3.0 built: Dec 17 2019 12:08:10
	     Copyright 1997-2019 SRI International
		   Sun Sep 13 14:22:49 2020

Maude> rew nil ["a" *] str("aabc") .
result Str: "b" "c"

Maude> rew ('A <- "a" / emp,  'B <- "b") ['A . 'B] str("abc") .
result Char: "c"

Maude> rew ('A <- "a" / emp,  'B <- "b") ['A . 'B] str("bc") .
result Char: "c"

Maude> rew ( 'S <- ( &('R1  . "c") ) . "a" + . 'R2 . (! [.]) , 'R1 <- "a"  . ('R1 ?) . "b", 'R2 <- "b" . ('R2 ?) . "c" ) [ 'S ] str("abc") .
result Str: eps

Maude> rew ( 'S <- ( &('R1  . "c") ) . "a" + . 'R2 . (! [.]) , 'R1 <- "a"  . ('R1 ?) . "b", 'R2 <- "b" . ('R2 ?) . "c" ) [ 'S ] str("aabbcc") .
result Str: eps

Maude> rew ( 'S <- ( &('R1  . "c") ) . "a" + . 'R2 . (! [.]) , 'R1 <- "a"  . ('R1 ?) . "b", 'R2 <- "b" . ('R2 ?) . "c" ) [ 'S ] str("aabbccc") .
result State: fail
```

`rew` tells Maude to rewrite the term until it reaches a normal form. For
instance, in the first line, the term is `nil ["a" *] str("aabc")` denoting the
parser-state `G [a*] aabc` where `G` is empty (no non-terminals are defined
here). After applying the rules defining the semantics of PEGs, the final state
is `"b" "c"` denoting the string (`bc`) not consumed by the expression. The
string `eps` is the empty string and the state `fail` denotes a failure (the
input string cannot be matched by the parsing expression). 

### peg-parser.maude

In this version, sequential composition, choices, negation and replication are encoded as suitable constructors on states (more details on Section 3.1 of our paper). For instance, sequential composition is encoded as follows: 

```
rl [Sequence] :  G [ (e . e') ] x 
      => COMP(  G [ e ] x  ,  G [ e' ] x  ) .
```

This avoids the use of conditional rewriting rules in the specification. 

This file can be used similarly as `peg-rules`. Here the example for `a^n b^n c^n`: 

```
$> maude peg-parser
		     \||||||||||||||||||/
		   --- Welcome to Maude ---
		     /||||||||||||||||||\
	     Maude 3.0 built: Dec 17 2019 12:08:10
	     Copyright 1997-2019 SRI International
		   Sun Sep 13 15:35:42 2020
Maude> rew ( 'S <- ( &('R1  . "c") ) . "a" + . 'R2 . (! [.]) , 'R1 <- "a"  . ('R1 ?) . "b", 'R2 <- "b" . ('R2 ?) . "c" ) [ 'S ] str("aaaaabbbbbccccc") .
result Str: eps
```

### peg-parser-count.maude
This theory is similar to the previous one with the only difference that the state of the parser includes the number of steps performed (including backtracks) before reaching the final state. 

```
$> maude peg-parser-count
		     \||||||||||||||||||/
		   --- Welcome to Maude ---
		     /||||||||||||||||||\
	     Maude 3.0 built: Dec 17 2019 12:08:10
	     Copyright 1997-2019 SRI International
		   Sun Sep 13 15:42:13 2020
Maude> rew parse( 'S <- ( &('R1  . "c") ) . "a" + . 'R2 . (! [.]) , 'R1 <- "a"  . ('R1 ?) . "b", 'R2 <- "b" . ('R2 ?) . "c" , 'S , str("aabbcc") ) .
result State: {eps ; 48}

Maude> rew parse( 'S <- ( &('R1  . "c") ) . "a" + . 'R2 . (! [.]) , 'R1 <- "a"  . ('R1 ?) . "b", 'R2 <- "b" . ('R2 ?) . "c" , 'S , str("aabbc") ) .
result State: fail(46)
```

The function `parse` receives as parameter the grammar, the initial expression and the string to be recognized. The final state `{eps ; 48}` tells us that all the string was consumed and `48` rules where applied to reach that state. 
On the other hand, the final state `fail(46)` indicates that the input string cannot be matched and the whole parser failed after `46` steps. 

In grammars with cuts, it is also possible to observe final states of the form
`error(n)`. This is a failure (similar to `fail`) but caused by a cut operator.
The number `n` represents also the number of steps before failing. 

For instance, the following grammar fails with an `error` if the string starts with `a` but it does not continue with `bc`. On input `abx`, the failure on `x` does not produce a backtrack to consider the second branch of the choice (more details on Section 4 of our paper). 

```
Maude> rew parse( 'A <- "a" . ["b" . "c"]e / "d", 'A, str("abx")) .
result State: error(8)

--- without cuts, the second branch is (unnecessarily) visited after the failure on x
Maude> rew parse( 'A <- "a" . "b" . "c" / "a" . "d", 'A, str("abx")) .
result State: fail(10)
```


### io-ext.maude
Auxiliary functions to read and write files


## Examples of grammars and tests
In the directory `examples` there are some tests performed on different grammars:

 - [anbncn](./examples/anbncn):  tests on the language `a^n b^n c^n`
 - [pascal](./examples/pascal): comments in the style of Pascal
 - [c89](./examples/c89): Ansi C 89
 - [json](./examples/json): grammar for JSON
 - [pallene](./examples/pallene): An extension of Lua

In each directory, the following files can be found: 

 - `README.md`, detailing the source of the benchmarks.
 - Maude files specifying the grammar for the given language as well as the version of the grammar annotated with cuts. 
 - `exec.maude`, a Maude file executing the grammar with and without cuts on all the input files. 
 - `results.csv`, summarizing the results. 

As an example, consider the directory `anbncn`. The valid input files are in the directory `yes` (i.e., strings of the form `a^nb^nc^n` for some given `n`). Using the Python script [mutation](./examples/mutation/mutate.py), we generate some non-valid inputs by deleting, inserting and substituting characters on the valid input files. Those files are stored in the the directory `no/`. The file `anbncn.maude` defines the grammar for the language and `anbncn-cut.maude` introduces some cuts (i.e., expressions of the form `["b"]e`). The file `exec.maude` loads both specifications and runs the tests on valid and non-valid input files:

```
--- Loading the specification
load anbncn .
--- running one test
erew read-file("./yes/10-abc.txt") . 
...
load anbncn-cut .
erew read-file("./yes/10-abc.txt") . 
...
```

By executing `$> maude exec`, all the tests are performed. 

In order to process a particular file, type the following commands: 

```
$> maude anbncn
		     \||||||||||||||||||/
		   --- Welcome to Maude ---
		     /||||||||||||||||||\
	     Maude 3.0 built: Dec 17 2019 12:08:10
	     Copyright 1997-2019 SRI International
		   Tue Sep 15 08:47:52 2020
Maude> erew read-file("./yes/500-abc.txt") .
erewrite in TEST : read-file("./yes/500-abc.txt") .
rewrites: 22074 in 226ms cpu (227ms real) (97472 rewrites/second)
result State: {eps ; 7023}
```

We have collected all the results and summarize them in the `csv` files of each directory, containing the final state of the parser for each input file as well as the number of steps performed to reach that state. These results are also reported in Table1 of our paper. Some minor differences are due to adjustments in the implementations performed after the submission of the paper (and will be included in the camera ready version). 
