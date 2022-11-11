# Assignment 2

## Goals of this assignment

This is an opportunity for you to use -- and perhaps wrestle with --
concurrency in an imperative programming language such as C. Some of
the ideas you may use here for solving the problems of cooperating
and competing threads/tasks can be used in other languages such as Java.

1. Write a program (`kosmos-sem.c`) that solves the **Kosmos-ethynyl-radical**
problem with the use of semaphores.

2. Write a program (`kosmos-mcv.c`) that solves the **Kosmos-ethynyl-radical**
problem with the use of mutexes and condition variables.

---

## The **Kosmos-ethynyl radical** problem

You are hired by an interstellar civilzation of much greater intelligence that
ours (!!) who are seeding the universe with the building blocks needed for a
one of their major projects. The specific task you have agreed to help solve
for them is to manage the chemical reaction needed to form **ethynyl radicals**
(which is made up of two carbon atoms and one hydrogen atom).

Despite the civilization's greater intelligence, they are having trouble getting
the carbon and hydrogen atoms to combine correctly due to some serious synchronization
problems. They are able to create each atom (i.e., one atom equals one thread), and so 
the challenge is to get two carbon threads and one hydrogen thread together at the
same time, regardless of the number or order of thread creation.

Each carbon atom invokes a function named `c_ready()` when it is ready to react, and
each hydrogen atom invokes a function named `h_ready()` when it is ready to react.
For this problem you are to complete the code for `c_ready` and `h_ready`, adding 
whatever code and data structures are needed to solve the problem.

Here are some other requirements for solutions:

* Skeleton files that will help you get started on the goal are available in
  this repository.

* The `h_ready` and `c_ready` functions must only delay until there are at least
  two carbon and one hydrogen atoms ready to combine into the radical.

* **You are not permitted** to wait until all threads / atoms are created such
  that you then match up atoms once you know what exists. This means your
  solution must work when the next atom/thread created is a hydrogen atom when
  there exist already two carbon atoms, or when the next atom/thread created is a
  carbon atom when there exist already one carbon atom and one hydrogen atoms.

* When an arriving atom/thread determines that a radical can be made, then that
  atom/thread must cause the radical to be made to indicate the identities of the
  carbon atoms and the identiy of the hydrogen atom are reported. Below is an
  example showing the format of the report, where each line indicates the atoms
  in a radical, and the atom/thread in parentheses was the one that initiated the
  creation of the radical. For example, below the 7th radical consists of carbon
  atoms 10 and 14 and hydrogen atom 7, and it was the hydrogen atom that triggered
  the creation of the radical.

```
001: c002 c001 h001 (c002)
002: c004 c009 h002 (h002)
003: c005 c006 h003 (h003)
004: c007 c008 h004 (h004)
005: c003 c013 h005 (h005)
006: c011 c012 h006 (h006)
007: c010 c014 h007 (h007)
008: c016 c015 h008 (c016)
009: c018 c017 h009 (c018)
010: c020 c019 h010 (c020)
```

* Because there are several layers of scheduling involved with POSIX threads
and the Linux thread library, you will be unable to reason correctly about the
fine details of program behavior by looking at `printf` statements. Therefore
all reports of radical creation are actually written to an internal log, and
that log is output at the end of the program. You have been provided with the
log routines (in `logging.[ch]`), which include the use of a mutex to ensure
there are no race conditions when multiple radicals are formed at the same
time. Put a little bit more dramatically, debugging `printf` statements will
lead you badly astray as you may try to intrepret them to indicate a certain
order of instruction operation (such as "before this point" or "after this
point"), but the truth will often be very very different.

---

## Part 1: `kosmos-sem.c`

For this part, you are to solve the problem using only POSIX semaphores. You are **not** 
permitted in this part to use mutexes or condition variables or any other items from 
the pthreads library in this part of the assignment.

Code that randomly creates each atom thread has been provided for you. Please carefully 
read the file as it clearly indicates parts of the program you are *permitted* to modify,
and other portions that you are *not permitted* to modify.

You are provided with a `makefile` in order to help with building executable
versions of your program. **If you wish to re-factor the overall structure of
the program, you must first get express written permission from David or Mike.**


## Part 2: `kosmos-mcv.c`

For this part, you are to solve the problem using only POSIX mutexes and
condition variables. You are **not** permitted in this part to use semaphores
or any other items from the pthreads library.

Code that randomly creates each atom thread has been provided for you. Please carefully 
read the file as it clearly indicates parts of the program you are *permitted* to modify,
and other portions that you are *not permitted* to modify.

You are provided with a `makefile` in order to help with building executable
versions of your program. **If you wish to re-factor the overall structure of
the program, you must first get express written permission from David or Mike.**

---

## A word about starvation

You will notice when you write a solution that the pthread scheduling of
threads is a little unpredictable. Some atoms/threads created later in the
program will appear in radicals before atoms that were created earlier. You may
even find that some atoms created earlier do not even appear in a radical. This
is fine -- and is perhaps an artifact of this problem being somewhat contrived.
If the program we were writing involved a long-running server process, then
atoms/threads that are never combined even though they have existed for a long
time would be considred an error and the behavior quite alarming. However, for
this assignment we will ignore this problem. Put differently, your solutions are
**not** required to have atoms combine in strict numeric order. (In fact, if
this is what we see in your solution, we will suspect some hanky-panky in your
code, which might even result in a failing grade for A#2.)

&copy; 2022 David Clark, Mike Zastre
