package LFSR

import chisel3._
import chisel3.util._
import barrelShift.barrelShift

class LFSR extends Module {
  val io = IO(new Bundle {
    val inputNum = Input(UInt(8.W))
    val outputNum = Output(UInt(255.W))
  })

  val tmpNum = RegInit(0.U(8.W))
  val tmpReg = RegInit(0.U(255.W))
  val firstLoad = RegInit(false.B)

  when (!firstLoad) {
    tmpNum := io.inputNum
    tmpReg := 0.U
    firstLoad := true.B
  } .otherwise {
    val result = tmpNum(0) ^ tmpNum(2) ^ tmpNum(3) ^ tmpNum(4)
    tmpNum := Cat(result, tmpNum(7, 1))
    tmpReg := Cat(result, tmpReg(254, 1))
  }

  io.outputNum := tmpReg
}
