# Project 2 - Mailboxes

This project uses a system of semaphores and mailboxes to add up values. Each thread is passed several different values that will be added up and printed out at the end along with a total operation count and duration.

## What it does

- This program reads values from INPUT.txt, formatted in pairs of two where the first number is the value and the second is the thread.
  - [Input.txt](./INPUT.txt)

## Compiling

- Use the makefile provided, or just use the following command
  - `-gcc -Wall mailbox.c`
