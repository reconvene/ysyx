// See README.md for license details.

package LFSR

import chisel3._
import chisel3.simulator.EphemeralSimulator._
import org.scalatest.freespec.AnyFreeSpec
import org.scalatest.matchers.must.Matchers

class LFSRSpec extends AnyFreeSpec with Matchers{
  "shifter should print the right number" in {
    simulate(new LFSR){ dut =>
      dut.reset.poke(true.B)
      dut.clock.step(3)
      dut.reset.poke(false.B)
      dut.io.inputNum.poke("b10101110".U)

      for(i<-0 until(255)){
        dut.clock.step(1)
      }
      println(dut.io.outputNum.peek().litValue.toString(2))
    }
  }
}