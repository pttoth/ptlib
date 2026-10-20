#!/bin/bash


#find out script directory
pushd $(dirname "${BASH_SOURCE[0]}") > /dev/null
scriptdir=$(pwd)
popd > /dev/null


#move to project root directory
project_root=$scriptdir/../..
pushd $project_root


#set up temp install directory
tmp_dir_ptlib="/tmp/pt_install"
mkdir -v $tmp_dir_ptlib

tmp_dir_install="${tmp_dir_ptlib}/install"
mkdir -v $tmp_dir_install

pushd $tmp_dir_install

# cfg: dependency source and version
repo_version=$(cat ${scriptdir}/data/cfg/PTLIB_VERSION)
download_link=https://github.com/pttoth/ptlib

# acquire dependency
git clone $download_link ptlib
cd ptlib
git pull
git checkout $repo_version

#call the lib's build script
source ./script/build_debian.sh

popd

# backup existing 'lib/' contents, if needed
time_now=$(date "+%y-%m-%d_%H%M%S")
mv -v ./lib/libptlib.a    "./lib/libptlib.a.bak_${time_now}"   2> /dev/null
mv -v ./lib/ptlib         "./lib/ptlib.bak_${time_now}"        2> /dev/null

#create 'lib' directory
mkdir -vp ./lib/ptlib
mkdir -vp ./lib/ptlib/include
mkdir -vp ./lib/ptlib/src

#move built libs into 'lib' directory
cp -v ${tmp_dir_install}/ptlib/bin/debian/libptlib.a ./lib

cp -vr ${tmp_dir_install}/ptlib/include/* ./lib/ptlib/include/
cp -vr ${tmp_dir_install}/ptlib/src/*     ./lib/ptlib/src/


#jump back to starting directory
popd > /dev/null
