<script lang="ts">
    import ThreePlot from "$lib/ThreePlot.svelte";
    import {State} from "$lib/state.svelte";

    import {MapControls} from "three/addons/controls/MapControls.js";
    import {
        Scene,
        WebGLRenderer,
        OrthographicCamera,
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

    const {loader, state, setProjectionScene} = $props<{
        loader: AbstractLoader,
        state: State,
        setProjectionScene: (scene: Scene) => void,
    }>();

    $effect(onSelectBox);

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
    controls.screenSpacePanning = true;
    controls.enableZoom = true;
    controls.zoomToCursor = true;
    controls.enableRotate = false;
    controls.minZoom = 0.5;
    controls.maxZoom = 50;

    let holeProjection: Line[] = [];
    let holeVertexProjections: Group[] = [];
    let plugVertexProjections: Group[] = [];
    let plugOutIndices: number[] = [];

    function onSelectBox(): () => void {
        if (state.selectedBox === null) {
            return () => {
            };
        }

        {
            for (const holeVertexProjection of loader.getHoleVertexProjections(state.selectedBox)) {
                const hull = new Shape(convexHull(holeVertexProjection));
                const hullGeometry = new ShapeGeometry(hull);
                const hullMaterial = new MeshBasicMaterial({
                    color: new Color(0, 0, 1),
                    transparent: true,
                    opacity: 0.5,
                    side: DoubleSide,
                    depthWrite: false
                });
                const hullMesh = new Mesh(hullGeometry, hullMaterial);

                const hullEdgesGeometry = new EdgesGeometry(hullGeometry);
                const hullEdgesMaterial = new LineBasicMaterial({color: new Color(0.5, 0.5, 0.5),});
                const hullEdges = new LineSegments(hullEdgesGeometry, hullEdgesMaterial);

                const group = new Group();
                group.add(hullMesh);
                group.add(hullEdges);

                holeVertexProjections.push(group);
            }

            for (const holeVertexProjection of holeVertexProjections) {
                scene.add(holeVertexProjection);
            }
        }

        {
            for (const edge of loader.getHoleProjection(state.selectedBox)) {
                const edgeGeometry = new BufferGeometry().setFromPoints([
                    new Vector3(edge.from.x, edge.from.y, 0),
                    new Vector3(edge.to.x, edge.to.y, 0)
                ]);
                const edgeMaterial = new LineBasicMaterial({color: new Color(1, 1, 1)});
                const edgeMesh = new Line(edgeGeometry, edgeMaterial);
                holeProjection.push(edgeMesh);
            }

            for (const edge of holeProjection) {
                scene.add(edge);
            }
        }

        return () => {
            for (const group of holeVertexProjections) {
                scene.remove(group);
            }
            holeVertexProjections = [];

            for (const edge of holeProjection) {
                scene.remove(edge);
            }
            holeProjection = [];
        };
    }

    function onSelectRectangle(): () => void {
        if (state.selectedRectangle === null) {
            return () => {
            };
        }

        {
            for (const rectangleOutIndex of loader.getPlugOutIndices(state.selectedBox, state.selectedRectangle)) {
                plugOutIndices.push(rectangleOutIndex);
            }
        }

        {
            const loaderRectangleProjections = loader.getPlugVertexProjections(state.selectedBox, state.selectedRectangle);
            const isIn = state.selectedRectangle == loader.getHoleInIndex(state.selectedBox);
            for (let index = 0; index < loaderRectangleProjections.length; index++) {
                const vertices = loaderRectangleProjections[index];

                const isOut = plugOutIndices.includes(index);

                const hull = new Shape(vertices);
                const hullGeometry = new ShapeGeometry(hull);
                const hullMaterial = new MeshBasicMaterial({
                    color: new Color(0, 1, 0),
                    transparent: true,
                    opacity: isIn ? 0.25 : (isOut ? 0.75 : 0.05),
                    side: DoubleSide,
                    depthWrite: false,
                });
                const hullMesh = new Mesh(hullGeometry, hullMaterial);

                const hullEdgesGeometry = new EdgesGeometry(hullGeometry);
                const hullEdgesMaterial = new LineBasicMaterial({color: new Color(0.5, 0.5, 0.5)});
                const hullEdges = new LineSegments(hullEdgesGeometry, hullEdgesMaterial);

                const group = new Group();
                group.add(hullMesh);
                group.add(hullEdges);

                plugVertexProjections.push(group);
            }
            for (const plugVertexProjection of plugVertexProjections) {
                scene.add(plugVertexProjection);
            }
        }

        return () => {
            for (const plugVertexProjection of plugVertexProjections) {
                scene.remove(plugVertexProjection);
            }
            plugVertexProjections = [];

            plugOutIndices = [];
        };
    }

    function setup(width: number, height: number) {
        resize(width, height);

        {
            const axesHelper = new AxesHelper(2);
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

<ThreePlot {setup} {resize} {draw}/>
