// See README.md for license details.

package selector

import chisel3._
import chisel3.simulator.EphemeralSimulator._
import org.scalatest.freespec.AnyFreeSpec
import org.scalatest.matchers.must.Matchers

class selectorSpec extends AnyFreeSpec with Matchers{
  "selector should print the right number" in {
    simulate(new selectorTop()){ dut =>
      dut.io.a.poke(1.U)
      dut.io.b.poke(0.U)
      dut.io.c.poke(1.U)
      dut.io.d.poke(0.U)

      for(i<-0 until(4)){
        dut.io.s.poke(i.U)
        println(s"mux21c's S:${dut.io.mux21s.peek().litValue}")
        dut.clock.step(1)
        println(s"y1:${dut.io.y1.peek().litValue}")
        println(s"y2:${dut.io.y2.peek().litValue}")
      }

    }
  }
}