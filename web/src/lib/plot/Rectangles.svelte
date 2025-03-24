<script lang="ts">
    import ThreePlot from "$lib/ThreePlot.svelte";
    import {State} from "$lib/State.svelte";
    import {PI, TWO_PI} from "$lib/Geometry";

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
        FrontSide, Color, BufferGeometry, Float32BufferAttribute
    } from "three";
    import type {AbstractLoader} from "$lib/loader/AbstractLoader";
    import type {Interval} from "$lib/Types";

    const {loader, state} = $props<{
        loader: AbstractLoader,
        state: State,
    }>();

    $effect(onSelectBox);

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
    controls.screenSpacePanning = true;
    controls.enableRotate = false;
    controls.enableZoom = true;
    controls.zoomToCursor = true;
    controls.minZoom = 0.01;
    controls.maxZoom = 100;

    let rectangleGroups: Group[] = [];

    function onSelectBox(): () => void {
        if (state.selectedBox === null) {
            return () => {
            };
        }

        const rectangles = loader.getRectangles(state.selectedBox);
        for (let index = 0; index < rectangles.length; index++) {
            const rectangle = rectangles[index];

            const theta: Interval = rectangle.theta.interval;
            const phi: Interval = rectangle.phi.interval;

            const isIn = index == loader.getHoleInIndex(state.selectedBox);
            const isTerminal = loader.getPlugOutIndices(state.selectedBox, index).length > 0;

            const rectangleGeometry = new PlaneGeometry(theta.len / TWO_PI, phi.len / PI);
            const rectangleMaterial = new MeshBasicMaterial({
                color: new Color(0, 1, 0),
                transparent: true,
                opacity: isIn ? 0.75 : (isTerminal ? 0.25 : 0.05),
                side: FrontSide,
                depthWrite: false
            });
            const rectangleMesh = new Mesh(rectangleGeometry, rectangleMaterial);
            rectangleMesh.position.set(theta.mid / TWO_PI, phi.mid / PI, 0);

            const rectangleGroup = new Group();
            rectangleGroup.add(rectangleMesh);

            if (isTerminal) {
                const rectangleEdgesGeometry = new EdgesGeometry(rectangleGeometry);
                const rectangleEdgesMaterial = new LineBasicMaterial({
                    color: new Color(0.5, 0.5, 0.5),
                });
                const rectangleEdges = new LineSegments(rectangleEdgesGeometry, rectangleEdgesMaterial);
                rectangleEdges.position.set(theta.mid / TWO_PI, phi.mid / PI, 0);
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

        const rectangle = rectangleGroups[state.selectedRectangle]!.children[0] as Mesh;
        const rectangleMaterial = rectangle.material as MeshBasicMaterial;
        const originalColor = rectangleMaterial.color.clone();
        rectangleMaterial.color.copy(new Color(1, 0, 0));
        const originalOpacity = rectangleMaterial.opacity;
        rectangleMaterial.opacity = 0.75;

        return () => {
            rectangleMaterial.color.copy(originalColor);
            rectangleMaterial.opacity = originalOpacity;
        };
    }

    function onDoubleClick(mouse: Vector2) {
        const raycaster = new Raycaster();
        raycaster.setFromCamera(mouse, camera);

        const rectangles = rectangleGroups
        .map(group => group.children[0] as Mesh)
        .filter(mesh => (mesh.material as MeshBasicMaterial).opacity > 0);

        const intersections = raycaster.intersectObjects(rectangles, false);

        function getArea(geometry: PlaneGeometry) {
            const vertices = geometry.parameters;
            return vertices.width * vertices.height;
        }

        intersections.sort((intersection, otherIntersection) => {
            const area = getArea((intersection.object as Mesh).geometry as PlaneGeometry);
            const otherArea = getArea((otherIntersection.object as Mesh).geometry as PlaneGeometry);
            const areaDifference = area - otherArea;
            return -areaDifference;
        });

        if (intersections.length === 0) {
            state.unselectRectangle();
            return;
        }

        if (state.selectedRectangle === null) {
            const newRectangleIndex = rectangleGroups.findIndex(group => group.children[0] === intersections[0]!.object);
            state.selectRectangle(newRectangleIndex);
            return;
        }

        const rectangle = rectangleGroups[state.selectedRectangle]!.children[0] as Mesh;
        const rectangleIndex = intersections.findIndex(intersection => intersection.object === rectangle);
        const newRectangleIndex = rectangleIndex === -1 ? 0 : (rectangleIndex + 1) % intersections.length;
        const newRectangle = intersections[newRectangleIndex]!.object as Mesh;
        const newRectangleGroupIndex = rectangleGroups.findIndex(group => group.children[0] === newRectangle);
        state.selectRectangle(newRectangleGroupIndex);
    }

    function setup(width: number, height: number) {
        resize(width, height);

        const axesHelper = new AxesHelper(2);
        scene.add(axesHelper);

        const edgeBuffer = [
            1, 1, 0, 1, 0, 0,
            1, 1, 0, 0, 1, 0,
        ]
        const edgeGeometry = new BufferGeometry();
        edgeGeometry.setAttribute('position', new Float32BufferAttribute(edgeBuffer, 3));
        const edgeMaterial = new LineBasicMaterial({color: new Color(0.5, 0.5, 0.5)});
        const edge = new LineSegments(edgeGeometry, edgeMaterial);
        scene.add(edge);

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

<ThreePlot {setup} {resize} {draw} {onDoubleClick}/>
