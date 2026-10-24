"""Instruction-count probe of the shipped RV32I hot functions.

Executes actual ELF instructions, checks final states/Tile counts/commands
against an independent reference. No cache, DDR, pipeline or cycle model;
instruction reductions cannot be interpreted as FPGA FPS improvements.
"""
from pathlib import Path
import hashlib,json,struct,subprocess
ROOT=Path(__file__).resolve().parents[1]
OUT=ROOT/'verification/r4_frontend'
NM=Path(r'D:\Efinity\efinity-riscv-ide-2026.1\toolchain\bin\riscv-none-elf-nm.exe')
MASK=0xffffffff
def signed(v,bits=32):
    return (v&((1<<(bits-1))-1))-(v&(1<<(bits-1)))
class Probe:
    def __init__(self,path):
        elf=path.read_bytes();assert elf[:7]==b'\x7fELF\x01\x01\x01'
        self.mem=bytearray(32*1024*1024)
        phoff=struct.unpack_from('<I',elf,28)[0]
        phsize,phnum=struct.unpack_from('<HH',elf,42)
        for i in range(phnum):
            kind,off,addr,_,size,_,_,_=struct.unpack_from('<8I',elf,phoff+i*phsize)
            if kind==1:self.mem[addr:addr+size]=elf[off:off+size]
        lines=subprocess.check_output([str(NM),'-n',str(path)],text=True).splitlines()
        self.sym={p[2]:int(p[0],16) for line in lines if len(p:=line.split())==3}
        self.decoded={}
    def put(self,a,v,n=4):self.mem[a:a+n]=(v&((1<<(8*n))-1)).to_bytes(n,'little')
    def get(self,a,n=4):return int.from_bytes(self.mem[a:a+n],'little')
    def call(self,name,args):
        r=[0]*32;r[1]=0xfffffff0;r[2]=0x00ff0000;r[3]=self.sym['__global_pointer$']
        r[10:10+len(args)]=args;pc=self.sym[name];steps=loads=stores=0
        while pc!=0xfffffff0:
            if pc not in self.decoded:
                ins=self.get(pc);assert ins&3==3,hex(pc)
                op=ins&127;rd=(ins>>7)&31;f=(ins>>12)&7;a=(ins>>15)&31;b=(ins>>20)&31;f7=ins>>25
                imm=signed(ins>>20,12)
                if op==0x23:imm=signed(((ins>>25)<<5)|((ins>>7)&31),12)
                elif op==0x63:imm=signed(((ins>>31)<<12)|(((ins>>7)&1)<<11)|(((ins>>25)&63)<<5)|(((ins>>8)&15)<<1),13)
                elif op==0x6f:imm=signed(((ins>>31)<<20)|(((ins>>12)&255)<<12)|(((ins>>20)&1)<<11)|(((ins>>21)&1023)<<1),21)
                elif op in (0x37,0x17):imm=ins&0xfffff000
                self.decoded[pc]=(op,rd,f,a,b,f7,imm)
            op,rd,f,a,b,f7,imm=self.decoded[pc];x=r[a];y=r[b];nxt=pc+4;value=None
            if op==0x37:value=imm
            elif op==0x17:value=pc+imm
            elif op==0x6f:value=nxt;nxt=pc+imm
            elif op==0x67:value=nxt;nxt=(x+imm)&~1
            elif op==0x63:
                take={0:lambda:x==y,1:lambda:x!=y,4:lambda:signed(x)<signed(y),5:lambda:signed(x)>=signed(y),6:lambda:x<y,7:lambda:x>=y}[f]()
                if take:nxt=pc+imm
            elif op==0x03:
                width={0:1,1:2,2:4,4:1,5:2}[f];value=self.get((x+imm)&MASK,width);loads+=1
                if f<4:value=signed(value,width*8)
            elif op==0x23:
                self.put((x+imm)&MASK,y,{0:1,1:2,2:4}[f]);stores+=1
            elif op==0x13:
                if f==0:value=x+imm
                elif f==1:value=x<<(b&31)
                elif f==2:value=int(signed(x)<imm)
                elif f==3:value=int(x<(imm&MASK))
                elif f==4:value=x^(imm&MASK)
                elif f==5:value=(signed(x) if f7==32 else x)>>(b&31)
                elif f==6:value=x|(imm&MASK)
                elif f==7:value=x&(imm&MASK)
            elif op==0x33:
                assert f7 in (0,1,32),('unsupported instruction',hex(pc))
                if f7==1:
                    if f==0:value=x*y
                    elif f==1:value=(signed(x)*signed(y))>>32
                    elif f==2:value=(signed(x)*y)>>32
                    elif f==3:value=(x*y)>>32
                    elif f in (4,6):
                        sx,sy=signed(x),signed(y)
                        if sy==0:value=-1 if f==4 else sx
                        else:
                            quotient=abs(sx)//abs(sy)
                            if (sx<0)!=(sy<0):quotient=-quotient
                            value=quotient if f==4 else sx-quotient*sy
                    elif f==5:value=x//y if y else MASK
                    else:value=x%y if y else x
                elif f==0:value=x-y if f7==32 else x+y
                elif f==1:value=x<<(y&31)
                elif f==2:value=int(signed(x)<signed(y))
                elif f==3:value=int(x<y)
                elif f==4:value=x^y
                elif f==5:value=(signed(x) if f7==32 else x)>>(y&31)
                elif f==6:value=x|y
                elif f==7:value=x&y
            elif op==0x0f:pass  # fence / VexRiscv D-cache invalidate: count, no cache timing model
            else:raise AssertionError((hex(pc),hex(op)))
            if value is not None and rd:r[rd]=value&MASK
            pc=nxt&MASK;steps+=1
            assert steps<10000000,'runaway'
        return {'instructions':steps,'loads':loads,'stores':stores,'return':r[10]}
