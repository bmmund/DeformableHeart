/**
 * Created by braden on 2016-10-05.
 */
var scene, camera, renderer;
var cube;
var curve;

init();
render();

function init() {
    var width = window.innerWidth;
    var height = window.innerHeight;
    scene = new THREE.Scene();
    //camera = new THREE.PerspectiveCamera( 75, window.innerWidth/window.innerHeight, 0.1, 1000 );
    //camera = new THREE.OrthographicCamera( width / - 2, width / 2, height / 2, height / - 2, 1, 1000 );
    camera = new THREE.OrthographicCamera(-2, 2, 2, -2, -2, 2 );

    renderer = new THREE.WebGLRenderer();
    renderer.setSize( window.innerWidth, window.innerHeight );
    document.body.appendChild( renderer.domElement );

    var geometry = new THREE.BoxGeometry( 1, 1, 1 );
    var material = new THREE.MeshBasicMaterial( { color: 0x00ff00 } );
    cube = new THREE.Mesh( geometry, material );
    //scene.add( cube );
    curve = new THREE.CubicBezierCurve(
        new THREE.Vector3( -1, 0, 0 ),
        new THREE.Vector3( -0.5, 1, 0 ),
        new THREE.Vector3( 0.5, 1, 0 ),
        new THREE.Vector3( 1, 0, 0 )
    );

    var path = new THREE.Path( curve.getPoints( 50 ) );

    var geometry = path.createPointsGeometry( 50 );
    var material = new THREE.LineBasicMaterial( { color : 0xff0000 } );

    // Create the final Object3d to add to the scene
    var curveObject = new THREE.Line( geometry, material );
    scene.add(curveObject);

    //camera.position.z = 10;
}

function render() {
    requestAnimationFrame( render );

    renderer.render(scene, camera);
};