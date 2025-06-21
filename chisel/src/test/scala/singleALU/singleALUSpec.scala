// See README.md for license details.

package singleALU

import chisel3._
import chisel3.simulator.EphemeralSimulator._
import org.scalatest.freespec.AnyFreeSpec
import org.scalatest.matchers.must.Matchers

import scala.math._

class singleALUSpec extends AnyFreeSpec with Matchers {
  "singleALU should print the right number" in {
    simulate(new singleALUTop) { dut =>
      for (i <- 0 until (8)) {
        dut.io.a.poke((i & 1).U)
        dut.io.b.poke(((i >> 1) & 1).U)
        dut.io.cin.poke((((i >> 2) & 1)).U)
        dut.clock.step(1)
        println(s"当前值:${dut.io.s.peek().litValue}")
        println(s"进位:${dut.io.cout.peek().litValue}")
      }

    }
  }
}