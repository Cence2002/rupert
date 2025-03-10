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

    $effect(onSelectRectangle);

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

    let rectangleGroups: Group[] = [];

    function onCover() {
        if (!cover) {
            return;
        }
    }

    function onSelectBox3() {
        if (selection.selectedBox3 === null) {
            return;
        }
        const box = cover!.boxes(selection.selectedBox3)
        for (let index = 0; index < box.rectanglesLength(); index++) {
            const rectangle = box.rectangles(index);

            const theta = rectangle.theta().interval();
            const phi = rectangle.phi().interval();

            const rectangleGeometry = new PlaneGeometry((theta.max() - theta.min()) / TWO_PI, (phi.max() - phi.min()) / PI);
            const rectangleMaterial = new MeshBasicMaterial({
                color: new Color(0, 1, 0),
                transparent: true,
                opacity: (index == box.in_()) ? 0.75 : (rectangle.outLength() > 0 ? 0.25 : 0.0),
                side: FrontSide,
                depthWrite: false
            });
            const rectangleMesh = new Mesh(rectangleGeometry, rectangleMaterial);
            rectangleMesh.position.set((theta.min() + theta.max()) / 2 / TWO_PI, (phi.min() + phi.max()) / 2 / PI, 0);

            const rectangleEdgesGeometry = new EdgesGeometry(rectangleGeometry);
            const rectangleEdgesMaterial = new LineBasicMaterial({
                color: new Color(0.5, 0.5, 0.5),
            });
            const rectangleEdges = new LineSegments(rectangleEdgesGeometry, rectangleEdgesMaterial);
            rectangleEdges.position.set((theta.min() + theta.max()) / 2 / TWO_PI, (phi.min() + phi.max()) / 2 / PI, 0);

            const rectangleGroup = new Group();
            rectangleGroup.add(rectangleMesh);
            if (rectangle.outLength() > 0) {
                rectangleGroup.add(rectangleEdges);
            }

            rectangleGroups.push(rectangleGroup);
        }

        for (const rectangleGroup of rectangleGroups.values()) {
            scene.add(rectangleGroup);
        }

        return () => {
            for (const rectangleGroup of rectangleGroups.values()) {
                scene.remove(rectangleGroup);
            }
            rectangleGroups = [];
        };
    }

    function onSelectRectangle() {
        if (selection.selectedRectangle === null) {
            return;
        }
        const rectangleGroup = rectangleGroups[selection.selectedRectangle];
        const rectangle = rectangleGroup.children[0] as Mesh;
        const rectangleMaterial = rectangle.material as MeshBasicMaterial;
        const originalColor = rectangleMaterial.color.clone();
        rectangleMaterial.color.copy(new Color(1, 0, 0));
        const originalOpacity = rectangleMaterial.opacity;
        rectangleMaterial.opacity = 0.9;
        return () => {
            rectangleMaterial.color.copy(originalColor);
            rectangleMaterial.opacity = originalOpacity;
        };
    }

    function onClick(mouse: Vector2) {
        const raycaster = new Raycaster();
        raycaster.setFromCamera(mouse, camera);

        const intersections = raycaster.intersectObjects(rectangleGroups
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
            selection.selectRectangle(null);
            return;
        }

        if (selection.selectedRectangle === null) {
            selection.selectRectangle(rectangleGroups.findIndex(group => group.children[0] === intersections[0].object));
            return;
        }

        const selectedRectangle = rectangleGroups[selection.selectedRectangle].children[0] as Mesh;
        const selectedRectangleIndex = intersections.findIndex(intersection => intersection.object === selectedRectangle);
        const newSelectedRectangleIndex = selectedRectangleIndex === -1 ? 0 : (selectedRectangleIndex + 1) % intersections.length;
        const newSelectedRectangle = intersections[newSelectedRectangleIndex].object as Mesh;
        const newSelectedRectangleIndexInGroups = rectangleGroups.findIndex(group => group.children[0] === newSelectedRectangle);
        selection.selectRectangle(newSelectedRectangleIndexInGroups);
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
