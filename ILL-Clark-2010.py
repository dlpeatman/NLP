"""
file: IIL.py   naive implementation of Clark,Eyraud&Habrard'08 algorithm
E. Stabler  updated 2014-07-08 with time, comments, etc 
"""
import ckyCBFG # for parsing with hypothesis grammar
import cky     # for parsing with target grammar
import time    # for timing the learner

def add_nesubstrings(sofar,w):  # non-empty substrings of w added to list sofar
    max=len(w)
    for i in range(max):
        for j in range(max+1):
            if i<j:
                s=w[i:j]
                if not(s in sofar): # avoid redundancy
                    sofar.append(s)
    sofar.sort()

def add_all_nesubstrings(sofar,s): # non-empty substrings of strings in s added
    for w in s:
        add_nesubstrings(sofar,w)
    sofar.sort()

"""
x = []
add_all_nesubstrings(x,[['the','cat','smiles'],[0,1]])
print x
"""

def add_contexts(sofar,w):  # contexts of substrings of w added to list sofar
    max=len(w)
    for i in range(max):
        for j in range(max+1):
            if i<j:
                lc=w[0:i]
                rc=w[j:max]
                c=(lc,rc)
                if not(c in sofar): # avoid redundancy
                    sofar.append(c)
    sofar.sort()

def add_all_contexts(sofar,s): # contexts for every w in s
    for w in s:
        add_contexts(sofar,w)
    sofar.sort()

"""
x = []
add_contexts(x,['the','cat','smiles'])

x = []
add_contexts(x,['a','b','c'])
"""

def odot((lc,rc),string):
    product=lc[:]
    product.extend(string)
    product.extend(rc)
    return product

def odotCartesianProduct(C,S):
    product=[]
    for c in C:
        for s in S:
            new = odot(c,s)
            product.append(new)
    return product

def FL(F,w,target): # from CEH'08, Def.5 on p.5
    ok = []
    for c in F:
        cw = odot(c,w)
        accepted = cky.accepts(target,cw)
#        print 'For FL, oracle says ',accepted,':',cw
        if accepted:
            ok.append(c)
    return ok

def g(K,F,target): # from CEH'08, Def.7 on p.6
    PL=[]
    P=[]
    for w in K:
        lhs = FL(F,w,target) # oracle called for this
        if len(w)==1:
            rhs = w[0]
            rule = (lhs,rhs)
            if not(rule in PL): # avoid redundancy
                PL.append(rule)
        else:
            lenw=len(w)
            for i in range(lenw):
                if 0<i and i<lenw:
                    fl1=FL(F,w[0:i],target) # oracle called for this
                    fl2=FL(F,w[i:lenw],target) # oracle called for this
                    rule = (lhs,fl1,fl2)
                    if not(rule in P): # avoid redundancy
                        P.append(rule)
    return(PL,P)

# First conditional check
def wInDMinusLG(w,D,G):
    for s in D:
        if not(ckyCBFG.accepts(G,s)):
            return True
    return False

# Second conditional check
def reallyLongCond(S,K,C,F,G):
    for v in S:
        FLv = FL(F,v,G)
        for u in K:
            FLu = FL(F,u,G)
            FLsubset = ckyCBFG.subset(FLu,FLv)
            for f in C:
                fouInL = ckyCBFG.accepts(G,odot(f,u))
                fovInL = ckyCBFG.accepts(G,odot(f,v))
                if FLsubset and fouInL and not(fovInL):
                    return True
    return False

def iil(S,target):  # target grammar is for oracle computation only
    K = []
    D = []
    F = [([],[])]
    (PL,P) = g(K,F,target)
    for w in S:
        print 'processing input:', w
        D.append(w)
        #print 'adding contexts...'
        ConD = []
        add_all_contexts(ConD,D)
        C = ConD
        #print 'adding substrings...'
        SubD = []
        add_all_nesubstrings(SubD,D)
        S = SubD
        
        #print 'first conditional check...'
        if wInDMinusLG(w,D,(PL,P)):
            K = S
            F = C
        else:
            #print 'second conditional check...'
            if reallyLongCond(S,K,C,F,(PL,P)):
                F = C
        
        (PL,P) = g(K,F,target)
#        ckyCBFG.prettyGrammar((PL,P))
    return (PL,P) # = g(K,F,target)


""" for L = { aba }

sample0=[['a','b','a']]
PL0 = [('A','a'),('B','b')]
P0 = [('S','A','BA'),('BA','B','A')]
t0 = time.time()
(PL,P) = iil(sample0,(PL0,P0))
t1 = time.time()
print t1 - t0, "seconds"
print
ckyCBFG.prettyGrammar((PL,P))
print
for w in sample0:
    if ckyCBFG.ckyChart((PL,P),w):
        print "Accepted by learner's grammar :",w
    else:
        print "Rejected by learner's grammar :",w
"""

""" for {0^n1^n| n>0}

sample0 = [[0,1],[0,0,1,1],[0,0,0,1,1,1]]
PL0 = [('A',0),('B',1)]
P0 = [('S','A','B'),('S','A','R'),('R','S','B')]
#ckyCBFG.ckyChart((PL0,P0),[0,1])
t0 = time.time()
(PL,P) = iil(sample0,(PL0,P0))
t1 = time.time()
print t1 - t0, "seconds"
ckyCBFG.prettyGrammar((PL,P))
for w in sample0:
    if ckyCBFG.ckyChart((PL,P),w):
        print "Accepted by learner's grammar :",w
    else:
        print "Rejected by learner's grammar :",w
"""


""" from Koopman,Sportiche,Stabler ISAT chapter 3
"""
sample0 = [
    ['she','will','put','it','there','then'],
    ['she','will','put','this','one','there','then'],
    ['she','will','put','it','on','this','one','then'],
    ['this','one','will','put','it','there','then'],
    ['this','girl','in','the','red','coat','will','put','a','picture','of','Bill','on','your','desk','before','tomorrow'],
    ['she','will','put','a','picture','of','Bill','on','your','desk','before','tomorrow'],
    ['this','girl','in','the','red','coat','will','put','it','on','your','desk','before','tomorrow'],
    ['this','girl','in','the','red','coat','will','put','a','picture','of','Bill','there','before','tomorrow'],
    ['this','one','will','put','a','picture','of','Bill','on','your','desk','before','tomorrow'],
    ['this','girl','in','the','red','coat','will','put','a','picture','of','Bill','on','it','before','tomorrow'],
    ['this','girl','in','the','red','one','will','put','a','picture','of','Bill','on','your','desk','before','tomorrow']
    ]
PL0 = [('DP','she'),
       ('DP','it'),
       ('DP','Bill'),
       ('DP','tomorrow'),
       ('D','this'), 
       ('D','the'),
       ('D','a'),
       ('D','your'),
       ('A','red'),
       ('NP','one'),
       ('N','girl'),
       ('N','picture'),
       ('N','coat'),
       ('N','desk'),
       ('P','in'),
       ('P','of'),
       ('P','on'),
       ('P','before'),
       ('T','will'),
       ('V','put'),
       ('D','a'),
       ('P','of'),
       ('PP','there'),
       ('PP','then'),
       ]
P0 = [('S','DP','TP'),
      ('DP','D','NP'),
      ('DP','D','N'),
      ('NP','N','PP'),
      ('PP','P','DP'),
      ('N','A','N'),
      ('NP','A','NP'),
      ('TP','T','VP'),
      ('VP','VP','PP'),
      ('VP','V','V2P'),
      ('V2P','DP','PP')
      ]
print 'checking sample with target grammar...'

for w in sample0:
    if cky.accepts((PL0,P0),w):
        print "Accepted by target grammar :",w
    else:
        print "Rejected by target grammar :",w

t0 = time.time()
(PL,P) = iil(sample0,(PL0,P0))
t1 = time.time()
print t1 - t0, "seconds"

ckyCBFG.prettyGrammar((PL,P))

for w in sample0:
    if ckyCBFG.ckyChart((PL,P),w):
        print "Accepted by learner's grammar :",w
    else:
        print "Rejected by learner's grammar :",w
