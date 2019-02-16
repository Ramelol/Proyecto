

module i2s_wb_if #(
	parameter WB_AW = 32,
	parameter WB_DW = 32
)(
	input			rst,
	input			wb_clk,

	input [WB_AW-1:0]	wb_adr_i,
	input [WB_DW-1:0]	wb_dat_i,
	input [WB_DW/8-1:0]	wb_sel_i,
	input			wb_we_i ,
	input			wb_cyc_i,
	input			wb_stb_i,
	input [2:0]		wb_cti_i,
	input [1:0]		wb_bte_i,
	output [WB_DW-1:0]	wb_dat_o,
	output reg		wb_ack_o,
	output			wb_err_o,
	output			wb_rty_o,

	output reg [WB_DW-1:0]	prescaler
);

assign wb_err_o = 0;
assign wb_rty_o = 0;
assign wb_dat_o = wb_adr_i[5:2] == 0 ? prescaler : 0;

always @(posedge wb_clk) begin
	if (wb_ack_o & wb_we_i) begin
		case (wb_adr_i[5:2])
			0: prescaler <= wb_dat_i;
		endcase
	end

	if (rst)
		prescaler <= 0;
end

always @(posedge wb_clk)
	if (rst)
		wb_ack_o <= 0;
	else
		wb_ack_o <= wb_cyc_i & wb_stb_i & !wb_ack_o;

endmodule
