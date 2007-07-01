# Simple regression tests for autobench, et al

RESDIR       = /home/jtjm/test/tmp
RESULTS_FILE = $(RESDIR)/results.tsv
PS_FILE      = $(RESDIR)/results.ps
NEW_FILE     = $(RESDIR)/newfile

.PHONY: crfile autobench bench2graph clean

test: clean crfile autobench bench2graph

crfile:
	crfile -s 102400 -w40 -f $(NEW_FILE)
	crfile -s 1024 -n -f $(NEW_FILE)2

autobench:
	autobench --single_host --file $(RESULTS_FILE)

bench2graph:
	bench2graph $(RESULTS_FILE) $(PS_FILE)

clean:
	rm -fr $(RESULTS_FILE) $(PS_FILE)
	rm -fr $(NEW_FILE)*
