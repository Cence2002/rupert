<script lang="ts">
    import ThreeElement from "$lib/ThreeElement.svelte";
    import {State} from "$lib/state.svelte";
    import {PI, TWO_PI} from "$lib/geometry";

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
        Vector2,
        Raycaster,
        FrontSide, Color
    } from "three";
    import type {AbstractLoader} from "$lib/loader/loader";

    let {loader, state} = $props<{
        loader: AbstractLoader,
        state: State,
    }>();

    $effect(onSelectBox3);

    $effect(onSelectRectangle);

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

    function onSelectBox3(): () => void {
        if (state.selectedBox === null) {
            return () => {
            };
        }
        const rectangles = loader.getRectangles(state.selectedBox);
        for (let index = 0; index < rectangles.length; index++) {
            const rectangle = rectangles[index];

            const thetaInterval = rectangle.theta.interval;
            const phiInterval = rectangle.phi.interval;

            const rectangleGeometry = new PlaneGeometry(thetaInterval.len / TWO_PI, phiInterval.len / PI);
            const rectangleMaterial = new MeshBasicMaterial({
                color: new Color(0, 1, 0),
                transparent: true,
                // opacity: (index == box.inIndex()) ? 0.75 : (rectangle.outIndicesLength() > 0 ? 0.25 : 0.0),
                opacity: (index == loader.getHoleInIndex(state.selectedBox)) ? 0.75 : (loader.getPlugOutIndices(state.selectedBox, index).length > 0 ? 0.25 : 0.0),
                side: FrontSide,
                depthWrite: false
            });
            const rectangleMesh = new Mesh(rectangleGeometry, rectangleMaterial);
            rectangleMesh.position.set(thetaInterval.mid / TWO_PI, phiInterval.mid / PI, 0);

            const rectangleEdgesGeometry = new EdgesGeometry(rectangleGeometry);
            const rectangleEdgesMaterial = new LineBasicMaterial({
                color: new Color(0.5, 0.5, 0.5),
            });
            const rectangleEdges = new LineSegments(rectangleEdgesGeometry, rectangleEdgesMaterial);
            rectangleEdges.position.set(thetaInterval.mid / TWO_PI, phiInterval.mid / PI, 0);

            const rectangleGroup = new Group();
            rectangleGroup.add(rectangleMesh);
            if (loader.getPlugOutIndices(state.selectedBox, index).length > 0) {
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

    function onSelectRectangle(): () => void {
        if (state.selectedRectangle === null) {
            return () => {
            };
        }
        const rectangleGroup = rectangleGroups[state.selectedRectangle]!;
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

    function onDoubleClick(mouse: Vector2) {
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
            state.unselectRectangle();
            return;
        }

        if (state.selectedRectangle === null) {
            state.selectRectangle(rectangleGroups.findIndex(group => group.children[0] === intersections[0]!.object));
            return;
        }

        const selectedRectangle = rectangleGroups[state.selectedRectangle]!.children[0] as Mesh;
        const selectedRectangleIndex = intersections.findIndex(intersection => intersection.object === selectedRectangle);
        const newselectedRectangleIndex = selectedRectangleIndex === -1 ? 0 : (selectedRectangleIndex + 1) % intersections.length;
        const newselectedRectangle = intersections[newselectedRectangleIndex]!.object as Mesh;
        const newselectedRectangleIndexInGroups = rectangleGroups.findIndex(group => group.children[0] === newselectedRectangle);
        state.selectRectangle(newselectedRectangleIndexInGroups);
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

<ThreeElement {setup} {resize} {draw} {onDoubleClick}/>
