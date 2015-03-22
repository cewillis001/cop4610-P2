#!/bin/sh
# submit code for assignment P2, after doing a few simple sanity checks
ASSIGNMENT=P2
#HOME=/tmp
#export HOME
GRADER=cop4610p@cs.fsu.edu
HOSTNAME=`uname -a`
SYSTEMS="Linux SunOS"
OSNAME=`uname -s`
PATH=/bin\:/usr/bin
if [ ${OSNAME} = "Linux" ]; then
  MAIL=mail
elif [ ${OSNAME} = "SunOS" ]; then
  MAIL=mailx
else
  echo unrecognized OS; this script must run on Linux or SunOS
  exit -1;
fi;
for SUFFIX in .tgz .uu; do
  if [ -f ${ASSIGNMENT}${SUFFIX} ]; then
    echo submit aborted: would overwrite file ${ASSIGNMENT}${SUFFIX}
    echo please rename ${ASSIGNMENT}${SUFFIX} or remove it, and then rerun this script
    exit -1;
  fi;
done;
if [ ! -f "testone" ]; then
      echo submit aborted: missing file "testone"
      echo please make sure you are in the directory that contains the files 
      echo you want to submit, and then rerun this script
      exit -1;
fi;
echo found "testone"
FILES="testone"
if [ -f "testall" ]; then
 echo found "testall"
 FILES=${FILES}" testall"
else
 echo file "testall" not found
 echo are you certain you do not want to submit a file "testall"?
fi;
for DIR in sol*; do
 NEW_DIR="1";
 for DIR2 in sol0 sol1 sol2 sol3; do
    if [ ${DIR} = ${DIR2} ]; then
       NEW_DIR="0";
    fi
 done;
 if [ ${NEW_DIR} = "1" ]; then
    echo found new test directory ${DIR}
    FILES=${FILES}" ${DIR}"
 fi
done
tar czpf ${ASSIGNMENT}.tgz ${FILES}
uuencode ${ASSIGNMENT}.tgz ${ASSIGNMENT}.tgz > ${ASSIGNMENT}.uu
${MAIL} -n -s "${ASSIGNMENT} submission" ${GRADER} < ${ASSIGNMENT}.uu
for SUFFIX in .tgz .uu; do
  rm -f ${ASSIGNMENT}${SUFFIX};
done;
echo "sent file(s) [ ${FILES} ] to ${GRADER}"
