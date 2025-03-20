<script lang="ts">
    import ThreeElement from "$lib/ThreeElement.svelte";
    import {State} from "$lib/state.svelte";

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
        SphereGeometry,
    } from "three";
    import {OrbitControls} from 'three/addons/controls/OrbitControls.js';
    import {ConvexGeometry} from 'three/addons/geometries/ConvexGeometry.js';
    import {ParametricGeometry} from 'three/addons/geometries/ParametricGeometry.js';
    import {AxesHelper} from "three";
    import {lerp} from "three/src/math/MathUtils.js";
    import {convexHull} from "$lib/geometry";
    import type {AbstractLoader} from "$lib/loader/loader";

    let {loader, selection, getProjectionScene} = $props<{
        loader: AbstractLoader,
        selection: State
        getProjectionScene: () => Scene | null
    }>();

    $effect(onBoxes);

    $effect(onSelectBox3);

    $effect(onSelectRectangle);

    const scene = new Scene();
    scene.up.set(0, 0, 1);

    const camera = new OrthographicCamera(-4, 4, 4, -4, 0.001, 1000);
    camera.up.set(0, 0, 1);
    camera.lookAt(0, 0, 2);
    camera.position.set(0, -100, 0);

    const renderer = new WebGLRenderer({antialias: true});
    renderer.autoClear = false;

    const controls = new OrbitControls(camera, renderer.domElement);
    controls.enableRotate = true;
    controls.enableZoom = true;
    controls.enablePan = true;

    const center = new Mesh(new SphereGeometry(0.01), new MeshBasicMaterial({color: new Color(1, 1, 1)}));
    scene.add(center);

    let holeGroup: Group;
    let holeRadius: number;

    let plugGroup: Group;
    let plugRadius: number;

    let rotatedProjectedHoleVertices: Mesh[] = [];
    let projectedPlugVertices: Mesh[] = [];

    let projectionEdges: Line[] = [];
    let projections: Group[] = [];
    let rectangleProjections: Group[] = [];
    let rectangleOut: number[] = [];

    let hole_theta_t = 0;
    let hole_phi_t = 0;
    let hole_alpha_t = 0;

    let plug_theta_t = 0;
    let plug_phi_t = 0;

    function onBoxes() {
        if (!selection.loaded) {
            return;
        }
        {
            let vertices = loader.getHole();
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
            let vertices = loader.getPlug();
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

        {
            controls.object.zoom = 4.0 / (holeRadius + plugRadius);
            camera.updateProjectionMatrix();
        }
    }

    function onSelectBox3() {
        if (selection.selectedBox3 === null) {
            return;
        }

        {
            const vertexProjections = loader.getHoleVertexProjections(selection.selectedBox3);
            for (let index = 0; index < vertexProjections.length; index++) {
                const vertices: Vector2[] = vertexProjections[index];

                const hull = new Shape(convexHull(vertices));
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
        }

        {
            const projection = loader.getHoleProjectionHull(selection.selectedBox3);
            for (let index = 0; index < projection.length; index++) {
                const edge = projection[index];
                const edgeGeometry = new BufferGeometry().setFromPoints([
                    new Vector3(edge.from.x, edge.from.y, 0),
                    new Vector3(edge.to.x, edge.to.y, 0)
                ]);
                const edgeMaterial = new LineBasicMaterial({
                    color: new Color(1, 1, 1),
                });
                const edgeMesh = new Line(edgeGeometry, edgeMaterial);
                projectionEdges.push(edgeMesh);
            }

            for (let projectionEdge of projectionEdges) {
                scene.add(projectionEdge);
            }
        }

        {
            const box = loader.getBox(selection.selectedBox3);
            const loaderHole = loader.getHole();
            for (let index = 0; index < loaderHole.length; index++) {
                const holeVertex = loaderHole[index];

                const resolution = 8;

                for (let theta_index = 0; theta_index <= resolution; theta_index++) {
                    const theta = lerp(box.theta.interval.min, box.theta.interval.max, theta_index / resolution);

                    function parametric(phi_t: number, alpha_t: number, target: Vector3) {
                        const phi = lerp(box.phi.interval.min, box.phi.interval.max, phi_t);
                        const alpha = lerp(box.alpha.interval.min, box.alpha.interval.max, alpha_t);

                        const projected_vertex = project(holeVertex, theta, phi);
                        const rotated_projected_vertex = rotate(projected_vertex, alpha);
                        target.copy(rotated_projected_vertex);
                    }

                    const projectedHoleVertexGeometry = new ParametricGeometry(parametric, 8, 8);
                    const projectedHoleVertexMaterial = new MeshBasicMaterial({
                        color: new Color(0.0, 0.0, 1.0),
                        side: DoubleSide,
                    });
                    const rotatedProjectedHoleVertex = new Mesh(projectedHoleVertexGeometry, projectedHoleVertexMaterial);
                    rotatedProjectedHoleVertex.position.set(0, 0, holeRadius);
                    rotatedProjectedHoleVertices.push(rotatedProjectedHoleVertex);
                }
                for (let phi_index = 0; phi_index <= resolution; phi_index++) {
                    const phi = lerp(box.phi.interval.min, box.phi.interval.max, phi_index / resolution);

                    function parametric(theta_t: number, alpha_t: number, target: Vector3) {
                        const theta = lerp(box.theta.interval.min, box.theta.interval.max, theta_t);
                        const alpha = lerp(box.alpha.interval.min, box.alpha.interval.max, alpha_t);

                        const projected_vertex = project(holeVertex, theta, phi);
                        const rotated_projected_vertex = rotate(projected_vertex, alpha);
                        target.copy(rotated_projected_vertex);
                    }

                    const projectedHoleVertexGeometry = new ParametricGeometry(parametric, 8, 8);
                    const projectedHoleVertexMaterial = new MeshBasicMaterial({
                        color: new Color(0.0, 0.0, 1.0),
                        side: DoubleSide,
                    });
                    const rotatedProjectedHoleVertex = new Mesh(projectedHoleVertexGeometry, projectedHoleVertexMaterial);
                    rotatedProjectedHoleVertex.position.set(0, 0, holeRadius);
                    rotatedProjectedHoleVertices.push(rotatedProjectedHoleVertex);
                }
                for (let alpha_index = 0; alpha_index <= resolution; alpha_index++) {
                    const alpha = lerp(box.alpha.interval.min, box.alpha.interval.max, alpha_index / resolution);

                    function parametric(theta_t: number, phi_t: number, target: Vector3) {
                        const theta = lerp(box.theta.interval.min, box.theta.interval.max, theta_t);
                        const phi = lerp(box.phi.interval.min, box.phi.interval.max, phi_t);

                        const projected_vertex = project(holeVertex, theta, phi);
                        const rotated_projected_vertex = rotate(projected_vertex, alpha);
                        target.copy(rotated_projected_vertex);
                    }

                    const projectedHoleVertexGeometry = new ParametricGeometry(parametric, 8, 8);
                    const projectedHoleVertexMaterial = new MeshBasicMaterial({
                        color: new Color(0.0, 0.0, 1.0),
                        side: DoubleSide,
                    });
                    const rotatedProjectedHoleVertex = new Mesh(projectedHoleVertexGeometry, projectedHoleVertexMaterial);
                    rotatedProjectedHoleVertex.position.set(0, 0, holeRadius);
                    rotatedProjectedHoleVertices.push(rotatedProjectedHoleVertex);
                }
            }
            for (let mesh of rotatedProjectedHoleVertices) {
                scene.add(mesh);
            }
        }

        return () => {
            for (let group of projections) {
                scene.remove(group);
            }
            projections = [];
            for (let edge of projectionEdges) {
                scene.remove(edge);
            }
            projectionEdges = [];
            for (let mesh of rotatedProjectedHoleVertices) {
                scene.remove(mesh);
            }
            rotatedProjectedHoleVertices = [];
        };
    }

    function onSelectRectangle() {
        if (selection.selectedRectangle === null) {
            return;
        }

        {
            const rectangleOutIndices = loader.getPlugOutIndices(selection.selectedBox3, selection.selectedRectangle);
            for (let index = 0; index < rectangleOutIndices.length; index++) {
                rectangleOut.push(rectangleOutIndices[index]);
            }
        }

        {
            const loaderRectangleProjections = loader.getPlugVertexProjections(selection.selectedBox3, selection.selectedRectangle);
            for (let index = 0; index < loaderRectangleProjections.length; index++) {
                const vertices = loaderRectangleProjections[index];

                const hull = new Shape(vertices);
                const hullGeometry = new ShapeGeometry(hull);
                const hullMaterial = new MeshBasicMaterial({
                    color: new Color(0, 1, 0),
                    transparent: true,
                    opacity: (selection.selectedRectangle == loader.getHoleInIndex(selection.selectedBox3)) ? 0.25 : (rectangleOut.includes(index) ? 0.5 : 0),
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

                rectangleProjections.push(group);
            }
            for (let group of rectangleProjections) {
                scene.add(group);
            }
        }

        {
            const rectangle = loader.getRectangle(selection.selectedBox3, selection.selectedRectangle);
            const loaderPlug = loader.getPlug();
            for (let index = 0; index < loaderPlug.length; index++) {
                const holeVertex = loaderPlug[index];

                function parametric(theta_t: number, phi_t: number, target: Vector3) {
                    const theta = lerp(rectangle.theta.interval.min, rectangle.theta.interval.max, theta_t);
                    const phi = lerp(rectangle.phi.interval.min, rectangle.phi.interval.max, phi_t);

                    //TODO: handle the case when vertex is on the z-axis, as the parametric surface will collapse to a line
                    const projected_vertex = project(holeVertex, theta, phi);
                    target.copy(projected_vertex);
                }

                const projectedPlugVertexGeometry = new ParametricGeometry(parametric, 8, 8);
                const projectedPlugVertexMaterial = new MeshBasicMaterial({
                    color: new Color(0.0, 1.0, 0.0),
                    side: DoubleSide,
                });
                const projectedPlugVertex = new Mesh(projectedPlugVertexGeometry, projectedPlugVertexMaterial);
                projectedPlugVertex.position.set(0, 0, 2 * holeRadius + plugRadius);
                projectedPlugVertices.push(projectedPlugVertex);
            }
            for (let group of projectedPlugVertices) {
                scene.add(group);
            }
        }
        return () => {
            for (let group of rectangleProjections) {
                scene.remove(group);
            }
            rectangleProjections = [];
            rectangleOut = [];
            for (let group of projectedPlugVertices) {
                scene.remove(group);
            }
            projectedPlugVertices = [];
        };
    }

    function project(vertex: Vector3, theta: number, phi: number): Vector3 {
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

    function rotate(vertex: Vector3, alpha: number): Vector3 {
        const cos_alpha = Math.cos(alpha);
        const sin_alpha = Math.sin(alpha);
        return new Vector3(
            vertex.x * cos_alpha - vertex.y * sin_alpha,
            vertex.x * sin_alpha + vertex.y * cos_alpha,
            vertex.z
        );
    }

    function projection_rotation_quaternion(theta: number, phi: number, alpha: number): Quaternion {
        const q_init_0 = new Quaternion().setFromAxisAngle(new Vector3(1, 0, 0), Math.PI);
        const q_init_1 = new Quaternion().setFromAxisAngle(new Vector3(0, 0, 1), Math.PI / 2);
        const q_theta = new Quaternion().setFromAxisAngle(new Vector3(0, 0, 1), theta);
        const q_phi = new Quaternion().setFromAxisAngle(new Vector3(1, 0, 0), -phi);
        const q_alpha = new Quaternion().setFromAxisAngle(new Vector3(0, 0, 1), alpha);
        const q_total = new Quaternion();
        q_total.copy(q_alpha).multiply(q_phi).multiply(q_theta).multiply(q_init_1).multiply(q_init_0);
        return q_total;
    }


    function setup(width: number, height: number) {
        resize(width, height);

        {
            const axesHelper = new AxesHelper(10);
            scene.add(axesHelper);
        }

        return renderer.domElement;
    }

    function draw() {
        renderer.clear();
        const projectionScene = getProjectionScene();
        if (projectionScene) {
            renderer.render(projectionScene, camera);
        }
        renderer.render(scene, camera);

        if (holeGroup && selection.selectedBox3 !== null) {
            const box = loader.getBox(selection.selectedBox3);

            holeGroup.quaternion.copy(projection_rotation_quaternion(
                lerp(box.theta.interval.min, box.theta.interval.max, (Math.sin(hole_theta_t) + 1) / 2),
                lerp(box.phi.interval.min, box.phi.interval.max, (Math.sin(hole_phi_t) + 1) / 2),
                lerp(box.alpha.interval.min, box.alpha.interval.max, (Math.sin(hole_alpha_t) + 1) / 2)
            ));

            hole_theta_t += 0.1;
            hole_phi_t += 0.1 / Math.sqrt(2);
            hole_alpha_t += 0.1 / Math.sqrt(3);
        }
        if (plugGroup && selection.selectedBox3 !== null && selection.selectedRectangle !== null) {
            const rectangle = loader.getRectangle(selection.selectedBox3, selection.selectedRectangle);

            plugGroup.quaternion.copy(projection_rotation_quaternion(
                lerp(rectangle.theta.interval.min, rectangle.theta.interval.max, (Math.sin(plug_theta_t) + 1) / 2),
                lerp(rectangle.phi.interval.min, rectangle.phi.interval.max, (Math.sin(plug_phi_t) + 1) / 2),
                0
            ));

            plug_theta_t += 0.1;
            plug_phi_t += 0.1 / Math.sqrt(2);
        }
        center.position.copy(controls.target);
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
</script>

<ThreeElement {setup} {resize} {draw}/>
