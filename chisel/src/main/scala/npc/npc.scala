package npc

import chisel3._
import chisel3.util._
import chisel3.experimental._
import npc.regGroup.regGroup
import multALU.multALU
import npc.finish.finishSim

package object riscv32Types {
  val word_len=32
}
import riscv32Types._

class npc extends Module{

  def immI(inst:UInt):SInt=inst(31,20).asSInt
  def immS(inst:UInt):SInt=Cat(inst(31,25),inst(11,7)).asSInt
  def immB(inst:UInt):SInt=Cat(inst(31),inst(7),inst(30,25),inst(11,8),0.U(1.W)).asSInt
  def immU(inst:UInt):SInt=Cat(inst(31,12),0.U(12.W)).asSInt
  def immJ(inst:UInt):SInt=Cat(inst(31),inst(19,12),inst(20),inst(30,21),0.U(1.W)).asSInt

  def hasFunct3(opcode:UInt):Bool= !(opcode==="b1101111".U || opcode==="b0110111".U || opcode==="b0010111".U)
  def hasFunct7(opcode: UInt, funct3: UInt): Bool = opcode==="b1110011".U || opcode === "b0110011".U || (opcode === "b0010011".U && (funct3 === "b001".U || funct3 === "b101".U))

  val io=IO(new Bundle {

    val readAddr=Output(UInt(word_len.W))
    val readData=Input(UInt(word_len.W))

    val writeAddr=Output(UInt(word_len.W))
    val writeData=Output(UInt(word_len.W))
    val writeEnable=Output(Bool())

    val pc=Output(UInt(word_len.W))
    val inst=Input(UInt(word_len.W))
  })

  val readAddrReg=RegInit(0.U(word_len.W))
  val pcReg=RegInit("h80000000".U(word_len.W))
  val gpr=Module(new regGroup())
  val finishSim=Module(new finishSim)
  finishSim.io.clock:=clock
  finishSim.io.reset:=reset
  finishSim.io.finishStatus:=0.U

  // 提取指令相关部分
  val opcode=io.inst(6,0)
  val funct3=Mux(hasFunct3(opcode),io.inst(14,13),0.U(3.W))
  val funct7=Mux(hasFunct7(opcode,funct3),io.inst(31,25),0.U(7.W))

  // 输入rd、rs1、rs2,重置写入状态和写入数据
  gpr.io.rd:=io.inst(11,7)
  gpr.io.writeEnable:=false.B
  gpr.io.writeData:=0.U
  gpr.io.rs1:=io.inst(19,15)
  gpr.io.rs2:=io.inst(24,20)
  io.writeEnable:=false.B

  val src1=gpr.io.rs1Data
  val src2=gpr.io.rs2Data

  // 提取立即数
  val imm=MuxCase(0.S,Seq(
    (opcode === "b0010011".U) -> immI(io.inst),
    (opcode === "b0000011".U) -> immI(io.inst),
    (opcode === "b0100011".U) -> immS(io.inst),
    (opcode === "b1100011".U) -> immB(io.inst),
    (opcode === "b0110111".U) -> immU(io.inst),
    (opcode === "b1101111".U) -> immJ(io.inst)
  ))

  // 根据opcode、funct3和funct7执行相应操作(已做掩码处理)
  switch(Cat(opcode,funct3,funct7)){
    is("b00100110000000000".U){
      gpr.io.writeData:=(src1.asSInt+imm).asUInt
      gpr.io.writeEnable:=true.B
    }
    is("b11100110000000000".U){
      finishSim.io.finishStatus:=1.U
    }
  }

  val writeAddrReg=RegInit(0.U(word_len.W))
  val writeDataReg=RegInit(0.U(word_len.W))

  io.readAddr:=readAddrReg
  io.writeAddr:=writeAddrReg
  io.writeData:=writeDataReg
  io.pc:=pcReg

  pcReg:=pcReg+4.U
  printf(p"pc = 0x${Hexadecimal(io.pc)} inst = 0x${Hexadecimal(io.inst)}\n")
  printf(p"writeTarget = ${gpr.io.rd} writeData = 0x${Hexadecimal(gpr.io.writeData)} writeEnable = ${gpr.io.writeEnable}\n")
}
