# JStormLib
NodeJS wrapper for StormLib. see(https://github.com/ladislav-zezula/StormLib). 

I just use it to generate StarCraft1 maps, so  no more options. 
Currently, just only working on windows.

# Installation
#### You need to compile StormLib(ReleaseAS|x64) first.
#### You also need to install node-gyp(https://github.com/nodejs/node-gyp)
#### install Nan(https://github.com/nodejs/nan)

````bash
npm install
````

# Build
````bash
node-gyp build
````

# TODO
Support MacOSX, Linux


# API

#### <code>JStormLib.open</code>
Open a MPQ archive, return a Promise.
````jsx
JStormLib.open(filepath).then(arcive => {
    //archive.
}, err => {
    console.log(err)
});
````


#### <code>JStormLib.create</code>
Create a new MPQ archive.
````jsx
JStormLib.create(filepath, flags, maxFileCount).then(arcive => {
    //archive.
}, err => {
    console.log(err)
});
````

|Name|Type|Description|
|----|----|-----------|
|filepath|string|Archive file name
|flags|int|Additional flags to specify creation details. <a href="#createrrchiveflags">see CreateArchiveFlags</a>
|maxFileCount|int|Limit for file count

##### CreateArchiveFlags
Specifies additional flags for MPQ creation process. This parameter can be combination of the following flags:

|Value|Description|
|-----|-----------|
|0x00100000|The newly created archive will have (listfile) present.|
|0x00200000|The newly created archive will have additional attributes in (attributes) file.|
|0x00400000|The newly created archive will be signed with weak digital signature (the "(signature) file).|
|0x00000000|The function creates a MPQ version 1.0 (up to 4 GB). This is the default value|
|0x01000000|The function creates a MPQ version 2.0 (supports MPQ of size greater than 4 GB).|
|0x02000000|The function creates a MPQ version 3.0 (introduced in WoW-Cataclysm Beta).|
|0x03000000|The function creates a MPQ version 4.0 (used in WoW-Cataclysm).|


#### <code>StormArchive.getLocale</code>
````js
JStormLib.open(filepath).then(arcive => {
    archive.getLocale();
}, err => {
    console.log(err)
});
````
Returns current locale ID for adding new files

#### <code>StormArchive.setLocale</code>
````js
archive.setLocale(lcid);
````
Changes default locale ID for adding new files

<a href="https://www.google.com/search?q=LCID">what is LCID?</a>

#### <code>StormArchive.getFileList</code>
````js
archive.getFileList();
// ['a.txt', 'b.txt]
````
get a list of all files in the current archive


#### <code>StormArchive.readFile</code>
````
archive.readFile(filename).then(fileBuffer => {
    //save the buffer;
}, err => {
    console.log(err);
})
````
read file data by the specified file name.


#### <code>StormArchive.writeFile</code>

````
archive.writeFile(buffer, filename, lcid, flags, compression).then(fileBuffer => {
    //save the buffer;
}, err => {
    console.log(err);
})
````
writes data to the archive.

|Name|Type|Description|
|----|----|-----------|
|buffer|Buffer| data to be written|
|filename|string| file name|
|lcid|int|language Code Identifier|
|flags|int|<a href="#writefileflags">See WriteFileFlags <a>|
|compression|int|<a href="#writefilecompression">See Compression</a>

##### WriteFileFlags

Specifies additional options about how to add the file to the MPQ. The value of this parameter can be a combination of the following values:

__StarCraft1 Map(.scx, .scm) use `0x00010200`__

|Value|Description|
|-----|-----------|
|0x00000100|The file will be compressed using IMPLODE compression method.
|0x00000200|The file will be compressed
|0x00010000|The file will be stored as encrypted.
|0x00020000|The file's encryption key will be adjusted according to file size in the archive. 
|0x02000000|The file will have the deletion marker.
|0x04000000|The file will have CRC for each file sector. Ignored if the file is not compressed or if the file is stored as single unit.
|0x01000000|The file will be added as single unit. Files stored as single unit cannot be encrypted, because Blizzard doesn't support them.
|0x80000000|If this flag is specified and the file is already in the MPQ, it will be replaced.


##### WriteFileCompression
Compression method of the first file block. This parameter is ignored if MPQ_FILE_COMPRESS is not specified in dwFlags. This parameter can be a combination of the following values:

__StarCraft1 Map(.scx, .scm) use `0x08`__

|Value|Description|
|-----|-----------|
|MPQ_COMPRESSION_HUFFMANN(0x01)|Use Huffman compression. This bit can only be combined with MPQ_COMPRESSION_ADPCM_MONO or MPQ_COMPRESSION_ADPCM_STEREO.
|MPQ_COMPRESSION_ZLIB (0x02)|Use ZLIB compression library. This bit cannot be combined with MPQ_COMPRESSION_BZIP2 or MPQ_COMPRESSION_LZMA.
|MPQ_COMPRESSION_PKWARE(0x08)|Use Pkware Data Compression Library. This bit cannot be combined with MPQ_COMPRESSION_LZMA.
|MPQ_COMPRESSION_BZIP2(0x10)|Use BZIP2 compression library. This bit cannot be combined with MPQ_COMPRESSION_ZLIB or MPQ_COMPRESSION_LZMA.
|MPQ_COMPRESSION_SPARSE(0x20)|Use SPARSE compression. This bit cannot be combined with MPQ_COMPRESSION_LZMA.
|MPQ_COMPRESSION_ADPCM_MONO(0x40)|Use IMA ADPCM compression for 1-channel (mono) WAVE files. This bit can only be combined with MPQ_COMPRESSION_HUFFMANN. This is lossy compression and should only be used for compressing WAVE files.
|MPQ_COMPRESSION_ADPCM_STEREO(0x80)|Use IMA ADPCM compression for 2-channel (stereo) WAVE files. This bit can only be combined with MPQ_COMPRESSION_HUFFMANN. This is lossy compression and should only be used for compressing WAVE files.
|MPQ_COMPRESSION_LZMA(0x12)|Use LZMA compression. This value can not be combined with any other compression method.
