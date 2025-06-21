// See README.md for license details.

package shiftReg

import chisel3._
import chisel3.simulator.EphemeralSimulator._
import org.scalatest.freespec.AnyFreeSpec
import org.scalatest.matchers.must.Matchers

class shiftRegSpec extends AnyFreeSpec with Matchers {
  "singleALU should print the right number" in {
    simulate(new shiftReg(8)) { dut =>
      dut.reset.poke(true.B)
      dut.io.inputNum.poke(-102.S)
      dut.clock.step(3)
      dut.reset.poke(false.B)

      for (i <- 1 until (8)) {
        dut.io.opcode.poke(i.U)
        dut.clock.step(1)
        println(dut.io.internalReg.peek().litValue.toInt.toBinaryString)
      }
      dut.io.opcode.poke(0.U)
      dut.clock.step(1)
      println(dut.io.internalReg.peek().litValue.toInt.toBinaryString)
    }
  }
}