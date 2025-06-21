package shiftReg

import chisel3._
import chisel3.util._


class shiftReg(width: Int) extends Module{
  val io=IO(new Bundle {
    val opcode = Input(UInt(3.W))
    val inputNum=Input(SInt(width.W))
    val internalReg=Output(UInt(width.W))
  })

  val tmpReg=RegInit(0.U(width.W))
  io.internalReg:=tmpReg

  switch(io.opcode){
    is(0.U) { tmpReg:=0.U }
    is(1.U) { tmpReg:=io.inputNum.asUInt }
    is(2.U) {tmpReg:=tmpReg>>1}
    is(3.U) {tmpReg:=tmpReg<<1}
    is(4.U) { tmpReg:=tmpReg(width-1) ## tmpReg(width-1,1) }
    is(5.U) { tmpReg:= io.inputNum(0) ## tmpReg(width-1,1) }
    is(6.U) {tmpReg:= tmpReg(0) ## tmpReg(width-1,1)}
    is(7.U) { tmpReg:= tmpReg(width-2,0) ## tmpReg(width-1) }
  }
}