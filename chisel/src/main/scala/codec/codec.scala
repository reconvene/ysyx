// See README.md for license details.

package codec

import chisel3._
import chisel3.util._

// 定义编码器,返回2的n次方
class simpleEncodec(w: Int) extends Module {
  val encodec = IO(new Bundle {
    val n = Input(UInt(log2Ceil(w + 1).W))
    val en = Input(Bool())
    val m = Output(UInt((w + 1).W))
  })
  encodec.m := Mux(encodec.en, (1.U << encodec.n), 0.U)
}

// 定义解码器，返回log2Ceil(m)
class simpleDecodec(w: Int) extends Module {
  val decodec = IO(new Bundle {
    val m = Input(UInt((w + 1).W))
    val en = Input(Bool())
    val n = Output(UInt(log2Ceil(w + 1).W))
  })
  decodec.n := Mux(decodec.en, PriorityEncoder(decodec.m), 0.U)
}


class codecTop(width: Int) extends Module {
  val encodeIO = IO(new Bundle {
    val n = Input(UInt(log2Ceil(width + 1).W))
    val en = Input(Bool())
    val m = Output(UInt((width + 1).W))
  })

  val simpleEncodec = Module(new simpleEncodec(width))
  simpleEncodec.encodec.n := Mux(encodeIO.n > width.U, 0.U, encodeIO.n)
  simpleEncodec.encodec.en := encodeIO.en
  encodeIO.m := simpleEncodec.encodec.m

  val decodeIO=IO(new Bundle {
    val m = Input(UInt((width + 1).W))
    val en = Input(Bool())
    val n = Output(UInt(log2Ceil(width + 1).W))
  })

  val simpleDecodec=Module(new simpleDecodec(width))
  simpleDecodec.decodec.m:=decodeIO.m
  simpleDecodec.decodec.en:=decodeIO.en
  decodeIO.n:=simpleDecodec.decodec.n
}