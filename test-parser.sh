#! /bin/sh

./clean.sh || exit 1

./build.sh || exit 1

ls examples/*.salt | while read source_path
do
    build/dump-ast $source_path > $source_path.ast.current
    diff $source_path.ast $source_path.ast.current > /dev/null && rm $source_path.ast.current || echo "failed for $source_path"
done
