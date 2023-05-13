#!/bin/bash

if echo "$1" | grep -q '^[A-Z][A-Za-z0-9]*[A-Z][a-z][a-z0-9]*'; then
    ProjectName=$1
    PROJECTNAME=`echo $1| tr "a-z" "A-Z"`
    projectname=`echo $1| tr "A-Z" "a-z"`
    echo "Going to rename the project to $ProjectName..."
else
    echo "Invalid project name; must be one like 'ProjectName', 'MyProject', or 'HBDInetd'."
    exit 1
fi

sed -i "s/FooBar/$ProjectName/g" `grep FooBar CMakeLists.txt source/ -rl`
sed -i "s/FOOBAR/$PROJECTNAME/g" `grep FOOBAR CMakeLists.txt source/ -rl`
sed -i "s/foobar/$projectname/g" `grep foobar CMakeLists.txt source/ -rl`

git mv source/cmake/target/FooBar.cmake source/cmake/target/$ProjectName.cmake

git mv rename-to-my-project.sh ready-to-remove-this-file.sh

exit 0
