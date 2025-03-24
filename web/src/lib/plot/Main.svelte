<script lang="ts">
    import ThreePlot from "$lib/ThreePlot.svelte";
    import {State} from "$lib/State.svelte";

    import {
        Mesh,
        MeshBasicMaterial,
        Scene,
        WebGLRenderer,
        EdgesGeometry,
        LineBasicMaterial,
        LineSegments,
        Vector3,
        Group,
        Color,
        DoubleSide,
        Quaternion,
        OrthographicCamera,
    } from "three";
    import {OrbitControls} from 'three/addons/controls/OrbitControls.js';
    import {ConvexGeometry} from 'three/addons/geometries/ConvexGeometry.js';
    import {ParametricGeometry} from 'three/addons/geometries/ParametricGeometry.js';
    import {AxesHelper} from "three";
    import {lerp} from "three/src/math/MathUtils.js";
    import type {AbstractLoader} from "$lib/loader/AbstractLoader";
    import {transformHoleVertex, transformPlugVertex, TWO_PI} from "$lib/Geometry";

    const {loader, state, getProjectionScene} = $props<{
        loader: AbstractLoader,
        state: State
        getProjectionScene: () => Scene | null
    }>();

    $effect(onLoad);

    $effect(onSelectBox);

    $effect(onSelectRectangle);

    const scene = new Scene();
    scene.up.set(0, 0, 0);

    const camera = new OrthographicCamera(0, 0, 0, 0, 0.01, 100);
    camera.up.set(0, 0, 1);
    camera.lookAt(0, 0, 0);
    camera.position.set(0, -10, 0);

    const renderer = new WebGLRenderer({antialias: true});
    renderer.autoClear = false;

    const controls = new OrbitControls(camera, renderer.domElement);
    controls.target.set(0, 0, 0);
    controls.enablePan = true;
    controls.screenSpacePanning = true;
    controls.panSpeed = 0.1;
    controls.enableRotate = true;
    controls.rotateSpeed = 0.1;
    controls.enableZoom = true;
    controls.zoomToCursor = true;
    controls.minZoom = 0.01;
    controls.maxZoom = 100;

    let holeGroup: Group;
    let holeRadius: number;

    let plugGroup: Group;
    let plugRadius: number;

    let rotatedProjectedHoleVertices: Mesh[] = [];
    let projectedPlugVertices: Mesh[] = [];

    let rectangleOut: number[] = [];

    class Wave {
        private phase: number = Math.random() * Math.PI * 2;
        private readonly frequency: number;

        constructor(frequency: number) {
            this.frequency = frequency;
        }

        public value(time: number): number {
            return Math.sin(this.phase + this.frequency * time * TWO_PI);
        }

        public normalizedValue(time: number): number {
            return (this.value(time) + 1) / 2;
        }
    }

    const startTime: number = Date.now();
    const animationSpeed: number = 1;
    const holeAnimation: { theta: Wave, phi: Wave, alpha: Wave } = {
        theta: new Wave(animationSpeed / Math.sqrt(1)),
        phi: new Wave(animationSpeed / Math.sqrt(2)),
        alpha: new Wave(animationSpeed / Math.sqrt(3))
    };
    const plugAnimation: { theta: Wave, phi: Wave } = {
        theta: new Wave(animationSpeed / Math.sqrt(1)),
        phi: new Wave(animationSpeed / Math.sqrt(2))
    };

    function onLoad() {
        if (!state.loaded) {
            return;
        }
        {
            const vertices = loader.getHole();
            holeRadius = Math.max(...vertices.map((v: Vector3) => v.length()));
            camera.position.setZ(2 * holeRadius);
            camera.lookAt(0, 0, 2 * holeRadius);
            camera.updateProjectionMatrix();
            controls.target.set(0, 0, 2 * holeRadius);
            controls.update();

            const hole = new ConvexGeometry(vertices);
            const holeMaterial = new MeshBasicMaterial({
                color: new Color(0, 0, 1),
                transparent: true,
                opacity: 0.5
            });
            const holeMesh = new Mesh(hole, holeMaterial);

            const holeEdges = new EdgesGeometry(hole);
            const holeEdgesMaterial = new LineBasicMaterial({
                color: new Color(0.5, 0.5, 0.5)
            });
            const holeEdgesMesh = new LineSegments(holeEdges, holeEdgesMaterial);

            holeGroup = new Group();
            holeGroup.add(holeMesh);
            holeGroup.add(holeEdgesMesh);
            holeGroup.position.set(0, 0, holeRadius);
            scene.add(holeGroup);
        }
        {
            const vertices = loader.getPlug();
            plugRadius = Math.max(...vertices.map((v: Vector3) => v.length()));

            const plug = new ConvexGeometry(vertices);
            const plugMaterial = new MeshBasicMaterial({
                color: new Color(0, 1, 0),
                transparent: true,
                opacity: 0.5
            });
            const plugMesh = new Mesh(plug, plugMaterial);

            const plugEdges = new EdgesGeometry(plug);
            const plugEdgesMaterial = new LineBasicMaterial({
                color: new Color(0.5, 0.5, 0.5)
            });
            const plugEdgesMesh = new LineSegments(plugEdges, plugEdgesMaterial);

            plugGroup = new Group();
            plugGroup.add(plugMesh);
            plugGroup.add(plugEdgesMesh);
            plugGroup.position.set(0, 0, 2 * holeRadius + plugRadius);
            scene.add(plugGroup);
        }
    }

    function onSelectBox(): () => void {
        if (state.selectedBox === null) {
            return () => {
            };
        }

        {
            const box = loader.getBox(state.selectedBox);
            const loaderHole = loader.getHole();
            for (let index = 0; index < loaderHole.length; index++) {
                const holeVertex = loaderHole[index];

                const resolution = 8;

                const parametricFactories = [
                    (index: number) => (phi_t: number, alpha_t: number, target: Vector3) => {
                        transformHoleVertex(holeVertex, box, index / resolution, phi_t, alpha_t, target);
                    },
                    (index: number) => (theta_t: number, alpha_t: number, target: Vector3) => {
                        transformHoleVertex(holeVertex, box, theta_t, index / resolution, alpha_t, target);
                    },
                    (index: number) => (theta_t: number, phi_t: number, target: Vector3) => {
                        transformHoleVertex(holeVertex, box, theta_t, phi_t, index / resolution, target);
                    }
                ];

                for (const factory of parametricFactories) {
                    for (let i = 0; i <= resolution; i++) {
                        const parametric = factory(i);
                        const geometry = new ParametricGeometry(parametric, 8, 8);
                        const material = new MeshBasicMaterial({
                            color: new Color(0.0, 0.0, 1.0),
                            side: DoubleSide,
                        });
                        const mesh = new Mesh(geometry, material);
                        mesh.position.set(0, 0, holeRadius);
                        rotatedProjectedHoleVertices.push(mesh);
                    }
                }
            }
            for (const mesh of rotatedProjectedHoleVertices) {
                scene.add(mesh);
            }
        }

        return () => {
            for (const mesh of rotatedProjectedHoleVertices) {
                scene.remove(mesh);
            }
            rotatedProjectedHoleVertices = [];
        };
    }

    function onSelectRectangle(): () => void {
        if (state.selectedRectangle === null) {
            return () => {
            };
        }

        {
            const rectangleOutIndices = loader.getPlugOutIndices(state.selectedBox, state.selectedRectangle);
            for (let index = 0; index < rectangleOutIndices.length; index++) {
                rectangleOut.push(rectangleOutIndices[index]);
            }
        }

        {
            const rectangle = loader.getRectangle(state.selectedBox, state.selectedRectangle);
            const loaderPlug = loader.getPlug();
            for (let index = 0; index < loaderPlug.length; index++) {
                const holeVertex = loaderPlug[index];

                //TODO: handle the case when vertex is on the z-axis, as the parametric surface will collapse to a line
                function parametric(theta_t: number, phi_t: number, target: Vector3) {
                    transformPlugVertex(holeVertex, rectangle, theta_t, phi_t, target);
                }

                const projectedPlugVertexGeometry = new ParametricGeometry(parametric, 8, 8);
                const projectedPlugVertexMaterial = new MeshBasicMaterial({
                    color: new Color(0.0, 1.0, 0.0),
                    side: DoubleSide,
                });
                const projectedPlugVertex = new Mesh(projectedPlugVertexGeometry, projectedPlugVertexMaterial);
                projectedPlugVertex.position.set(0, 0, 2 * holeRadius + plugRadius);
                projectedPlugVertices.push(projectedPlugVertex);
            }
            for (let group of projectedPlugVertices) {
                scene.add(group);
            }
        }
        return () => {
            rectangleOut = [];
            for (let group of projectedPlugVertices) {
                scene.remove(group);
            }
            projectedPlugVertices = [];
        };
    }

    function projection_rotation_quaternion(theta: number, phi: number, alpha: number): Quaternion {
        const q_init_0 = new Quaternion().setFromAxisAngle(new Vector3(1, 0, 0), Math.PI);
        const q_init_1 = new Quaternion().setFromAxisAngle(new Vector3(0, 0, 1), Math.PI / 2);
        const q_theta = new Quaternion().setFromAxisAngle(new Vector3(0, 0, 1), theta);
        const q_phi = new Quaternion().setFromAxisAngle(new Vector3(1, 0, 0), -phi);
        const q_alpha = new Quaternion().setFromAxisAngle(new Vector3(0, 0, 1), alpha);
        const q_total = new Quaternion();
        q_total.copy(q_alpha).multiply(q_phi).multiply(q_theta).multiply(q_init_1).multiply(q_init_0);
        return q_total;
    }

    function updateAnimation() {
        const currentTime = Date.now();
        const time = (currentTime - startTime) / 1000;

        if (holeGroup && state.selectedBox !== null) {
            const box = loader.getBox(state.selectedBox);

            holeGroup.quaternion.copy(projection_rotation_quaternion(
                lerp(box.theta.interval.min, box.theta.interval.max, holeAnimation.theta.normalizedValue(time)),
                lerp(box.phi.interval.min, box.phi.interval.max, holeAnimation.phi.normalizedValue(time)),
                lerp(box.alpha.interval.min, box.alpha.interval.max, holeAnimation.alpha.normalizedValue(time))
            ));
        }
        if (plugGroup && state.selectedBox !== null && state.selectedRectangle !== null) {
            const rectangle = loader.getRectangle(state.selectedBox, state.selectedRectangle);

            plugGroup.quaternion.copy(projection_rotation_quaternion(
                lerp(rectangle.theta.interval.min, rectangle.theta.interval.max, plugAnimation.theta.normalizedValue(time)),
                lerp(rectangle.phi.interval.min, rectangle.phi.interval.max, plugAnimation.phi.normalizedValue(time)),
                0
            ));
        }
    }


    function setup(width: number, height: number) {
        resize(width, height);

        const axesHelper = new AxesHelper(2);
        scene.add(axesHelper);

        return renderer.domElement;
    }

    function draw() {
        updateAnimation();

        renderer.clear();
        const projectionScene = getProjectionScene();
        if (projectionScene) {
            renderer.render(projectionScene, camera);
        }
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
