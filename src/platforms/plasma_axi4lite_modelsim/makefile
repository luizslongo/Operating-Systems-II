IMPLEMENTATION_DIR=../plasma_axi4lite_ml605

run:
	make -f $(IMPLEMENTATION_DIR)/makefile coregen_generate
	cp ../../mach/common/coregen/ram_amba_128k.mif .
	vsim < simulation_cmds.do
	
run_dec:
	make -f $(IMPLEMENTATION_DIR)/makefile coregen_generate
	cp ../../mach/common/coregen/ram_amba_128k.mif .
	vsim < simulation_cmds_dec.do

clean:
	rm -rf *.vcd *.lxt *.log transcript work *.wlf *.mif *~
