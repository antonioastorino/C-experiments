# C-experiments
Unit test and return values a resembling Rust style.

## List of implemented modules
* `class_string` - you know what I mean
* `class_json` - a simple JSON parser performing
	* conversion from text to an instance of the JSON "class"
* `fs_utils` - implementing command line commands
	* mkdir
	* mkdir -p
	* rmdir (A.K.A. rm -r)
	* rm
	* rm -r
* `converter` - implementing conversion from
	* C-string to signed int
	* C-string to float
* `logger` - guess what it is
* `mem` - wrapper for memory (de)allocators which, combined with the `build-run.sh` script, performs memory leak detection.
* `result` - makes C look a bit like Rust