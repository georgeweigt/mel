[Real Programmers Don’t Use Fortran, Either!](https://georgeweigt.github.io/nather.pdf) by Ed Nather (Story of Mel)

#

Program `mel.c` runs Mel Kaye's blackjack program `bkjck.tx` by simulating an LGP-30 computer.
Note that `bkjck.tx` only runs on the LGP-30, not the RPC-4000.

To build and run

```
make
./a.out
```

The first column of output is player's cards, second column is dealer's cards.

```
               A - c          6 - d
               2 - c
Card?
```

Enter one of the following to be dealt a card.

```
yes
ok
si
ja
oui
```

Anything else means stand.

Press the return key 18 times to get a blackjack.
