import {Matrix3, Matrix4, Vector2, Vector3} from "three";
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

export function projectionMatrix(theta: number, phi: number): Matrix4 {
    const zAxis = new Vector3(0, 0, 1);
    const xAxis = new Vector3(1, 0, 0);

    const Rz = new Matrix4().makeRotationAxis(zAxis, theta);
    const Rx = new Matrix4().makeRotationAxis(xAxis, phi);

    return Rx.multiply(Rz);
}

export function rotationMatrix(alpha: number): Matrix4 {
    const zAxis = new Vector3(0, 0, 1);
    return new Matrix4().makeRotationAxis(zAxis, alpha);
}

export function transformationMatrix(theta: number, phi: number, alpha: number): Matrix4 {
    const projection = projectionMatrix(theta, phi);
    const rotation = rotationMatrix(alpha);
    return rotation.multiply(projection);
}

function lerp(min: number, max: number, t: number): number {
    return min + t * (max - min);
}

export function transformHoleVertex(vertex: Vector3, box: Box, theta_t: number, phi_t: number, alpha_t: number, target: Vector3) {
    const theta = lerp(box.theta.interval.min, box.theta.interval.max, theta_t);
    const phi = lerp(box.phi.interval.min, box.phi.interval.max, phi_t);
    const alpha = lerp(box.alpha.interval.min, box.alpha.interval.max, alpha_t);

    const transformation = transformationMatrix(theta, phi, alpha);
    const transformed_vertex = vertex.clone().applyMatrix4(transformation);
    target.copy(transformed_vertex);
}

export function transformPlugVertex(vertex: Vector3, rectangle: Rectangle, theta_t: number, phi_t: number, target: Vector3) {
    const theta = lerp(rectangle.theta.interval.min, rectangle.theta.interval.max, theta_t);
    const phi = lerp(rectangle.phi.interval.min, rectangle.phi.interval.max, phi_t);

    const projection = projectionMatrix(theta, phi);
    const projected_vertex = vertex.clone().applyMatrix4(projection);
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

function closestPoint(points, point, tolerance) {
    let minDistance = Infinity;
    let closest = null;
    for (let i = 0; i < points.length; i++) {
        const dist = points[i].distanceTo(point);
        if (tolerance < dist && dist < minDistance) {
            minDistance = dist;
            closest = points[i].clone();
        }
    }
    return closest;
}

export function symmetries(points, tolerance) {
    const rotations = [];
    const reflections = [];
    if (points.length < 3) {
        return symmetries;
    }

    const A = points[0].clone();
    const B = closestPoint(points, A, tolerance);

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

export function angleBetweenMatrices(m1: Matrix4, m2: Matrix4): number {
    const relativeMatrix = m1.clone().multiply(m2.clone().invert());
    const elements = relativeMatrix.elements;
    const trace = elements[0] + elements[5] + elements[10];
    const cosAngle = (trace - 1) / 2;
    const clampedCosAngle = Math.max(-1, Math.min(1, cosAngle));
    const angle = Math.acos(clampedCosAngle);
    if (isNaN(angle)) {
        console.error("NaN angle for trace:", trace, "m1:", m1, "m2:", m2);
    }
    return angle;
}
