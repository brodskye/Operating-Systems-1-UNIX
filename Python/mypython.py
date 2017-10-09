#!/usr/bin/env python

#Name: David Jansen
#Date: 7/26/2017
#Class: CS 344
#Assignment: Program Py- Python
#Description: This program writes 10 random lowercase letters to 3 different text file, it then outputs those letters to the user.  This program also generates 2 random numbers, multiplies them and prints both the numbers and their product.

#Import string for the letters.
import string
#Import random to use pythons random functions.
import random

#open my first file and write 10 random letters to it using a for loop and a function that picks randomly out of the string.letters.  I had to set it to only go from 0 to 26 because string.letters also holds uppercase letters.
f= open("Anaconda.txt", "w+")
for i in range(10):
	f.write(random.choice(string.letters[0:26]))
f.write("\n")
f.close()

#Open the file again and write out its contents, I also remove the newline because .read() adds one.
f= open("Anaconda.txt", "r+")
a = f.read().strip()
print a
f.close()


#open my second file and write 10 random letters to it using a for loop and a function that picks randomly out of the string.letters.  I had to set it to only go from 0 to 26 because string.letters also holds uppercase letters.
f= open("Cobra.txt", "w+")
for i in range(10):
	f.write(random.choice(string.letters[0:26]))
f.write("\n")
f.close()

#Open the file again and write out its contents, I also remove the newline because .read() adds one.
f= open("Cobra.txt", "r+")
b = f.read().strip()
print b
f.close()


#open my third file and write 10 random letters to it using a for loop and a function that picks randomly out of the string.letters.  I had to set it to only go from 0 to 26 because string.letters also holds uppercase letters.
f= open("Viper.txt", "w+")
for i in range(10):
	f.write(random.choice(string.letters[0:26]))
f.write("\n")
f.close()

#Open the file again and write out its contents, I also remove the newline because .read() adds one.
f= open("Viper.txt", "r+")
c = f.read().strip()
print c
f.close()


#Get two random numbers
num1 = random.randint(1, 42)
num2 = random.randint(1, 42)

#print the random numbers and their product.
print(num1) 
print(num2)
print(num1*num2)
