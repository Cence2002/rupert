<script lang="ts">
    import ThreeElement from "$lib/ThreeElement.svelte";
    import {Cover} from "$lib/flatbuffers/flat-buffers/cover";
    import {Selection} from "$lib/state.svelte";

    import {OrbitControls} from 'three/addons/controls/OrbitControls.js';
    import {BoxGeometry, Mesh, MeshBasicMaterial, PerspectiveCamera, Scene, WebGLRenderer, EdgesGeometry, LineBasicMaterial, LineSegments, Vector2, Raycaster, Group} from "three";
    import {AxesHelper} from "three";

    let {cover, selection} = $props<{
        cover: Cover | undefined,
        selection: Selection,
    }>();

    $effect(onCover);

    $effect(onSelectBox3);

    let camera: PerspectiveCamera;
    let scene: Scene;
    let renderer: WebGLRenderer;
    let box3s: Map<number, Group> = new Map();

    function onCover() {
        if (!cover) {
            return;
        }
        {
            for (let index = 0; index < cover!.box3sLength(); index++) {
                const box3 = cover!.box3s(index);

                const phi = box3.phi().interval();
                const theta = box3.theta().interval();
                const alpha = box3.alpha().interval();

                const box3Geometry = new BoxGeometry((phi.max() - phi.min()) / (2 * Math.PI), (theta.max() - theta.min()) / Math.PI, (alpha.max() - alpha.min()) / (2 * Math.PI));
                const box3Material = new MeshBasicMaterial({color: 0x0000ff, transparent: true, opacity: 0.5});
                const box3Mesh = new Mesh(box3Geometry, box3Material);
                box3Mesh.position.set((phi.min() + phi.max()) / (2 * Math.PI) / 2, (theta.min() + theta.max()) / Math.PI / 2, (alpha.min() + alpha.max()) / (2 * Math.PI) / 2);

                const box3EdgesGeometry = new EdgesGeometry(box3Geometry);
                const box3EdgesMaterial = new LineBasicMaterial({color: 0x00ffff});
                const box3Edges = new LineSegments(box3EdgesGeometry, box3EdgesMaterial);
                box3Edges.position.set((phi.min() + phi.max()) / (2 * Math.PI) / 2, (theta.min() + theta.max()) / Math.PI / 2, (alpha.min() + alpha.max()) / (2 * Math.PI) / 2);

                const box3Group = new Group();
                box3Group.add(box3Mesh);
                box3Group.add(box3Edges);

                box3s.set(index, box3Group);
            }
        }
        {
            for (const box3 of box3s.values()) {
                scene.add(box3);
            }
        }
    }

    function onSelectBox3() {
        if (selection.selectedBox3 === null) {
            return;
        }
        const selectedBox = box3s.get(selection.selectedBox3)!;
        const mesh = selectedBox.children[0] as Mesh;

        if (mesh && mesh.material instanceof MeshBasicMaterial) {
            mesh.material.color.setRGB(1, 0, 0);
        }
        return () => {
        };
    }

    function setup(width: number, height: number) {
        {
            scene = new Scene();
            scene.up.set(0, 0, 1);
        }

        {
            camera = new PerspectiveCamera(60, width / height, 0.1, 1000);
            camera.up.set(0, 0, 1);
            camera.lookAt(0.5, 0.5, 0.5);
            camera.position.set(0.5, -1.5, 0.5);
        }

        {
            renderer = new WebGLRenderer({antialias: true});
            renderer.setSize(width, height);
        }

        {
            const controls = new OrbitControls(camera, renderer.domElement);
            controls.enableRotate = true;
            controls.enableZoom = true;
            controls.enablePan = false;
            controls.enableDamping = false;
            controls.target.set(0.5, 0.5, 0.5);
            controls.update();
        }

        {
            const axesHelper = new AxesHelper(10);
            scene.add(axesHelper);
        }

        {
            const domainGeometry = new BoxGeometry(1, 1, 1);
            const domainEdgesGeometry = new EdgesGeometry(domainGeometry);
            const domainEdgeMaterial = new LineBasicMaterial({color: 0x7f7f7f});
            const domainEdges = new LineSegments(domainEdgesGeometry, domainEdgeMaterial);
            domainEdges.position.set(0.5, 0.5, 0.5);
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
    }

    function onClick(x: number, y: number) {

        const mouse = new Vector2(
            2 * x - 1,
            1 - 2 * y
        );
        console.log('Mouse clicked!', mouse);

        const raycaster = new Raycaster();
        raycaster.setFromCamera(mouse, camera);

        for (const [index, box3] of box3s) {
            const intersects = raycaster.intersectObjects(box3.children);

            if (intersects.length > 0) {
                // box3.children[0].material.color.set(0xff0000);
            }
        }

        selection.selectBox3(0);
    }
</script>

<ThreeElement {setup} {resize} {draw} {onClick}/>
