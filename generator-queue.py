from collections import defaultdict
import random

# Here's an example grammar with actual words:
MyG = (
    (
        ('VT','killed',0.2),('VT','befriended',0.8),('V','laughed',0.6),\
        ('V','jumped',0.4),('D','a',0.25),('D','the',0.25),('D','every',0.25),\
        ('D','his',0.25),('N','dog',0.3),('N','alien',0.25),('N','car',.45),\
        ('NAME','Mary',0.5),('NAME','John',0.5)
    ),(
        ('S','DP','VP',1.0),('VP','V',0.5),('VP','VT','DP',0.5),('DP','NAME',0.3),\
        ('DP','D','N',0.7)
    ),
)

# Creates a dictionary of grammar rules
# Rules are put into lists and indexed by their left-hand-side item
# i.e.
#    VP -> V DP
#    VP -> V CP
# turns into:
#  'VP': 
def processGrammar(G):
    # lexical productions
    d = defaultdict(list)
    for tup in G[0]:
        d[tup[0]].append(tup[1:])
    for tup in G[1]:
        d[tup[0]].append(tup[1:])
    return d

def chooseRule(rules):
    # Determine what rule to use that starts with 'key'
    # Generate a random number
    # Iterate through the rules until the random number is in the range of the
    # rules probability
    # Probability range is (sum of previous rules' probs to sum plus current prob)
    prob = random.random()
    probSum = 0
    tup = ()
    for tup in rules:
        if prob >= probSum and prob < (probSum + tup[len(tup)-1]):
            break
        probSum += tup[len(tup)-1]
    return tup

def generateSentence(grammar, key):
    # Generate a grammar dictionary
    G = processGrammar(grammar)
    
    # Create a string to store the words of our sentence
    sentence = ''
    # Create a queue to hold the rules as we build our sentence tree depth-first
    keyQueue = [key]
    while True:
        # If the queue is empty, we're done
        if len(keyQueue) == 0:
            break
        
        # Get the first element in the queue
        # If it's not a key in the dictionary, it's a word -> add it to the sentence
        newKey = keyQueue.pop(0)
        if not newKey in G:
            sentence += newKey + ' '
            continue
        
        # Otherwise, choose a new rule for the key
        # Add all the new constituents of the rule to the queue
        newRule = chooseRule(G[newKey])
        for i in range(len(newRule)-1):
            keyQueue.insert(i,newRule[i])


    
    # Return the finished sentence
    return sentence