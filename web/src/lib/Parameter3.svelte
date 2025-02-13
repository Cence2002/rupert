<script lang="ts">
    import ThreeElement from "$lib/ThreeElement.svelte";
    import {Cover} from "$lib/flatbuffers/flat-buffers/cover";
    import {Selection} from "$lib/state.svelte";

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
        Vector3,
    } from "three";

    let {cover, selection} = $props<{
        cover: Cover | undefined,
        selection: Selection,
    }>();

    $effect(onCover);

    $effect(onSelectBox3);

    let camera: PerspectiveCamera;
    let scene: Scene;
    let renderer: WebGLRenderer;
    const box3Groups: Map<number, Group> = new Map();

    const defaultColor = new Vector3(0, 0, 1);
    const selectedColor = new Vector3(1, 0, 0);
    const edgeColor = new Vector3(0.5, 0.5, 0.5);

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
                const box3Material = new MeshBasicMaterial({transparent: true, opacity: 0.5});
                box3Material.color.setFromVector3(defaultColor);
                const box3Mesh = new Mesh(box3Geometry, box3Material);
                box3Mesh.position.set((phi.min() + phi.max()) / (2 * Math.PI) / 2, (theta.min() + theta.max()) / Math.PI / 2, (alpha.min() + alpha.max()) / (2 * Math.PI) / 2);

                const box3EdgesGeometry = new EdgesGeometry(box3Geometry);
                const box3EdgesMaterial = new LineBasicMaterial();
                box3EdgesMaterial.color.setFromVector3(edgeColor);
                const box3Edges = new LineSegments(box3EdgesGeometry, box3EdgesMaterial);
                box3Edges.position.set((phi.min() + phi.max()) / (2 * Math.PI) / 2, (theta.min() + theta.max()) / Math.PI / 2, (alpha.min() + alpha.max()) / (2 * Math.PI) / 2);

                const box3Group = new Group();
                box3Group.add(box3Mesh);
                box3Group.add(box3Edges);

                box3Groups.set(index, box3Group);
            }
        }
        {
            for (const box3Group of box3Groups.values()) {
                scene.add(box3Group);
            }
        }
    }

    function onSelectBox3() {
        if (selection.selectedBox3 === null) {
            return;
        }
        const box3Group = box3Groups.get(selection.selectedBox3)!;
        const box3 = box3Group.children[0] as Mesh;

        const box3Material = box3.material as MeshBasicMaterial;
        box3Material.color.setFromVector3(selectedColor);
        return () => {
            for (const box3Group of box3Groups.values()) {
                const box3 = box3Group.children[0] as Mesh;
                const box3Material = box3.material as MeshBasicMaterial;
                box3Material.color.setFromVector3(defaultColor);
            }
        };
    }

    function onClick(mouse: Vector2) {
        const raycaster = new Raycaster();
        raycaster.setFromCamera(mouse, camera);

        for (const [index, box3Group] of box3Groups.entries()) {
            const box3 = box3Group.children[0] as Mesh;
            const intersections = raycaster.intersectObject(box3, false);

            if (intersections.length > 0) {
                if (selection.selectedBox3 === index) {
                    selection.selectBox3(null);
                } else {
                    selection.selectBox3(index);
                }
                return;
            }
        }

        selection.selectBox3(null);
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
</script>

<ThreeElement {setup} {resize} {draw} {onClick}/>
