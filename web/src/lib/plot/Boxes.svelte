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
        Color,
        SphereGeometry,
    } from "three";
    import type {AbstractLoader} from "$lib/loader/loader";

    let {loader, state} = $props<{
        loader: AbstractLoader,
        state: State,
    }>();

    $effect(onLoad);

    $effect(onSelectBox);

    const scene = new Scene();
    scene.up.set(0, 0, 1);

    const camera = new PerspectiveCamera(60, 1, 0.001, 1000);
    camera.up.set(0, 0, 1);
    camera.lookAt(0.5, 0.5, 0.5);
    camera.position.set(0.5, -1.5, 0.5);

    const renderer = new WebGLRenderer({antialias: true});

    const controls = new OrbitControls(camera, renderer.domElement);
    controls.enableRotate = true;
    controls.enableZoom = true;
    controls.enablePan = true;
    controls.target.set(0.5, 0.5, 0.5);

    const center = new Mesh(new SphereGeometry(0.01), new MeshBasicMaterial({color: new Color(1, 1, 1)}));
    scene.add(center);

    const boxGroups: Group[] = [];

    function onLoad() {
        if (!state.loaded) {
            return;
        }
        {
            const boxes = loader.getBoxes();
            for (let index = 0; index < boxes.length; index++) {
                const box = boxes[index];

                const thetaInterval = box.theta.interval;
                const phiInterval = box.phi.interval;
                const alphaInterval = box.alpha.interval;

                const boxGeometry = new BoxGeometry(thetaInterval.len / TWO_PI, phiInterval.len / PI, alphaInterval.len / TWO_PI);
                const boxMaterial = new MeshBasicMaterial({
                    color: new Color(0, 0, 1),
                    transparent: true,
                    opacity: box.terminal ? 0.5 : 0.0,
                    depthWrite: false
                });
                const boxMesh = new Mesh(boxGeometry, boxMaterial);
                boxMesh.position.set(thetaInterval.mid / TWO_PI, phiInterval.mid / PI, alphaInterval.mid / TWO_PI);

                const boxEdgesGeometry = new EdgesGeometry(boxGeometry);
                const boxEdgesMaterial = new LineBasicMaterial({
                    color: new Color(0.5, 0.5, 0.5),
                });
                const boxEdges = new LineSegments(boxEdgesGeometry, boxEdgesMaterial);
                boxEdges.position.set(thetaInterval.mid / TWO_PI, phiInterval.mid / PI, alphaInterval.mid / TWO_PI);

                const boxGroup = new Group();
                boxGroup.add(boxMesh);
                if (box.terminal) {
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
        const boxGroup = boxGroups[state.selectedBox]!;
        const box = boxGroup.children[0] as Mesh;
        const boxMaterial = box.material as MeshBasicMaterial;
        const originalColor = boxMaterial.color.clone();
        boxMaterial.color.copy(new Color(1, 0, 0));
        const boxMaterialOpacity = boxMaterial.opacity;
        boxMaterial.opacity = 0.9;

        return () => {
            boxMaterial.color.copy(originalColor);
            boxMaterial.opacity = boxMaterialOpacity;
        };
    }

    function onDoubleClick(mouse: Vector2) {
        const raycaster = new Raycaster();
        raycaster.setFromCamera(mouse, camera);

        const intersections = raycaster.intersectObjects(boxGroups
                .map(group => group.children[0] as Mesh)
                .filter(mesh => (mesh.material as MeshBasicMaterial).opacity > 0),
            false);

        function getVolume(geometry: BoxGeometry) {
            const {width, height, depth} = geometry.parameters;
            return width * height * depth;
        }

        intersections.sort((intersection, otherIntersection) => {
            const volume = getVolume((intersection.object as Mesh).geometry as BoxGeometry);
            const volumeSize = getVolume((otherIntersection.object as Mesh).geometry as BoxGeometry);
            const volumeDifference = volume - volumeSize;
            const distanceDifference = intersection.distance - otherIntersection.distance;
            return distanceDifference - volumeDifference / 1000;
        });

        if (intersections.length === 0) {
            state.unselectBox();
            return;
        }

        if (state.selectedBox === null) {
            state.selectBox(boxGroups.findIndex(group => group.children[0] === intersections[0]!.object));
            return;
        }

        const selectedBox = boxGroups[state.selectedBox]!.children[0] as Mesh;
        const selectedBoxIndex = intersections.findIndex(intersection => intersection.object === selectedBox);
        const newselectedBoxIndex = selectedBoxIndex === -1 ? 0 : (selectedBoxIndex + 1) % intersections.length;
        const newselectedBox = intersections[newselectedBoxIndex]!.object as Mesh;
        const newselectedBoxIndexInGroups = boxGroups.findIndex(group => group.children[0] === newselectedBox);
        state.selectBox(newselectedBoxIndexInGroups);
    }

    function setup(width: number, height: number) {
        resize(width, height);

        {
            const axesHelper = new AxesHelper(10);
            scene.add(axesHelper);
        }

        {
            const domainGeometry = new BoxGeometry(1, 1, 1);
            const domainEdgesGeometry = new EdgesGeometry(domainGeometry);
            const domainEdgesMaterial = new LineBasicMaterial({color: 0x7f7f7f});
            const domainEdges = new LineSegments(domainEdgesGeometry, domainEdgesMaterial);
            domainEdges.position.set(0.5, 0.5, 0.5);
            scene.add(domainEdges);
        }

        return renderer.domElement;
    }

    function draw() {
        renderer.render(scene, camera);
        center.position.copy(controls.target);
    }

    function resize(width: number, height: number) {
        camera.aspect = width / height;
        camera.updateProjectionMatrix();
        renderer.setSize(width, height);
        controls.update();
    }
</script>

<ThreePlot {setup} {resize} {draw} {onDoubleClick}/>
