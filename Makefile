LOG = _tmp
CHNG = 6
TAKE = 6
TRY = 10000
VISIBLE = 0 # true = 1, false = 0

run: PokerExec Stock.ini
	./PokerExec $(LOG) $(TRY) Stock.ini $(VISIBLE)

PokerExec: lib/CardLib.c lib/PokerExec.c PokerOpe.c
	gcc -O3 -Ilib -oPokerExec -DCHNG=$(CHNG) -DTAKE=$(TAKE) lib/CardLib.c lib/PokerExec.c PokerOpe.c -Wno-unused-result

Stock.ini: StockInit
	./StockInit $(TRY) > Stock.ini

StockInit: lib/CardLib.c lib/StockInit.c
	gcc -Ilib -oStockInit lib/CardLib.c lib/StockInit.c

clean:
	rm -rf PokerExec StockInit Stock.ini

