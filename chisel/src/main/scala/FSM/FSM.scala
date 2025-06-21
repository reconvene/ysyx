package FSM

import chisel3._
import chisel3.util._
import chisel3.util.experimental.decode._

class simpleMooreFSM extends Module{
  val io=IO(new Bundle {
    val in=Input(Bits(1.W))
    val out=Output(UInt(1.W))
  })

  // 声明初始化寄存器
  val initialState=RegInit(0.U(5.W))
  // 声明状态寄存器
  val stateReg=RegInit(0.U(4.W))
  // 声明摩尔寄存器
  val mooreStateReg=RegNext(stateReg,0.U(4.W))

  // 初始化寄存器每次往后填1
  initialState:= initialState(3,0) ##  1.U
  // 状态寄存器每次往右推入被输入的二进制
  stateReg:= io.in ## stateReg(3,1)

  // 初始化完成并且摩尔寄存器全为0或1时输出为1
  io.out:=Mux(initialState.andR && (mooreStateReg.andR || !mooreStateReg.orR), 1.U,0.U)
  printf("stateReg: %b\n", stateReg.asUInt)
  printf("mooreStateReg: %b\n", mooreStateReg.asUInt)
  printf("out: %b\n", io.out.asUInt)

}