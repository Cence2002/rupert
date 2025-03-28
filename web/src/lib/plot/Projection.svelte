<script lang="ts">
    import ThreePlot from "$lib/ThreePlot.svelte";
    import {State} from "$lib/State.svelte";

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
    import {convexHull} from "$lib/Geometry";
    import type {AbstractLoader} from "$lib/loader/AbstractLoader";

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
    camera.lookAt(0, 0, 0);
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

    const holeVertexProjectionsMaterial = new MeshBasicMaterial({
        color: new Color(0, 0, 1),
        transparent: true,
        opacity: 0.5,
        side: DoubleSide,
        depthWrite: false
    });

    function onSelectBox(): () => void {
        if (state.selectedBox === null) {
            return () => {
            };
        }

        const holeProjectionEdges: Line[] = [];
        for (const holeProjectionEdgeData of loader.getHoleProjection(state.selectedBox)) {
            const holeProjectionEdgeGeometry = new BufferGeometry().setFromPoints([
                new Vector3(holeProjectionEdgeData.from.x, holeProjectionEdgeData.from.y, 0),
                new Vector3(holeProjectionEdgeData.to.x, holeProjectionEdgeData.to.y, 0)
            ]);
            const holeProjectionEdgeMaterial = new LineBasicMaterial({color: new Color(1, 1, 1)});
            const holeProjectionEdge = new Line(holeProjectionEdgeGeometry, holeProjectionEdgeMaterial);

            holeProjectionEdges.push(holeProjectionEdge);
        }

        for (const edge of holeProjectionEdges) {
            scene.add(edge);
        }

        const holeVertexProjectionGroups: Group[] = [];
        for (const holeVertexProjectionData of loader.getHoleVertexProjections(state.selectedBox)) {
            const holeVertexProjectionShape = new Shape(convexHull(holeVertexProjectionData));
            const holeVertexProjectionGeometry = new ShapeGeometry(holeVertexProjectionShape);
            const holeVertexProjection = new Mesh(holeVertexProjectionGeometry, holeVertexProjectionsMaterial);

            const holeVertexProjectionEdgesGeometry = new EdgesGeometry(holeVertexProjectionGeometry);
            const holeVertexProjectionEdgesMaterial = new LineBasicMaterial({color: new Color(0.5, 0.5, 0.5),});
            const holeVertexProjectionEdges = new LineSegments(holeVertexProjectionEdgesGeometry, holeVertexProjectionEdgesMaterial);

            const holeVertexProjectionGroup = new Group();
            holeVertexProjectionGroup.add(holeVertexProjection);
            holeVertexProjectionGroup.add(holeVertexProjectionEdges);

            holeVertexProjectionGroups.push(holeVertexProjectionGroup);
        }

        for (const holeVertexProjectionGroup of holeVertexProjectionGroups) {
            scene.add(holeVertexProjectionGroup);
        }

        return () => {
            for (const holeProjectionEdge of holeProjectionEdges) {
                scene.remove(holeProjectionEdge);
            }

            for (const holeVertexProjectionGroup of holeVertexProjectionGroups) {
                scene.remove(holeVertexProjectionGroup);
            }
        };
    }

    function onSelectRectangle(): () => void {
        if (state.selectedRectangle === null) {
            return () => {
            };
        }

        const outVertexIndices = loader.getOutVertexIndices(state.selectedBox, state.selectedRectangle);

        let plugVertexProjectionGroups: Group[] = [];
        const isInRectangle = state.selectedRectangle == loader.getInRectangleIndex(state.selectedBox);
        for (const [vertexIndex, plugVertexProjection] of loader.getPlugVertexProjections(state.selectedBox, state.selectedRectangle).entries()) {
            const isOutVertex = outVertexIndices.includes(vertexIndex);

            const plugVertexProjectionShape = new Shape(plugVertexProjection);
            const plugVertexProjectionGeometry = new ShapeGeometry(plugVertexProjectionShape);
            const plugVertexProjectionMaterial = new MeshBasicMaterial({
                color: new Color(0, 1, 0),
                transparent: true,
                opacity: (isInRectangle || isOutVertex) ? 0.5 : 0.25,
                side: DoubleSide,
                depthWrite: false,
            });
            const plugVertexProjectionMesh = new Mesh(plugVertexProjectionGeometry, plugVertexProjectionMaterial);

            const plugVertexProjectionEdgesGeometry = new EdgesGeometry(plugVertexProjectionGeometry);
            const plugVertexProjectionEdgesMaterial = new LineBasicMaterial({color: new Color(0.5, 0.5, 0.5)});
            const plugVertexProjectionEdges = new LineSegments(plugVertexProjectionEdgesGeometry, plugVertexProjectionEdgesMaterial);

            const plugVertexProjectionGroup = new Group();
            plugVertexProjectionGroup.add(plugVertexProjectionMesh);
            plugVertexProjectionGroup.add(plugVertexProjectionEdges);

            plugVertexProjectionGroups.push(plugVertexProjectionGroup);
        }
        for (const plugVertexProjection of plugVertexProjectionGroups) {
            scene.add(plugVertexProjection);
        }

        return () => {
            for (const plugVertexProjectionGroup of plugVertexProjectionGroups) {
                scene.remove(plugVertexProjectionGroup);
            }
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
