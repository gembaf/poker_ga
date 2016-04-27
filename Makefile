compile_strategy:
	gcc -O3 -Ilib -oPokerExec -DTYPE=05-05 -DCHNG=5 -DTAKE=5 lib/CardLib.c lib/PokerExec.c PokerOpe.c -Wno-unused-result

run:
	./PokerExec _hoge 10000 Stock.ini 0

compile_stock:
	gcc -Ilib -oStockInit lib/CardLib.c lib/StockInit.c

create_stock:
	./StockInit 10000 > Stock.ini

