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

function projectVertex(vertex: Vector3, theta: number, phi: number): Vector3 {
    const cos_theta = Math.cos(theta);
    const sin_theta = Math.sin(theta);
    const cos_phi = Math.cos(phi);
    const sin_phi = Math.sin(phi);
    return new Vector3(
        vertex.x * cos_theta - vertex.y * sin_theta,
        (vertex.x * sin_theta + vertex.y * cos_theta) * cos_phi - vertex.z * sin_phi,
        (vertex.x * sin_theta + vertex.y * cos_theta) * sin_phi + vertex.z * cos_phi
    );
}

function rotateVertex(vertex: Vector3, alpha: number): Vector3 {
    const cos_alpha = Math.cos(alpha);
    const sin_alpha = Math.sin(alpha);
    return new Vector3(
        vertex.x * cos_alpha - vertex.y * sin_alpha,
        vertex.x * sin_alpha + vertex.y * cos_alpha,
        vertex.z
    );
}

function lerp(min: number, max: number, t: number): number {
    return min + t * (max - min);
}

export function transformHoleVertex(vertex: Vector3, box: Box, theta_t: number, phi_t: number, alpha_t: number, target: Vector3) {
    const theta = lerp(box.theta.interval.min, box.theta.interval.max, theta_t);
    const phi = lerp(box.phi.interval.min, box.phi.interval.max, phi_t);
    const alpha = lerp(box.alpha.interval.min, box.alpha.interval.max, alpha_t);

    const projected_vertex = projectVertex(vertex, theta, phi);
    const rotated_projected_vertex = rotateVertex(projected_vertex, alpha);
    target.copy(rotated_projected_vertex);
}

export function transformPlugVertex(vertex: Vector3, rectangle: Rectangle, theta_t: number, phi_t: number, target: Vector3) {
    const theta = lerp(rectangle.theta.interval.min, rectangle.theta.interval.max, theta_t);
    const phi = lerp(rectangle.phi.interval.min, rectangle.phi.interval.max, phi_t);

    const projected_vertex = projectVertex(vertex, theta, phi);
    target.copy(projected_vertex);
}


function createOrthonormalBasis(p, q) {
    const v1 = p.clone().normalize();
    const proj = v1.clone().multiplyScalar(q.dot(v1));
    const v2 = q.clone().sub(proj).normalize();
    const v3 = new Vector3().crossVectors(v1, v2).normalize();
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
    const symmetries = [];
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

            const imageBasis = createOrthonormalBasis(imageA, imageB);
            const rotationMatrix = new Matrix3().multiplyMatrices(imageBasis, basis.clone().transpose());

            let valid = true;
            for (const point of points) {
                const imagePoint = point.clone().applyMatrix3(rotationMatrix);
                if (!points.some(other => imagePoint.distanceTo(other) < tolerance)) {
                    valid = false;
                    break;
                }
            }

            if (valid) {
                symmetries.push(rotationMatrix);
            }
        }
    }
    return symmetries;
}
