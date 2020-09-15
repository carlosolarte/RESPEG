#!/usr/bin/python3

''' This is a simple script to perform some mutations on a valid input string
from a grammar. Changes include insert, substitute and delete some symbols. '''

# Examples of use
# ./mutate.py --help
# ./mutate.py --output "." --input "anbncn.txt" --nins 1 --inschar "x,y,z" --ndel 2 --delchar "a,b,c" --nfiles 2 --nsubs 2 --sfrom "a,b" --sto "1,2" 

from random import randrange,choice, seed
import argparse
import os
import sys



class Conf:
    def __init__(self, nins, tins, ndel, tdel, nsubs, dsubs):
        ''' Parameters are:
            - nins: number of insertions
            - tins: list of characters that can be inserted
            - ndel: number of deletions
            - tdel: tokens that can be deleted
            - nsubs: number of substitutions
            - dsusb: dictionaly mapping symbols to a list of symbols that can 
                     be used for the substitution
        '''
        self.nins = nins
        self.tins = tins
        self.ndel = ndel
        self.tdel = tdel
        self.nsubs = nsubs
        self.dsubs = dsubs

def mutate(s, conf):
    seed()
    l = list(s)
    changes = 0
    #Insertions
    for i in range(conf.nins):
        pos = randrange(len(l))
        val = choice(conf.tins)
        print(f'Inserting {val} in position {pos}')
        l.insert(pos,val)
        changes += 1

    #deletions
    for i in range(conf.ndel):
        for j in range(5): #5 attempts
            v = choice(conf.tdel)
            pos = randrange(len(l))
            lch = l[pos+1:]
            if v in l:
                if v in lch:
                    lch.remove(v) 
                    l = l[:pos] + lch
                else:
                    l.remove(v)

                print(f'Removed {v}')
                changes += 1
                break

    #substitutions
    ll = len(l)
    lsubs = list(conf.dsubs.keys())
    for i in range(conf.nsubs):
        for j in range(5):
            pos = randrange(ll)
            l1,l2 = l[:pos] , l[pos+1:]
            v = choice(lsubs)
            nv = choice(conf.dsubs[v])
            if v in l:
                if v in l2:
                    idx = l2.index(v) 
                    l2[idx] = nv
                    l = l1 + l2 
                else:
                    idx = l.index(v)
                    l[idx] = nv

                changes +=1
                print(f'Substituted {v} with {nv}')
                break

    print(f'Total changes: {changes}')
    return ''.join(l)


def process(args):
    '''Reading the input file and producing output files with the configuration'''
    C = Conf(args.nins, args.inschar.split(' '), 
            args.ndel, args.delchar.split(' '), 
            args.nsubs, dict(zip(args.sfrom.split(" "), args.sto.split(" "))))
    with open(args.input) as f:
        s = f.read()

    bname = os.path.basename(args.input)
    fnbase = os.path.splitext(bname)[0] + "-"

    for i in range(args.nfiles):
        sm = mutate(s, C)
        fname = os.path.join(args.output, fnbase + f'{i:02}' + ".txt")
        with open(fname,'w') as f:
            f.write(sm)
            print(f'File {fname} created.')


parser=argparse.ArgumentParser( 
    description='''
        Mutating a string with insertions, deletions and substitutions
        ''' , formatter_class=argparse.RawTextHelpFormatter )

parser.add_argument('--output', type=str, default=".",
                        help='Directory where the modified files will be stored')


parser.add_argument('--input', type=str, 
                        help='Input file with the (correct) string')

parser.add_argument('--nins', type=int, default="0",
        help='Number of inserts')

parser.add_argument('--inschar', type=str, default="",
        help='Space separated list of characters that can be inserted')

parser.add_argument('--ndel', type=int, default="0",
        help='Number of deletes')

parser.add_argument('--delchar', type=str, default="",
        help='Space separated list of characters that can be deleted')

parser.add_argument('--nsubs', type=int, default="0",
        help='Number of substitutions')

parser.add_argument('--sfrom', type=str, default="",
        help='Space separated list of characters that can be substituted')

parser.add_argument('--sto', type=str, default="",
        help='Space separated list of characters after substitutions')

parser.add_argument('--nfiles', type=int, default="1",
        help='Number of outputs to be generated')

args=parser.parse_args()


process(args)

#python3 mutate.py --output "." --input "anbncn.txt" --nins 1 --inschar "x,y,z" --ndel 2 --delchar "a,b,c" --nfiles 2 --nsubs 2 --sfrom "a,b" --sto "1,2" 
