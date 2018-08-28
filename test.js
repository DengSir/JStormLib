const JStormLib = require('./lib/JStormLib');
const fs = require('fs');

JStormLib.createSc("d:/dddaaa.scx").then((archive) => {
    var file = fs.readFileSync('d:/scenario.chk');
    archive.writeScFile(file, 'staredit\\scenario.chk').then(() => {
        console.log('ok');
    }, err => {
        console.log(err);
    })
}, (error) => {
    console.log(error);
});
