#!/bin/bash

usage() {
    echo "USAGE: make_package.sh [OPTIONS] <dir>"
    echo "  <dir> is a directory containing conanfile.py and test_package."
    echo ""
    echo "OPTIONS:"
    echo "  -h          Print this message and exit"
    echo "  -d          Destination folder"
    echo "  -k          Keep destination package folder"
    echo ""
    echo "RETURNS:"
    echo "  0  success"
    echo "  1  packaging error"
    echo "  2  script error"
}

# Argument and option handling
# ============================

while getopts "d:kh" arg; do
    case "${arg}" in
        d)
            destination_folder="${OPTARG}"
            ;;
        k)
            keep_folder="TRUE"
            ;;
        h)
            echo "Substitute variables in conanfile and build package"
            echo ""
            usage
            exit 0
            ;;
    esac
done

shift $((OPTIND-1))

if [ $# -eq 0 ] ; then
    >&2 echo "Error: missing directory"
    >&2 echo ""
    usage
    exit 2
fi

if [ $# -gt 1 ] ; then
    >&2 echo "Error: too many arguments"
    >&2 echo ""
    exit 2
fi

conan_directory=$1

if [ ! -d "$conan_directory" ] ; then
    >&2 echo "Error: invalid directory"
    >&2 echo "  $conan_directory"
    exit 2
fi

if [ -z "$destination_folder" ] ; then
    destination_folder="conan_packaging"
fi

if [ -d "$destination_folder" ] ; then
    >&2 echo "Error: destination folder already exists"
    >&2 echo "  $destination_folder"
    exit 2
fi

# Packaging
# =========

if [ -z "$CONAN_PACKAGE_VERSION" ] ; then
    # TODO: get version at runtime.
    VERSION_STR="1.0.2-rc.1"

    # If this is not a release, add git information to version string.
    if [ -z "$VERSION_RELEASE" ] ; then
        VERSION_SUFFIX="-$(git rev-parse --short HEAD)"

        # Check whether there are changes in the repository.
        if [ -n "$(git status --porcelain)" ] ; then
            # There are changes.
            VERSION_SUFFIX="${VERSION_SUFFIX}-dirty"
        fi

        VERSION_STR="${VERSION_STR}${VERSION_SUFFIX}"
    fi

    export CONAN_PACKAGE_VERSION="$VERSION_STR"
fi

echo "Package version: $CONAN_PACKAGE_VERSION"

if [ -z "$CONAN_PACKAGE_COMMIT" ] ; then
    export CONAN_PACKAGE_COMMIT="$(git rev-parse HEAD)"
fi

echo "Package commit: $CONAN_PACKAGE_COMMIT"

if [ -z "$CONAN_CHANNEL" ] ; then
    export CONAN_CHANNEL="testing"
fi

echo "Package channel: $CONAN_CHANNEL"

cp -r "$conan_directory" "$destination_folder" || exit 2

sed -i"" -e "s/<version>/$CONAN_PACKAGE_VERSION/g" "$destination_folder"/conanfile.py
sed -i"" -e "s/<version>/$CONAN_PACKAGE_VERSION/g" "$destination_folder"/test_package/conanfile.py
sed -i"" -e "s/<commit>/$CONAN_PACKAGE_COMMIT/g" "$destination_folder"/conanfile.py

current_dir="$(pwd)"
cd "$destination_folder" && conan test_package
result=$?
if [ $result -ne 0 ] ; then
    echo "Error: packaging failed with return code $result"
    exit 1
fi
cd "$current_dir"

if [ "$keep_folder" != "TRUE" ] ; then
    rm -rf "$destination_folder" || exit 2
fi

exit 0
