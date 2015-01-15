#!/bin/bash
#
# script which creates the release from the basemelees git repos.


# print usage, if no argument is specified
if [ $# -eq 0 ] 
then
    echo "Usage: $0 <version>"
    exit 1
fi

# get the version
version=$1

# move to basemelees folder
cd ..
if [ ! -f Title.txt ]
then
	echo "Execute from basemelees/tools currently executed from wrong folder."
	exit 1
fi

# create the new scenario folder
if [ -d BaseMeleesV$version.ocf ] || [ -f BaseMeleesV$version.ocf ]
then
	echo "Release folder or product already exists, remove before running this program."
	exit 1
fi
mkdir BaseMeleesV$version.ocf

# copy standard files into the folder
cp Authors.txt BaseMeleesV$version.ocf/Authors.txt
cp DescDE.rtf BaseMeleesV$version.ocf/DescDE.rtf
cp DescUS.rtf BaseMeleesV$version.ocf/DescUS.rtf
cp Folder.txt BaseMeleesV$version.ocf/Folder.txt
cp License.txt BaseMeleesV$version.ocf/License.txt
cp Title.txt BaseMeleesV$version.ocf/Title.txt
cp Title.png BaseMeleesV$version.ocf/Title.png
cp Version.txt BaseMeleesV$version.ocf/Version.txt

# copy the local objects folder
rsync -av --exclude=".*" --exclude="*~" Objects.ocd/* BaseMeleesV$version.ocf/Objects.ocd

# copy the scenario folders
rsync -av --exclude=".*" --exclude="*~" CliffFight.ocs/* BaseMeleesV$version.ocf/CliffFight.ocs
rsync -av --exclude=".*" --exclude="*~" Labyrinth.ocs/* BaseMeleesV$version.ocf/Labyrinth.ocs
rsync -av --exclude=".*" --exclude="*~" Skylands.ocs/* BaseMeleesV$version.ocf/Skylands.ocs

# pack the folder with c4group
c4group BaseMeleesV$version.ocf -p

#succeeded
exit
