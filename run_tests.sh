set -e
sh build.sh
ctest --test-dir build --output-on-failure