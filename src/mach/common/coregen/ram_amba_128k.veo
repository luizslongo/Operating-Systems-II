/*******************************************************************************
*     This file is owned and controlled by Xilinx and must be used             *
*     solely for design, simulation, implementation and creation of            *
*     design files limited to Xilinx devices or technologies. Use              *
*     with non-Xilinx devices or technologies is expressly prohibited          *
*     and immediately terminates your license.                                 *
*                                                                              *
*     XILINX IS PROVIDING THIS DESIGN, CODE, OR INFORMATION "AS IS"            *
*     SOLELY FOR USE IN DEVELOPING PROGRAMS AND SOLUTIONS FOR                  *
*     XILINX DEVICES.  BY PROVIDING THIS DESIGN, CODE, OR INFORMATION          *
*     AS ONE POSSIBLE IMPLEMENTATION OF THIS FEATURE, APPLICATION              *
*     OR STANDARD, XILINX IS MAKING NO REPRESENTATION THAT THIS                *
*     IMPLEMENTATION IS FREE FROM ANY CLAIMS OF INFRINGEMENT,                  *
*     AND YOU ARE RESPONSIBLE FOR OBTAINING ANY RIGHTS YOU MAY REQUIRE         *
*     FOR YOUR IMPLEMENTATION.  XILINX EXPRESSLY DISCLAIMS ANY                 *
*     WARRANTY WHATSOEVER WITH RESPECT TO THE ADEQUACY OF THE                  *
*     IMPLEMENTATION, INCLUDING BUT NOT LIMITED TO ANY WARRANTIES OR           *
*     REPRESENTATIONS THAT THIS IMPLEMENTATION IS FREE FROM CLAIMS OF          *
*     INFRINGEMENT, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS          *
*     FOR A PARTICULAR PURPOSE.                                                *
*                                                                              *
*     Xilinx products are not intended for use in life support                 *
*     appliances, devices, or systems. Use in such applications are            *
*     expressly prohibited.                                                    *
*                                                                              *
*     (c) Copyright 1995-2011 Xilinx, Inc.                                     *
*     All rights reserved.                                                     *
*******************************************************************************/

// The Xilinx LogiCORE IP Block Memory Generator replaces the Dual Port Block Memory and Single Port Block Memory LogiCOREs, but is not a direct drop-in replacement.  It should be used in all new Xilinx designs. The core supports RAM and ROM functions over a wide range of widths and depths. Use this core to generate block memories with symmetric or asymmetric read and write port widths, as well as cores which can perform simultaneous write operations to separate locations, and simultaneous read operations from the same location. For more information on differences in interface and feature support between this core and the Dual Port Block Memory and Single Port Block Memory LogiCOREs, please consult the data sheet.

// Interfaces:
//    AXI4_SLAVE_S_AXI
//    AXILite_SLAVE_S_AXI

// The following must be inserted into your Verilog file for this
// core to be instantiated. Change the instance name and port connections
// (in parentheses) to your own signal names.

//----------- Begin Cut here for INSTANTIATION Template ---// INST_TAG
ram_amba_128k your_instance_name (
  .s_aclk(s_aclk), // input s_aclk
  .s_aresetn(s_aresetn), // input s_aresetn
  .s_axi_awaddr(s_axi_awaddr), // input [31 : 0] s_axi_awaddr
  .s_axi_awvalid(s_axi_awvalid), // input s_axi_awvalid
  .s_axi_awready(s_axi_awready), // output s_axi_awready
  .s_axi_wdata(s_axi_wdata), // input [31 : 0] s_axi_wdata
  .s_axi_wstrb(s_axi_wstrb), // input [3 : 0] s_axi_wstrb
  .s_axi_wvalid(s_axi_wvalid), // input s_axi_wvalid
  .s_axi_wready(s_axi_wready), // output s_axi_wready
  .s_axi_bresp(s_axi_bresp), // output [1 : 0] s_axi_bresp
  .s_axi_bvalid(s_axi_bvalid), // output s_axi_bvalid
  .s_axi_bready(s_axi_bready), // input s_axi_bready
  .s_axi_araddr(s_axi_araddr), // input [31 : 0] s_axi_araddr
  .s_axi_arvalid(s_axi_arvalid), // input s_axi_arvalid
  .s_axi_arready(s_axi_arready), // output s_axi_arready
  .s_axi_rdata(s_axi_rdata), // output [31 : 0] s_axi_rdata
  .s_axi_rresp(s_axi_rresp), // output [1 : 0] s_axi_rresp
  .s_axi_rvalid(s_axi_rvalid), // output s_axi_rvalid
  .s_axi_rready(s_axi_rready) // input s_axi_rready
);
// INST_TAG_END ------ End INSTANTIATION Template ---------

// You must compile the wrapper file ram_amba_128k.v when simulating
// the core, ram_amba_128k. When compiling the wrapper file, be sure to
// reference the XilinxCoreLib Verilog simulation library. For detailed
// instructions, please refer to the "CORE Generator Help".

