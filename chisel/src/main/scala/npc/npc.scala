package npc

import chisel3._
import chisel3.util._
import chisel3.experimental._
import npc.ALU.multALU
import npc.dataMemory.dataMemory
import npc.regGroup._
import npc.decoder._
import npc.immExtender.immExtender
import npc.finish.finishSim
import riscv32Types._

class npc extends Module{
//  def hasFunct3(opcode:UInt):Bool= !(opcode==="b1101111".U || opcode==="b0110111".U || opcode==="b0010111".U)
//  def hasFunct7(opcode: UInt, funct3: UInt): Bool = opcode==="b1110011".U || opcode === "b0110011".U || (opcode === "b0010011".U && (funct3 === "b001".U || funct3 === "b101".U))

  val io=IO(new Bundle {

    val readAddr=Output(UInt(word_len.W))
    val readType=Output(UInt(1.W))
    val readEnable=Output(Bool())
    val readData=Input(UInt(word_len.W))
    val readReady=Input(Bool())

    val writeAddr=Output(UInt(word_len.W))
    val writeData=Output(UInt(word_len.W))
    val writeEnable=Output(Bool())

    val byteNum=Output(UInt(3.W))

    val pc=Output(UInt(word_len.W))
    val inst=Input(UInt(word_len.W))

    val quitState=Output(Bool())
  })


  // 声明/初始化相关模块
//  val readAddrReg=RegInit(0.U(word_len.W))
  val pcReg=RegInit("h80000000".U(word_len.W))
  val gpr=Module(new regGroup())
  val csr=Module(new csrGroup())
  val mem=Module(new dataMemory())
  val decoder=Module(new decoder())
  val systemDecoder=Module(new systemDecoder())
  val mainALU=Module(new multALU(32))
  val pcALU=Module(new multALU(32))
  val immExtender=Module(new immExtender())
  val finishSim=Module(new finishSim())

  // 提取指令相关部分
  val opcode=io.inst(6,0)
  val funct3=io.inst(14,12)
  val funct7=io.inst(31,25)

  // 输入rd、rs1、rs2,重置写入状态和写入数据
  gpr.io.rd:=io.inst(11,7)
  gpr.io.writeEnable:=false.B
  gpr.io.rs1:=io.inst(19,15)
  gpr.io.rs2:=io.inst(24,20)
  io.writeEnable:=false.B

  val src1=gpr.io.rs1Data
  val src2=gpr.io.rs2Data

  // 初始化解码器
  decoder.io.opcode:=opcode
  decoder.io.funct3:=funct3
  decoder.io.funct7:=funct7

  // 初始化立即数提取器
  immExtender.io.inst:=io.inst
  immExtender.io.immType:=decoder.io.immType
  val imm=immExtender.io.immSrc

  // 初始化system类型指令解码器
  systemDecoder.io.funct3:=funct3
  systemDecoder.io.rs1:=io.inst(19,15)
  systemDecoder.io.rd:=io.inst(11,7)
  systemDecoder.io.imm:=imm

  // 连接主解码器与system解码器
  decoder.io.systemDecoderResult:=systemDecoder.io.csrCrtl

  // 初始化主加法器
  mainALU.io.a:=src1.asSInt
  mainALU.io.b:=Mux(decoder.io.aluSrc,imm,src2.asSInt)
  mainALU.io.opcode:=decoder.io.aluOp
  mainALU.io.cin:=0.U

  // 连接主加法器与解码器
  decoder.io.zero:=mainALU.io.zero
  decoder.io.compareResult:=mainALU.io.out(0)

  // 初始化pc加法器
  pcALU.io.a:=Mux(decoder.io.pcAluSrc,pcReg.asSInt,src1.asSInt)
  pcALU.io.b:=imm
  pcALU.io.cin:=0.U
  pcALU.io.opcode:=typeAdd.U

  // 初始化内存
  mem.io.targetAddr:=mainALU.io.out.asUInt
  mem.io.funct3:=funct3
  mem.io.rs2:=src2
  mem.io.memWrite:=decoder.io.memWrite
  mem.io.memRead:=decoder.io.memRead

  io.readAddr:=mem.io.readAddr
  io.readType:=mem.io.readType
  io.readEnable:=mem.io.readEnable
  io.writeAddr:=mem.io.writeAddr
  io.writeData:=mem.io.writeData
  io.writeEnable:=mem.io.writeEnable
  io.byteNum:=mem.io.byteNum

  // 初始化CSR寄存器
  csr.io.csr:=imm(11,0).asUInt
  csr.io.opcode:=decoder.io.systemInstCtrl.csrOpcode
  csr.io.ecallIF:=decoder.io.systemInstCtrl.ecallIF
  csr.io.mretIF:=decoder.io.systemInstCtrl.mretIF
  csr.io.src1:=src1
  csr.io.currentPC:=pcReg
  csr.io.writeEnable:=decoder.io.systemInstCtrl.csrWrite

  // 寄存器写入
  gpr.io.writeEnable:=Mux(!io.readReady && decoder.io.memRead, false.B, decoder.io.regWrite)
  gpr.io.writeData:=MuxCase(0.U,Seq(
    (decoder.io.regWriteType===typeAluResult.U) -> mainALU.io.out.asUInt,
    (decoder.io.regWriteType===typeMemRead.U) -> io.readData,
    (decoder.io.regWriteType===typeNextPC.U) -> (pcReg+4.U),
    (decoder.io.regWriteType===typeImmU.U) -> imm.asUInt,
    (decoder.io.regWriteType===typePCTarget.U) -> pcALU.io.out.asUInt,
    (decoder.io.regWriteType===typeCsrRead.U) -> csr.io.csrData
  ))

  // pc更新
  // 如果一直在尝试请求并且没有数据返回则一直不更新pc
  when(io.readReady || !decoder.io.memRead){
    io.readEnable:=false.B
    pcReg:=MuxCase(pcReg+4.U,Seq(
      (decoder.io.pcNextType===typePCNext.U) -> (pcReg+4.U),
      (decoder.io.pcNextType===typePCAluResult.U) -> pcALU.io.out.asUInt,
      (decoder.io.pcNextType===typeCsrResult.U) -> csr.io.csrData
    ))
  }
  io.pc:=pcReg

  // 连接终止模块
  finishSim.io.clock:=clock
  finishSim.io.reset:=reset
  finishSim.io.finishStatus:=decoder.io.systemInstCtrl.ebreakIF

  // 连接退出状态
  io.quitState:=gpr.io.a0State

/*  printf(p"\npc = 0x${Hexadecimal(io.pc)} inst = 0x${Hexadecimal(io.inst)}\n")
  printf(p"gprWriteTarget = ${gpr.io.rd} gprWriteData = 0x${Hexadecimal(gpr.io.writeData)} gprWriteEnable = ${gpr.io.writeEnable}\n")
  printf(p"regWriteType = ${decoder.io.regWriteType}, aluOp = ${decoder.io.aluOp}, aluSrc = ${decoder.io.aluSrc}, mainALU.out = 0x${Hexadecimal(mainALU.io.out.asUInt)}\n")
  printf(p"mainALU.a = 0x${Hexadecimal(mainALU.io.a)}, mainALU.b = 0x${Hexadecimal(mainALU.io.b)}\n")
  printf(p"immType = ${decoder.io.immType}, imm = 0x${Hexadecimal(imm.asUInt)}, pcALU.out = 0x${Hexadecimal(pcALU.io.out.asUInt)}\n")
  printf(p"memReadEnable = ${io.readEnable}, readReady = ${io.readReady}, memWriteEnable = ${decoder.io.memWrite}, readAddr = 0x${Hexadecimal(io.readAddr)}, readData = 0x${Hexadecimal(io.readData)}\n")
  printf(p"pcNextType = ${decoder.io.pcNextType}, pcNext = 0x${Hexadecimal(pcReg)}\n")
  printf(p"csrWrite = ${decoder.io.systemInstCtrl.csrWrite}, csrRead = ${decoder.io.systemInstCtrl.csrRead}, mretIF = ${systemDecoder.io.csrCrtl.mretIF}\n")*/
}
