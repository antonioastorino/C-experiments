# C-experiments
Unit test and return values resembling Rust style.

## List of implemented modules
* `class_string` - you know what I mean
* `class_json` - a simple JSON parser performing
	* JsonObj_new        conversion from a JSON string to an instance of the JSON "class"
	* JsonObj_destroy    free memory allocated by JsonObj_new
	* Json_get           retrieve values from a JsonObj instance
* `fs_utils` - implementing command line commands
	* fs_utils_mkdir                  (mkdir <folder>)
	* fs_utils_mkdir_p                (mkdir -p <folder>)
	* fs_utils_rmdir                  (rm -r <folder>)
	* fs_utils_rm_from_path_as_char_p (rm <file>)
	* fs_utils_append                 (echo "<something>" >> <file>)
	* fs_utils_create_with_content    (echo "<something>" > <file>)
* `converter` - implementing conversion from
	* str_to_int        (C-string to signed int)
	* str_to_size_t     (C-string to size_t)
	* str_to_float      (C-string to float)
* `logger` - guess what it is
* `mem` - wrapper for memory (de)allocators which, combined with the `build-run.sh` script, performs memory leak detection.
* `result` - makes C look a bit like Rust
