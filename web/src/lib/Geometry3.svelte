<script lang="ts">
    import ThreeElement from "$lib/ThreeElement.svelte";
    import {Cover} from "$lib/flatbuffers/flat-buffers/cover";
    import {Selection} from "$lib/state.svelte";

    import {OrbitControls} from 'three/addons/controls/OrbitControls.js';
    import {Mesh, MeshBasicMaterial, PerspectiveCamera, Scene, WebGLRenderer, EdgesGeometry, LineBasicMaterial, LineSegments, Vector3, Group} from "three";
    import {ConvexGeometry} from 'three/addons/geometries/ConvexGeometry.js';
    import {AxesHelper} from "three";

    let {cover, selection} = $props<{
        cover: Cover | undefined,
        selection: Selection,
    }>();

    $effect(onCover);

    $effect(onSelectBox3);

    $effect(onSelectBox2);

    let camera: PerspectiveCamera;
    let scene: Scene;
    let renderer: WebGLRenderer;
    let holeGroup: Group | undefined;
    let plugGroup: Group | undefined;

    const holeColor = new Vector3(0, 0, 1);
    const plugColor = new Vector3(0, 1, 0);
    const edgeColor = new Vector3(0.5, 0.5, 0.5);

    function onCover() {
        if (!cover) {
            return;
        }
        let holeRadius: number;
        {
            const coverHole = cover!.hole();
            let vertices: Vector3[] = [];
            for (let index = 0; index < coverHole.verticesLength(); index++) {
                const vertex = coverHole.vertices(index);
                vertices.push(new Vector3(vertex.x(), vertex.y(), vertex.z()));
            }
            holeRadius = Math.max(...vertices.map(v => v.length()));

            const hole = new ConvexGeometry(vertices);
            const holeMaterial = new MeshBasicMaterial({transparent: true, opacity: 0.5});
            holeMaterial.color.setFromVector3(holeColor);
            const holeMesh = new Mesh(hole, holeMaterial);

            const holeEdges = new EdgesGeometry(hole);
            const holeEdgesMaterial = new LineBasicMaterial();
            holeEdgesMaterial.color.setFromVector3(edgeColor);
            const holeEdgesMesh = new LineSegments(holeEdges, holeEdgesMaterial);

            holeGroup = new Group();
            holeGroup.add(holeMesh);
            holeGroup.add(holeEdgesMesh);
            holeGroup.position.set(0, 0, holeRadius);
            scene.add(holeGroup);
        }
        let plugRadius: number;
        {
            const coverPlug = cover!.plug();
            let vertices = [];
            for (let index = 0; index < coverPlug.verticesLength(); index++) {
                const vertex = coverPlug.vertices(index);
                vertices.push(new Vector3(vertex.x(), vertex.y(), vertex.z()));
            }
            plugRadius = Math.max(...vertices.map(v => v.length()));

            const plug = new ConvexGeometry(vertices);
            const plugMaterial = new MeshBasicMaterial({transparent: true, opacity: 0.5});
            plugMaterial.color.setFromVector3(plugColor);
            let plugMesh = new Mesh(plug, plugMaterial);

            const plugEdges = new EdgesGeometry(plug);
            const plugEdgesMaterial = new LineBasicMaterial();
            plugEdgesMaterial.color.setFromVector3(edgeColor);
            let plugEdgesMesh = new LineSegments(plugEdges, plugEdgesMaterial);

            plugGroup = new Group();
            plugGroup.add(plugMesh);
            plugGroup.add(plugEdgesMesh);
            plugGroup.position.set(0, 0, 2 * holeRadius + plugRadius);
            scene.add(plugGroup);
        }
    }

    function onSelectBox3() {
        return () => {
        };
    }

    function onSelectBox2() {
        return () => {
        };
    }

    function setup(width: number, height: number) {
        {
            scene = new Scene();
            scene.up.set(0, 0, 1);
        }

        {
            camera = new PerspectiveCamera(60, width / height, 0.1, 1000);
            camera.up.set(0, 0, 1);
            camera.lookAt(0, 0, 2);
            camera.position.set(0, -8, 3);
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
            controls.target.set(0, 0, 3);
            controls.update();
        }

        {
            const axesHelper = new AxesHelper(10);
            scene.add(axesHelper);
        }

        return renderer.domElement;
    }

    function draw() {
        renderer.render(scene, camera);
        if (holeGroup) {
            holeGroup.rotation.x += 0.002;
            holeGroup.rotation.y += 0.004;
            holeGroup.rotation.z += 0.006;
        }
        if (plugGroup) {
            plugGroup.rotation.x -= 0.002;
            plugGroup.rotation.y -= 0.004;
            plugGroup.rotation.z -= 0.006;
        }
    }

    function resize(width: number, height: number) {
        camera.aspect = width / height;
        camera.updateProjectionMatrix();
        renderer.setSize(width, height);
    }
</script>

<ThreeElement {setup} {resize} {draw}/>
