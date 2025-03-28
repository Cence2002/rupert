// automatically generated by the FlatBuffers compiler, do not modify

import * as flatbuffers from 'flatbuffers';

import { Interval } from '../flat-buffers/interval';


export class Id {
  bb: flatbuffers.ByteBuffer|null = null;
  bb_pos = 0;
  __init(i:number, bb:flatbuffers.ByteBuffer):Id {
  this.bb_pos = i;
  this.bb = bb;
  return this;
}

depth():number {
  return this.bb!.readUint8(this.bb_pos);
}

bits():number {
  return this.bb!.readUint16(this.bb_pos + 2);
}

interval(obj?:Interval):Interval|null {
  return (obj || new Interval()).__init(this.bb_pos + 8, this.bb!);
}

static sizeOf():number {
  return 24;
}

static createId(builder:flatbuffers.Builder, depth: number, bits: number, interval_min: number, interval_max: number):flatbuffers.Offset {
  builder.prep(8, 24);
  builder.prep(8, 16);
  builder.writeFloat64(interval_max);
  builder.writeFloat64(interval_min);
  builder.pad(4);
  builder.writeInt16(bits);
  builder.pad(1);
  builder.writeInt8(depth);
  return builder.offset();
}

}
