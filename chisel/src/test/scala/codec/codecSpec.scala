// See README.md for license details.

package codec

import chisel3._
import chisel3.simulator.EphemeralSimulator._
import org.scalatest.freespec.AnyFreeSpec
import org.scalatest.matchers.must.Matchers
import scala.math._

class codecSpec extends AnyFreeSpec with Matchers{
  "codec should print the right number" in {
    simulate(new codecTop(4)){ dut =>

      for(i<-0 until(5)){
        dut.encodeIO.n.poke(i.U)
        dut.encodeIO.en.poke(true.B)
        dut.decodeIO.m.poke(pow(2,i).toInt.U)
        dut.decodeIO.en.poke(true.B)
        dut.clock.step(1)
        println(dut.encodeIO.m.peek().litValue)
        println(dut.decodeIO.n.peek().litValue)
        dut.encodeIO.en.poke(false.B)
        dut.decodeIO.en.poke(false.B)
        dut.clock.step(1)
        println(dut.encodeIO.m.peek().litValue)
        println(dut.decodeIO.n.peek().litValue)
      }

    }
  }
}