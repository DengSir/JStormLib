var log = require('npmlog');
var exec = require('child_process').exec;
var spawn = require('child_process').spawn;
var path = require('path');
var fs = require('fs');
function findMsbuild(cb) {
    var msbuildPath;
    log.verbose('could not find "msbuild.exe" in PATH - finding location in registry')
    var notfoundErr = 'Can\'t find "msbuild.exe". Do you have Microsoft Visual Studio C++ 2008+ installed?'
    var cmd = 'reg query "HKLM\\Software\\Microsoft\\MSBuild\\ToolsVersions" /s'
    if (process.arch !== 'ia32')
        cmd += ' /reg:32'
    exec(cmd, function (err, stdout, stderr) {
        if (err) {
            return callback(new Error(err.message + '\n' + notfoundErr))
        }
        var reVers = /ToolsVersions\\([^\\]+)$/i
            , rePath = /\r\n[ \t]+MSBuildToolsPath[ \t]+REG_SZ[ \t]+([^\r]+)/i
            , msbuilds = []
            , r
            , msbuildPath
        stdout.split('\r\n\r\n').forEach(function (l) {
            if (!l) return
            l = l.trim()
            if (r = reVers.exec(l.substring(0, l.indexOf('\r\n')))) {
                var ver = parseFloat(r[1], 10)
                if (ver >= 3.5) {
                    if (r = rePath.exec(l)) {
                        msbuilds.push({
                            version: ver,
                            path: r[1]
                        })
                    }
                }
            }
        })
        msbuilds.sort(function (x, y) {
            return (x.version < y.version ? -1 : 1)
        })
            ; (function verifyMsbuild() {
                if (!msbuilds.length) return callback(new Error(notfoundErr))
                msbuildPath = path.resolve(msbuilds.pop().path, 'msbuild.exe')
                fs.stat(msbuildPath, function (err, stat) {
                    if (err) {
                        if (err.code == 'ENOENT') {
                            if (msbuilds.length) {
                                return verifyMsbuild()
                            } else {
                                callback(new Error(notfoundErr))
                            }
                        } else {
                            callback(err)
                        }
                        return
                    }
                    cb(msbuildPath);
                })
            })()
    })
}

function findSln() {
    var slnPath = path.resolve(__dirname, '../StormLib/StormLib_vs15.sln');
    if (fs.existsSync(slnPath)) {
        return slnPath;
    }
    return void 2333;
}

findMsbuild(msbuildPath => {
    let buildType = 'ReleaseAS';
    let p = 'x64';
    let argv = ['/p:Configuration=' + buildType + ';Platform=' + p];
    var slnPath = findSln();
    if (slnPath) {
        argv.unshift(slnPath);
        spawn(msbuildPath, argv, { stdio: [0, 1, 2] });
    }
});

