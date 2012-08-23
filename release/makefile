alz : alz.cpp *.hpp
	g++ alz.cpp -o alz

test_bitwriter : test_bitwriter.cpp *.hpp
	g++ $< -o $@

test_suffix : test_suffix.cpp *.hpp
	g++ $< -o $@

TESTSTR = "mahi mahi"
test : alz
	echo $(TESTSTR) > test.txt
	./alz c test.txt test.lz
	./alz s test.txt test.lzslow
	./alz d test.lz test.txt.dec
	diff test.txt test.txt.dec
	ls -s test.lz test.lzslow

all : alz 
