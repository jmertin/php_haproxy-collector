#!/bin/sh

# Get installer data
if [ -f ./00_infosource.cfg ]
then
    source ./00_infosource.cfg
    source ./.build
   
fi

# Create build version
BUILDVER=${RELEASE}b${BUILD}

# Used to remove non needed docker images/system cleanup.
# Do no enable in production!
#echo -n "Removing dangling docker images"
#docker rmi $(docker images -f "dangling=true" -q)


echo
echo -n ">>> Build $FileBase image [y/n]?: "
read Build

if [ "$Build" = "y" ]
   then

       echo
       echo "*** If you want to apply OS Update, don't use the cache."
       echo -n ">>> Use cache for build [y/n]?: "
       read Cache

       PREFIX=`hostname -s`

       if [ "$Cache" == "y" ]
       then
           ### Build PHP Probe volume
           docker build -t bcp/$FileBase .
       else
           ### Build PHP Probe volume
           docker build --no-cache -t bcp/$FileBase .
       fi
       # Tag the built image
       echo "*** Tagging image to bcp/$FileBase:$BUILDVER" 
       docker tag bcp/$FileBase:latest bcp/$FileBase:$BUILDVER

       if [ -n "$DOCKER_REGISTRY" ]
       then
	   echo "*** Tagging image to ${DOCKER_REGISTRY}/bcp/$FileBase:$PHPMONITVER" 
	   docker tag bcp/$FileBase:latest ${DOCKER_REGISTRY}/bcp/$FileBase:$BUILDVER
	   echo "*** Pushing image to registry" 
	   docker push ${DOCKER_REGISTRY}/bcp/$FileBase:$BUILDVER
       fi
fi
