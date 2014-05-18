from collections import defaultdict
import random

# Here's an example grammar with actual words:
PL0 = [('VT','killed',0.2),('VT','befriended',0.8),('V','laughed',0.6),\
        ('V','jumped',0.4),('D','a',0.25),('D','the',0.25),('D','every',0.25),\
        ('D','his',0.25),('N','dog',0.3),('N','alien',0.25),('N','car',.45),\
        ('NAME','Mary',0.5),('NAME','John',0.5)]
P0 = [('S','DP','VP',1.0),('VP','V',0.5),('VP','VT','DP',0.5),('DP','NAME',0.3),\
        ('DP','D','N',0.7)]
"""
Returns a default dictionary given a grammar
For example, ('S','A','R') in input turns into a key value pair with 
'S' as key and a tuple of ('A','R') as value
"""
def processGrammar(G):
    # lexical productions
    d = defaultdict(list)
    for tup in G[0]:
        d[tup[0]].append(tup[1:])
    for tup in G[1]:
        d[tup[0]].append(tup[1:])
    return d


def generateSentence(grammar, key):
    # Base case: We hit a word/the termination of the grammar, just return it
    if not key in grammar:
        return key + ' '
    
    # Determine what rule to use that starts with 'key'
    # Generate a random number
    # Iterate through the rules until the random number is in the range of the
    # rules probability
    # Probability range is (sum of previous rules' probs to sum plus current prob)
    prob = random.random()
    probSum = 0
    tup = ()
    for tup in grammar[key]:
        if prob >= probSum and prob < (probSum + tup[len(tup)-1]):
            break
        probSum += tup[len(tup)-1]
    
    # Now that we've chosen a random rule (weighted by its probability),
    # We recurse on each element of the rule, adding the result to sentence
    sentence = ""
    for i in range(0, len(tup)-1):
        sentence += str(generateSentence(grammar, tup[i]))
    
    # Return the finished sentence (a concatenation of all recursive calls)
    return sentence