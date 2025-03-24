import {Vector2, Vector3} from "three";
import {lerp} from "three/src/math/MathUtils";
import type {Box} from "$lib/types";

export const PI = Math.PI;
export const TWO_PI = 2 * Math.PI;

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
        -vertex.x * sin_theta + vertex.y * cos_theta,
        (vertex.x * cos_theta + vertex.y * sin_theta) * cos_phi - vertex.z * sin_phi,
        -(vertex.x * cos_theta + vertex.y * sin_theta) * sin_phi - vertex.z * cos_phi
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

export function transformPlugVertex(vertex: Vector3, box: Box, theta_t: number, phi_t: number, target: Vector3) {
    const theta = lerp(box.theta.interval.min, box.theta.interval.max, theta_t);
    const phi = lerp(box.phi.interval.min, box.phi.interval.max, phi_t);

    const projected_vertex = projectVertex(vertex, theta, phi);
    target.copy(projected_vertex);
}

export function transformHoleVertex(vertex: Vector3, box: Box, theta_t: number, phi_t: number, alpha_t: number, target: Vector3) {
    const theta = lerp(box.theta.interval.min, box.theta.interval.max, theta_t);
    const phi = lerp(box.phi.interval.min, box.phi.interval.max, phi_t);
    const alpha = lerp(box.alpha.interval.min, box.alpha.interval.max, alpha_t);

    const projected_vertex = projectVertex(vertex, theta, phi);
    const rotated_projected_vertex = rotateVertex(projected_vertex, alpha);
    target.copy(rotated_projected_vertex);
}