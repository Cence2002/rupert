// automatically generated by the FlatBuffers compiler, do not modify

import * as flatbuffers from 'flatbuffers';

import { Vector3 } from '../flat-buffers/vector3';


export class Polyhedron {
  bb: flatbuffers.ByteBuffer|null = null;
  bb_pos = 0;
  __init(i:number, bb:flatbuffers.ByteBuffer):Polyhedron {
  this.bb_pos = i;
  this.bb = bb;
  return this;
}

static getRootAsPolyhedron(bb:flatbuffers.ByteBuffer, obj?:Polyhedron):Polyhedron {
  return (obj || new Polyhedron()).__init(bb.readInt32(bb.position()) + bb.position(), bb);
}

static getSizePrefixedRootAsPolyhedron(bb:flatbuffers.ByteBuffer, obj?:Polyhedron):Polyhedron {
  bb.setPosition(bb.position() + flatbuffers.SIZE_PREFIX_LENGTH);
  return (obj || new Polyhedron()).__init(bb.readInt32(bb.position()) + bb.position(), bb);
}

vertices(index: number, obj?:Vector3):Vector3|null {
  const offset = this.bb!.__offset(this.bb_pos, 4);
  return offset ? (obj || new Vector3()).__init(this.bb!.__vector(this.bb_pos + offset) + index * 24, this.bb!) : null;
}

verticesLength():number {
  const offset = this.bb!.__offset(this.bb_pos, 4);
  return offset ? this.bb!.__vector_len(this.bb_pos + offset) : 0;
}

static startPolyhedron(builder:flatbuffers.Builder) {
  builder.startObject(1);
}

static addVertices(builder:flatbuffers.Builder, verticesOffset:flatbuffers.Offset) {
  builder.addFieldOffset(0, verticesOffset, 0);
}

static startVerticesVector(builder:flatbuffers.Builder, numElems:number) {
  builder.startVector(24, numElems, 8);
}

static endPolyhedron(builder:flatbuffers.Builder):flatbuffers.Offset {
  const offset = builder.endObject();
  return offset;
}

static createPolyhedron(builder:flatbuffers.Builder, verticesOffset:flatbuffers.Offset):flatbuffers.Offset {
  Polyhedron.startPolyhedron(builder);
  Polyhedron.addVertices(builder, verticesOffset);
  return Polyhedron.endPolyhedron(builder);
}
}
