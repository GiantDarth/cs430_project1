# ppmrw
### CS430 - Project 1 (Images)
**Christopher Robert Philabaum**

**Northern Arizona University (Fall 2016)**

ppmrw is a tool that allows one to convert from one format of PPM to another (P3 and P6, aka. ASCII or raw binary).

See [official ppm format](http://netpbm.sourceforge.net/doc/ppm.html) for further reference.

**Note:**
* This program does not support P1, P2, P4, P5, or P7.
* This program does not support 2-byte channels (aka. >= 256 and <= 65535 values).
* All forms of comments following any whitespace or value in the header are taken into account and properly ignored in parsing.
* The program should format the output file properly by never exceeding 70 characters (as per the documentation). Pixels are spaced with three characters (except when reaching the character limit), and channels / samples are separated by a single space. Rows are spaced by an extra new line.

## Usage
`ppmrw [3|6] /path/to/input.ppm /path/to/output.ppm`

### parameters:
1. `mode`: Indicates the output format to write, 3 (as in *P3*) for ASCII, or 6 (as in *P6*) for binary.
2. `inputFile`: A valid path, absolute or relative (to *pwd*), to the input file.
3. `outputFile`: A valid path, absolute or relative (to *pwd*), to the output file.

All parameters are *required* and not optional. All parameters must be used in the exact order provided above.

## Compile
`make`: Compiles the program into `out/` as `out/ppmrw`

`make clean`: Removes all object code and the `out/` directory altogether

## Grader Notes
* Because make compiles `ppmrw` to `out/`, in order to run it properly it should be used as `out/ppmrw [3|6] /path/to/input.ppm /path/to/output.ppm`.
* I have two test images in the `img/` directory, one for ASCII/text, and one for binary.
* Even though I have `.clang_complete` as a file, this is only used in conjunction with a package for **Atom**, and the Makefile properly uses `gcc` instead.
* The output file should contain the line/comment "# Created with ppmrw (Christopher Philabaum &lt;cp723@nau.edu&gt;)" following the magic number.
