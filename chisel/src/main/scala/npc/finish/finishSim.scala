package npc.finish

import chisel3._
import chisel3.util._

class finishSim extends BlackBox with HasBlackBoxResource {
  val io = IO(new Bundle {
    val clock = Input(Clock())
    val reset = Input(Bool())
    val finishStatus = Input(Bool())
  })
  addResource("/vsrc/finishSim.sv")
}
