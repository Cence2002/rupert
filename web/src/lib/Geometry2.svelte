<script lang="ts">
    import ThreeElement from "$lib/ThreeElement.svelte";
    import {Cover} from "$lib/flatbuffers/flat-buffers/cover";
    import {Selection} from "$lib/state.svelte";

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
        FrontSide,
        Group,
        Vector3,
        Line,
        BufferGeometry,
        Color
    } from "three";
    import {convexHull} from "$lib/geometry";

    let {cover, selection} = $props<{
        cover: Cover | undefined,
        selection: Selection,
    }>();

    $effect(onCover);

    $effect(onSelectBox3);

    $effect(onSelectBox2);


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

    let projectionEdges: Line[] = [];
    let projections: Group[] = [];
    let box2Projections: Group[] = [];
    let box2Out: number[] = [];

    function onCover() {
        if (!cover) {
            return;
        }
    }

    function onSelectBox3() {
        if (selection.selectedBox3 === null) {
            return;
        }
        const box3 = cover!.box3s(selection.selectedBox3);

        for (let index = 0; index < box3.projectionsLength(); index++) {
            const projection = box3.projections(index);
            const vertices: Vector2[] = [];
            for (let index2 = 0; index2 < projection.vectorsLength(); index2++) {
                const vertex = projection.vectors(index2);
                vertices.push(new Vector2(vertex.x(), vertex.y()));
            }

            const hull = new Shape(convexHull(vertices));
            const hullGeometry = new ShapeGeometry(hull);
            const hullMaterial = new MeshBasicMaterial({
                color: new Color(0, 0, 1),
                transparent: true,
                opacity: 0.25,
                side: FrontSide,
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

        const projection = box3.projection();
        for (let index = 0; index < projection.edgesLength(); index++) {
            const edge = projection.edges(index);
            const from = new Vector2(edge.from().x(), edge.from().y());
            const to = new Vector2(edge.to().x(), edge.to().y());
            const edgeGeometry = new BufferGeometry().setFromPoints([
                new Vector3(from.x, from.y, 0),
                new Vector3(to.x, to.y, 0)]);
            const edgeMaterial = new LineBasicMaterial({
                color: new Color(1, 1, 1),
            });
            const edgeMesh = new Line(edgeGeometry, edgeMaterial);
            projectionEdges.push(edgeMesh);
        }

        for (let projectionEdge of projectionEdges) {
            scene.add(projectionEdge);
        }

        return () => {
            for (let group of projections) {
                scene.remove(group);
            }
            projections = [];
            for (let arrow of projectionEdges) {
                scene.remove(arrow);
            }
            projectionEdges = [];
        };
    }

    function onSelectBox2() {
        if (selection.selectedBox2 === null) {
            return;
        }
        const box3 = cover!.box3s(selection.selectedBox3);
        const box2 = box3.box2s(selection.selectedBox2);

        for (let index = 0; index < box2.outLength(); index++) {
            box2Out.push(box2.out(index));
        }

        for (let index = 0; index < box2.projectionsLength(); index++) {
            const projection = box2.projections(index);
            const vertices: Vector2[] = [];
            for (let index2 = 0; index2 < projection.vectorsLength(); index2++) {
                const vertex = projection.vectors(index2);
                vertices.push(new Vector2(vertex.x(), vertex.y()));
            }

            const hull = new Shape(vertices);
            const hullGeometry = new ShapeGeometry(hull);
            const hullMaterial = new MeshBasicMaterial({
                color: new Color(0, 1, 0),
                transparent: true,
                opacity: (selection.selectedBox2 == box3.in_()) ? 0.25 : (box2Out.includes(index) ? 0.5 : 0),
                side: FrontSide,
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

            box2Projections.push(group);
        }
        for (let group of box2Projections) {
            scene.add(group);
        }

        return () => {
            for (let group of box2Projections) {
                scene.remove(group);
            }
            box2Projections = [];
            box2Out = [];
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
