<script lang="ts">
    import ThreeElement from "$lib/ThreeElement.svelte";
    import {Cover} from "$lib/flatbuffers/flatbuffers_generated";
    import {Selection} from "$lib/state.svelte";

    import {MapControls} from 'three/addons/controls/MapControls.js';
    import {
        PlaneGeometry,
        Mesh,
        MeshBasicMaterial,
        OrthographicCamera,
        Scene,
        WebGLRenderer,
        EdgesGeometry,
        LineBasicMaterial,
        LineSegments,
        AxesHelper,
        Group,
        Vector3,
        Vector2,
        Raycaster,
        FrontSide
    } from "three";

    let {cover, selection} = $props<{
        cover: Cover | undefined,
        selection: Selection,
    }>();

    $effect(onCover);

    $effect(onSelectBox3);

    $effect(onSelectBox2);

    let camera: OrthographicCamera;
    let scene: Scene;
    let renderer: WebGLRenderer;
    let box2Groups: Group[] = [];

    const defaultColor = new Vector3(0, 1, 0);
    const selectedColor = new Vector3(1, 0, 0);
    const edgeColor = new Vector3(0.5, 0.5, 0.5);

    function onCover() {
        if (!cover) {
            return;
        }
    }

    function onSelectBox3() {
        if (selection.selectedBox3 === null) {
            return;
        }
        {
            const box3 = cover!.box3s(selection.selectedBox3)
            console.log(box3.in_(), box3.box2sLength())
            for (let index = 0; index < box3.box2sLength(); index++) {
                const box2 = box3.box2s(index);

                const phi = box2.phi().interval();
                const theta = box2.theta().interval();

                const box2Geometry = new PlaneGeometry(phi.max() - phi.min(), theta.max() - theta.min());
                const box2Material = new MeshBasicMaterial({
                    transparent: true,
                    opacity: (index == box3.in_()) ? 1 : (box2.outLength() > 0 ? 0.25 : 0),
                    side: FrontSide,
                    depthWrite: false
                });
                box2Material.color.setFromVector3(defaultColor);
                const box2Mesh = new Mesh(box2Geometry, box2Material);
                box2Mesh.position.set((phi.min() + phi.max()) / 2, (theta.min() + theta.max()) / 2, 0);

                const box2EdgesGeometry = new EdgesGeometry(box2Geometry);
                const box2EdgesMaterial = new LineBasicMaterial();
                box2EdgesMaterial.color.setFromVector3(edgeColor);
                const box2Edges = new LineSegments(box2EdgesGeometry, box2EdgesMaterial);
                box2Edges.position.set((phi.min() + phi.max()) / 2, (theta.min() + theta.max()) / 2, 0);

                const box2Group = new Group();
                box2Group.add(box2Mesh);
                box2Group.add(box2Edges);

                box2Groups.push(box2Group);
            }
        }
        {
            for (const box2Group of box2Groups.values()) {
                scene.add(box2Group);
            }
        }
        return () => {
            for (const box2Group of box2Groups.values()) {
                scene.remove(box2Group);
            }
            box2Groups = [];
        };
    }

    function onSelectBox2() {
        if (selection.selectedBox2 === null) {
            return;
        }
        const box2Group = box2Groups[selection.selectedBox2];
        const box2 = box2Group.children[0] as Mesh;
        const box2Material = box2.material as MeshBasicMaterial;
        const originalColor = box2Material.color.clone();
        box2Material.color.setFromVector3(selectedColor);
        const originalOpacity = box2Material.opacity;
        box2Material.opacity = 0.9;
        return () => {
            box2Material.color.copy(originalColor);
            box2Material.opacity = originalOpacity;
        };
    }

    function onClick(mouse: Vector2) {
        const raycaster = new Raycaster();
        raycaster.setFromCamera(mouse, camera);

        const intersections = raycaster.intersectObjects(box2Groups.map(group => group.children[0] as Mesh), false);

        function getArea(geometry: PlaneGeometry) {
            const vertices = geometry.parameters;
            return vertices.width * vertices.height;
        }

        intersections.sort((intersection, otherIntersection) => {
            const area = getArea((intersection.object as Mesh).geometry as PlaneGeometry);
            const areaSize = getArea((otherIntersection.object as Mesh).geometry as PlaneGeometry);
            const areaDifference = area - areaSize;
            return -areaDifference;
        });

        if (intersections.length === 0) {
            // selection.selectBox2(null);
            return;
        }

        if (selection.selectedBox2 === null) {
            selection.selectBox2(box2Groups.findIndex(group => group.children[0] === intersections[0].object));
            return;
        }

        const selectedBox2 = box2Groups[selection.selectedBox2].children[0] as Mesh;
        const selectedBox2Index = intersections.findIndex(intersection => intersection.object === selectedBox2);
        const newSelectedBox2Index = selectedBox2Index === -1 ? 0 : (selectedBox2Index + 1) % intersections.length;
        const newSelectedBox2 = intersections[newSelectedBox2Index].object as Mesh;
        const newSelectedBox2IndexInGroups = box2Groups.findIndex(group => group.children[0] === newSelectedBox2);
        selection.selectBox2(newSelectedBox2IndexInGroups);
    }

    function setCameraBounds(width: number, height: number, zoom: number = 1.5) {
        const aspect = width / height;
        camera.left = (1 - zoom * aspect) / 2;
        camera.right = (1 + zoom * aspect) / 2;
        camera.top = (1 + zoom) / 2;
        camera.bottom = (1 - zoom) / 2;
        camera.updateProjectionMatrix();
    }

    function setup(width: number, height: number) {
        {
            scene = new Scene();
            scene.up.set(0, 1, 0);
        }

        {
            camera = new OrthographicCamera(0, 0, 0, 0, -1, 1);
            setCameraBounds(width, height);
            camera.up.set(0, 1, 0);
            camera.lookAt(0.5, 0.5, 0);
        }

        {
            renderer = new WebGLRenderer({antialias: true});
            renderer.setSize(width, height);
        }

        {
            const controls = new MapControls(camera, renderer.domElement);
            controls.enablePan = true;
            controls.enableZoom = true;
            controls.enableRotate = false;
            controls.screenSpacePanning = true;
            controls.zoomToCursor = true;
            controls.update();
        }

        {
            const axesHelper = new AxesHelper(10);
            scene.add(axesHelper);
        }

        {
            const domainGeometry = new PlaneGeometry(1, 1);
            const domainEdgesGeometry = new EdgesGeometry(domainGeometry);
            const domainEdgeMaterial = new LineBasicMaterial({color: 0x7f7f7f});
            const domainEdges = new LineSegments(domainEdgesGeometry, domainEdgeMaterial);
            domainEdges.position.set(0.5, 0.5, 0);
            scene.add(domainEdges);
        }

        return renderer.domElement;
    }

    function draw() {
        renderer.render(scene, camera);
    }

    function resize(width: number, height: number) {
        setCameraBounds(width, height);
        renderer.setSize(width, height);
    }
</script>

<ThreeElement {setup} {resize} {draw} {onClick}/>
