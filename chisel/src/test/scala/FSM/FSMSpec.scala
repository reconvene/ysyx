// See README.md for license details.

package FSM

import chisel3._
import chisel3.simulator.EphemeralSimulator._
import org.scalatest.freespec.AnyFreeSpec
import org.scalatest.matchers.must.Matchers

class FSMSpec extends AnyFreeSpec with Matchers{
  "FSM should print the right state" in {
    simulate(new simpleMooreFSM){ dut =>
      dut.reset.poke(true.B)
      dut.clock.step(3)
      dut.reset.poke(false.B)
      dut.clock.step(1)
      for(i<-0 until(7)){
        dut.io.in.poke(1.U)
        dut.clock.step(1)
        println(s"After:${dut.io.out.peek().litValue}")
      }
      for(i<-0 until(7)){
        dut.io.in.poke(0.U)
        dut.clock.step(1)
        println(s"After:${dut.io.out.peek().litValue}")
      }
    }
  }
}