#!/bin/bash

usage_str="\
USAGE: make_package.sh [OPTIONS] <dir>
  Substitute variables in conanfile and build package
"

description_str="\
DESCRIPTION:
  <dir> is a directory containing a conanfile.py and a test package. Its
  contents are copied into a destination folder that must not already exist,
  named conan_packaging by default, and version and commit values are
  substituted. If the package is successfully generated, the destination
  folder is removed afterwards. Version and commit can be defined by setting
  the environment variables CONAN_PACKAGE_VERSION and CONAN_PACKAGE_COMMIT,
  respectively, and the package channel can be set by defining
  CONAN_CHANNEL.
"

options_and_returns_str="\
OPTIONS:
  -h  Print help and exit
  -d  Destination folder name
  -k  Keep destination package folder

RETURNS:
  0  success
  1  packaging error
  2  script error
"

usage() {
    printf "%s\n%s" \
        "$usage_str" \
        "$options_and_returns_str"
}

full_usage() {
    printf "%s\n%s\n%s" \
        "$usage_str" \
        "$description_str" \
        "$options_and_returns_str"
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
            full_usage
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
    usage
    exit 2
fi

conan_directory=$1

if [ ! -d "$conan_directory" ] ; then
    >&2 echo "Error: invalid directory"
    >&2 echo "  $conan_directory"
    >&2 echo ""
    usage
    exit 2
fi

if [ -z "$destination_folder" ] ; then
    destination_folder="conan_packaging"
fi

if [ -d "$destination_folder" ] ; then
    >&2 echo "Error: destination folder already exists"
    >&2 echo "  $destination_folder"
    >&2 echo ""
    usage
    exit 2
fi

# Packaging
# =========

if [ -z "$CONAN_PACKAGE_COMMIT" ] ; then
    current_dir="$(pwd)"
    cd "$conan_directory"
    CONAN_PACKAGE_COMMIT="$(git rev-parse HEAD)"
    cd "$current_dir"
fi

# Get first seven characters in string.
CONAN_PACKAGE_COMMIT_SHORT="$(echo $CONAN_PACKAGE_COMMIT | awk '{print substr($0,0,7)}')"

if [ -z "$CONAN_PACKAGE_VERSION" ] ; then
    # TODO: get version at runtime.
    VERSION_STR="1.0.2-rc.1"

    # If this is not a release, add git information to version string.
    if [ -z "$VERSION_RELEASE" ] ; then
        VERSION_STR="${VERSION_STR}+${CONAN_PACKAGE_COMMIT_SHORT}"
    fi

    CONAN_PACKAGE_VERSION="$VERSION_STR"
fi

if [ -z "$CONAN_CHANNEL" ] ; then
    export CONAN_CHANNEL="testing"
fi

cp -r "$conan_directory" "$destination_folder" || exit 2

sed -i"" -e "s/<version>/$CONAN_PACKAGE_VERSION/g" "$destination_folder"/conanfile.py
sed -i"" -e "s/<version>/$CONAN_PACKAGE_VERSION/g" "$destination_folder"/test_package/conanfile.py
sed -i"" -e "s/<commit>/$CONAN_PACKAGE_COMMIT/g" "$destination_folder"/conanfile.py

current_dir="$(pwd)"
cd "$destination_folder" && conan test_package
result=$?
if [ $result -ne 0 ] ; then
    >&2 echo "Error: packaging failed with return code $result"
    exit 1
fi
cd "$current_dir"

if [ "$keep_folder" != "TRUE" ] ; then
    rm -rf "$destination_folder" || exit 2
fi

exit 0
