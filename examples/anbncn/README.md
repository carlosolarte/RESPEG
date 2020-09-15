# Grammar anbncn

The files `anbncn.maude` and  `anbncn-cut.maude` define the grammar for the
language with and without cuts. The file `exec.maude` executes all the test on
valid and non-valid inputs described below.
## Valid inputs 

The valid input files are 

 - yes/10-abc.txt
 - yes/50-abc.txt
 - yes/100-abc.txt
 - yes/200-abc.txt
 - yes/500-abc.txt
 - yes/1000-abc.txt

for different values of n. 

## Non-valid inputs

The directory ``` no/``` contains invalid inputs that result from mutations on the original files as described below. 


### Deleting characters
Generating 20 files per valid input file where 5 characters (a,b or c) are deleted. 

```
../mutation/mutate.py --output "./no/del" --ndel 5 --delchar "a b c" --nfiles 20 --input "./yes/10-abc.txt"
../mutation/mutate.py --output "./no/del" --ndel 5 --delchar "a b c" --nfiles 20 --input "./yes/50-abc.txt"
../mutation/mutate.py --output "./no/del" --ndel 5 --delchar "a b c" --nfiles 20 --input "./yes/100-abc.txt"
../mutation/mutate.py --output "./no/del" --ndel 5 --delchar "a b c" --nfiles 20 --input "./yes/500-abc.txt"
../mutation/mutate.py --output "./no/del" --ndel 5 --delchar "a b c" --nfiles 20 --input "./yes/1000-abc.txt"


```

### Inserting a character

Generating 20 files inserting a character (a, b, or c) on valid inputs. 

```
../mutation/mutate.py --output "./no/ins" --nins 1 --inschar "a b c" --nfiles 20 --input "./yes/10-abc.txt"
../mutation/mutate.py --output "./no/ins" --nins 1 --inschar "a b c" --nfiles 20 --input "./yes/50-abc.txt"
../mutation/mutate.py --output "./no/ins" --nins 1 --inschar "a b c" --nfiles 20 --input "./yes/100-abc.txt"
../mutation/mutate.py --output "./no/ins" --nins 1 --inschar "a b c" --nfiles 20 --input "./yes/200-abc.txt"
../mutation/mutate.py --output "./no/ins" --nins 1 --inschar "a b c" --nfiles 20 --input "./yes/500-abc.txt"
../mutation/mutate.py --output "./no/ins" --nins 1 --inschar "a b c" --nfiles 20 --input "./yes/1000-abc.txt"

```

### Substitutions
Generating 20 files substituting "c" with "x".

```
../mutation/mutate.py --output "./no/subs" --nsubs 1 --sfrom "c" --sto "x" --nfiles 20 --input "./yes/10-abc.txt"
../mutation/mutate.py --output "./no/subs" --nsubs 1 --sfrom "c" --sto "x" --nfiles 20 --input "./yes/50-abc.txt"
../mutation/mutate.py --output "./no/subs" --nsubs 1 --sfrom "c" --sto "x" --nfiles 20 --input "./yes/100-abc.txt"
../mutation/mutate.py --output "./no/subs" --nsubs 1 --sfrom "c" --sto "x" --nfiles 20 --input "./yes/200-abc.txt"
../mutation/mutate.py --output "./no/subs" --nsubs 1 --sfrom "c" --sto "x" --nfiles 20 --input "./yes/500-abc.txt"
../mutation/mutate.py --output "./no/subs" --nsubs 1 --sfrom "c" --sto "x" --nfiles 20 --input "./yes/1000-abc.txt"
```

## Results
Executing the file `exec.maude`, it is possible to obtain the results for all
test cases. The summary is in [results.ods](results.ods) (also in
[results.csv](results.csv)) .

