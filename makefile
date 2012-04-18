alz : alz.cpp
	g++ alz.cpp -o alz

test_bitwriter : test_bitwriter.cpp *.hpp
	g++ $< -o $@
tests : test_bitwriter
