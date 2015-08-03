#!/bin/bash
#
# script which creates the medal rule from the basemelees git repos.


# move to basemelees folder
cd ../..
if [ ! -f Title.txt ]
then
	echo "Execute from basemelees/misc, currently executed from wrong folder."
	exit 1
fi

# check for version file and take version
if [ ! -f Objects.ocd/Medals.ocd/Version.txt ]
then
	echo "Version.txt not found, please create this file."
	exit 1
fi
version="`cat Objects.ocd/Medals.ocd/Version.txt`"

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
