#! /bin/bash -e
#
# replacement text for this commit
cat << EOF > commit.$$
Replace svn_commit.sh and svn_status.sh by commit.sh. This is a simpler
procedure for commits that is also used by other projects.

AUTHORS@  undate version number to current date.
commit.sh@
configure@  undate version number to current date.
configure.ac@  undate version number to current date.
cppad/config.h@  undate version number to current date.
cppad/configure.hpp@  undate version number to current date.
only_date.sh@ check if only the date (version number) has changed.
new_stable.sh@ remove automatic editing of to svn_commit.sh and svn_status.sh.
svn_commit.sh@ replace by commit.sh.
svn_status.sh@ replace by commit.sh.
EOF
# -----------------------------------------------------------------------
if [ "$1" == 'files' ]
then
	# -------------------------------------------------
	svn status | sed -n -e '/^[ADMRC] /p' | \
		sed -e 's/^[ADMRC] [+ ]*//'| \
		sort -u >> list.$$
	# -------------------------------------------------
	list=`cat list.$$`
	for file in $list
	do
		if [ -e "$file" ]
		then
			sed -f svn_commit.sed $file > commit.$$
			if ! diff $file commit.$$ > /dev/null
			then
				echo "commit.sh aborting: suggest changes to $file:"
				diff $file commit.$$
				rm commit.$$
				exit 1
			fi
		fi
	done
	#
	echo "cp commit.sh commit.old"
	cp commit.sh commit.old
	#
	echo "svn revert commit.sh"
	svn revert commit.sh
	#
	sed -n -e '1,/^$/p'   < commit.sh >  commit.$$ 
	sed list.$$ -e 's/$/@/'           >> commit.$$ 
	rm  list.$$
	sed -n -e '/^EOF/,$p' < commit.sh >> commit.$$ 
	#
	echo "diff commit.sh commit.$$"
	if diff commit.sh commit.$$
	then
		echo "commit.sh: exiting because commit.sh has not changed"
		rm commit.$$
		exit 1
	fi
	#
	echo "mv commit.$$ commit.sh"
	mv commit.$$ commit.sh
	#
	chmod +x commit.sh
	exit 0
fi
# -----------------------------------------------------------------------
if [ "$1" != 'run' ]
then
cat << EOF
usage: ./commit.sh files
       ./commit.sh run

The first from changes the list of files at the beginning of commit.sh 
so that it all the files that have changed status.
You should then edit commit.sh by hand (as per the instructions at the 
beginning of commit.sh) before running the second form.

The second form actually commits the list of files (provided that you reply
y to the [y/n] prompt that commit.sh generates).
EOF
	rm commit.$$
	exit 0
fi
# -----------------------------------------------------------------------
list=`sed -e '/@/! d' -e 's/@.*//' commit.$$`
msg=`sed -e '/@ *$/d' -e 's|.*/\([^/]*@\)|\1|' -e 's|@|:|' commit.$$`
rm commit.$$
echo "svn commit -m \""
echo "$msg"
echo "\" \\"
echo "$list"
read -p "is this ok [y/n] ?" response
if [ "$response" != "y" ]
then
	exit 1
fi
#
if ! svn commit -m "$msg" $list
then
	echo "commit.sh: commit failed"
	exit 1
fi
#
echo "mv commit.sh commit.old"
mv commit.sh commit.old
#
svn revert commit.sh