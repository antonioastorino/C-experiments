# C-experiments
Unit test and return values resembling Rust style.
Class-like implementation of useful tools.
> Developed and tested on Mac only.

## Build and run
```
bin/build-run.sh [<mode>]
```
Without `<mode>` being specified, the program will build and compile using `main.c` as entry point.
In case `<mode>` is equal to `test` or `debug`, the `main-test.c` will be used and the unit tests will run. However, in `debug` mode, the program will attach `lldb` to the built binary.
> You should always debug the unit tests using the provided functionality, instead of running `lldb` yourself. The `build-run.sh` script serves to set up files and folders in the `test/artifacts` directory. Without them, some tests will fail.

## Clean up
```
bin/cleanup.sh
```

## List of implemented modules
* `class_string` - a class-like module implementing
	* String_new              (create a string struct using a C formatter)
	* String_join             (equivalent to `.join("<something>")` in other languages)
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

## Limitations
The JSON parser currently does not support JSON objects starting with an array, e. g.
```json
[
	{ "key1": "value1" },
	{ "key2": "value2" }
]
```

I think I have covered all the other cases - see examples in `test/assets/`.