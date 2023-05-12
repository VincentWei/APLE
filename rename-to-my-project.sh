#!/bin/bash

ProjectName=HelloWorld
PROJECTNAME=HELLOWORLD
projectname=helloworld

sed -i "s/FooBar/$ProjectName/g" `grep FooBar * -rl`
sed -i "s/FOOBAR/$PROJECTNAME/g" `grep FOOBAR * -rl`
sed -i "s/foobar/$projectname/g" `grep foobar * -rl`

mv source/cmake/target/FooBar.cmake $ProjectName.cmake

git rm rename-to-my-project.sh
