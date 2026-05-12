module wrapper(
	input clk,
	input reset,

	input [1:0] address, // p/ distinguir acesso a dado de entrada, leitura ou signals
	input write,
	input read,
	input [31:0] writedata,
	output reg [31:0] readdata,
	
	output reg [31:0] data_in,
	output reg enable,
	output reg rst,
	output reg clr,
	input [31:0] data_out
);
	
	always @(posedge clk) begin
		if (reset) begin
			data_in <= 32'b0;
			readdata <= 32'b0;
			enable <= 1'b0;
			rst <= 1'b0;
			clr <= 1'b0;
		end
		else begin
			if (write == 1 && address == 2'b00) begin
				data_in <= writedata;
			end 
			else if (read == 1 && address == 2'b10) begin
				readdata <= data_out;
			end
			else if (write == 1 && address == 2'b01) begin
				enable <= writedata[0];
				rst <= writedata[1];
				clr <= writedata[2];
			end
		end
	
	end

endmodule