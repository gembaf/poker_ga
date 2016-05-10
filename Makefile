CHNG = 6
TAKE = 6
TRY = 10000

VISIBLE = 0 # true = 1, false = 0

run: PokerExec Deck.txt
	./PokerExec $(TRY) Deck.txt $(VISIBLE)

ga: PokerExecGA Deck.txt
	./PokerExecGA $(TRY) Deck.txt $(VISIBLE)

PokerExec: lib/CardLib.c lib/PokerExec.c lib/Poker.c PokerOpe.c
	gcc -O3 -Ilib -oPokerExec -DCHNG=$(CHNG) -DTAKE=$(TAKE) lib/CardLib.c lib/Poker.c lib/PokerExec.c PokerOpe.c -Wno-unused-result

PokerExecGA: lib/CardLib.c lib/PokerExecGA.c lib/Poker.c PokerOpeGA.c
	gcc -O3 -Ilib -oPokerExecGA -DCHNG=$(CHNG) -DTAKE=$(TAKE) lib/CardLib.c lib/Poker.c lib/PokerExecGA.c PokerOpeGA.c -Wno-unused-result

Deck.txt: DeckInit
	./DeckInit $(TRY) > Deck.txt

DeckInit: lib/CardLib.c lib/DeckInit.c
	gcc -Ilib -oDeckInit lib/CardLib.c lib/DeckInit.c

clean:
	rm -rf PokerExec PokerExecGA DeckInit Deck.txt

