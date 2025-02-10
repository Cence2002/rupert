<script lang="ts">
    import ThreeElement from "$lib/ThreeElement.svelte";
    import {BoxGeometry, Mesh, MeshBasicMaterial, PerspectiveCamera, Scene, WebGLRenderer, EdgesGeometry, LineBasicMaterial, LineSegments} from "three";
    import {OrbitControls} from 'three/addons/controls/OrbitControls.js';

    let camera: PerspectiveCamera;
    let scene: Scene;
    let renderer: WebGLRenderer;
    let cubes: Mesh[] = [];

    function setup(width: number, height: number) {
        scene = new Scene();
        camera = new PerspectiveCamera(60, width / height, 0.1, 1000);
        renderer = new WebGLRenderer({antialias: true});
        renderer.setSize(width, height);

        const material = new MeshBasicMaterial({color: 0x00ffff, transparent: true, opacity: 0.3});

        const n = 10;
        for (let x = 0; x < n; x++) {
            for (let y = 0; y < n; y++) {
                for (let z = 0; z < n; z++) {
                    const geometry = new BoxGeometry(2 / n, 2 / n, 2 / n);
                    const cube = new Mesh(geometry, material);
                    cube.position.set(
                        x / n * 2 - 1 + 1 / n,
                        y / n * 2 - 1 + 1 / n,
                        z / n * 2 - 1 + 1 / n,
                    );

                    if (cube.position.dot(cube.position) > 1.5 || cube.position.x ** 2 + cube.position.y ** 2 < 0.5) {
                        continue;
                    }

                    scene.add(cube);
                    cubes.push(cube);
                }
            }
        }

        const frameGeometry = new BoxGeometry(2, 2, 2);
        const edges = new EdgesGeometry(frameGeometry);
        const edgeMaterial = new LineBasicMaterial({color: 0xff0000});
        const cubeFrame = new LineSegments(edges, edgeMaterial);
        scene.add(cubeFrame);

        camera.position.set(1.5,1.5,3);

        const controls = new OrbitControls(camera, renderer.domElement);
        controls.enableRotate = true;
        controls.enableZoom = true;
        controls.enablePan = false;
        controls.enableDamping = false;

        return renderer.domElement;
    }

    function draw() {
        renderer.render(scene, camera);
    }

    function resize(width: number, height: number) {
        camera.aspect = width / height;
        camera.updateProjectionMatrix();
        renderer.setSize(width, height);
    }
</script>

<ThreeElement {setup} {resize} {draw}/>
