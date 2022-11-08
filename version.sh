#!/bin/bash

V_MAJOR=1
V_MINOR=1
V_HOST=`hostname`
V_COMMIT=`git log --pretty=format:'%h' -n 1`
V_BUILD=`date +"%Y%m%d.%H%M%S"`
VERSION=$V_MAJOR.$V_MINOR.$V_HOST.$V_COMMIT.$V_BUILD
echo $VERSION