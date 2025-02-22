<script lang="ts">
    import ThreeElement from "$lib/ThreeElement.svelte";
    import {Cover} from "$lib/flatbuffers/flat-buffers/cover";
    import {Selection} from "$lib/state.svelte";

    import {
        Mesh,
        MeshBasicMaterial,
        Scene,
        WebGLRenderer,
        EdgesGeometry,
        LineBasicMaterial,
        LineSegments,
        Vector3,
        Group,
        Color,
        Vector2,
        Shape,
        ShapeGeometry,
        DoubleSide,
        BufferGeometry,
        Line,
        Quaternion,
        OrthographicCamera,
    } from "three";
    import {OrbitControls} from 'three/addons/controls/OrbitControls.js';
    import {ConvexGeometry} from 'three/addons/geometries/ConvexGeometry.js';
    import {ParametricGeometry} from 'three/addons/geometries/ParametricGeometry.js';
    import {AxesHelper} from "three";
    import {lerp} from "three/src/math/MathUtils.js";

    let {cover, selection} = $props<{
        cover: Cover | undefined,
        selection: Selection,
    }>();

    $effect(onCover);

    $effect(onSelectBox3);

    $effect(onSelectBox2);

    const scene = new Scene();
    scene.up.set(0, 0, 1);

    const camera = new OrthographicCamera(-4, 4, 4, -4, 0.001, 1000);
    camera.up.set(0, 0, 1);
    camera.lookAt(0, 0, 2);
    camera.position.set(0, -10, 0);

    const renderer = new WebGLRenderer({antialias: true});

    const controls = new OrbitControls(camera, renderer.domElement);
    controls.enableRotate = true;
    controls.enableZoom = true;
    controls.enablePan = false;

    let holeGroup: Group;
    let holeRadius: number;

    let plugGroup: Group;
    let plugRadius: number;

    const resolution = 8;
    let rotatedHoleVertexGroups: Group[] = [];
    let rotatedPlugVertexGroups: Group[] = [];
    let rotatedHoleVerticesGroup: Group;

    let projectionEdges: Line[] = [];
    let projections: Group[] = [];
    let box2Projections: Group[] = [];
    let box2Out: number[] = [];

    let hole_phi_t = 0;
    let hole_theta_t = 0;
    let hole_alpha_t = 0;

    let plug_phi_t = 0;
    let plug_theta_t = 0;

    function onCover() {
        if (!cover) {
            return;
        }
        {
            const coverHole = cover!.hole();
            let vertices: Vector3[] = [];
            for (let index = 0; index < coverHole.verticesLength(); index++) {
                const vertex = coverHole.vertices(index);
                vertices.push(new Vector3(vertex.x(), vertex.y(), vertex.z()));
            }
            holeRadius = Math.max(...vertices.map(v => v.length()));
            camera.position.setZ(2 * holeRadius);
            controls.target.setZ(2 * holeRadius);

            const hole = new ConvexGeometry(vertices);
            const holeMaterial = new MeshBasicMaterial({
                color: new Color(0, 0, 1),
                transparent: true,
                opacity: 0.5
            });
            const holeMesh = new Mesh(hole, holeMaterial);

            const holeEdges = new EdgesGeometry(hole);
            const holeEdgesMaterial = new LineBasicMaterial({
                color: new Color(0.5, 0.5, 0.5)
            });
            const holeEdgesMesh = new LineSegments(holeEdges, holeEdgesMaterial);

            holeGroup = new Group();
            holeGroup.add(holeMesh);
            holeGroup.add(holeEdgesMesh);
            holeGroup.position.set(0, 0, holeRadius);
            scene.add(holeGroup);
        }
        {
            const coverPlug = cover!.plug();
            let vertices = [];
            for (let index = 0; index < coverPlug.verticesLength(); index++) {
                const vertex = coverPlug.vertices(index);
                vertices.push(new Vector3(vertex.x(), vertex.y(), vertex.z()));
            }
            plugRadius = Math.max(...vertices.map(v => v.length()));

            const plug = new ConvexGeometry(vertices);
            const plugMaterial = new MeshBasicMaterial({
                color: new Color(0, 1, 0),
                transparent: true,
                opacity: 0.5
            });
            let plugMesh = new Mesh(plug, plugMaterial);

            const plugEdges = new EdgesGeometry(plug);
            const plugEdgesMaterial = new LineBasicMaterial({
                color: new Color(0.5, 0.5, 0.5)
            });
            let plugEdgesMesh = new LineSegments(plugEdges, plugEdgesMaterial);

            plugGroup = new Group();
            plugGroup.add(plugMesh);
            plugGroup.add(plugEdgesMesh);
            plugGroup.position.set(0, 0, 2 * holeRadius + plugRadius);
            scene.add(plugGroup);
        }
    }

    function onSelectBox3() {
        if (selection.selectedBox3 === null) {
            return;
        }
        const box3 = cover!.box3s(selection.selectedBox3);

        for (let index = 0; index < box3.projectionsLength(); index++) {
            const projection = box3.projections(index);
            const vertices: Vector2[] = [];
            for (let index2 = 0; index2 < projection.verticesLength(); index2++) {
                const vertex = projection.vertices(index2);
                vertices.push(new Vector2(vertex.x(), vertex.y()));
            }

            const convexHull = computeConvexHull(vertices);
            const hull = new Shape(convexHull);
            const hullGeometry = new ShapeGeometry(hull);
            const hullMaterial = new MeshBasicMaterial({
                color: new Color(0, 0, 1),
                transparent: true,
                opacity: 0.25,
                side: DoubleSide,
                depthWrite: false
            });
            const hullMesh = new Mesh(hullGeometry, hullMaterial);

            const hullEdgesGeometry = new EdgesGeometry(hullGeometry);
            const hullEdgesMaterial = new LineBasicMaterial({
                color: new Color(0.5, 0.5, 0.5),
            });
            const hullEdges = new LineSegments(hullEdgesGeometry, hullEdgesMaterial);

            const group = new Group();
            group.add(hullMesh);
            group.add(hullEdges);

            projections.push(group);
        }
        for (let group of projections) {
            scene.add(group);
        }

        const projection = box3.projection();
        for (let index = 0; index < projection.edgesLength(); index++) {
            const edge = projection.edges(index);
            const point = new Vector2(edge.point().x(), edge.point().y());
            const direction = new Vector2(edge.direction().x(), edge.direction().y());
            const edgeGeometry = new BufferGeometry().setFromPoints([
                new Vector3(point.x, point.y, 0),
                new Vector3(point.x + direction.x, point.y + direction.y, 0)]);
            const edgeMaterial = new LineBasicMaterial({
                color: new Color(1, 1, 1),
            });
            const edgeMesh = new Line(edgeGeometry, edgeMaterial);
            projectionEdges.push(edgeMesh);
        }

        for (let projectionEdge of projectionEdges) {
            scene.add(projectionEdge);
        }

        const coverHole = cover!.hole();
        const allVertices = [];
        for (let index = 0; index < coverHole.verticesLength(); index++) {
            const vertex = coverHole.vertices(index);
            let vertices = [];
            const phi_interval = box3.phi().interval();
            for (let phi_index = 0; phi_index <= resolution; phi_index++) {
                const phi = lerp(phi_interval.min(), phi_interval.max(), phi_index / resolution);
                for (let theta_index = 0; theta_index <= resolution; theta_index++) {
                    const theta = lerp(box3.theta().interval().min(), box3.theta().interval().max(), theta_index / resolution);
                    const projected = project(new Vector3(vertex.x(), vertex.y(), vertex.z()), phi, theta);
                    for (let alpha_index = 0; alpha_index <= resolution; alpha_index++) {
                        const alpha = lerp(box3.alpha().interval().min(), box3.alpha().interval().max(), alpha_index / resolution);
                        const rotated_projected = rotate(projected, alpha);
                        vertices.push(rotated_projected);
                        allVertices.push(rotated_projected);
                    }
                }
            }
            const rotatedHoleVertex = new ConvexGeometry(vertices);
            const rotatedHoleVertexMaterial = new MeshBasicMaterial({
                color: new Color(0, 0, 1),
                transparent: true,
                opacity: 0.5
            });
            const rotatedHoleVertexMesh = new Mesh(rotatedHoleVertex, rotatedHoleVertexMaterial);
            const rotatedHoleVertexEdges = new EdgesGeometry(rotatedHoleVertex);
            const rotatedHoleVertexEdgesMaterial = new LineBasicMaterial({
                color: new Color(0.5, 0.5, 0.5)
            });
            const rotatedHoleVertexEdgesMesh = new LineSegments(rotatedHoleVertexEdges, rotatedHoleVertexEdgesMaterial);
            const rotatedHoleVertexGroup = new Group();
            rotatedHoleVertexGroup.add(rotatedHoleVertexMesh);
            rotatedHoleVertexGroup.add(rotatedHoleVertexEdgesMesh);
            rotatedHoleVertexGroup.position.set(0, 0, holeRadius);
            rotatedHoleVertexGroups.push(rotatedHoleVertexGroup);
        }
        for (let group of rotatedHoleVertexGroups) {
            scene.add(group);
        }
        {
            const rotatedHoleVertex = new ConvexGeometry(allVertices);
            const rotatedHoleVertexMaterial = new MeshBasicMaterial({
                color: new Color(0, 0, 1),
                transparent: true,
                opacity: 0.5
            });
            const rotatedHoleVertexMesh = new Mesh(rotatedHoleVertex, rotatedHoleVertexMaterial);
            const rotatedHoleVertexEdges = new EdgesGeometry(rotatedHoleVertex);
            const rotatedHoleVertexEdgesMaterial = new LineBasicMaterial({
                color: new Color(0.5, 0.5, 0.5)
            });
            const rotatedHoleVertexEdgesMesh = new LineSegments(rotatedHoleVertexEdges, rotatedHoleVertexEdgesMaterial);
            rotatedHoleVerticesGroup = new Group();
            rotatedHoleVerticesGroup.add(rotatedHoleVertexMesh);
            // rotatedHoleVerticesGroup.add(rotatedHoleVertexEdgesMesh);
            rotatedHoleVerticesGroup.position.set(0, 0, holeRadius);
        }
        scene.add(rotatedHoleVerticesGroup);
        return () => {
            for (let group of projections) {
                scene.remove(group);
            }
            projections = [];
            for (let edge of projectionEdges) {
                scene.remove(edge);
            }
            projectionEdges = [];
            for (let group of rotatedHoleVertexGroups) {
                scene.remove(group);
            }
            rotatedHoleVertexGroups = [];
            scene.remove(rotatedHoleVerticesGroup);
        };
    }

    function onSelectBox2() {
        if (selection.selectedBox2 === null) {
            return;
        }
        const box3 = cover!.box3s(selection.selectedBox3);
        const box2 = box3.box2s(selection.selectedBox2);

        for (let index = 0; index < box2.outLength(); index++) {
            box2Out.push(box2.out(index));
        }

        for (let index = 0; index < box2.projectionsLength(); index++) {
            const projection = box2.projections(index);
            const vertices: Vector2[] = [];
            for (let index2 = 0; index2 < projection.verticesLength(); index2++) {
                const vertex = projection.vertices(index2);
                vertices.push(new Vector2(vertex.x(), vertex.y()));
            }

            const hull = new Shape(computeConvexHull(vertices));
            const hullGeometry = new ShapeGeometry(hull);
            const hullMaterial = new MeshBasicMaterial({
                color: new Color(0, 1, 0),
                transparent: true,
                // TODO: fix so that it's the bo2's index that is checked
                // opacity: (index == box3.in_()) ? 0.25 : (box2Out.includes(index) ? 0.5 : 0),
                opacity: (box2Out.includes(index) ? 0.5 : 0),
                side: DoubleSide,
                depthWrite: false,
            });
            const hullMesh = new Mesh(hullGeometry, hullMaterial);

            const hullEdgesGeometry = new EdgesGeometry(hullGeometry);
            const hullEdgesMaterial = new LineBasicMaterial({
                color: new Color(0.5, 0.5, 0.5),
            });
            const hullEdges = new LineSegments(hullEdgesGeometry, hullEdgesMaterial);

            const group = new Group();
            group.add(hullMesh);
            group.add(hullEdges);

            box2Projections.push(group);
        }
        for (let group of box2Projections) {
            scene.add(group);
        }

        const coverPlug = cover!.plug();
        for (let index = 0; index < coverPlug.verticesLength(); index++) {
            const vertex = coverPlug.vertices(index);
            let vertices = [];
            const phi_interval = box2.phi().interval();
            for (let phi_index = 0; phi_index <= resolution; phi_index++) {
                const phi = lerp(phi_interval.min(), phi_interval.max(), phi_index / resolution);
                for (let theta_index = 0; theta_index <= resolution; theta_index++) {
                    const theta = lerp(box2.theta().interval().min(), box2.theta().interval().max(), theta_index / resolution);
                    const raw_vertex = new Vector3(vertex.x(), vertex.y(), vertex.z());
                    const projected = project(raw_vertex, phi, theta);
                    vertices.push(projected);
                }
            }
            const rotatedPlugVertex = new ConvexGeometry(vertices);
            const rotatedPlugVertexMaterial = new MeshBasicMaterial({
                color: new Color(0, 1, 0),
                transparent: true,
                opacity: 0.5
            });
            const rotatedPlugVertexMesh = new Mesh(rotatedPlugVertex, rotatedPlugVertexMaterial);
            const rotatedPlugVertexEdges = new EdgesGeometry(rotatedPlugVertex);
            const rotatedPlugVertexEdgesMaterial = new LineBasicMaterial({
                color: new Color(0.5, 0.5, 0.5)
            });
            const rotatedPlugVertexEdgesMesh = new LineSegments(rotatedPlugVertexEdges, rotatedPlugVertexEdgesMaterial);
            const rotatedPlugVertexGroup = new Group();
            rotatedPlugVertexGroup.add(rotatedPlugVertexMesh);
            rotatedPlugVertexGroup.add(rotatedPlugVertexEdgesMesh);
            rotatedPlugVertexGroup.position.set(0, 0, 2 * holeRadius + plugRadius);
            rotatedPlugVertexGroups.push(rotatedPlugVertexGroup);
        }
        for (let group of rotatedPlugVertexGroups) {
            scene.add(group);
        }
        return () => {
            for (let group of box2Projections) {
                scene.remove(group);
            }
            box2Projections = [];
            box2Out = [];
            for (let group of rotatedPlugVertexGroups) {
                scene.remove(group);
            }
            rotatedPlugVertexGroups = [];
        };
    }

    function project(vertex: Vector3, phi: number, theta: number): Vector3 {
        const cos_phi = Math.cos(phi * 2 * Math.PI);
        const sin_phi = Math.sin(phi * 2 * Math.PI);
        const cos_theta = Math.cos(theta * Math.PI);
        const sin_theta = Math.sin(theta * Math.PI);
        return new Vector3(
            -vertex.x * sin_phi + vertex.y * cos_phi,
            (vertex.x * cos_phi + vertex.y * sin_phi) * cos_theta - vertex.z * sin_theta,
            -(vertex.x * cos_phi + vertex.y * sin_phi) * sin_theta - vertex.z * cos_theta
        );
    }

    function rotate(vertex: Vector3, alpha: number): Vector3 {
        const cos_alpha = Math.cos(alpha * 2 * Math.PI);
        const sin_alpha = Math.sin(alpha * 2 * Math.PI);
        return new Vector3(
            vertex.x * cos_alpha - vertex.y * sin_alpha,
            vertex.x * sin_alpha + vertex.y * cos_alpha,
            vertex.z
        );
    }

    function projection_rotation_quaternion(phi: number, theta: number, alpha: number): Quaternion {
        const q_init_0 = new Quaternion().setFromAxisAngle(new Vector3(1, 0, 0), Math.PI);
        const q_init_1 = new Quaternion().setFromAxisAngle(new Vector3(0, 0, 1), Math.PI / 2);
        const q_phi = new Quaternion().setFromAxisAngle(new Vector3(0, 0, 1), phi * 2 * Math.PI);
        const q_theta = new Quaternion().setFromAxisAngle(new Vector3(1, 0, 0), -theta * Math.PI);
        const q_alpha = new Quaternion().setFromAxisAngle(new Vector3(0, 0, 1), alpha * 2 * Math.PI);
        const q_total = new Quaternion();
        q_total.copy(q_alpha).multiply(q_theta).multiply(q_phi).multiply(q_init_1).multiply(q_init_0);
        return q_total;
    }


    function setup(width: number, height: number) {
        resize(width, height);

        {
            const axesHelper = new AxesHelper(10);
            scene.add(axesHelper);
        }

        {
            function parametric(u: number, v: number, target: Vector3) {
                const phi = lerp(-Math.PI, Math.PI, u) / 3;
                const theta = lerp(-Math.PI / 2, Math.PI / 2, v) / 3;
                const x = Math.cos(phi) * Math.cos(theta);
                const y = Math.sin(phi) * Math.cos(theta);
                const z = Math.sin(theta);
                target.set(x, y, z);
            }

            const parametricGeometry = new ParametricGeometry(parametric, 10, 10);
            const parametricMaterial = new MeshBasicMaterial({
                color: new Color(0.5, 0.5, 0.5),
                transparent: true,
                opacity: 0.5,
                side: DoubleSide,
            });
            const parametricMesh = new Mesh(parametricGeometry, parametricMaterial);
            scene.add(parametricMesh);
        }

        return renderer.domElement;
    }

    function draw() {
        renderer.render(scene, camera);

        if (holeGroup && selection.selectedBox3 !== null) {
            const box3 = cover!.box3s(selection.selectedBox3);

            const phi_interval = box3.phi().interval();
            const theta_interval = box3.theta().interval();
            const alpha_interval = box3.alpha().interval();

            holeGroup.quaternion.copy(projection_rotation_quaternion(
                lerp(phi_interval.min(), phi_interval.max(), (Math.sin(hole_phi_t) + 1) / 2),
                lerp(theta_interval.min(), theta_interval.max(), (Math.sin(hole_theta_t) + 1) / 2),
                lerp(alpha_interval.min(), alpha_interval.max(), (Math.sin(hole_alpha_t) + 1) / 2)
            ));

            hole_phi_t += 0.1;
            hole_theta_t += 0.1 * Math.sqrt(2);
            hole_alpha_t += 0.1 * Math.sqrt(3);
        }
        if (plugGroup && selection.selectedBox3 !== null && selection.selectedBox2 !== null) {
            const box3 = cover!.box3s(selection.selectedBox3);
            const box2 = box3.box2s(selection.selectedBox2);

            const phi_interval = box2.phi().interval();
            const theta_interval = box2.theta().interval();

            plugGroup.quaternion.copy(projection_rotation_quaternion(
                lerp(phi_interval.min(), phi_interval.max(), (Math.sin(plug_phi_t) + 1) / 2),
                lerp(theta_interval.min(), theta_interval.max(), (Math.sin(plug_theta_t) + 1) / 2),
                0
            ));

            plug_phi_t += 0.1;
            plug_theta_t += 0.1 * Math.sqrt(2);
        }
    }

    function resize(width: number, height: number, zoom: number = 10) {
        const aspect = width / height;
        camera.left = -zoom * aspect / 2;
        camera.right = zoom * aspect / 2;
        camera.top = zoom / 2;
        camera.bottom = -zoom / 2;
        camera.updateProjectionMatrix();
        renderer.setSize(width, height);
        controls.update();
    }

    function computeConvexHull(points: Vector2[]): Vector2[] {
        points.sort((a, b) => a.x - b.x || a.y - b.y);

        const cross = (o: Vector2, a: Vector2, b: Vector2) =>
            (a.x - o.x) * (b.y - o.y) - (a.y - o.y) * (b.x - o.x);

        let lower: Vector2[] = [];
        for (let p of points) {
            while (lower.length >= 2 && cross(lower[lower.length - 2], lower[lower.length - 1], p) <= 0) {
                lower.pop();
            }
            lower.push(p);
        }

        let upper: Vector2[] = [];
        for (let i = points.length - 1; i >= 0; i--) {
            let p = points[i];
            while (upper.length >= 2 && cross(upper[upper.length - 2], upper[upper.length - 1], p) <= 0) {
                upper.pop();
            }
            upper.push(p);
        }

        upper.pop();
        lower.pop();
        return lower.concat(upper);
    }
</script>

<ThreeElement {setup} {resize} {draw}/>
