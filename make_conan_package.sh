#!/bin/bash

version_str="1.0.0"

usage_str="\
USAGE: $0 [OPTIONS] <dir>
  Substitute variables in conanfile and build Conan package
"

description_str="\
DESCRIPTION:
  <dir> must be a directory containing a conanfile.py and a test package. Its
  contents are copied into a destination folder that must not already exist,
  named conan_packaging by default, and the strings '<version>' and '<commit>'
  are substituted. The package is built and the destination folder is removed
  afterwards, unless -k is used. The destination folder name can be changed
  with -d.

  Version and commit values can be defined by setting the environment variables
  PACKAGE_VERSION and PACKAGE_COMMIT, respectively. If PACKAGE_VERSION is not
  set, 0.1.0-rc.1 is used. If IS_RELEASE is not set, a '+' character and the
  first seven digits of the commit SHA-1 is appended to PACKAGE_VERSION. If
  PACKAGE_COMMIT is not set, the current commit SHA-1 is used.
"

options_and_returns_str="\
OPTIONS:
  -h         Print help and exit
  -d <dest>  Destination folder name
  -k         Keep destination package folder
  -v         Print version and exit

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

version() {
    printf "$0 version %s\n" "$version_str"
}

# Argument and option handling
# ============================

while getopts "d:khv" arg; do
    case "${arg}" in
        d)
            dest_folder="${OPTARG}"
            ;;
        k)
            keep_folder="TRUE"
            ;;
        h)
            full_usage
            exit 0
            ;;
        v)
            version
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

conan_dir=$1

if [ ! -d "$conan_dir" ] ; then
    >&2 echo "Error: invalid directory"
    >&2 echo "  $conan_dir"
    >&2 echo ""
    usage
    exit 2
fi

if [ -z "$dest_folder" ] ; then
    dest_folder="conan_packaging"
fi

if [ -d "$dest_folder" ] ; then
    >&2 echo "Error: destination folder already exists"
    >&2 echo "  $dest_folder"
    >&2 echo ""
    usage
    exit 2
fi

# Packaging
# =========

if [ -z "$PACKAGE_COMMIT" ] ; then
    current_dir="$(pwd)"
    cd "$conan_dir"
    PACKAGE_COMMIT="$(git rev-parse HEAD)"
    cd "$current_dir"
fi

# Get first seven characters in string.
COMMIT_SHORT="$(echo $PACKAGE_COMMIT | awk '{print substr($0,0,7)}')"

if [ -z "$PACKAGE_VERSION" ] ; then
    PACKAGE_VERSION="0.1.0-rc.1"
fi

# If this is not a release, add commit information to version string.
if [ -z "$IS_RELEASE" ] ; then
    PACKAGE_VERSION="${PACKAGE_VERSION}+${COMMIT_SHORT}"
fi

cp -r "$conan_dir" "$dest_folder" || exit 2

sed -i"" -e "s/<version>/$PACKAGE_VERSION/g" "$dest_folder"/conanfile.py
sed -i"" -e "s/<version>/$PACKAGE_VERSION/g" "$dest_folder"/test_package/conanfile.py
sed -i"" -e "s/<commit>/$PACKAGE_COMMIT/g" "$dest_folder"/conanfile.py

# Print script and packaging information.
version
echo "PACKAGE_VERSION=${PACKAGE_VERSION}"
echo "PACKAGE_COMMIT=${PACKAGE_COMMIT}"
echo "conan_dir=${conan_dir}"
echo "dest_folder=${dest_folder}"

current_dir="$(pwd)"
cd "$dest_folder" && conan test_package
result=$?
if [ $result -ne 0 ] ; then
    >&2 echo "Error: packaging failed with return code $result"
    exit 1
fi
cd "$current_dir"

if [ "$keep_folder" != "TRUE" ] ; then
    rm -rf "$dest_folder" || exit 2
fi

exit 0
