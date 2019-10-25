import random
import sys

# Create alphabet
alphabet = list("abcdefghijklmnopqrstuvwxyz")

def randSubstitutionCipher(inputFileName, outputFileName, preserveCaps=True, preservePunctuation=True):
    # Make a copy of the alphabet and scramble it
    cipherAlphabet = alphabet[:]
    random.shuffle(cipherAlphabet)

    # Open all relevant files
    inFile = open(inputFileName, 'r')
    outFile = open(outputFileName, 'w')
    cipherAlphabetFile = open('cipherAlphabet.txt', 'w')

    # initialize varaibles
    outputString = ""

    # Swap the characters of the input string with the cipher alphabet
    for plainChar in inFile.readline():
        if plainChar in alphabet:
            # Set this character to the correct index of the cipher alphabet
            cipherChar = cipherAlphabet[alphabet.index(plainChar.lower())]
            # Make capital if the input is capital (bad cryptography, but fun)
            if plainChar.isupper() & preserveCaps:
                cipherChar = cipherChar.upper()
        # Preserve punctuation
        elif preservePunctuation:
            cipherChar = plainChar
        else:
            cipherChar = ""
        # Append to output string
        outputString += cipherChar

    outFile.write(outputString)
    cipherAlphabetFile.write("".join(cipherAlphabet))

def encodeSubstitutionCipher(inputFileName, outputFileName, cipherAlphabet, preserveCaps=True, preservePunctuation=True):
    # Open all relevant files
    inFile = open(inputFileName, 'r')
    outFile = open(outputFileName, 'w')
    cipherAlphabetFile = open('cipherAlphabet.txt', 'w')

    # initialize varaibles
    outputString = ""

    # Swap the characters of the input string with the cipher alphabet
    for plainChar in inFile.readline():
        if plainChar in alphabet:
            # Set this character to the correct index of the cipher alphabet
            cipherChar = cipherAlphabet[alphabet.index(plainChar.lower())]
            # Make capital if the input is capital (bad cryptography, but fun)
            if plainChar.isupper() & preserveCaps:
                cipherChar = cipherChar.upper()
        # Preserve punctuation
        elif preservePunctuation:
            cipherChar = plainChar
        else:
            cipherChar = ""
        # Append to output string
        outputString += cipherChar

    outFile.write(outputString)
    cipherAlphabetFile.write("".join(cipherAlphabet))

def decodeSubstitutionCipher(inputFileName, outputFileName, cipherAlphabet):
    # Open all relevant files
    inFile = open(inputFileName, 'r')
    outFile = open(outputFileName, 'w')

    # initialize varaibles
    outputString = ""

    # Swap the characters of the input string with the cipher alphabet
    for cipherChar in inFile.readline():
        if cipherChar in alphabet:
            # Set this character to the correct index of the cipher alphabet
            plainChar = alphabet[cipherAlphabet.index(cipherChar.lower())]
            # Make capital if the input is capital (bad cryptography, but fun)
            if cipherChar.isupper():
                plainChar = plainChar.upper()
        # Preserve punctuation
        else:
            plainChar = cipherChar
        # Append to output string
        outputString += plainChar

    outFile.write(outputString)

# Test script
randSubstitutionCipher('plaintext.txt', 'ciphertext.txt')
cipherAlphabet = open('cipherAlphabet.txt').read()
decodeSubstitutionCipher('ciphertext.txt', 'decoded plaintext.txt', cipherAlphabet)
