# C-experiments
Unit test and return values resembling Rust style.

## List of implemented modules
* `class_string` - a class-like module implementing
	* String_new              (create a string struct using a C formatter)
        * String_join             (equivalient to `.join("<something>")` in other languages)
    	* String_destroy          (free memory allocated by a String struct)
        * String_starts_with
    	* String_between_patterns (returns the string between two specified patterns)
    	* String_replace_char
    	* String_replace_pattern
    	* String_replace_pattern_with_format (generic for replacing a pattern with any primitive type with a specific format)
    	* ...
* `class_json` - a simple JSON parser performing
	* JsonObj_new        (conversion from a JSON string to an instance of the JSON "class")
	* JsonObj_destroy    (free memory allocated by JsonObj_new)
	* Json_get           (retrieve values from a JsonObj instance)
* `fs_utils` - implementing command line commands
	* fs_utils_mkdir                  (`mkdir <folder>`)
	* fs_utils_mkdir_p                (`mkdir -p <folder>`)
	* fs_utils_rmdir                  (`rmdir <folder>`)
	* fs_utils_rm_r                   (`rm -r <file>` or `<folder>`)
	* fs_utils_rm_from_path_as_char_p (`rm <file>`)
	* fs_utils_append                 (`echo "<text>" >> <file>`)
	* fs_utils_create_with_content    (`echo "<text>" > <file>`)
* `converter` - implementing conversion from
	* str_to_int        (C-string to signed int)
	* str_to_size_t     (C-string to size_t)
	* str_to_float      (C-string to float)
* `logger` - guess what it is
* `mem` - wrapper for memory (de)allocators which, combined with the `build-run.sh` script, performs memory leak detection.
* `result` - makes C look a bit like Rust
