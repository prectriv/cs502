# Project 2 - Mailboxes

This project uses a system of semaphores and mailboxes to add up values. Each thread is passed several different values that will be added up and printed out at the end along with a total operation count and duration.

## What it does

- This program reads values from the command line, formatted in pairs of two where the first number is the value and the second is the thread.
- use the EOF command, ctrl + D to signify you're complete.

## Compiling
 Use the makefile provided, or just use the following command
> `-gcc -Wall mailbox.c`

## Running
> ./mailbox <threadcount> [nb]
