#!/usr/bin/env python
#-*- coding: utf8 -*-
from math import pi, sin, cos
from os import linesep
from sys import argv

ls = linesep
blockSep = ls + ls


def getK(frequency):
    return int( 0.5 + float(frameSize * frequency)/sampleRate )

def getW(k):
    return ((2.0 * pi)/frameSize) * k

def cArrayDefinition(sequence, type_, name):
    size = str(len(sequence))
    body = ", ".join(str(e) for e in sequence)
    
    definition = "static " + type_ + " " + name + "[" + size + "] = {" + ls
    definition += body
    definition += ls + "};"

    return definition



frequencies = [697, 770, 852, 941, 1209, 1336, 1477, 1633]

if __name__ == "__main__":
    sampleRate = int(argv[1])
    frameSize = int(argv[2])


    ks = [getK(f) for f in frequencies]
    ws = [getW(k) for k in ks]
    sines = [sin(w) for w in ws]
    cosines = [cos(w) for w in ws]


    definitions = [ \
            cArrayDefinition(ks, "int", "ks"), \
            cArrayDefinition(ws, "float", "ws"), \
            cArrayDefinition(sines, "float", "sines"), \
            cArrayDefinition(cosines, "float", "cosines") \
        ]

    print blockSep.join(definitions)

