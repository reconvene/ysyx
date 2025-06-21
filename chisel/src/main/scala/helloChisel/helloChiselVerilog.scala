// See README.md for license details.

package helloChisel
// _root_ disambiguates from package chisel3.util.circt if user imports chisel3.util._

import _root_.circt.stage.ChiselStage

object helloChiselVerilog extends App {
  ChiselStage.emitSystemVerilogFile(
    new helloChiselTop,
    args,
    firtoolOpts = Array("-disable-all-randomization")
  )
}
