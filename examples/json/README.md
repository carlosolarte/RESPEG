# Grammar for JSON

The files `json.maude` and  `json-cut.maude` define the grammar for JSON with
and without cuts. 


## Valid files
The original (valid) JSON files where taken from [this
repository](https://github.com/RichardHightower/json-parsers-benchmark).  The
files are in the directory `./yes`.


## Generating the benchmarks
For each valid input file, 10 incorrect files were generated where at most 10
characters (`} ]`) are deleted. 


```
../mutation/mutate.py --output "./no/del" --ndel 10 --delchar "} ]" --nfiles 10 --input "yes/1.json"
../mutation/mutate.py --output "./no/del" --ndel 10 --delchar "} ]" --nfiles 10 --input "yes/2.json"
../mutation/mutate.py --output "./no/del" --ndel 10 --delchar "} ]" --nfiles 10 --input "yes/3.json"
../mutation/mutate.py --output "./no/del" --ndel 10 --delchar "} ]" --nfiles 10 --input "yes/4.json"
../mutation/mutate.py --output "./no/del" --ndel 10 --delchar "} ]" --nfiles 10 --input "yes/5.json"
../mutation/mutate.py --output "./no/del" --ndel 10 --delchar "} ]" --nfiles 10 --input "yes/6.json"
../mutation/mutate.py --output "./no/del" --ndel 10 --delchar "} ]" --nfiles 10 --input "yes/7.json"
../mutation/mutate.py --output "./no/del" --ndel 10 --delchar "} ]" --nfiles 10 --input "yes/8.json"
../mutation/mutate.py --output "./no/del" --ndel 10 --delchar "} ]" --nfiles 10 --input "yes/9.json"

```
## Results
Executing the file `exec.maude`, it is possible to obtain the results for all
test cases. The summary is in [results.ods](results.ods) (also in [results.csv](results.csv)).  

