// See README.md for license details.

package PS2

import chisel3._
import chisel3.simulator.EphemeralSimulator._
import org.scalatest.freespec.AnyFreeSpec
import org.scalatest.matchers.must.Matchers



class PS2Spec extends AnyFreeSpec with Matchers {
  def ps2Sender(target:ps2Top,inputChar:Char): Unit = {
    target.io.inputChar.poke(inputChar.toInt.U(8.W))
    target.clock.step(22)
    println(target.io.outputChar.peek().litValue)
  }
  "PS2 should print the right number" in {
    simulate(new ps2Top) { dut =>
      // 初始化电路
      dut.reset.poke(true.B)
      dut.clock.step(3)
      dut.reset.poke(false.B)
      println("0".head.toInt)
      ps2Sender(dut,'0')
      println("1".head.toInt)
      ps2Sender(dut,'1')
      println("a".head.toInt)
      ps2Sender(dut,'a')
      println("f".head.toInt)
      ps2Sender(dut,'f')
      println("?".head.toInt)
      ps2Sender(dut,'?')

    }
  }
}