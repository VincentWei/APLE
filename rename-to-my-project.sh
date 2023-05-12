#!/bin/bash

if echo "$1" | grep -q '^[A-Z][a-z][a-z0-9]*[A-Z][a-z][a-z0-9]*'; then
    ProjectName=$1
    PROJECTNAME=`echo $1| tr "a-z" "A-Z"`
    projectname=`echo $1| tr "A-Z" "a-z"`
    echo $ProjectName
    echo $PROJECTNAME
    echo $projectname
else
    echo "Invalid project name; must be one like 'ProjectName', 'MyProject', or 'My1Pt1'."
    exit 1
fi

sed -i "s/FooBar/$ProjectName/g" `grep FooBar * -rl`
sed -i "s/FOOBAR/$PROJECTNAME/g" `grep FOOBAR * -rl`
sed -i "s/foobar/$projectname/g" `grep foobar * -rl`

mv source/cmake/target/FooBar.cmake source/cmake/target/$ProjectName.cmake

git rm rename-to-my-project.sh

exit 0
