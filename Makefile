# file: Makefile
# author: Ted Baker

SOL_DIRS=sol0 sol1 sol2 sol3
OTHERS=testone submitP2.sh common originals results

default:
	./testone sol0
	./testone sol1
	./testone sol2
	./testone sol3
clean:
	rm -rf *# *~ results/*
	for DIR in $(SOL_DIRS); do cd $${DIR}; make -s clean; cd ..; done;
archive: clean
	tar czvpf P2.tgz Makefile $(SOL_DIRS) $(OTHERS)
	chmod 644 P2.tgz				 


