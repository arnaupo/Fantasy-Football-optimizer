all: execute_programs

execute_programs:
	@read -p "bench?: " input && \
	echo && \
	echo EXH &&\
	./exh data_base.txt public_benchs/$$input.txt sol.txt && \
	cat sol.txt && \
	echo && \
	echo GREEDY &&\
	./greedy data_base.txt public_benchs/$$input.txt sol.txt && \
	cat sol.txt && \
	echo && \
	echo MH &&\
	./mh data_base.txt public_benchs/$$input.txt sol.txt && \
	cat sol.txt
