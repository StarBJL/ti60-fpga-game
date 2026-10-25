`timescale 1ns/1ps
module hdmi_packets_tb;
  localparam [31:0] BASE=32'h01000000;
  reg clk=0,pix_clk=0,rst=1,axi_rst=1,pix_rst_n=0;
  always #2.5 clk=~clk;
  initial begin #0.731; forever #6.722689 pix_clk=~pix_clk; end
  reg [15:0] apb_paddr=0;
  reg apb_psel=0,apb_penable=0,apb_pwrite=0;
  reg [31:0] apb_pwdata=0;
  wire [31:0] apb_prdata;
  wire apb_pready,apb_pslverror,irq,axi_arvalid,axi_rready;
  wire [31:0] axi_araddr;
  wire [7:0] axi_arlen;
  wire [2:0] axi_arsize;
  wire [1:0] axi_arburst;
  wire [3:0] axi_arid;
  reg axi_arready=0,axi_rvalid=0,axi_rlast=0;
  reg [31:0] axi_rdata=0;
  reg audio_valid=0;
  reg [31:0] audio_data=0;
  integer audio_seq=0;
  reg feed_audio=1;
  wire aud_wready;
  always @(posedge clk) if(!rst&&audio_valid&&aud_wready) audio_seq=audio_seq+1;
  always @(negedge clk) begin
    audio_valid=!rst&&feed_audio&&aud_level<128;
    audio_data={16'((audio_seq*1001)^16'h5a5a),16'(audio_seq*37+11)};
  end
  wire [8:0] aud_level;
  wire [9:0] tmds_clk,tmds_d0,tmds_d1,tmds_d2;
  hdmi_fb_top #(.DUAL_MODE(1),.H_ACT(1280),.H_FP(110),.H_SYNC(40),.H_BP(220),
    .V_ACT(720),.V_FP(5),.V_SYNC(5),.V_BP(20),.HS_POL(1),.VS_POL(1),
    .FIFO_AW(11),.ACR_CTS(74375),.AVI_VIC(4),.AVI_PB2(8'h28)) dut(
    .clk(clk),.rst(rst),.axi_rst(axi_rst),.pix_clk(pix_clk),.pix_rst_n(pix_rst_n),
    .apb_paddr(apb_paddr),.apb_psel(apb_psel),.apb_penable(apb_penable),
    .apb_pwrite(apb_pwrite),.apb_pwdata(apb_pwdata),.apb_prdata(apb_prdata),
    .apb_pready(apb_pready),.apb_pslverror(apb_pslverror),.irq(irq),
    .axi_arvalid(axi_arvalid),.axi_arready(axi_arready),.axi_araddr(axi_araddr),
    .axi_arlen(axi_arlen),.axi_arsize(axi_arsize),.axi_arburst(axi_arburst),.axi_arid(axi_arid),
    .axi_rvalid(axi_rvalid),.axi_rready(axi_rready),.axi_rdata(axi_rdata),.axi_rlast(axi_rlast),
    .axi_rresp(2'd0),.axi_rid(4'd0),.aud_wvalid(audio_valid),.aud_wdata(audio_data),
    .aud_wready(aud_wready),.aud_level(aud_level),.tmds_clk(tmds_clk),
    .tmds_d0(tmds_d0),.tmds_d1(tmds_d1),.tmds_d2(tmds_d2));
  task apb_write(input [15:0] address,input [31:0] value);
    begin
      @(negedge clk);apb_paddr=address;apb_pwdata=value;apb_psel=1;apb_pwrite=1;
      @(negedge clk);apb_penable=1;
      @(negedge clk);apb_psel=0;apb_penable=0;apb_pwrite=0;
    end
  endtask
  function automatic [15:0] pixel(input integer x,y);
    pixel=(x*13)^(y*241)^16'h1357;
  endfunction
  function automatic [31:0] memory_word(input [31:0] addr);
    integer p;
    begin p=(addr-BASE)>>1;memory_word={pixel((p+1)%1280,(p+1)/1280),pixel(p%1280,p/1280)};end
  endfunction
  function automatic [23:0] rgb(input [15:0] p);
    rgb={p[15:11],p[15:13],p[10:5],p[10:9],p[4:0],p[4:2]};
  endfunction
  function automatic [7:0] tmds_decode(input [9:0] sym);
    reg [7:0] q,d;
    integer k;
    begin
      q=sym[9]?~sym[7:0]:sym[7:0];
      d[0]=q[0];
      for(k=1;k<8;k=k+1) d[k]=(q[k]^q[k-1])^!sym[8];
      tmds_decode=d;
    end
  endfunction
  reg [31:0] addresses[0:65535];
  integer due[0:65535];
  integer wp=0,rp=0,beat=0,syscycle=0,ar_count=0,r_count=0;
  reg [31:0] rng=32'h5a937213;
  always @(posedge clk) begin
    if(rst) begin wp=0;rp=0;beat=0;syscycle=0;ar_count=0;r_count=0;end
    else begin
      syscycle=syscycle+1;
      if(axi_arvalid&&axi_arready) begin
        if(axi_arlen!=63||axi_arsize!=2||axi_arburst!=1||axi_arid!=0) $fatal(1,"AXI burst shape");
        if(axi_araddr<BASE||axi_araddr+256>BASE+1843200) $fatal(1,"DMA exceeds fixed720 canvas: %h",axi_araddr);
        if((axi_araddr-BASE)%256!=0) $fatal(1,"unaligned burst");
        addresses[wp]=axi_araddr; due[wp]=syscycle+2+rng[7:4]; wp=wp+1; ar_count=ar_count+1;
      end
      if(axi_rvalid&&axi_rready) begin
        if(axi_rlast!=(beat==63)) $fatal(1,"RLAST");
        if(beat==63) begin rp=rp+1;beat=0;end else beat=beat+1;
        r_count=r_count+1;
      end
    end
  end
  always @(negedge clk) begin
    rng={rng[30:0],rng[31]^rng[21]^rng[1]^rng[0]};
    axi_arready=!rst&&(rng[2:0]!=0);
    axi_rvalid=!rst&&(rp<wp)&&(syscycle>=due[rp])&&(rng[6:4]!=0);
    axi_rdata=memory_word(addresses[rp]+beat*4);
    axi_rlast=(beat==63);
  end
  integer pixels=0,frames=0,mode_changes=0,avi720=0,avi1080=0;
  integer old_x,old_y,sx,sy,expected_count;
  reg old_mode=0,check_pixel;
  reg [23:0] expected_rgb;
  reg [23:0] tmds_expect0=0,tmds_expect1=0,tmds_expect2=0;
  reg tmds_check0=0,tmds_check1=0,tmds_check2=0;
  integer tmds_pixels=0;reg[31:0] frame_hash=32'h811c9dc5;
  integer sum,j;
  always @(posedge pix_clk) begin
    old_x=dut.h1;old_y=dut.v1;
    check_pixel=!dut.pix_rst&&dut.de1&&dut.frame_en;
    sx=dut.mode_1080?(2*old_x/3):old_x;
    sy=dut.mode_1080?(2*old_y/3):old_y;
    expected_rgb=rgb(pixel(sx,sy));
    if(!dut.pix_rst&&dut.u_island.lty==2) begin
      sum=8'h82+2+13;
      for(j=0;j<7;j=j+1) sum=sum+((dut.u_island.l_s0>>(j*8))&255);
      if((sum&255)!=0) $fatal(1,"AVI checksum");
      if(dut.u_island.l_s0[39:32]!=(dut.mode_1080?34:4)) $fatal(1,"AVI VIC");
      if(dut.mode_1080) avi1080=avi1080+1;else avi720=avi720+1;
    end
    #1;
    if(tmds_check2) begin
      if({tmds_decode(tmds_d2),tmds_decode(tmds_d1),tmds_decode(tmds_d0)}!==tmds_expect2)
        $fatal(1,"TMDS video decode differs from expected source pixel");
      tmds_pixels=tmds_pixels+1;
    end
    tmds_expect2=tmds_expect1;tmds_expect1=tmds_expect0;tmds_expect0=expected_rgb;
    tmds_check2=tmds_check1;tmds_check1=tmds_check0;tmds_check0=check_pixel;
    if(!dut.pix_rst&&old_mode!=dut.mode_1080) begin
      if(dut.de0||dut.h_cnt!=0||dut.v_cnt!=dut.out_vss) $fatal(1,"mode changed outside vsync boundary");
      mode_changes=mode_changes+1;
      $display("MODE_COMMIT %0d at %0t",dut.mode_1080,$time);
    end
    old_mode=dut.mode_1080;
    if(check_pixel) begin
      if(dut.uf1) $fatal(1,"unexpected video underrun");
      if({dut.r2,dut.g2,dut.b2}!==expected_rgb)
        $fatal(1,"pixel mode=%0d x=%0d y=%0d source=%0d,%0d got=%h expected=%h",dut.mode_1080,old_x,old_y,sx,sy,{dut.r2,dut.g2,dut.b2},expected_rgb);
      pixels=pixels+1;frame_hash=(frame_hash ^ expected_rgb)*32'h01000193;
      if(old_x==(dut.mode_1080?1919:1279)&&old_y==(dut.mode_1080?1079:719)) begin
        expected_count=dut.mode_1080?2073600:921600;
        if(pixels!=expected_count) $fatal(1,"frame pixel count %0d expected %0d",pixels,expected_count);
        frames=frames+1;pixels=0;
        $display("FRAME_HASH %h",frame_hash);frame_hash=32'h811c9dc5;
        $display("FRAME_PASS %0d mode=%0d pixels=%0d",frames,dut.mode_1080,expected_count);
      end
    end
  end
  integer tracefd,ph=0,cyclep=0,hh[0:4],vv[0:4],mm[0:4],pp[0:4],ii;
  initial begin
    tracefd=$fopen("tmds_blank.txt","w");
    for(ii=0;ii<5;ii=ii+1) begin hh[ii]=-1;vv[ii]=-1;mm[ii]=0;pp[ii]=0;end
  end
  always @(posedge pix_clk) begin
    for(integer k=4;k>0;k=k-1) begin hh[k]=hh[k-1];vv[k]=vv[k-1];mm[k]=mm[k-1];pp[k]=pp[k-1];end
    hh[0]=dut.pix_rst?-1:dut.h_cnt;vv[0]=dut.v_cnt;mm[0]=dut.mode_1080;pp[0]=ph;
    cyclep=cyclep+1;
    #1;
    if(hh[4]>=0&&(hh[4]>=(mm[4]?1920:1280)||vv[4]>=(mm[4]?1080:720)))
      $fwrite(tracefd,"%0d %0d %0d %0d %0d %h %h %h\n",cyclep,mm[4],pp[4],hh[4],vv[4],tmds_d0,tmds_d1,tmds_d2);
  end
  initial begin
    repeat(20) @(negedge clk); rst=0;axi_rst=0;pix_rst_n=1;
    apb_write(0,1);
    // Two FIFO-fed complete frames at each mode, then two 1 kHz tone frames.
    wait(frames==2);apb_write('h50,1);
    wait(dut.mode_1080);wait(frames==4);apb_write('h50,0);
    wait(!dut.mode_1080);wait(frames==6);
    ph=1;feed_audio=0;apb_write('h20,'h37);
    wait(frames==8);apb_write('h50,1);
    wait(dut.mode_1080);wait(frames==10);
    // Finish after the InfoFrames in vertical blanking.
    wait(dut.v_cnt==1095&&dut.h_cnt==100);
    repeat(10) @(negedge clk);
    if(dut.uf_cnt!=0||avi720==0||avi1080==0) $fatal(1,"UF/AVI coverage");
    $fclose(tracefd);
    $display("PACKET_TRACE_PASS: frames=%0d mode_changes=%0d TMDS pixels=%0d audio_sent=%0d audio_UF=%0d",frames,mode_changes,tmds_pixels,dut.aud_sent,dut.aud_uf);
    $finish;
  end
  initial begin #400000000; $fatal(1,"watchdog"); end
endmodule
