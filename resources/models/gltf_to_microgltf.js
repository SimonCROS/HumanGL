const capitalize = (s) => String(s[0]).toUpperCase() + String(s).slice(1).toLowerCase();

let fs = require('fs');
let obj;
try {
    obj = JSON.parse(fs.readFileSync(process.argv[2], 'utf8'));
} catch (error) {
    console.error("Error reading the file");
    return;
}

delete obj.asset;
delete obj.images;
delete obj.materials;
delete obj.skins;
delete obj.samplers;
delete obj.textures;

for (let accessor of obj.accessors) {
    delete accessor.min;
    delete accessor.max;
    if (accessor.type)
        accessor.type = `microgltf::${capitalize(accessor.type)}`
    delete accessor.name
}

for (let animation of obj.animations) {
    for (let animationChannel of animation.channels) {
        animationChannel.target.path = `microgltf::Path${capitalize(animationChannel.target.path)}`
    }
    for (let animationSampler of animation.samplers) {
        animationSampler.interpolation = `microgltf::${capitalize(animationSampler.interpolation)}`
    }
    delete animation.name;
}

for (let mesh of obj.meshes) {
    for (let primitive of mesh.primitives) {
        delete primitive.material;
    }
    delete mesh.name
}

for (let node of obj.nodes) {
    if (node.translation)
        node.translation = [node.translation]
    if (node.rotation)
        node.rotation = [[node.rotation[3], ...node.rotation.slice(0, -1)]]
    if (node.scale)
        node.scale = [node.scale]
    if (node.matrix)
        node.matrix = [node.matrix]
    delete node.skin
    delete node.name
}

for (let bufferView of obj.bufferViews) {
    delete bufferView.name
}

for (let scene of obj.scenes) {
    delete scene.name
}

console.log(
    JSON
    .stringify(obj)
    .replaceAll('"zzname":', '"name":')
    .replaceAll('[', '{')
    .replaceAll(']', '}')
    .replaceAll(/(?<!"attributes":\s*{(?:[^}])*)"([^"]*)":/g, '.$1 = ')
    .replaceAll(/"(microgltf::[^"]*)"/g, '$1')
    .replaceAll(/("[^"]*")\s*:\s*(\d+)/g, '{$1,$2}')
);
