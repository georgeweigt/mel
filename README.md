[The Story of Mel](https://georgeweigt.github.io/nather.pdf)

[LGP-30 Documentation](https://georgeweigt.github.io/LGP-30/index.html)

#

Program `mel.c` runs Mel Kaye's blackjack program `bkjck.tx` by simulating an LGP-30 computer.

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
