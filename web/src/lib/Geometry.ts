import {Matrix3, Vector2, Vector3} from "three";
import type {Rectangle, Box} from "$lib/Types";

export const PI = Math.PI;
export const TWO_PI = 2 * Math.PI;
export const PHI = (1 + Math.sqrt(5)) / 2;

export function convexHull(points: Vector2[]): Vector2[] {
    if (points.length < 3) return points;

    const hull: Vector2[] = [];
    const leftmost: Vector2 = points.reduce((left, p) => (p.x < left.x ? p : left), points[0]!);
    let current: Vector2 = leftmost;

    do {
        hull.push(current);
        let next = points[0] === current ? points[1]! : points[0]!;

        for (const point of points) {
            const cross = new Vector2().subVectors(next, current).cross(new Vector2().subVectors(point, current));
            if (cross < 0 || (cross === 0 && current.distanceTo(point) > current.distanceTo(next))) {
                next = point;
            }
        }

        current = next;
    } while (current !== leftmost);

    return hull;
}

function makeRotationZ(theta: number): Matrix3 {
    const c = Math.cos(theta), s = Math.sin(theta);
    return new Matrix3().set(
        c, -s, 0,
        s, c, 0,
        0, 0, 1
    );
}

function makeRotationX(phi: number): Matrix3 {
    const c = Math.cos(phi), s = Math.sin(phi);
    return new Matrix3().set(
        1, 0, 0,
        0, c, -s,
        0, s, c
    );
}

export function projectionMatrix(theta: number, phi: number): Matrix3 {
    return makeRotationX(phi).multiply(makeRotationZ(theta));
}

export function rotationMatrix(alpha: number): Matrix3 {
    return makeRotationZ(alpha);
}

export function transformationMatrix(theta: number, phi: number, alpha: number): Matrix3 {
    return rotationMatrix(alpha).clone().multiply(projectionMatrix(theta, phi));
}

export function anglesFromTransformationMatrix(m: Matrix3): { theta: number; phi: number; alpha: number } {
    const e = m.elements;
    const r11 = e[0], r12 = e[3], r13 = e[6];
    const r21 = e[1], r22 = e[4], r23 = e[7];
    const r31 = e[2], r32 = e[5], r33 = e[8];

    let phi = Math.acos(Math.min(1, Math.max(-1, r33)));

    let theta: number;
    let alpha: number;

    if (Math.abs(Math.sin(phi)) < 1e-10) {
        theta = 0;
        alpha = 0;
    } else {
        theta = Math.atan2(r31, r32);
        alpha = Math.atan2(r13, -r23);
    }

    if (theta < 0) {
        theta += TWO_PI;
    }
    if (phi < 0) {
        phi += TWO_PI;
    }
    if (alpha < 0) {
        alpha += TWO_PI;
    }

    return {theta, phi, alpha};
}


function lerp(min: number, max: number, t: number): number {
    return min + t * (max - min);
}

export function transformHoleVertex(vertex: Vector3, box: Box, theta_t: number, phi_t: number, alpha_t: number, target: Vector3) {
    const theta = lerp(box.theta.interval.min, box.theta.interval.max, theta_t);
    const phi = lerp(box.phi.interval.min, box.phi.interval.max, phi_t);
    const alpha = lerp(box.alpha.interval.min, box.alpha.interval.max, alpha_t);

    const transformation = transformationMatrix(theta, phi, alpha);
    const transformed_vertex = vertex.clone().applyMatrix3(transformation);
    target.copy(transformed_vertex);
}

export function transformPlugVertex(vertex: Vector3, rectangle: Rectangle, theta_t: number, phi_t: number, target: Vector3) {
    const theta = lerp(rectangle.theta.interval.min, rectangle.theta.interval.max, theta_t);
    const phi = lerp(rectangle.phi.interval.min, rectangle.phi.interval.max, phi_t);

    const projection = projectionMatrix(theta, phi);
    const projected_vertex = vertex.clone().applyMatrix3(projection);
    target.copy(projected_vertex);
}


function createOrthonormalBasis(p, q, reflection = false) {
    const v1 = p.clone().normalize();
    const proj = v1.clone().multiplyScalar(q.clone().normalize().dot(v1));
    const v2 = q.clone().normalize().sub(proj).normalize();
    let v3 = v1.clone().cross(v2).normalize();
    if (reflection) {
        v3 = v3.clone().multiplyScalar(-1);
    }
    return new Matrix3().set(
        v1.x, v2.x, v3.x,
        v1.y, v2.y, v3.y,
        v1.z, v2.z, v3.z
    );
}

export function symmetries(points, tolerance) {
    const rotations = [];
    const reflections = [];
    if (points.length < 3) {
        return {rotations, reflections};
    }

    const A = points[0].clone();
    const B = (points[1].distanceTo(A) < points[2].distanceTo(A)) ? points[1].clone() : points[2].clone();

    const basis = createOrthonormalBasis(A, B);

    for (const imageA of points) {
        for (const imageB of points) {
            if (imageB.equals(imageA)) {
                continue;
            }

            if (Math.abs(imageA.distanceTo(imageB) - A.distanceTo(B)) > tolerance) {
                continue;
            }

            const imageRotationBasis = createOrthonormalBasis(imageA, imageB);
            const rotationMatrix = new Matrix3().multiplyMatrices(imageRotationBasis, basis.clone().transpose());

            const imageReflectionBasis = createOrthonormalBasis(imageA, imageB, true);
            const reflectionMatrix = new Matrix3().multiplyMatrices(imageReflectionBasis, basis.clone().transpose());

            let rotationValid = true;
            for (const point of points) {
                const imagePoint = point.clone().applyMatrix3(rotationMatrix);
                if (!points.some(other => imagePoint.distanceTo(other) < tolerance)) {
                    rotationValid = false;
                    break;
                }
            }

            if (rotationValid) {
                rotations.push(rotationMatrix);
            }

            let reflectionValid = true;
            for (const point of points) {
                const imagePoint = point.clone().applyMatrix3(reflectionMatrix);
                if (!points.some(other => imagePoint.distanceTo(other) < tolerance)) {
                    reflectionValid = false;
                    break;
                }
            }
            if (reflectionValid) {
                reflections.push(reflectionMatrix);
            }
        }
    }
    return {rotations, reflections};
}

export function angleBetweenMatrices(m1: Matrix3, m2: Matrix3): number {
    const relativeMatrix = m2.clone().multiply(m1.clone().invert());
    const elements = relativeMatrix.elements;
    const trace = elements[0] + elements[4] + elements[8];
    const cosAngle = (trace - 1) / 2;
    const clampedCosAngle = Math.max(-1, Math.min(1, cosAngle));
    const angle = Math.acos(clampedCosAngle);
    if (isNaN(angle)) {
        console.error("NaN angle for trace:", trace, "m1:", m1, "m2:", m2);
    }
    return angle;
}
