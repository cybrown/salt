#! /bin/sh

CC="clang"
CFLAGS="-g -Wall"

find src -name '*.c' | while read source_path
do
    a=${source_path#src/}
    output_path=build/o/${a%.*}.o
    if [ -f $output_path ]
    then
        if [ "$source_path" -nt "$output_path" ]
        then
            echo Source modified: "$source_path"
            $CC $CFLAGS -c "$source_path" -o "$output_path"
        fi
    else
        echo New file: "$source_path" to "$output_path"
        if [ ! -d $(dirname $output_path) ]
        then
            mkdir -p $(dirname $output_path)
        fi
        $CC $CFLAGS -c "$source_path" -o "$output_path"
    fi
done

echo "Linking..."
ls build/o/cmd/*.o | while read entry_point
do
    $CC $CFLAGS $entry_point build/o/lib/* -o build/$(basename ${entry_point%.*}).exe
done
