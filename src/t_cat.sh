#!/bin/bash

COUNTER_SUCCESS=0
COUNTER_FAIL=0
DIFF_RES=""
echo "" > log.txt

for var in -n -b -s -e -t -e -t
do
  for var2 in n b s e t e t
  do
        if [ $var != -$var2 ] 
        then
          TEST1="$var$var2 cat/Makefile cat/test1.txt cat/textfile.txt cat/cat.c"
          echo "$TEST1"
          cat/./s21_cat $TEST1 > s21_cat.txt
          cat $TEST1 > cat.txt
          DIFF_RES="$(diff -s s21_cat.txt cat.txt)"
          if [ "$DIFF_RES" == "Files s21_cat.txt and cat.txt are identical" ]
            then
              (( COUNTER_SUCCESS++ ))
            else
              echo "FAILED TEST $TEST1" >> log.txt
              (( COUNTER_FAIL++ ))
          fi
          rm s21_cat.txt cat.txt

          TEST3="$var$var2 cat/cat.c cat/Makefile cat/textfile.txt "
          echo "$TEST3"
          cat/./s21_cat $TEST3 > s21_cat.txt
          cat $TEST3 > cat.txt
          DIFF_RES="$(diff -s s21_cat.txt cat.txt)"
          if [ "$DIFF_RES" == "Files s21_cat.txt and cat.txt are identical" ]
            then
              (( COUNTER_SUCCESS++ ))
            else
              echo "$TEST3" >> log.txt
              (( COUNTER_FAIL++ ))
          fi
          rm s21_cat.txt cat.txt

          TEST4="$var$var2 cat/cat.c"
          echo "$TEST4"
          cat/./s21_cat $TEST4 > s21_cat.txt
          cat $TEST4 > cat.txt
          DIFF_RES="$(diff -s s21_cat.txt cat.txt)"
          if [ "$DIFF_RES" == "Files s21_cat.txt and cat.txt are identical" ]
            then
              (( COUNTER_SUCCESS++ ))
            else
              echo "$TEST4" >> log.txt
              (( COUNTER_FAIL++ ))
          fi
          rm s21_cat.txt cat.txt

        fi
  done
done

echo "SUCCESS: $COUNTER_SUCCESS"
echo "FAIL: $COUNTER_FAIL"