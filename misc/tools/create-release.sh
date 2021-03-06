#!/bin/bash
#
# script which creates the release from the basemelees git repos.


# get list of scenarios to release
scen_list=`cat scenario_list.txt`

# move to basemelees folder
cd ../..
if [ ! -f Title.txt ]
then
	echo "Execute from basemelees/misc, currently executed from wrong folder."
	exit 1
fi

# check for version file and take version
if [ ! -f Version.txt ]
then
	echo "Version.txt not found, please create this file."
	exit 1
fi
version="`cat Version.txt`"

# create the new scenario folder
if [ -d BaseMeleesV$version.ocf ] || [ -f BaseMeleesV$version.ocf ]
then
	echo "Release folder or product already exists, remove before running this program."
	exit 1
fi
mkdir BaseMeleesV$version.ocf

# copy standard files into the folder
cp Authors.txt BaseMeleesV$version.ocf/Authors.txt
cp DescDE.txt BaseMeleesV$version.ocf/DescDE.txt
cp DescUS.txt BaseMeleesV$version.ocf/DescUS.txt
cp Folder.txt BaseMeleesV$version.ocf/Folder.txt
cp License.txt BaseMeleesV$version.ocf/License.txt
cp Title.txt BaseMeleesV$version.ocf/Title.txt
cp Title.png BaseMeleesV$version.ocf/Title.png
cp Version.txt BaseMeleesV$version.ocf/Version.txt

# copy all .ocd's and .ocg's in the main folder
for dir in $(find -maxdepth 1 -type d -name "*.ocd" -o -name "*.ocg"); 
do
	# exclude hidden and temporary files
	rsync -a --exclude=".*" --exclude="*~" $dir BaseMeleesV$version.ocf
done

# copy selected scenarios which are ready to release
for dir in $scen_list
do
	# exclude hidden and temporary files
	rsync -a --exclude=".*" --exclude="*~" $dir BaseMeleesV$version.ocf
done

# pack the folder with c4group
c4group BaseMeleesV$version.ocf -p

#succeeded
exit
