// See README.md for license details.

package barrelShift

import chisel3._
import chisel3.simulator.EphemeralSimulator._
import org.scalatest.freespec.AnyFreeSpec
import org.scalatest.matchers.must.Matchers

import scala.math._

class barrelShiftSpec extends AnyFreeSpec with Matchers{
  "shifter should print the right binary" in {
    simulate(new barrelShift(16)){ dut =>
      dut.io.din.poke("b1100101011001010".U)
      dut.io.moveNum.poke(7.U)
      dut.io.moveDirection.poke(0.U)
      dut.io.moveLogic.poke(1.U)
      dut.clock.step(1)
      println(dut.io.dout.peek().litValue.toInt.toBinaryString)
    }
  }
}