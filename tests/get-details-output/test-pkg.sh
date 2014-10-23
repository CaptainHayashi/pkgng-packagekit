#!/bin/sh

################################################################################
# Tests pkcon against pkg to make sure the pkg backend works correctly for
# GetDetails for one package.
#
# More black-box tests should be written later
#
# Usage: ./test-pkg [PACKAGE] [PK_REPO]
#     or ./test.sh (runs this over a set of packages)
#     (You may need to make/install any programs listed below)
#
# TODO: Specific version/arch requirements in the PackageID

################################################################################
# Programs used in this test (relative paths will require make-ing, names
# imply external program installation).
CMD_PKG=pkg
CMD_PKCON=pkcon

CMD_GETCAT_DIR=../../backend/bin/getcat/
CMD_GETCAT="${CMD_GETCAT_DIR}pkgpk-getcat"

( cd "${CMD_GETCAT_DIR}"; make )
if [ $? -ne 0 ]
then
	echo "couldn't make getcat"
	exit 2
fi


################################################################################
# Retrieve the arguments.

if [ $# -gt 0 ]
then 
	PACKAGE="$1"
else
	PACKAGE="pkg"
fi
# Translate into an evaluation condition so we can use it with a specific
# rquery repository
ECOND="-e \"%n==${PACKAGE}\""

if [ $# -gt 1 ]
then
	PK_REPO="$2"
else
	PK_REPO=""
fi

################################################################################
# Convert from PackageKit repository to pkgng repository and query type.

if [ -z "${PK_REPO}" ]			# Wildcard (local or any remote repo)
then
	TRY_LOCAL=1
	TRY_REMOTE=1
	REPO=""
elif [ "${PK_REPO}" == "installed" ]	# Locally installed package
then
	TRY_LOCAL=1
	TRY_REMOTE=0
else					# Specific remote repo
	TRY_LOCAL=0
	TRY_REMOTE=1
	REPO="-r \"${PK_REPO}\""
fi

FOUND=0
if [ ${TRY_LOCAL} -gt 0 ]
then
	# Does the package exist locally?
	TEST=`eval ${CMD_PKG} query ${ECOND} \"%n\"`
	if [ -n "${TEST}" ]
	then
		FOUND=1
		TRY_REMOTE=0
		QTYPE="${CMD_PKG} query"
	fi
fi
if [ ${TRY_REMOTE} -gt 0 ]
then
	# Does it exist remotely?
	TEST="`eval pkg rquery ${REPO} ${ECOND} \"%n\"`"
	if [ -n "${TEST}" ]
	then
		FOUND=1
		QTYPE="${CMD_PKG} rquery ${REPO}"
	fi
fi
PKGCMD="${QTYPE} ${ECOND}"

# TODO: behave if FOUND is 0

PK_ID="${PACKAGE};;;${PK_REPO}"
echo "testing with package ${PACKAGE}"
echo "  pkg invocation:   ${PKGCMD} ..."
echo "  pkcon invocation: ${CMD_PKCON} get-details ${PK_ID}"

################################################################################
# Infer data not available from a query string but used in PackageKit.

# Get the expected ABI through some invasive inspection of the pkg config dump
ARCH="`pkg -vv | grep ABI | sed 's/ *ABI: *//'`"

# Get the PackageKit group of the package
ORIGIN="`eval ${PKGCMD} \"%o\"`"
GROUP="`${CMD_GETCAT} ${ORIGIN}`"

# Get the licences by mashing the `pkg query` licence list and or/and/single
# identifier into one "licence or licence or licence" string.
LICENSE=`\
	eval ${PKGCMD} \"%L %l\" |	# "GPL or\nBSD or\n"
	tr "\n" " " |			# "GPL or BSD or ",
	sed 's/ *[^ ]* *$//'`		# "GPL or BSD"

################################################################################
# Build and run the queries.

# Build a format string for the query that mocks up a pkcon GetDetails result
# This has to be done in such an ugly way because we're eval'ing.
FMT='Package description
  package:     %n-%v.${ARCH}
  license:     ${LICENSE}
  group:       ${GROUP}
  description: %e
  size:        %sb bytes
  url:         %w'

# Now run both queries and diff them
PKG_FILE="${PACKAGE}-pkg"
PKCON_FILE="${PACKAGE}-pkcon"

eval ${PKGCMD} \""${FMT}"\" > "${PKG_FILE}"
${CMD_PKCON} get-details "${PK_ID}" |
	awk -f "../strip_messages.awk" > "${PKCON_FILE}"

../test-diff.sh "${PACKAGE}-pkg" "${PACKAGE}-pkcon"
return $?
