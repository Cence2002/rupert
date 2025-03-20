<script lang="ts">
    import ThreePlot from "$lib/ThreePlot.svelte";
    import {State} from "$lib/state.svelte";
    import {PI, TWO_PI} from "$lib/geometry";

    import {OrbitControls} from 'three/addons/controls/OrbitControls.js';
    import {
        BoxGeometry,
        Mesh,
        MeshBasicMaterial,
        PerspectiveCamera,
        Scene,
        WebGLRenderer,
        EdgesGeometry,
        LineBasicMaterial,
        LineSegments,
        Vector2,
        AxesHelper,
        Raycaster,
        Group,
        Color, Float32BufferAttribute, BufferGeometry,
    } from "three";
    import type {AbstractLoader} from "$lib/loader/loader";
    import type {Interval} from "$lib/types";

    const {loader, state} = $props<{
        loader: AbstractLoader,
        state: State,
    }>();

    $effect(onLoad);

    $effect(onSelectBox);

    const scene = new Scene();
    scene.up.set(0, 0, 1);

    const camera = new PerspectiveCamera(60, 1, 0.01, 100);
    camera.up.set(0, 0, 1);
    camera.lookAt(0.5, 0.5, 0.5);
    camera.position.set(0.5, -1, 0.5);

    const renderer = new WebGLRenderer({antialias: true});

    const controls = new OrbitControls(camera, renderer.domElement);
    controls.target.set(0.5, 0.5, 0.5);
    controls.enableRotate = true;
    controls.enableZoom = true;
    controls.zoomToCursor = true;
    controls.enablePan = true;
    controls.screenSpacePanning = true;
    controls.minDistance = 0.05;
    controls.maxDistance = 5;
    controls.rotateSpeed = 0.5;

    const boxGroups: Group[] = [];

    function onLoad() {
        if (!state.loaded) {
            return;
        }
        {
            const boxes = loader.getBoxes();
            for (const box of boxes) {
                const theta: Interval = box.theta.interval;
                const phi: Interval = box.phi.interval;
                const alpha: Interval = box.alpha.interval;

                const boxGeometry = new BoxGeometry(theta.len / TWO_PI, phi.len / PI, alpha.len / TWO_PI);
                const boxMaterial = new MeshBasicMaterial({
                    color: new Color(0, 0, 1),
                    transparent: true,
                    opacity: box.terminal ? 0.5 : 0.05,
                    depthWrite: false
                });
                const boxMesh = new Mesh(boxGeometry, boxMaterial);
                boxMesh.position.set(theta.mid / TWO_PI, phi.mid / PI, alpha.mid / TWO_PI);

                const boxGroup = new Group();
                boxGroup.add(boxMesh);

                if (box.terminal) {
                    const boxEdgesGeometry = new EdgesGeometry(boxGeometry);
                    const boxEdgesMaterial = new LineBasicMaterial({
                        color: new Color(0.5, 0.5, 0.5),
                    });
                    const boxEdges = new LineSegments(boxEdgesGeometry, boxEdgesMaterial);
                    boxEdges.position.set(theta.mid / TWO_PI, phi.mid / PI, alpha.mid / TWO_PI);
                    boxGroup.add(boxEdges);
                }

                boxGroups.push(boxGroup);
            }
        }

        {
            for (const boxGroup of boxGroups) {
                scene.add(boxGroup);
            }
        }
    }

    function onSelectBox(): () => void {
        if (state.selectedBox === null) {
            return () => {
            };
        }
        const box = boxGroups[state.selectedBox]!.children[0] as Mesh;
        const boxMaterial = box.material as MeshBasicMaterial;
        const originalColor = boxMaterial.color.clone();
        boxMaterial.color.copy(new Color(1, 0, 0));
        const boxMaterialOpacity = boxMaterial.opacity;
        boxMaterial.opacity = 0.5;

        return () => {
            boxMaterial.color.copy(originalColor);
            boxMaterial.opacity = boxMaterialOpacity;
        };
    }

    function onDoubleClick(mouse: Vector2) {
        const raycaster = new Raycaster();
        raycaster.setFromCamera(mouse, camera);

        const boxes = boxGroups
        .map(group => group.children[0] as Mesh)
        .filter(mesh => (mesh.material as MeshBasicMaterial).opacity > 0);

        const intersections = raycaster.intersectObjects(boxes, false);

        function getVolume(geometry: BoxGeometry) {
            const {width, height, depth} = geometry.parameters;
            return width * height * depth;
        }

        intersections.sort((intersection, otherIntersection) => {
            const volume = getVolume((intersection.object as Mesh).geometry as BoxGeometry);
            const otherVolume = getVolume((otherIntersection.object as Mesh).geometry as BoxGeometry);
            const volumeRatio = volume / otherVolume;
            const distanceDifference = intersection.distance - otherIntersection.distance;
            return distanceDifference - Math.log(volumeRatio) / 1000;
        });

        if (intersections.length === 0) {
            state.unselectBox();
            return;
        }

        if (state.selectedBox === null) {
            state.selectBox(boxGroups.findIndex(group => group.children[0] === intersections[0]!.object));
            return;
        }

        const box = boxGroups[state.selectedBox]!.children[0] as Mesh;
        const boxIndex = intersections.findIndex(intersection => intersection.object === box);
        const newBoxIndex = boxIndex === -1 ? 0 : (boxIndex + 1) % intersections.length;
        const newBox = intersections[newBoxIndex]!.object as Mesh;
        const newBoxGroupIndex = boxGroups.findIndex(group => group.children[0] === newBox);
        state.selectBox(newBoxGroupIndex);
    }

    function setup(width: number, height: number) {
        resize(width, height);

        {
            const axesHelper = new AxesHelper(2);
            scene.add(axesHelper);
        }

        {
            const edgePositions = [
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
            const domainEdgesGeometry = new BufferGeometry();
            domainEdgesGeometry.setAttribute('position', new Float32BufferAttribute(edgePositions, 3));
            const domainEdgesMaterial = new LineBasicMaterial({opacity: 0.5});
            domainEdgesMaterial.color.copy(new Color(0.5, 0.5, 0.5));
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

<ThreePlot {setup} {resize} {draw} {onDoubleClick}/>
