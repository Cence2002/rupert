import {Vector2, Vector3} from "three";
import {PI, TWO_PI} from "$lib/Geometry";

//TODO: Replace with interfaces
//TODO: Add other types (Polygon, Polyhedron, etc.)
//TODO: Use override keyword for methods

export class Interval {
    constructor(
        public min: number,
        public max: number,
        public mid: number,
        public len: number
    ) {
    }
}

export class Id {
    constructor(
        public depth: number,
        public bits: number,
        public interval: Interval
    ) {
    }
}

export class Rectangle {
    constructor(
        public theta: Id,
        public phi: Id,
        public terminal: boolean
    ) {
    }

    public position(): Vector3 {
        return new Vector3(
            this.theta.interval.mid / TWO_PI,
            this.phi.interval.mid / PI,
            0,
        );
    }

    public scale(): Vector3 {
        return new Vector3(
            this.theta.interval.len / TWO_PI,
            this.phi.interval.len / PI,
            1,
        );
    }
}

export class Box {
    constructor(
        public theta: Id,
        public phi: Id,
        public alpha: Id,
        public terminal: boolean
    ) {
    }

    public position(): Vector3 {
        return new Vector3(
            this.theta.interval.mid / TWO_PI,
            this.phi.interval.mid / PI,
            this.alpha.interval.mid / TWO_PI,
        );
    }

    public scale(): Vector3 {
        return new Vector3(
            this.theta.interval.len / TWO_PI,
            this.phi.interval.len / PI,
            this.alpha.interval.len / TWO_PI,
        );
    }
}

export class TerminalBox {
    constructor(
        public box: Box,
        public rectangles: Rectangle[]
    ) {
    }
}

export class Edge {
    constructor(
        public from: Vector2,
        public to: Vector2
    ) {
    }
}