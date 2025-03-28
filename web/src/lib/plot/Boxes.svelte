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
    } from "three";
    import type {AbstractLoader} from "$lib/loader/AbstractLoader";

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

        const boxEdgesGeometry = new BufferGeometry();
        boxEdgesGeometry.setAttribute('position', new Float32BufferAttribute(edgeBuffer, 3));
        const boxEdges = new LineSegments(boxEdgesGeometry, boxEdgesMaterial);
        scene.add(boxEdges);
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
