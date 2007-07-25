all:
	cat ../v/std.v | sed -e 's/^/\"/g' -e 's/$$/\\n\"/g' > std.h
	g++ -g *.cpp -o v

test:
	./v ../scripts/test.v
