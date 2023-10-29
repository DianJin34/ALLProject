from cmath import log
from lib2to3.pytree import LeafPattern
from posixpath import split
import sys
import math

def get_parameter_vectors():
    '''
    This function parses e.txt and s.txt to get the  26-dimensional multinomial
    parameter vector (characters probabilities of English and Spanish) as
    descibed in section 1.2 of the writeup

    Returns: tuple of vectors e and s
    '''
    #Implementing vectors e,s as lists (arrays) of length 26
    #with p[0] being the probability of 'A' and so on
    e=[0]*26
    s=[0]*26

    with open('e.txt',encoding='utf-8') as f:
        for line in f:
            #strip: removes the newline character
            #split: split the string on space character
            char,prob=line.strip().split(" ")
            #ord('E') gives the ASCII (integer) value of character 'E'
            #we then subtract it from 'A' to give array index
            #This way 'A' gets index 0 and 'Z' gets index 25.
            e[ord(char)-ord('A')]=float(prob)
    f.close()

    with open('s.txt',encoding='utf-8') as f:
        for line in f:
            char,prob=line.strip().split(" ")
            s[ord(char)-ord('A')]=float(prob)
    f.close()

    return (e,s)

def shred(filename):
    #Using a dictionary here. You may change this to any data structure of
    #your choice such as lists (X=[]) etc. for the assignment
    X=dict()
    with open (filename,encoding='utf-8') as f:
        # TODO: add your code here
        content = f.read().upper()
        for i in range(ord('A'), ord("Z")+1):
            X[chr(i)] = content.count(chr(i))
    return X



# TODO: add your code here for the assignment
# You are free to implement it as you wish!
# Happy Coding!
text = shred("samples/letter3.txt")
print("Q1")
for i in range(ord('A'), ord("Z")+1):
    print(chr(i) + ' ' + str(text[chr(i)]))

print("Q2")
Eng = get_parameter_vectors()[0]
Span = get_parameter_vectors()[1]
A = text["A"]
print("%.4f"%(A*math.log(Eng[0])))
print("%.4f"%(A*math.log(Span[0])))

print("Q3")
F_Eng = math.log(0.6)
F_Span = math.log(0.4)
Letter = list(text.keys())
for i in range(26):
    F_Eng += text[Letter[i]]*math.log(Eng[i])
    F_Span += text[Letter[i]]*math.log(Span[i])
print("%.4f"%(F_Eng))
print("%.4f"%(F_Span))

print("Q4")
if F_Span - F_Eng >= 100:
    print("0.0000")
elif F_Span - F_Eng <= -100:
    print("1.0000")
else:
    print("%.4f"%(1/(1+math.e**(F_Span - F_Eng))))