<script lang="ts">
    import ThreePlot from "$lib/ThreePlot.svelte";
    import {State} from "$lib/State.svelte";

    import {OrbitControls} from 'three/addons/controls/OrbitControls.js';
    import {
        BoxGeometry,
        MeshBasicMaterial,
        PerspectiveCamera,
        Scene,
        WebGLRenderer,
        LineBasicMaterial,
        LineSegments,
        Vector2,
        AxesHelper,
        Raycaster,
        Color,
        Float32BufferAttribute,
        BufferGeometry,
        Matrix4,
        InstancedMesh,
        Vector3,
        DoubleSide,
        Quaternion,
        EdgesGeometry,
        Matrix3,
        Mesh
    } from "three";
    import type {AbstractLoader} from "$lib/loader/AbstractLoader";
    import {PI, projectionMatrix, symmetries, TWO_PI, anglesFromTransformationMatrix} from "$lib/Geometry";
    import {lerp} from "three/src/math/MathUtils.js";

    const {loader, state} = $props<{
        loader: AbstractLoader,
        state: State,
    }>();

    $effect(onLoad);

    $effect(onSelectBox);

    const scene = new Scene();
    scene.up.set(0, 0, 1);

    const camera = new PerspectiveCamera(45, 1, 0.001, 100);
    camera.up.set(0, 0, 1);
    camera.position.set(0.5, -1.5, 0.5);
    camera.lookAt(0.5, 0.5, 0.5);

    const renderer = new WebGLRenderer({antialias: true});

    const controls = new OrbitControls(camera, renderer.domElement);
    controls.target.set(0.5, 0.5, 0.5);
    controls.enablePan = true;
    controls.screenSpacePanning = true;
    controls.panSpeed = 0.5;
    controls.enableRotate = true;
    controls.rotateSpeed = 0.5;
    controls.enableZoom = true;
    controls.minDistance = 0.01;
    controls.maxDistance = 100;

    // TODO: stats from https://github.com/mrdoob/three.js/blob/master/examples/misc_controls_fly.html

    let boxes: InstancedMesh;

    const boxesMaterial = new MeshBasicMaterial({
        color: new Color(0, 0, 1),
        transparent: true,
        opacity: 0.25,
        side: DoubleSide,
        depthWrite: false,
    });
    const boxEdgesMaterial = new LineBasicMaterial({
        color: new Color(0.5, 0.5, 1),
        transparent: false,
    });

    const selectedBoxEdgesMaterial = new MeshBasicMaterial({
        color: new Color(1, 0, 0),
        transparent: true,
        side: DoubleSide,
        depthTest: false,
    });


    function onLoad() {
        if (!state.loaded) {
            return;
        }

        {
            const hole = loader.getHole();

            function parametric(R, theta, phi, target) {
                const matrix = projectionMatrix(theta, phi);
                const totalMatrix = matrix.clone().multiply(R);
                const angles = anglesFromTransformationMatrix(totalMatrix);
                target.set(angles.theta / TWO_PI, angles.phi / PI, angles.alpha / TWO_PI);
            }

            // for (let symmetry of symmetries(hole, 1e-6).rotations) {
            //     const resolution = 100;
            //     const threshold = 4 / resolution;
            //     const vertices = [];
            //     const grid = [];
            //     for (let i = 0; i <= resolution; i++) {
            //         grid[i] = [];
            //         const theta = lerp(1e-3, TWO_PI - 1e-3, i / resolution);
            //         for (let j = 0; j <= resolution; j++) {
            //             const phi = lerp(1e-3, PI - 1e-3, j / resolution);
            //             const point = new Vector3();
            //             parametric(symmetry, theta, phi, point);
            //             vertices.push(point.x, point.y, point.z);
            //             grid[i][j] = i * (resolution + 1) + j;
            //         }
            //     }
            //     const indices = [];
            //     for (let i = 0; i < resolution; i++) {
            //         for (let j = 0; j < resolution; j++) {
            //             const a = grid[i][j];
            //             const b = grid[i + 1][j];
            //             const c = grid[i][j + 1];
            //             const d = grid[i + 1][j + 1];
            //             const va = new Vector3(vertices[3 * a], vertices[3 * a + 1], vertices[3 * a + 2]);
            //             const vb = new Vector3(vertices[3 * b], vertices[3 * b + 1], vertices[3 * b + 2]);
            //             const vc = new Vector3(vertices[3 * c], vertices[3 * c + 1], vertices[3 * c + 2]);
            //             const vd = new Vector3(vertices[3 * d], vertices[3 * d + 1], vertices[3 * d + 2]);
            //             if (va.distanceTo(vb) < threshold && va.distanceTo(vc) < threshold && vb.distanceTo(vd) < threshold && vc.distanceTo(vd) < threshold) {
            //                 indices.push(a, b, c);
            //                 indices.push(b, d, c);
            //             }
            //         }
            //     }
            //     const geometry = new BufferGeometry();
            //     geometry.setAttribute('position', new Float32BufferAttribute(vertices, 3));
            //     geometry.setIndex(indices);
            //     geometry.computeVertexNormals();
            //     const material = new MeshBasicMaterial({
            //         color: new Color(Math.random(), Math.random(), Math.random()),
            //         transparent: true,
            //         opacity: 0.2,
            //         side: DoubleSide,
            //         depthWrite: false,
            //         alphaTest: 0.01,
            //     });
            //     const mesh = new Mesh(geometry, material);
            //     mesh.renderOrder = 1;
            //     scene.add(mesh);
            // }
        }

        const boxesGeometry = new BoxGeometry(1, 1, 1);
        boxes = new InstancedMesh(boxesGeometry, boxesMaterial, loader.getBoxCount());

        const vertices = [
            new Vector3(-0.5, -0.5, -0.5),
            new Vector3(0.5, -0.5, -0.5),
            new Vector3(0.5, 0.5, -0.5),
            new Vector3(-0.5, 0.5, -0.5),
            new Vector3(-0.5, -0.5, 0.5),
            new Vector3(0.5, -0.5, 0.5),
            new Vector3(0.5, 0.5, 0.5),
            new Vector3(-0.5, 0.5, 0.5),
        ];
        const vertexIndices = [
            [0, 1], [1, 2], [2, 3], [3, 0],
            [4, 5], [5, 6], [6, 7], [7, 4],
            [0, 4], [1, 5], [2, 6], [3, 7],
        ];

        const matrix = new Matrix4();
        const edgeBuffer: number[] = [];
        for (const [boxIndex, box] of loader.getBoxes().entries()) {
            matrix.identity();
            matrix.makeTranslation(box.position());
            matrix.scale(box.scale());
            boxes.setMatrixAt(boxIndex, matrix);

            if (!box.terminal) {
                continue;
            }
            const transformedVertices = vertices.map(vertex => vertex.clone().applyMatrix4(matrix));
            for (const [vertexIndex, otherVertexIndex] of vertexIndices) {
                const vertex = transformedVertices[vertexIndex];
                const otherVertex = transformedVertices[otherVertexIndex];
                edgeBuffer.push(vertex.x, vertex.y, vertex.z);
                edgeBuffer.push(otherVertex.x, otherVertex.y, otherVertex.z);
            }
        }
        scene.add(boxes);


        function getVolume(matrix: Matrix4) {
            const scale = new Vector3();
            matrix.decompose(new Vector3(), new Quaternion(), scale);
            return scale.x * scale.y * scale.z;
        }

        const totalVolume = loader.getBoxes().reduce((totalVolume, box) => {
            const matrix = new Matrix4();
            matrix.makeTranslation(box.position());
            matrix.scale(box.scale());
            return totalVolume + getVolume(matrix);
        }, 0);
        console.log("Total volume: " + (totalVolume.toFixed(5) * 100) + "%");

        const boxEdgesGeometry = new BufferGeometry();
        boxEdgesGeometry.setAttribute('position', new Float32BufferAttribute(edgeBuffer, 3));
        const boxEdges = new LineSegments(boxEdgesGeometry, boxEdgesMaterial);
        scene.add(boxEdges);

        const nonTerminalBoxesMaterial = new MeshBasicMaterial({
            color: new Color(0, 1, 0),
            transparent: true,
            opacity: 0.25,
            side: DoubleSide,
            depthWrite: false,
        });
        const nonTerminalBoxes = new InstancedMesh(boxesGeometry, nonTerminalBoxesMaterial, loader.getNonterminalBoxCount());
        for (const [boxIndex, box] of loader.getNonterminalBoxes().entries()) {
            matrix.identity();
            matrix.makeTranslation(box.position());
            matrix.scale(box.scale());
            nonTerminalBoxes.setMatrixAt(boxIndex, matrix);
        }
        scene.add(nonTerminalBoxes);
    }

    function onSelectBox() {
        if (state.selectedBox === null) {
            return;
        }

        const box = loader.getBox(state.selectedBox);

        controls.target.copy(box.position());
        controls.update();

        const selectedBoxGeometry = new BoxGeometry(1, 1, 1);
        const selectedBoxEdgesGeometry = new EdgesGeometry(selectedBoxGeometry);
        const selectedBoxEdges = new LineSegments(selectedBoxEdgesGeometry, selectedBoxEdgesMaterial);
        selectedBoxEdges.position.copy(box.position());
        selectedBoxEdges.scale.copy(box.scale());
        selectedBoxEdges.renderOrder = 1;
        scene.add(selectedBoxEdges);

        return () => {
            scene.remove(selectedBoxEdges);
        };
    }

    function onClick(mouse: Vector2) {
        const raycaster = new Raycaster();
        raycaster.setFromCamera(mouse, camera);


        const intersectedInstances = [];
        const allIntersectedInstances = raycaster.intersectObject(boxes, false);
        for (const intersectedInstance of allIntersectedInstances) {
            if (!allIntersectedInstances.some(otherIntersectedInstance =>
                otherIntersectedInstance.instanceId === intersectedInstance.instanceId &&
                otherIntersectedInstance.distance < intersectedInstance.distance
            )) {
                intersectedInstances.push(intersectedInstance);
            }
        }

        if (intersectedInstances.length === 0) {
            state.unselectBox();
            return;
        }

        function getVolume(matrix: Matrix4) {
            const scale = new Vector3();
            matrix.decompose(new Vector3(), new Quaternion(), scale);
            return scale.x * scale.y * scale.z;
        }

        intersectedInstances.sort((intersectedInstance, otherIntersectedInstance) => {
            const intersectedInstanceMatrix = new Matrix4();
            const otherIntersectedInstanceMatrix = new Matrix4();
            boxes.getMatrixAt(intersectedInstance.instanceId!, intersectedInstanceMatrix);
            boxes.getMatrixAt(otherIntersectedInstance.instanceId!, otherIntersectedInstanceMatrix);

            const volume = getVolume(intersectedInstanceMatrix);
            const otherVolume = getVolume(otherIntersectedInstanceMatrix);
            const volumeRatio = volume / otherVolume;

            const distanceDifference = intersectedInstance.distance - otherIntersectedInstance.distance;

            return distanceDifference - Math.log(volumeRatio) / 1000;
        });

        const intersectedInstanceIds = intersectedInstances.map(hit => hit.instanceId);

        if (state.selectedBox === null || !intersectedInstanceIds.includes(state.selectedBox)) {
            state.selectBox(intersectedInstanceIds[0]);
            return;
        }

        const selectedInstanceIdIndex = intersectedInstanceIds.indexOf(state.selectedBox);
        const newSelectedInstanceIdIndex = (selectedInstanceIdIndex + 1) % intersectedInstanceIds.length;
        state.selectBox(intersectedInstanceIds[newSelectedInstanceIdIndex]);
    }

    function setup(width: number, height: number) {
        resize(width, height);

        {
            const axesHelper = new AxesHelper(2);
            scene.add(axesHelper);
        }

        {
            const vertices = [
                new Vector3(0, 0, 0),
                new Vector3(1, 0, 0),
                new Vector3(1, 1, 0),
                new Vector3(0, 1, 0),
                new Vector3(0, 0, 1),
                new Vector3(1, 0, 1),
                new Vector3(1, 1, 1),
                new Vector3(0, 1, 1),
            ];
            const vertexIndices = [
                [1, 2], [2, 3],
                [4, 5], [5, 6], [6, 7], [7, 4],
                [1, 5], [2, 6], [3, 7],
            ];
            const edgeBuffer: number[] = [];
            for (const [vertexIndex, otherVertexIndex] of vertexIndices) {
                const vertex = vertices[vertexIndex];
                const otherVertex = vertices[otherVertexIndex];
                edgeBuffer.push(vertex.x, vertex.y, vertex.z);
                edgeBuffer.push(otherVertex.x, otherVertex.y, otherVertex.z);
            }
            const domainEdgesGeometry = new BufferGeometry();
            domainEdgesGeometry.setAttribute('position', new Float32BufferAttribute(edgeBuffer, 3));
            const domainEdgesMaterial = new LineBasicMaterial({
                color: new Color(1, 1, 1),
                transparent: true,
                opacity: 0.5,
            });
            const domainEdges = new LineSegments(domainEdgesGeometry, domainEdgesMaterial);
            scene.add(domainEdges);
        }

        return renderer.domElement;
    }

    function draw() {
        renderer.render(scene, camera);
    }

    function resize(width: number, height: number) {
        camera.aspect = width / height;
        camera.updateProjectionMatrix();

        renderer.setSize(width, height);

        controls.update();
    }
</script>

<ThreePlot {setup} {resize} {draw} {onClick}/>
