const fs = require('fs-extra');
const path = require('path');

const nodeAddon = './build/Release/StormLib.node';
if (fs.existsSync(nodeAddon)) {
    fs.copy(nodeAddon, './lib/StormLib.node');
}