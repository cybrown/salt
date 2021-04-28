#! /bin/sh

./clean.sh || exit 1

./build.sh || exit 1

ls examples/*.salt | while read source_path
do
    build/main $source_path -r -s > $source_path.last_value.current
    diff $source_path.last_value $source_path.last_value.current > /dev/null && rm $source_path.last_value.current || echo "failed for $source_path"
done
