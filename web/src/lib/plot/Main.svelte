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
        OrthographicCamera, Vector2, Raycaster, Matrix4,
    } from "three";
    import {OrbitControls} from 'three/addons/controls/OrbitControls.js';
    import {ConvexGeometry} from 'three/addons/geometries/ConvexGeometry.js';
    import {ParametricGeometry} from 'three/addons/geometries/ParametricGeometry.js';
    import {AxesHelper} from "three";
    import {lerp} from "three/src/math/MathUtils.js";
    import type {AbstractLoader} from "$lib/loader/AbstractLoader";
    import {PHI, transformationMatrix, transformHoleVertex, transformPlugVertex, TWO_PI} from "$lib/Geometry";

    const {loader, state, getProjectionScene} = $props<{
        loader: AbstractLoader,
        state: State
        getProjectionScene: () => Scene | null
    }>();

    let projectionScene: Scene | null = null;

    $effect(onLoad);

    $effect(onSelectBox);

    $effect(onSelectRectangle);

    const scene = new Scene();
    scene.up.set(0, 0, 0);

    const camera = new OrthographicCamera(0, 0, 0, 0, 0.001, 100);
    camera.up.set(0, 0, 1);
    camera.lookAt(0, 0, 0);
    camera.position.set(0, -10, 0);

    const renderer = new WebGLRenderer({antialias: true});
    renderer.autoClear = false;

    const controls = new OrbitControls(camera, renderer.domElement);
    controls.target.set(0, 0, 0);
    controls.enablePan = true;
    controls.screenSpacePanning = true;
    controls.panSpeed = 0.5;
    controls.enableRotate = true;
    controls.rotateSpeed = 0.5;
    controls.enableZoom = true;
    controls.zoomToCursor = true;
    controls.minZoom = 0.01;
    controls.maxZoom = 100;

    let holeRadius: number;
    let holeGroup: Group;

    let plugRadius: number;
    let plugGroup: Group;

    let transformedHoleVertices: Mesh[] = [];
    let transformedPlugVertices: Mesh[] = [];

    const resolution = 4;

    const holeMaterial = new MeshBasicMaterial({
        color: new Color(0, 0, 1),
        transparent: true,
        opacity: 0.5,
    });

    const holeEdgesMaterial = new LineBasicMaterial({
        color: new Color(0.5, 0.5, 1.0),
    });

    const plugMaterial = new MeshBasicMaterial({
        color: new Color(0, 1, 0),
        transparent: true,
        opacity: 0.5,
    });

    const plugEdgesMaterial = new LineBasicMaterial({
        color: new Color(0.5, 1.0, 0.5),
    });

    class Animation {
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

    const holeAnimation: { theta: Animation, phi: Animation, alpha: Animation } = {
        theta: new Animation(animationSpeed / (Math.random() + 1)),
        phi: new Animation(animationSpeed / (Math.random() + 1)),
        alpha: new Animation(animationSpeed / (Math.random() + 1))
    };
    const plugAnimation: { theta: Animation, phi: Animation } = {
        theta: new Animation(animationSpeed / (Math.random() + 1)),
        phi: new Animation(animationSpeed / (Math.random() + 1))
    };

    function onLoad() {
        if (!state.loaded) {
            return;
        }

        {
            const vertices = loader.getHole();
            holeRadius = Math.max(...vertices.map((v: Vector3) => v.length()));

            const holeGeometry = new ConvexGeometry(vertices);
            const holeMesh = new Mesh(holeGeometry, holeMaterial);

            const holeEdges = new EdgesGeometry(holeGeometry);
            const holeEdgesMesh = new LineSegments(holeEdges, holeEdgesMaterial);

            holeGroup = new Group();
            holeGroup.add(holeMesh);
            holeGroup.add(holeEdgesMesh);
            holeGroup.position.set(0, 0, -holeRadius);
            scene.add(holeGroup);
        }

        {
            const vertices = loader.getPlug();
            plugRadius = Math.max(...vertices.map((v: Vector3) => v.length()));

            const plugGeometry = new ConvexGeometry(vertices);
            const plugMesh = new Mesh(plugGeometry, plugMaterial);

            const plugEdges = new EdgesGeometry(plugGeometry);
            const plugEdgesMesh = new LineSegments(plugEdges, plugEdgesMaterial);

            plugGroup = new Group();
            plugGroup.add(plugMesh);
            plugGroup.add(plugEdgesMesh);
            plugGroup.position.set(0, 0, plugRadius);
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
            for (const holeVertex of loaderHole) {
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

                for (const parametricFactory of parametricFactories) {
                    for (let index = 0; index <= resolution; index++) {
                        const parametric = parametricFactory(index);
                        const geometry = new ParametricGeometry(parametric, resolution, resolution);
                        const material = new MeshBasicMaterial({
                            color: new Color(0.0, 0.0, 1.0),
                            side: DoubleSide,
                        });
                        const mesh = new Mesh(geometry, material);
                        mesh.position.set(0, 0, -holeRadius);
                        transformedHoleVertices.push(mesh);
                    }
                }
            }
            for (const transformedHoleVertex of transformedHoleVertices) {
                scene.add(transformedHoleVertex);
            }
        }

        return () => {
            for (const transformedHoleVertex of transformedHoleVertices) {
                scene.remove(transformedHoleVertex);
            }
            transformedHoleVertices = [];
        };
    }

    function onSelectRectangle(): () => void {
        if (state.selectedRectangle === null) {
            return () => {
            };
        }

        {
            const rectangle = loader.getRectangle(state.selectedBox, state.selectedRectangle);
            for (const plugVertex of loader.getPlug()) {
                //TODO: handle the case when vertex is on the z-axis, as the parametric surface will collapse to a line
                function parametric(theta_t: number, phi_t: number, target: Vector3) {
                    transformPlugVertex(plugVertex, rectangle, theta_t, phi_t, target);
                }

                const transformedPlugVertexGeometry = new ParametricGeometry(parametric, resolution, resolution);
                const transformedPlugVertexMaterial = new MeshBasicMaterial({
                    color: new Color(0.0, 1.0, 0.0),
                    side: DoubleSide,
                });
                const transformedPlugVertex = new Mesh(transformedPlugVertexGeometry, transformedPlugVertexMaterial);
                transformedPlugVertex.position.set(0, 0, plugRadius);
                transformedPlugVertices.push(transformedPlugVertex);
            }
            for (const transformedPlugVertex of transformedPlugVertices) {
                scene.add(transformedPlugVertex);
            }
        }

        return () => {
            for (const transformedPlugVertex of transformedPlugVertices) {
                scene.remove(transformedPlugVertex);
            }
            transformedPlugVertices = [];
        };
    }

    function transformationQuaternion(theta: number, phi: number, alpha: number): Quaternion {
        const matrix = transformationMatrix(theta, phi, alpha);
        const rotationMatrix = new Matrix4();
        rotationMatrix.set(
            matrix.elements[0], matrix.elements[3], matrix.elements[6], 0,
            matrix.elements[1], matrix.elements[4], matrix.elements[7], 0,
            matrix.elements[2], matrix.elements[5], matrix.elements[8], 0,
            0, 0, 0, 1
        );
        const quaternion = new Quaternion();
        quaternion.setFromRotationMatrix(rotationMatrix);
        return quaternion;
    }

    function updateAnimation() {
        const currentTime = Date.now();
        const time = (currentTime - startTime) / 1000;

        if (holeGroup && state.selectedBox !== null) {
            const box = loader.getBox(state.selectedBox);

            holeGroup.quaternion.copy(transformationQuaternion(
                lerp(box.theta.interval.min, box.theta.interval.max, holeAnimation.theta.normalizedValue(time)),
                lerp(box.phi.interval.min, box.phi.interval.max, holeAnimation.phi.normalizedValue(time)),
                lerp(box.alpha.interval.min, box.alpha.interval.max, holeAnimation.alpha.normalizedValue(time))
            ));
        } else if (holeGroup) {
            holeGroup.quaternion.copy(transformationQuaternion(0, 0, 0));
        }

        if (plugGroup && state.selectedBox !== null && state.selectedRectangle !== null) {
            const rectangle = loader.getRectangle(state.selectedBox, state.selectedRectangle);

            plugGroup.quaternion.copy(transformationQuaternion(
                lerp(rectangle.theta.interval.min, rectangle.theta.interval.max, plugAnimation.theta.normalizedValue(time)),
                lerp(rectangle.phi.interval.min, rectangle.phi.interval.max, plugAnimation.phi.normalizedValue(time)),
                0
            ));
        } else if (plugGroup) {
            plugGroup.quaternion.copy(transformationQuaternion(0, 0, 0));
        }
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
        updateAnimation();

        renderer.clear();
        if (projectionScene !== null) {
            renderer.render(projectionScene, camera);
        } else {
            projectionScene = getProjectionScene();
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
