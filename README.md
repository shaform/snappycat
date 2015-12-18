# snappycat
A command line tool to decompress snappy files produced by Hadoop.

The snappy files produced by Hadoop contain data headers produced by [BlockCompressorStream](https://github.com/apache/hadoop/blob/master/hadoop-common-project/hadoop-common/src/main/java/org/apache/hadoop/io/compress/BlockCompressorStream.java).
Each block is prepended with two 32-bit integers stating the size of the decompressed data and size of the compressed data, respectively.h
Therefore, snappy library cannot directly decompress it. This command line utility handles the headers
and decompresses the data without any dependencies on the Hadoop libraries.

## Known Issues

For large files, [libsnappy1.1.3+ branch](https://github.com/shaform/snappycat/tree/libsnappy1.1.3) may be required.


## Build

snappycat requires the [snappy library](https://github.com/google/snappy) to build.

To install it under Ubuntu, run:

```bash
sudo apt-get install libsnappy-dev
```

## Usage

List the files to decompress as arguments, and `snappycat` would output the concatenated results to the standard output.

```bash
./snappycat DIRECTORY/*.snappy
```

When no arguments are given, standard input is used as the source of the compressed data.

```bash
cat DIRECTARY/*.snappy | snappycat
```

`snappycat` is able to handle the input correctly even if some files don't have any records.

To save the output as a file:

```bash
./snappycat DIRECTORY/*.snappy > output.txt
```
