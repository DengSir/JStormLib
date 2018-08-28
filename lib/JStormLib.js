const fs = require('fs');
const path = require('path');
var addon = fs.existsSync(path.resolve(__dirname, 'StormLib.node')) ?
    require('./StormLib.node') :
    require('../build/Release/StormLib.node');

module.exports = {
    open: function (archiveFie) {
        return new Promise((resolve, reject) => {
            addon.open(archiveFie, (err, stormLib, handle) => {
                if (err) {
                    reject(err);
                } else {
                    setPromiseMethod(stormLib);
                    resolve(stormLib);
                }
            });
        })
    },

    create: function (archiveFile, flags, maxFileCount, callback = () => { }) {
        return new Promise((resolve, reject) => {
            addon.create(archiveFile, flags, maxFileCount, (err, stormLib) => {
                if (err) {
                    reject(err);
                } else {
                    setPromiseMethod(stormLib);
                    resolve(stormLib);
                }
            });
        });
    },

    createSc: function (archiveFie, callback) {
        return new Promise((resolve, reject) => {
            addon.create(archiveFie, 0, 1024, (err, stormLib) => {
                if (err) {
                    reject(err);
                } else {
                    setPromiseMethod(stormLib);
                    resolve(stormLib);
                }
            });
        });
    }
}

function setPromiseMethod(stormLib) {
    var _readFile = stormLib.__proto__.readFile;
    stormLib.readFile = stormLib.__proto__.readFile = function (filename) {
        return new Promise((resolve, reject) => {
            _readFile.call(this, filename, (err, buffer) => {
                if (err)
                    reject(err);
                else
                    resolve(buffer);
            });
        });
    }

    var _writeFile = stormLib.__proto__.writeFile;
    stormLib.writeFile = stormLib.__proto__.writeFile = function (buffer, filename, lcid = 0, flags = 0, compression = 0) {
        return new Promise((resolve, reject) => {
            _writeFile.call(this, buffer, filename, lcid, flags, compression, (err) => {
                if (err)
                    reject(err);
                else
                    resolve();
            });
        });
    }

    stormLib.writeScFile = function (buffer, filename, lcid = 0) {
        return stormLib.writeFile(buffer, filename, lcid, 0x00010200, 0x08);
    }
}

