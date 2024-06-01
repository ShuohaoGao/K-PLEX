all : 
	g++ -std=c++11 -O3 -g -w main.cpp -o kPEX  -DNO_PROGRESS_BAR -DNDEBUG

seq_kPEX:
	g++ -std=c++11 -O3 -g -w main.cpp -o seq_kPEX  -DNO_PROGRESS_BAR -DNDEBUG -DSeqRB

degen_kPEX:
	g++ -std=c++11 -O3 -g -w main.cpp -o degen_kPEX  -DNO_PROGRESS_BAR -DNDEBUG -Denable_CTCP -DNO_STRONG_HEURIS -DNO_SQRT

ego_kPEX:
	g++ -std=c++11 -O3 -g -w main.cpp -o ego_kPEX  -DNO_PROGRESS_BAR -DNDEBUG -DEGO -Denable_CTCP

clean:
	rm kPEX seq_kPEX degen_kPEX ctcp_kPEX