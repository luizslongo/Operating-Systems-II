IMPLEMENTATION_DIR=../plasma_ml605

run:
	vsim < simulation_cmds.do

run_fakeram:
	make -f $(IMPLEMENTATION_DIR)/makefile coregen_generate
	vsim < simulation_cmds_fakeram.do
	
clean:
	rm -rf *.vcd *.lxt *.log transcript work *.wlf *~
