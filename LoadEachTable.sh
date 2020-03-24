#!/bin/sh
make a1test.out
for i in 1 2 3 4 5 6 7; do
  printf "1\n${i}\n" | ./a1test.out
done
