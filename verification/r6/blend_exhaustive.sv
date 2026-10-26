module test;
reg clk=0;always #5 clk=~clk;
reg [15:0] s,d;reg [8:0] a;reg add;wire [15:0] y;
blit_blend565 dut(.clk(clk),.s(s),.d(d),.a(a),.add(add),.y(y));
integer si,di,ai,ad,n=0,r,g,b,rr,gg,bb;
initial begin
 for(ad=0;ad<2;ad=ad+1)for(ai=0;ai<=256;ai=ai+1)for(si=0;si<64;si=si+1)for(di=0;di<64;di=di+1)begin
  @(negedge clk);s={si[4:0],si[5:0],~si[4:0]};d={di[4:0],di[5:0],~di[4:0]};a=ai;add=ad;
  r=(s[15:11]*ai+d[15:11]*(ad?256:256-ai)+128)>>8;
  g=(s[10:5]*ai+d[10:5]*(ad?256:256-ai)+128)>>8;
  b=(s[4:0]*ai+d[4:0]*(ad?256:256-ai)+128)>>8;
  rr=r>31?31:r;gg=g>63?63:g;bb=b>31?31:b;
  @(posedge clk);#1;if(y!==((rr<<11)|(gg<<5)|bb))$fatal(1,"blend mismatch s=%h d=%h a=%d add=%d got=%h",s,d,a,add,y);n=n+1;
 end
 $display("PASS %0d blend/add input combinations; every channel pair and Alpha 0..256",n);$finish;
end
endmodule
