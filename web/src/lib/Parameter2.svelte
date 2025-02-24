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
        FrontSide, Color
    } from "three";

    let {cover, selection} = $props<{
        cover: Cover | undefined,
        selection: Selection,
    }>();

    $effect(onCover);

    $effect(onSelectBox3);

    $effect(onSelectBox2);

    const PI = Math.PI;
    const TWO_PI = 2 * Math.PI;

    const scene = new Scene();
    scene.up.set(0, 1, 0);

    const camera = new OrthographicCamera(0, 0, 0, 0, -1, 1);
    camera.up.set(0, 1, 0);
    camera.lookAt(0.5, 0.5, 0);
    camera.position.set(0, 0, 0);

    const renderer = new WebGLRenderer({antialias: true});

    const controls = new MapControls(camera, renderer.domElement);
    controls.enablePan = true;
    controls.enableZoom = true;
    controls.enableRotate = false;
    controls.screenSpacePanning = true;
    controls.zoomToCursor = true;

    let box2Groups: Group[] = [];

    function onCover() {
        if (!cover) {
            return;
        }
    }

    function onSelectBox3() {
        if (selection.selectedBox3 === null) {
            return;
        }
        const box3 = cover!.box3s(selection.selectedBox3)
        for (let index = 0; index < box3.box2sLength(); index++) {
            const box2 = box3.box2s(index);

            const theta = box2.theta().interval();
            const phi = box2.phi().interval();

            const box2Geometry = new PlaneGeometry((theta.max() - theta.min()) / PI, (phi.max() - phi.min()) / TWO_PI);
            const box2Material = new MeshBasicMaterial({
                color: new Color(0, 1, 0),
                transparent: true,
                opacity: (index == box3.in_()) ? 0.75 : (box2.outLength() > 0 ? 0.25 : 0.0),
                side: FrontSide,
                depthWrite: false
            });
            const box2Mesh = new Mesh(box2Geometry, box2Material);
            box2Mesh.position.set((theta.min() + theta.max()) / 2 / PI, (phi.min() + phi.max()) / 2 / TWO_PI, 0);

            const box2EdgesGeometry = new EdgesGeometry(box2Geometry);
            const box2EdgesMaterial = new LineBasicMaterial({
                color: new Color(0.5, 0.5, 0.5),
            });
            const box2Edges = new LineSegments(box2EdgesGeometry, box2EdgesMaterial);
            box2Edges.position.set((theta.min() + theta.max()) / 2 / PI, (phi.min() + phi.max()) / 2 / TWO_PI, 0);

            const box2Group = new Group();
            box2Group.add(box2Mesh);
            if (box2.outLength() > 0) {
                box2Group.add(box2Edges);
            }

            box2Groups.push(box2Group);
        }

        for (const box2Group of box2Groups.values()) {
            scene.add(box2Group);
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
        box2Material.color.copy(new Color(1, 0, 0));
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

        const intersections = raycaster.intersectObjects(box2Groups
                .map(group => group.children[0] as Mesh)
                .filter(mesh => (mesh.material as MeshBasicMaterial).opacity > 0),
            false);

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
            selection.selectBox2(null);
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

    function setup(width: number, height: number) {
        resize(width, height);

        {
            const axesHelper = new AxesHelper(10);
            scene.add(axesHelper);
        }

        {
            const domainGeometry = new PlaneGeometry(1, 1);
            const domainEdgesGeometry = new EdgesGeometry(domainGeometry);
            const domainEdgesMaterial = new LineBasicMaterial({color: 0x7f7f7f});
            const domainEdges = new LineSegments(domainEdgesGeometry, domainEdgesMaterial);
            domainEdges.position.set(0.5, 0.5, 0);
            scene.add(domainEdges);
        }

        return renderer.domElement;
    }

    function draw() {
        renderer.render(scene, camera);
    }

    function resize(width: number, height: number, zoom: number = 1.5) {
        const aspect = width / height;
        camera.left = (1 - zoom * aspect) / 2;
        camera.right = (1 + zoom * aspect) / 2;
        camera.top = (1 + zoom) / 2;
        camera.bottom = (1 - zoom) / 2;
        camera.updateProjectionMatrix();
        renderer.setSize(width, height);
        controls.update();
    }
</script>

<ThreeElement {setup} {resize} {draw} {onClick}/>
