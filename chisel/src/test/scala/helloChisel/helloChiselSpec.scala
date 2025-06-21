// See README.md for license details.

package helloChisel

import chisel3._
import chisel3.simulator.EphemeralSimulator._
import org.scalatest.freespec.AnyFreeSpec
import org.scalatest.matchers.must.Matchers

class helloChiselSpec extends AnyFreeSpec with Matchers{
  "helloChisel should light the two leds at the same time" in {
    simulate(new helloChiselTop()){ dut =>

      for (i <- 0 until 20000) {
        if ((i-3)%5000==0){
          println(s"currentClockStep:$i")
          println(s"counter:${dut.io.counterValue.peek().litValue}")
          println(s"LED1Status:${dut.io.led1Output.peek().litToBoolean}")
          println(s"LED2Status:${dut.io.led2Output.peek().litToBoolean}")
        }
        dut.clock.step(1)
      }

    }
  }
}