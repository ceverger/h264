parser1:

	g++ -g parser1.cpp files.cpp byte.cpp h264.cpp -o parser1

parser2:

	g++ -g parser2.cpp files.cpp byte.cpp h264.cpp -o parser2

parser3:

	g++ -g parser3.cpp files.cpp byte.cpp h264.cpp -o parser3

packer:

	g++ -g packer.cpp files.cpp byte.cpp h264.cpp -o packer

unpacker:

	g++ -g unpacker.cpp files.cpp byte.cpp h264.cpp -o unpacker

