import * as THREE from 'three';
import { OrbitControls } from 'three/addons/controls/OrbitControls.js';

// Scene setup
const scene = new THREE.Scene();
scene.background = new THREE.Color(0x87ceeb); // Sky blue

// Camera setup
const camera = new THREE.PerspectiveCamera(
    75, // FOV
    window.innerWidth / window.innerHeight, // Aspect ratio
    0.1, // Near plane
    1000 // Far plane
);
camera.position.z = 10;

// Add objects
const geoCube = new THREE.BoxGeometry();
const material = new THREE.MeshPhongMaterial({
    color: 0x00ff00,
    shininess: 60
});
const cube = new THREE.Mesh(geoCube, material);
cube.rotation.x = 0.5;
cube.rotation.y = 0.5;
scene.add(cube);

// Add a second cube
const blueCubeMaterial = new THREE.MeshPhongMaterial({
    color: 0x0000ff, // Blue color
    shininess: 60
});
const blueCube = new THREE.Mesh(geoCube, blueCubeMaterial);
blueCube.position.x = 2; // Position it to the side of the first cube
scene.add(blueCube);

const blueCubeTwo = new THREE.Mesh(geoCube, blueCubeMaterial);
blueCubeTwo.position.x = -2;
scene.add(blueCubeTwo);

const geoSphere = new THREE.SphereGeometry();
const matSphere = new THREE.MeshPhongMaterial({
    color: 0xff0000,
    shininess: 100
});
const sphere = new THREE.Mesh(geoSphere, matSphere);
sphere.position.y = 3;
scene.add(sphere);
let sphereRadius = 1;
let radiusRange = 1;
let omega = Math.PI;

// Add lighting
const ambientLight = new THREE.AmbientLight(0x404040);
scene.add(ambientLight);

const directionalLight = new THREE.DirectionalLight(0xffffff, 1);
directionalLight.position.set(1, 1, 1);
scene.add(directionalLight);

// Renderer setup
const renderer = new THREE.WebGLRenderer({ antialias: true });
renderer.setSize(window.innerWidth, window.innerHeight);
renderer.setPixelRatio(window.devicePixelRatio);
document.body.appendChild(renderer.domElement);

// Controls
const controls = new OrbitControls(camera, renderer.domElement);
controls.enableDamping = true;

// Animation update
function animate() {
    requestAnimationFrame(animate);

    cube.rotation.x += 0.01;
    blueCube.rotation.y += 0.01;
    blueCubeTwo.rotation.x += 0.01;
    blueCubeTwo.rotation.y += 0.01;

    const time = Date.now() * 0.001;
    // torus.rotation.x += Math.sin(time) * 0.5;

    // Try scaling the sphere uniformly with a cosine function with sphere.scale.set()
    const scale = sphereRadius + (radiusRange * Math.cos(omega*time));
    sphere.scale.set(scale, scale, scale);

    controls.update();
    renderer.render(scene, camera);
}
animate();

// Handle window resize
window.addEventListener('resize', () => {
    camera.aspect = window.innerWidth / window.innerHeight;
    camera.updateProjectionMatrix();
    renderer.setSize(window.innerWidth, window.innerHeight);
});