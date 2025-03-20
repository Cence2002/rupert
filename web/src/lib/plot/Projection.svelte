<script lang="ts">
    import ThreeElement from "$lib/ThreeElement.svelte";
    import {State} from "$lib/state.svelte";

    import {MapControls} from "three/addons/controls/MapControls.js";
    import {
        Scene,
        WebGLRenderer,
        OrthographicCamera,
        Vector2,
        Shape,
        ShapeGeometry,
        MeshBasicMaterial,
        Mesh,
        EdgesGeometry,
        LineBasicMaterial,
        LineSegments,
        AxesHelper,
        Group,
        Vector3,
        Line,
        BufferGeometry,
        Color,
        DoubleSide
    } from "three";
    import {convexHull} from "$lib/geometry";
    import type {AbstractLoader} from "$lib/loader/loader";

    let {loader, selection, setProjectionScene} = $props<{
        loader: AbstractLoader,
        selection: State,
        setProjectionScene: (scene: Scene) => void,
    }>();

    $effect(onSelectBox3);

    $effect(onSelectRectangle);


    const scene = new Scene();
    setProjectionScene(scene);
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

    let projectionEdges: Line[] = [];
    let projections: Group[] = [];
    let rectangleProjections: Group[] = [];
    let rectangleOut: number[] = [];

    function onSelectBox3(): () => void {
        if (selection.selectedBox3 === null) {
            return () => {
            };
        }

        {
            const vertexProjections = loader.getHoleVertexProjections(selection.selectedBox3);
            for (let index = 0; index < vertexProjections.length; index++) {
                const vertices: Vector2[] = vertexProjections[index];

                const hull = new Shape(convexHull(vertices));
                const hullGeometry = new ShapeGeometry(hull);
                const hullMaterial = new MeshBasicMaterial({
                    color: new Color(0, 0, 1),
                    transparent: true,
                    opacity: 0.25,
                    side: DoubleSide,
                    depthWrite: false
                });
                const hullMesh = new Mesh(hullGeometry, hullMaterial);

                const hullEdgesGeometry = new EdgesGeometry(hullGeometry);
                const hullEdgesMaterial = new LineBasicMaterial({
                    color: new Color(0.5, 0.5, 0.5),
                });
                const hullEdges = new LineSegments(hullEdgesGeometry, hullEdgesMaterial);

                const group = new Group();
                group.add(hullMesh);
                group.add(hullEdges);

                projections.push(group);
            }
            for (let group of projections) {
                scene.add(group);
            }
        }

        {
            const projection = loader.getHoleProjectionHull(selection.selectedBox3);
            for (let index = 0; index < projection.length; index++) {
                const edge = projection[index];
                const edgeGeometry = new BufferGeometry().setFromPoints([
                    new Vector3(edge.from.x, edge.from.y, 0),
                    new Vector3(edge.to.x, edge.to.y, 0)
                ]);
                const edgeMaterial = new LineBasicMaterial({
                    color: new Color(1, 1, 1),
                });
                const edgeMesh = new Line(edgeGeometry, edgeMaterial);
                projectionEdges.push(edgeMesh);
            }

            for (let projectionEdge of projectionEdges) {
                scene.add(projectionEdge);
            }
        }

        return () => {
            for (let group of projections) {
                scene.remove(group);
            }
            projections = [];
            for (let edge of projectionEdges) {
                scene.remove(edge);
            }
            projectionEdges = [];
        };
    }

    function onSelectRectangle(): () => void {
        if (selection.selectedRectangle === null) {
            return () => {
            };
        }

        {
            const rectangleOutIndices = loader.getPlugOutIndices(selection.selectedBox3, selection.selectedRectangle);
            for (let index = 0; index < rectangleOutIndices.length; index++) {
                rectangleOut.push(rectangleOutIndices[index]);
            }
        }

        {
            const loaderRectangleProjections = loader.getPlugVertexProjections(selection.selectedBox3, selection.selectedRectangle);
            for (let index = 0; index < loaderRectangleProjections.length; index++) {
                const vertices = loaderRectangleProjections[index];

                const hull = new Shape(vertices);
                const hullGeometry = new ShapeGeometry(hull);
                const hullMaterial = new MeshBasicMaterial({
                    color: new Color(0, 1, 0),
                    transparent: true,
                    opacity: (selection.selectedRectangle == loader.getHoleInIndex(selection.selectedBox3)) ? 0.25 : (rectangleOut.includes(index) ? 0.5 : 0),
                    side: DoubleSide,
                    depthWrite: false,
                });
                const hullMesh = new Mesh(hullGeometry, hullMaterial);

                const hullEdgesGeometry = new EdgesGeometry(hullGeometry);
                const hullEdgesMaterial = new LineBasicMaterial({
                    color: new Color(0.5, 0.5, 0.5),
                });
                const hullEdges = new LineSegments(hullEdgesGeometry, hullEdgesMaterial);

                const group = new Group();
                group.add(hullMesh);
                group.add(hullEdges);

                rectangleProjections.push(group);
            }
            for (let group of rectangleProjections) {
                scene.add(group);
            }
        }

        return () => {
            for (let group of rectangleProjections) {
                scene.remove(group);
            }
            rectangleProjections = [];
            rectangleOut = [];
        };
    }

    function setup(width: number, height: number) {
        resize(width, height);

        {
            const axesHelper = new AxesHelper(10);
            scene.add(axesHelper);
        }

        return renderer.domElement;
    }

    function draw() {
        renderer.render(scene, camera);
    }

    function resize(width: number, height: number, zoom: number = 8) {
        const aspect = width / height;
        camera.left = -zoom * aspect / 2;
        camera.right = zoom * aspect / 2;
        camera.top = zoom / 2;
        camera.bottom = -zoom / 2;
        camera.updateProjectionMatrix();
        renderer.setSize(width, height);
        controls.update();
    }
</script>

<ThreeElement {setup} {resize} {draw}/>
