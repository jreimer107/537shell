module PSA_16bit(Sum, Error, A, B);
  input [15:0] A, B;
  output [15:0] Sum;
  output Error;

  wire E0, E1, E2, E3;

  addsub_4bit adder3(Sum[15:12], E3, A[15:12], B[15:12], 0);
  addsub_4bit adder2(Sum[12:8], E3, A[12:8], B[12:8], 0);
  addsub_4bit adder1(Sum[7:4], E3, A[7:4], B[7:4], 0);
  addsub_4bit adder0(Sum[3:0], E3, A[3:0], B[3:0], 0);

  assign Error = E0 + E1 + E2 + E3;
endmodule

module addsub_4bit (Sum, Ovfl, A, B, sub);
	input [3:0] A, B;
	input sub;
	output [3:0] Sum;
	output Ovfl;

	wire C1, C2, C3, C4;
	wire [3:0] B_real;

	assign B_real = sub ? ~B : B;
	assign Ovfl = (A[3] ^~ B_real[3]) & (A[3] ^ Sum[3]);

	full_adder_1bit FA0(.A(A[0]), .B(B_real[0]), .Cin(sub), .Cout(C1), .S(Sum[0]));
	full_adder_1bit FA1(.A(A[1]), .B(B_real[1]), .Cin(C1), .Cout(C2), .S(Sum[1]));
	full_adder_1bit FA2(.A(A[2]), .B(B_real[2]), .Cin(C2), .Cout(C3), .S(Sum[2]));
	full_adder_1bit FA3(.A(A[3]), .B(B_real[3]), .Cin(C3), .Cout(C4), .S(Sum[3]));
endmodule

module full_adder_1bit (A, B, Cin, Cout, S);
	input A, B, Cin;
	output Cout, S;

	wire S1, C1, C2;

	xor (S1, A, B);
	xor (S, S1, Cin);
	and (C1, S1, Cin);
	and (C2, A, B);
	or (Cout, C1, C2);
endmodule

module Shifter (Shift_Out, Shift_In, Shift_Val, Mode);
  input [15:0] Shift_In; //This is the number to perform shift operation on
  input [3:0] Shift_Val; //Shift amount (used to shift the ‘Shift_In’)
  input  Mode; // To indicate SLL or SRA
  output [15:0] Shift_Out; //Shifter value

  wire [15:0] shift0, shift1, shift2, shift3;
  assign shift0 = Shift_Val[0] ? (!Mode ? {Shift_In[15], Shift_In[15:1]} : {Shift_In[14:0], 1'b0}) : Shift_In;
  assign shift1 = Shift_Val[1] ? (!Mode ? {{2{shift0[15]}}, shift0[15:2]} : {shift0[13:0], 2'b0}) : shift0;
  assign shift2 = Shift_Val[2] ? (!Mode ? {{4{shift0[15]}}, shift1[15:4]} : {shift1[11:0], 4'b0}) : shift1;
  assign Shift_Out = Shift_Val[3] ? (!Mode ? {{8{shift0[15]}}, shift2[15:8]} : {shift2[7:0],  8'b0}) : shift2;

endmodule


module PSA_tb();
  reg [15:0] A, B;
  wire [15:0] S;
  wire Error;
  integer testSum;

  PSA_16bit DUT(.A(A), .B(B), .Error(Error), .Sum(S));

  initial begin
    repeat(100) begin
      A = %random;
      B = %random;
      testSum = A + B;

      if (testS > 15'h7FFF || testS < 15'8000) begin
        if (!Error) begin
          $display("Should be overflow: S %d", testSum);
          $stop();
        end
      end
      else begin
        if (testS[15:0] != S) begin
          $display("Incorrect sum: Expected %d, actual %d", testSum, S);
          $stop();
        end
      end
    end
endmodule

module shifter_tb();
  reg [15:0] in;
  reg [3:0] val;
  reg mode;
  wire [15:0] out;

  reg testOut;

  Shifter DUT(.Shift_In(in), .Shift_Out(out), .Shift_Val(val), .Mode(mode));

  initial begin
    repeat(100) begin
      in = %random;
      val = %random;
      mode = %random;

      if (mode) testOut = in << val;
      else testOut = in >>> val;

      if (testOut != out) begin
        $display("Incorrect result.");
        $stop();
      end
    end
  end
endmodule
