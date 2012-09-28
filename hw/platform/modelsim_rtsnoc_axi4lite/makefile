
run:
	cp ../../mach/common/coregen/ram_amba_128k.mif .
	vsim < modelsim_rtsnoc_axi4lite.do

run_max:	
	cp ../../mach/common/coregen/ram_amba_128k.mif .
	sudo ionice -c1 -n0 nice -n -20 vsim < modelsim_rtsnoc_axi4lite.do
	
clean:
	rm -rf *.vcd *.lxt *.log transcript work *.wlf *.mif *~
