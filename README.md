# RESPEG: Rewriting Semantics for PEGs

## General Description

We give a rewriting logic semantics for Paring Expression Grammars
([PEGs](https://en.wikipedia.org/wiki/Parsing_expression_grammar)) and
implement such rewrite theory in
[Maude](http://maude.cs.illinois.edu/w/index.php/The_Maude_System) (a rewriting
engine). This repository contains the Maude files of the specification as well
as some examples of grammars and benchmarks. 

We start with a one-to-one encoding of the PEG semantics as rewrite rules. This
theory helped us to understand the meaning and behavior of PEG expressions.
Although rewrite theories are executable specifications, the implementation of
this theory in Maude is not efficient even for toy examples. We propose another
rewrite theory that introduces more structure on the representation of PEG's
rules, thus obtaining more efficient decision and verification procedures. The
rewrite theories included here formalize also the notion of local and global
cuts as described in our paper "*A semantic framework for PEGs*" (see a
pre-print of the paper [here](paper.pdf)). Cuts help the designer of the
grammar to control the backtracks during parsing. The directory `examples/`
contains the examples of grammars (with and without cuts) discussed in the
paper and the data collected on some benchmarks. 


## Getting Started

This project was tested with [Maude
3.0](http://maude.cs.illinois.edu/w/index.php/Maude_download_and_installation).
No extra libraries or tools are needed for reproducing the experiments and data
in the paper, besides the interpreter of Maude.  There is a simple
[script](./examples/mutation) written in [Python3](https://python.org/) for
automatically generating new benchmarks.

The binary distribution of  [Core Maude
3.0](http://maude.cs.illinois.edu/w/index.php/Maude_download_and_installation)
includes the Maude interpreter and also the `prelude` files. Binary files are
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

Execute permissions must be assigned to the file  `maude-CVC4.[linux64|darwin64]`:

```
$> chmod +x maude-CVC4.[linux64|darwin64]
```

After the  execution of  `maude-CVC4.[linux64|darwin64]`, you should observe the following:

```
$> ./maude-CVC4.linux64

		     \||||||||||||||||||/
		   --- Welcome to Maude ---
		     /||||||||||||||||||\
	     Maude 3.0 built: Dec 17 2019 12:08:10
	     Copyright 1997-2019 SRI International
		   Sun Sep 13 14:15:36 2020

```

To exit the Maude interpreter, type `quit . ` (mind the space and the `.`
after `quit`). 

In some Linux systems, the library `libtinfo.so.5` is needed to execute Maude.
If Maude exists with a message similar to

```
error while loading shared libraries: libtinfo.so.5: cannot open shared object file: No such file or directory
```

please install `libtinfo5`. For instance, in Ubuntu, it suffices to type

```
$> sudo apt install libtinfo5
```


From now on, we shall assume that the executable above is included in your
current `PATH` environment variable:

```
$> export PATH=$PATH:path-to-maude-directory
```

and we shall simply write `maude` instead of `maude-CVC4.[linux64|darwin64]` to
refer to the Maude's interpreter. 

`.maude` files are directly executed with the Maude engine. Hence, the command
`$> maude file.maude` executes the specification written in `file.maude`. 

We have also created a Virtual Machine available
[here](https://drive.google.com/file/d/18Rhz5IFJtiRcXmpceVNv_gYzgXfHh5VA/view?usp=sharing)
(login=password=guest). Maude is already installed there (just type `maude`)
and the present repository is cloned in `/home/guest/RESPEG`. Please, do a `git
pull` to bring the last version of the files. 


## Structure of the project

In the following we describe the files of the project that correspond to the
rewrite theories explained in our paper. In each case, we provide some simple
examples. Some other examples are also included in the end of the files (after
the command `eof`). More interestingly, the directory `examples/` contains
concrete grammars of different programming languages (e.g., c89 and json) and
sets of benchmarks showing how the introduction of cuts in the grammar may save
some (unnecessary) backtracks. 


### peg-syntax.maude

This functional theory defines the syntax for building parsing expression,
grammars and strings (see Sections 2 and 3 of our paper). 

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

Following the mechanism proposed in our paper (Section 4) for throwing and
capturing errors in PEGs, valid expressions include also:

```
p := [e]e | [e]c
```

### peg-rules.maude

This file specifies a naive interpreter for PEGs: operational rules are
directly translated as rewrite rules.

States for the parser are terms of the form

```
G[e] x
```

where `G` is a grammar, `e` is a parsing expressions and `x` the input (see
Section 2 of our paper). 

This rewrite theory is not quite efficient (some tests in the end of the file).
The main reason is that it extensively uses conditional rules for given meaning
to choices, sequential composition and negation. 

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
is `"b" "c"`, corresponding to the string (`bc`) not consumed by the
expression. The string `eps` is the empty string and the state `fail` denotes a
failure (the input string cannot be matched by the parsing expression). 

### peg-parser.maude

In this rewrite theory, sequential composition, choices, negation and
replication are encoded as suitable constructors on states (more details on
Section 3.1 of our paper). For instance, sequential composition is encoded as
follows: 

```
rl [Sequence] :  G [ (e . e') ] x 
      => COMP(  G [ e ] x  ,  G [ e' ] x  ) .
```

where `COMP` is a constructor that gives meaning to the expression `e e'`. 
This avoids the use of conditional rewrite rules in the specification. 

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

This theory is similar to the previous one with the only difference that the
state of the parser includes the number of steps performed (including
backtracks) before reaching the final state. 

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

The function `parse` receives as parameter the grammar, the initial expression
and the string to be parsed. The final state `{eps ; 48}` tells us that all the
string was consumed and `48` rules where applied to reach that state.  On the
other hand, the final state `fail(46)` indicates that the input string cannot
be matched and the parser failed after `46` steps. 

In grammars with cuts, it is also possible to observe final states of the form
`error(n)`. This is a failure (similar to `fail`) but caused by a cut operator.
The number `n` represents also the number of steps before failing. 

As a simple example, the following grammar fails with an `error` if the string
starts with `a` but it does not continue with `bc`. On input `abx`, the failure
on `x` produces an error and the second alternative of the choice can be safely
ignored (more details on Section 4 of our paper). 

```
Maude> rew parse( 'A <- "a" . try("b" . "c") / "d", 'A, str("abx")) .
result State: error(8)

--- without cuts, the second branch is (unnecessarily) visited after the failure on x
Maude> rew parse( 'A <- "a" . "b" . "c" / "a" . "d", 'A, str("abx")) .
result State: fail(10)
```
### peg-narrowing.maude

In the appendix of the paper we describe a preliminary attempt of using
symbolic techniques to implement a derivative parser for PEGs. Such analyses
are implemented in the file `peg-narrowing.maude`.

### io-ext.maude

Auxiliary functions to read and write files

## Examples and benchmarks 

In the directory `examples` there are some tests performed on different grammars:

 - [anbncn](./examples/anbncn):  tests on the language `a^n b^n c^n`
 - [pascal](./examples/pascal): comments in the style of Pascal (Example 4.2 in our paper)
 - [c89](./examples/c89): Ansi C 89 (Section 4.5 in our paper)
 - [json](./examples/json): grammar for JSON (Section 4.5 in our paper)
 - [pallene](./examples/pallene): An extension of Lua (Section 4.5 in our paper)

In each directory, the following files can be found: 

 - `README.md`, detailing the source of the benchmarks.
 - Maude files specifying the grammar for the given language as well as the version of the grammar annotated with cuts. 
 - `exec.maude`, a Maude file executing the grammar with and without cuts on all the input files. 
 - `results.csv`, summarizing the results. 

As an example, consider the directory `anbncn`. The valid input files are in
the directory `yes` (i.e., strings of the form `a^nb^nc^n` for some given `n`).
Using the Python3 script [mutation](./examples/mutation/mutate.py), we generate
some non-valid inputs by deleting, inserting and substituting characters on the
valid input files. Those files are stored in the directory `no/`. The file
`anbncn.maude` defines the grammar for the language and `anbncn-cut.maude`
introduces some cuts (i.e., expressions of the form `["b"]e`). The file
`exec.maude` loads both specifications and runs the tests on valid and
non-valid input files:

```
--- Excerpt of exec.maude
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

Maude> erew read-file("./no/del/10-abc-00.txt") .
erewrite in TEST : read-file("./no/del/10-abc-00.txt") .
rewrites: 198 in 0ms cpu (0ms real) (613003 rewrites/second)
result State: fail(46)
```

We have collected all the results and summarized them in the `csv` files of each
directory, containing the final state of the parser for each input file as well
as the number of steps performed to reach that state. These results are also
reported in Table1 of our paper. 

The bash script `example/exec-all.sh` can be used to run all the benchmarks and
generate the CSV files wit the results. Note that the line 7 of that script

```
MAUDE=maude
```

specifies the location of the Maude interpreter. Once this variable is
configured (e.g., it can be set to `/home/guest/maude/maude-CVC4.linux64`) add
execute permissions and run the script to re-generate all the CSV file when
needed:

```
$> chmod + exec-all.sh
$> ./exec-all.sh
```

