#! /bin/sh

./clean.sh || exit 1

./build.sh || exit 1

ls examples/*.salt | while read source_path
do
    build/dump-opcodes $source_path > $source_path.opcodes.current
    diff $source_path.opcodes $source_path.opcodes.current > /dev/null && rm $source_path.opcodes.current || echo "failed for $source_path"
done
