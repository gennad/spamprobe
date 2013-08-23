#!/bin/bash

GOOD_MAIL="$HOME/newton/IMAP/active $HOME/newton/IMAP/archive $HOME/newton/IMAP/saved/companies/other $HOME/newton/IMAP/saved/consult/GE/CMS $HOME/newton/IMAP/saved/Projects/OpenSource"
SPAM_MAIL="$HOME/newton/IMAP/SpamArchive $HOME/newton/IMAP/TestSpam"
#SPAM_MAIL="$HOME/newton/IMAP/SpamArchive"
PERCENTAGE=75

DBDIR=$HOME/sptest

VERBOSE=""
SPEXEC="./spamprobe"

(
#for i in 1 2 3 4 5 6 7 8 9 10 ; do
for i in 1 2 3 4 5 ; do
    echo CREATING TEST CASE $i USING PERCENTAGE $PERCENTAGE
    ruby splitmail.rb $PERCENTAGE _good $GOOD_MAIL
    ruby splitmail.rb $PERCENTAGE _spam $SPAM_MAIL
    bash runtest.sh
done
) 2>&1 | tee _multi_results
