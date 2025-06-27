// See README.md for license details.

package PS2

import chisel3._
import chisel3.simulator.EphemeralSimulator._
import org.scalatest.freespec.AnyFreeSpec
import org.scalatest.matchers.must.Matchers


class PS2Spec extends AnyFreeSpec with Matchers {
  def ps2Sender(target: ps2Top, inputChar: Char): Unit = {
    target.io.inputChar.poke(inputChar.toInt.U(8.W))
    target.clock.step(24)
  }

  "PS2 should print the right number" in {
    simulate(new ps2Top) { dut =>
      // 初始化电路
      dut.reset.poke(true.B)
      dut.clock.step(3)
      dut.reset.poke(false.B)
      val charArr = Array('0', '1', '2')

      for (i <- 0 until (3)) {
        ps2Sender(dut, charArr(i))
      }

    }
  }
}