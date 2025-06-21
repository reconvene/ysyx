// See README.md for license details.

package multALU

import chisel3._
import chisel3.simulator.EphemeralSimulator._
import org.scalatest.freespec.AnyFreeSpec
import org.scalatest.matchers.must.Matchers



class multALUSpec extends AnyFreeSpec with Matchers {

  val width:Int=4
  "multALU should print the right number" in {
    simulate(new multALU((width))) { dut =>
      for (i <- 0 until (8)) {
        for (j <- 0 until (8)) {
          dut.io.a.poke(i.U)
          dut.io.b.poke(j.U)
          dut.io.cin.poke(0.U)
          dut.clock.step(1)
          println(dut.io.a.peek().litValue)
          println(dut.io.b.peek().litValue)
          println(s"当前值:${dut.io.out.peek().litValue.toInt.toBinaryString}")
          assert(dut.io.out.peek().litValue.toInt==(i+j))
        }
      }

    }
  }
}