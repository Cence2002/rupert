<script lang="ts">
    import ThreePlot from "$lib/ThreePlot.svelte";
    import {State} from "$lib/State.svelte";
    import {PI, TWO_PI} from "$lib/Geometry";

    import {OrbitControls} from 'three/addons/controls/OrbitControls.js';
    import {
        BoxGeometry,
        Mesh,
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
        AmbientLight,
        DirectionalLight, DoubleSide,
    } from "three";
    import type {AbstractLoader} from "$lib/loader/AbstractLoader";
    import {Box, Interval} from "$lib/Types";

    const {loader, state} = $props<{
        loader: AbstractLoader,
        state: State,
    }>();

    $effect(onLoad);

    $effect(onSelectBox);

    const scene = new Scene();
    scene.up.set(0, 0, 1);

    const camera = new PerspectiveCamera(45, 1, 0.01, 100);
    camera.up.set(0, 0, 1);
    camera.lookAt(0.5, 0.5, 0.5);
    camera.position.set(0.5, -1.5, 0.5);

    const renderer = new WebGLRenderer({antialias: true});

    const controls = new OrbitControls(camera, renderer.domElement);
    controls.target.set(0.5, 0.5, 0.5);
    controls.enablePan = true;
    controls.screenSpacePanning = true;
    controls.panSpeed = 0.1;
    controls.enableRotate = true;
    controls.rotateSpeed = 0.1;
    controls.enableZoom = true;
    controls.zoomToCursor = true;
    controls.minDistance = 0.01;
    controls.maxDistance = 100;

    let boxInstancedMesh: InstancedMesh;

    function onLoad() {
        if (!state.loaded) {
            return;
        }

        const boxes: Box[] = loader.getBoxes();
        const boxGeometry = new BoxGeometry(1, 1, 1);
        const boxMaterial = new MeshBasicMaterial({
            color: new Color(0, 0, 1),
            transparent: true,
            opacity: 0.25,
            depthWrite: false,
            side: DoubleSide,
        });
        boxInstancedMesh = new InstancedMesh(boxGeometry, boxMaterial, boxes.length);

        const boxMatrix = new Matrix4();
        for (let boxIndex = 0; boxIndex < boxes.length; boxIndex++) {
            const box = boxes[boxIndex];
            const theta: Interval = box.theta.interval;
            const phi: Interval = box.phi.interval;
            const alpha: Interval = box.alpha.interval;

            boxMatrix.identity();
            boxMatrix.makeTranslation(theta.mid / TWO_PI, phi.mid / PI, alpha.mid / TWO_PI);
            boxMatrix.scale(new Vector3(theta.len / TWO_PI, phi.len / PI, alpha.len / TWO_PI));
            boxInstancedMesh.setMatrixAt(boxIndex, boxMatrix);
        }
        scene.add(boxInstancedMesh);
        const ambientLight = new AmbientLight(0xffffff, 0.4); // soft white light
        scene.add(ambientLight);
        const directionalLight = new DirectionalLight(0xffffff, 0.8);
        directionalLight.position.set(1, -2, 2);
        scene.add(directionalLight);
    }

    function onSelectBox() {
        if (state.selectedBox === null) {
            return;
        }

        const box = loader.getBox(state.selectedBox);
        const theta: Interval = box.theta.interval;
        const phi: Interval = box.phi.interval;
        const alpha: Interval = box.alpha.interval;

        const boxGeometry = new BoxGeometry(theta.len / TWO_PI, phi.len / PI, alpha.len / TWO_PI);
        const boxMaterial = new MeshBasicMaterial({
            color: new Color(1, 0, 0),
            transparent: true,
            opacity: 0.5,
            depthWrite: true,
            side: DoubleSide,
        });
        const boxMesh = new Mesh(boxGeometry, boxMaterial);
        boxMesh.position.set(theta.mid / TWO_PI, phi.mid / PI, alpha.mid / TWO_PI);
        boxMesh.scale.set(1.05, 1.05, 1.05);
        scene.add(boxMesh);

        return () => {
            scene.remove(boxMesh);
        };
    }

    function onDoubleClick(mouse: Vector2) {
        const raycaster = new Raycaster();
        raycaster.setFromCamera(mouse, camera);

        const intersectedInstances = raycaster.intersectObject(boxInstancedMesh, false);
        const intersectedInstanceIds = [...new Set(
            intersectedInstances.map(hit => hit.instanceId)
        )];


        function getVolume(geometry: BoxGeometry) {
            const {width, height, depth} = geometry.parameters;
            return width * height * depth;
        }

        if (intersectedInstanceIds.length === 0) {
            state.unselectBox();
            return;
        }

        intersectedInstanceIds.sort((intersectedInstanceId, otherIntersectedInstanceId) => {
            const volume = getVolume((boxInstancedMesh.geometry as BoxGeometry));
            const otherVolume = getVolume((boxInstancedMesh.geometry as BoxGeometry));
            const volumeRatio = volume / otherVolume;
            const distanceDifference = intersectedInstanceId - otherIntersectedInstanceId;
            return distanceDifference - Math.log(volumeRatio) / 1000;
        });

        if (state.selectedBox === null) {
            state.selectBox(intersectedInstanceIds[0]);
            return;
        }

        const selectedInstanceId = intersectedInstanceIds.indexOf(state.selectedBox);
        const newSelectedInstanceId = (selectedInstanceId + 1) % intersectedInstanceIds.length;
        state.selectBox(intersectedInstanceIds[newSelectedInstanceId]);
    }

    function setup(width: number, height: number) {
        resize(width, height);

        const axesHelper = new AxesHelper(2);
        scene.add(axesHelper);

        const edgeBuffer = [
            1, 0, 0, 1, 1, 0,
            1, 0, 0, 1, 0, 1,
            0, 1, 0, 0, 1, 1,
            0, 1, 0, 1, 1, 0,
            0, 0, 1, 1, 0, 1,
            0, 0, 1, 0, 1, 1,
            1, 1, 1, 0, 1, 1,
            1, 1, 1, 1, 0, 1,
            1, 1, 1, 1, 1, 0,
        ];
        const edgesGeometry = new BufferGeometry();
        edgesGeometry.setAttribute('position', new Float32BufferAttribute(edgeBuffer, 3));
        const edgesMaterial = new LineBasicMaterial({color: new Color(0.5, 0.5, 0.5)});
        const edges = new LineSegments(edgesGeometry, edgesMaterial);
        scene.add(edges);

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

<ThreePlot {setup} {resize} {draw} {onDoubleClick}/>
