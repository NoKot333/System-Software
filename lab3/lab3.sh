#!/bin/bash

awk '{if ($3 > 30)
 a=1
else
 a=0
if ($4 > 30)
 b=1
else
 b=0
if ($5>30)
 c=1
else
 c=0
sum = a+b+c
if (sum<2)
print $1
}' example.txt
