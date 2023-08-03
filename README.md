[The Story of Mel](https://georgeweigt.github.io/nather.pdf)

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

```
               A - c          4 - d
              l0 - s
Card? 
                              9 - d
                              K - s
               blackjack      bust           score = -$3.00
```
