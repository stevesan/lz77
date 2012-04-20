alz : alz.cpp *.hpp
	g++ alz.cpp -o alz

test_bitwriter : test_bitwriter.cpp *.hpp
	g++ $< -o $@

test_suffix : test_suffix.cpp *.hpp
	g++ $< -o $@

test_hashmap : test_hashmap.cpp
	g++ $< -o $@

tests : test_bitwriter

test1 : alz
	echo "mahi mahi" > mahi.txt
	./alz c mahi.txt mahi.lz
	./alz d mahi.lz mahi.dec.txt
	diff mahi.txt mahi.dec.txt

test2short : alz
	echo "mahi mahi" > mahi.txt
	./alz s mahi.txt mahi.s
	./alz c mahi.txt mahi.c
	./alz d mahi.c mahi.d
	diff mahi.txt mahi.d
	ls -l mahi.s mahi.c

test2 : alz
	echo "mahi mahi mahifd fd" > mahi.txt
	./alz s mahi.txt mahi.s
	./alz c mahi.txt mahi.c
	./alz d mahi.c mahi.d
	diff mahi.txt mahi.d
	ls -l mahi.s mahi.c

test3 : alz
	echo "mahifd fd" > test3.txt
	./alz c test3.txt test3.lz
	./alz d test3.lz test3.dec.txt
	diff test3.txt test3.dec.txt

test4 : alz
	echo "abababababab" > mahi.txt
	./alz c mahi.txt mahi.lz
	./alz d mahi.lz mahi.dec.txt
	diff mahi.txt mahi.dec.txt

TESTSTR = "mahi mahi"
test :
	echo $(TESTSTR) > test.txt
	./alz c test.txt test.lz
	./alz d test.lz test.dec.txt
	diff test.txt test.dec.txt

test_mild :
	./alz c config.sub config.sub.c
	./alz s config.sub config.sub.s
	./alz d config.sub.c config.sub.d
	diff config.sub config.sub.d
	ls -l config.sub.s config.sub.c

test_heavy :
	./alz c work/displace.bin work/displace.bin.c
	./alz d work/displace.bin.c work/displace.bin.d
	diff work/displace.bin work/displace.bin.d

all : alz test_suffix
