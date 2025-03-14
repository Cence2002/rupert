// automatically generated by the FlatBuffers compiler, do not modify

import * as flatbuffers from 'flatbuffers';

import { Id } from '../flat-buffers/id';
import { Projection } from '../flat-buffers/projection';


export class Rectangle {
  bb: flatbuffers.ByteBuffer|null = null;
  bb_pos = 0;
  __init(i:number, bb:flatbuffers.ByteBuffer):Rectangle {
  this.bb_pos = i;
  this.bb = bb;
  return this;
}

static getRootAsRectangle(bb:flatbuffers.ByteBuffer, obj?:Rectangle):Rectangle {
  return (obj || new Rectangle()).__init(bb.readInt32(bb.position()) + bb.position(), bb);
}

static getSizePrefixedRootAsRectangle(bb:flatbuffers.ByteBuffer, obj?:Rectangle):Rectangle {
  bb.setPosition(bb.position() + flatbuffers.SIZE_PREFIX_LENGTH);
  return (obj || new Rectangle()).__init(bb.readInt32(bb.position()) + bb.position(), bb);
}

theta(obj?:Id):Id|null {
  const offset = this.bb!.__offset(this.bb_pos, 4);
  return offset ? (obj || new Id()).__init(this.bb_pos + offset, this.bb!) : null;
}

phi(obj?:Id):Id|null {
  const offset = this.bb!.__offset(this.bb_pos, 6);
  return offset ? (obj || new Id()).__init(this.bb_pos + offset, this.bb!) : null;
}

projections(index: number, obj?:Projection):Projection|null {
  const offset = this.bb!.__offset(this.bb_pos, 8);
  return offset ? (obj || new Projection()).__init(this.bb!.__indirect(this.bb!.__vector(this.bb_pos + offset) + index * 4), this.bb!) : null;
}

projectionsLength():number {
  const offset = this.bb!.__offset(this.bb_pos, 8);
  return offset ? this.bb!.__vector_len(this.bb_pos + offset) : 0;
}

outIndices(index: number):number|null {
  const offset = this.bb!.__offset(this.bb_pos, 10);
  return offset ? this.bb!.readUint32(this.bb!.__vector(this.bb_pos + offset) + index * 4) : 0;
}

outIndicesLength():number {
  const offset = this.bb!.__offset(this.bb_pos, 10);
  return offset ? this.bb!.__vector_len(this.bb_pos + offset) : 0;
}

outIndicesArray():Uint32Array|null {
  const offset = this.bb!.__offset(this.bb_pos, 10);
  return offset ? new Uint32Array(this.bb!.bytes().buffer, this.bb!.bytes().byteOffset + this.bb!.__vector(this.bb_pos + offset), this.bb!.__vector_len(this.bb_pos + offset)) : null;
}

static startRectangle(builder:flatbuffers.Builder) {
  builder.startObject(4);
}

static addTheta(builder:flatbuffers.Builder, thetaOffset:flatbuffers.Offset) {
  builder.addFieldStruct(0, thetaOffset, 0);
}

static addPhi(builder:flatbuffers.Builder, phiOffset:flatbuffers.Offset) {
  builder.addFieldStruct(1, phiOffset, 0);
}

static addProjections(builder:flatbuffers.Builder, projectionsOffset:flatbuffers.Offset) {
  builder.addFieldOffset(2, projectionsOffset, 0);
}

static createProjectionsVector(builder:flatbuffers.Builder, data:flatbuffers.Offset[]):flatbuffers.Offset {
  builder.startVector(4, data.length, 4);
  for (let i = data.length - 1; i >= 0; i--) {
    builder.addOffset(data[i]!);
  }
  return builder.endVector();
}

static startProjectionsVector(builder:flatbuffers.Builder, numElems:number) {
  builder.startVector(4, numElems, 4);
}

static addOutIndices(builder:flatbuffers.Builder, outIndicesOffset:flatbuffers.Offset) {
  builder.addFieldOffset(3, outIndicesOffset, 0);
}

static createOutIndicesVector(builder:flatbuffers.Builder, data:number[]|Uint32Array):flatbuffers.Offset;
/**
 * @deprecated This Uint8Array overload will be removed in the future.
 */
static createOutIndicesVector(builder:flatbuffers.Builder, data:number[]|Uint8Array):flatbuffers.Offset;
static createOutIndicesVector(builder:flatbuffers.Builder, data:number[]|Uint32Array|Uint8Array):flatbuffers.Offset {
  builder.startVector(4, data.length, 4);
  for (let i = data.length - 1; i >= 0; i--) {
    builder.addInt32(data[i]!);
  }
  return builder.endVector();
}

static startOutIndicesVector(builder:flatbuffers.Builder, numElems:number) {
  builder.startVector(4, numElems, 4);
}

static endRectangle(builder:flatbuffers.Builder):flatbuffers.Offset {
  const offset = builder.endObject();
  return offset;
}

}
