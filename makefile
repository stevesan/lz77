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
	./alz s mahi.txt mahi.lzslow
	./alz c mahi.txt mahi.lz
	diff mahi.lz mahi.lzslow
	./alz d mahi.lz mahi.dec.txt
	diff mahi.txt mahi.dec.txt

test2 : alz
	echo "mahi mahi mahifd fd" > mahi.txt
	./alz s mahi.txt mahi.lzslow
	./alz c mahi.txt mahi.lz
	diff mahi.lz mahi.lzslow
	./alz d mahi.lz mahi.dec.txt
	diff mahi.txt mahi.dec.txt

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
	./alz c config.sub config.sub.comp
	./alz s config.sub config.sub.compslow
#diff config.sub.comp config.sub.compslow
	./alz d config.sub.comp config.sub.dec
	diff config.sub config.sub.dec

test_heavy :
	time ./alz c displace.bin displace.bin.comp
	time ./alz d displace.bin.comp displace.bin.dec
	diff displace.bin displace.bin.dec

all : alz test_suffix
