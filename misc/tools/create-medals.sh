#!/bin/bash
#
# script which creates the medal rule from the basemelees git repos.


# print usage, if no argument is specified
if [ $# -eq 0 ] 
then
    echo "Usage: $0 <version>"
    exit 1
fi

# get the version
version=$1

# move to basemelees folder
cd ../..
if [ ! -f Title.txt ]
then
	echo "Execute from basemelees/misc, currently executed from wrong folder."
	exit 1
fi

# create the new medal object
if [ -d MedalsV$version.ocd ] || [ -f MedalsV$version.ocd ]
then
	echo "Medals folder or product already exists, remove before running this program."
	exit 1
fi
mkdir MedalsV$version.ocd

# just copy everything inside Medals.ocd
rsync -a --exclude=".*" --exclude="*~" Objects.ocd/Medals.ocd/* MedalsV$version.ocd

# pack the folder with c4group
c4group MedalsV$version.ocd -p

#succeeded
exit
