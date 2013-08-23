SpamProbe v1.4 README

LEGALESE

Burton Computer Corporation
http://www.burton-computer.com

Copyright (C) 2002,2003,2004,2005,2006 Burton Computer Corporation ALL
RIGHTS RESERVED

This program is open source software; you can redistribute it and/or
modify it under the terms of the Q Public License (QPL) version
1.0. Use of this software in whole or in part, including linking it
(modified or unmodified) into other programs is subject to the terms
of the QPL.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the Q Public
License for more details.

You should have received a copy of the Q Public License along with
this program; see the file LICENSE.txt.  If not, visit the Burton
Computer Corporation or CoolDevTools web site QPL pages at:

    http://www.burton-computer.com/qpl.html


INTRODUCTION

Welcome to SpamProbe!  Are you tired of the constant bombardment of
your inbox by unwanted email pushing everything from porn to get rich
quick schemes?  Have you tried other spam filters but become
disenchanted with them when you realized that their manually generated
rule sets weren't updated fast enough to keep up with spammers wording
changes?  Or that they generated unwanted false positive scores?

SpamProbe operates on a different basis entirely.  Instead of using
pattern matching and a set of human generated rules SpamProbe relies
on a Bayesian analysis of the frequency of words used in spam and
non-spam emails received by an individual person.  The process is
completely automatic and tailors itself to the kinds of emails that
each person receives.

My work on SpamProbe was inspired by an excellent article by Paul
Graham.  He describes the basic idea and his results.  You can read
his article here:

  http://www.paulgraham.com/spam.html

I highly recommend reading the article and the other spam related
links on his site for excellent insights into why spam is a problem
and how you can defeat it.  Of course running SpamProbe is an
excellent step! :-)


FEATURES

 * Spam detection using Bayesian analysis of terms contained in each
   email.  Words used often in spams but not in good email tend to
   indicate that a message is spam.

 * Written in C++ for good performance.  Database access using PBL or
   BerkeleyDB for quick startup and fast term count retrieval.

 * Recognition and decoding of MIME attachments in quoted-printable
   and base64 encoding.  Automatically skips non-text attachments.
   MIME decoding enables SpamProbe to make decisions based on words in
   the emails rather than base64 gobbledigook.

 * Counts two word phrases as well as single words for higher
   precision.

 * Counts words in from, to, cc, received, and subject headers
   separately for improved accuracy since some words are better
   indicators of spam when they appear in the subject than in the
   body.  Received headers after the first are counted separately from
   first Received header since they are likely to be forged in spams.

 * Ignores HTML tags in emails for scoring purposes unless the -h
   command line option is used.  Many spams use HTML and few humans do
   so HTML tends to become a powerful recognizer of spams.  However in
   the author's opinion this also substantially increases the
   likelihood of false positives if someone does send a non-spam email
   containing HTML tags.  SpamProbe does pull urls from inside of html
   tags however since those tend to be spammer specific.

 * Supports MBOX, MBX and Maildir mailbox formats.

 * Analyzes image attachments to extract useful terms for scoring.
   This catches the majority of spams with only images and no words.

 * Locks mboxes and databases using fcntl file locking to avoid
   problems when multiple emails arrive simultaneously.  Can
   optionally be compiled to use Berkeley DB's environment functions
   to provide better interaction with db_dump, db_verify, etc.

 * Scores only the Received, Subject, To, From, and Cc headers.  All
   other headers are ignored to make it hard for spammers to hide
   non-spammy words in X- headers to fool the filter.  The -H command
   line option can be used to override this.

 * Supports Content-Length: field in mbox headers.  This can be
   disabled using -Y option to use only From_ to recognize new
   messages.

 * Uses MD5 digest of emails to recognize reclassification of an
   already classified spam to avoid distortion of the word counts if
   emails are reclassified.  This way emails can be kept in an mbox
   that is repeatedly scanned by spamprobe without counting them more
   than once.  Digest printed along with score can be stored in emails
   so that binary attachments can be removed by MUA after scoring
   without breaking SpamProbe's ability to reclassify the email later.

 * Provides a time stamp based database cleanup command to remove
   terms from the database if their counts never rise above a certain
   threshold value (normally 2).  Tends to limit the otherwise
   unbounded growth of the database as each message adds some unique
   terms.  Also provides a purge command to remove all terms with
   counts below a specified minimum no matter what their age.

 * The -X option uses a more aggressive scoring algorithm which uses
   all significantly good or spammy terms when scoring a message and
   also gives more weight to within message frequency of terms.  This
   method improves recall and works best when the database contains a
   few thousand messages.

 * edit-term command allows users to directly modify the counts of
   individual terms.  For example to force a particular term to be
   considered spammy or good.

 * train mode can be used in place of receive to minimize database
   writes by only updating database for difficult to classify
   messages.

 * auto-train mode can be used to build a database efficiently from a
   collection of email stored in multiple files.

 * Multiple database options including BerkeleyDB, PBL, and fixed size
   hash file.


INSTALLATION

SpamProbe is distributed in source code form.  The current version
uses autoconf to handle generating an appropriate Makefile for your
system.  Generally you should be able to install SpamProbe like this:

  ./configure
  make
  make install

The configure script accepts some useful command line options
including:

 --prefix=/path/to/install

   Use this to specify a directory tree to install SpamProbe and its
   man page into.  The default is /usr/local.

 --with-pbl=/path/to/pbl

   Use this to specify the directory where you installed PBL (Peter
   Graf's Program Base Library) if the configure script is not able to
   find it on its own.  See notes below for more information about the
   relative merits of PBL and Berkeley DB.

 --with-db=/path/to/db

   If you would rather use Berkeley DB than PBL use this to specify
   the directory where you installed Berkeley DB if the configure
   script is not able to find it on its own.

 --enable-cdb

   Use this to enable use of Berkeley DB's CDB mode.  In this mode SP
   will create and use a Berkeley DB "environment" and take advantage
   of the "concurrent data store" feature of Berkeley DB.
   Theoretically this allows better interaction with Berkeley DB's
   utilities (db_verify, db_recover, etc) but some people have
   reported problems with this mode so it is now optional.

 --enable-default-8bit

   Use this if you want to change SP's default handling of eight bit
   characters.  Normally 8 bit characters are converted to the letter
   z in the database to save space (this can be changed at runtime
   using the -8 command line option).  If you normally receive 8 bit
   characters in email then you might want to compile with this flag
   to avoid the need to remember to type -8 all of the time.

 --enable-assert

   Use this if you want to enable the assertions in the source code
   for debugging purposes.  Generally this is not desirable for
   production use since the assertions reduce performance.

 --enable-big-endian

   Causes all database reads and writes to use big-endian byte order
   even on little-endian platforms.  Use this if you need to copy a
   database across different platforms and want to guarantee byte
   level data portability.  This option slows down database I/O on
   little endian systems.

If you want to statically link the BerkeleyDB library run configure
with the appropriate command line option in the LDFLAGS variable.  For
gcc you do could something like this:

  LDFLAGS=-static ./configure
  make
  make install

If you want to use Berkeley DB and configure does not find your
BerkeleyDB library and you know it is installed but has a non-standard
name then you can explicitly define the library to use.  For example
some RedHat 6.2 systems might require:

  CPPFLAGS=-I/usr/include/db3 ./configure

You need GNU G++ or some other C++ compiler to compile SpamProbe.  You
also need either PBL or Berkeley DB installed on your system.  PBL
uses more disk space and may require you to download and install it on
your system (see below) but is preferred because it supports
transactions and is faster under heavy loads.

Berkeley DB comes preinstalled on some operating systems so using it
may be easier.  It also creates smaller data files than PBL.  However
some users have reported data corruption issues with Berkeley DB in
the past so use of PBL is recommended.  That having been said the
author of SpamProbe used Berkeley DB for almost a year without any
problems in production so the reported issues seem to be OS or system
specific.


BUILD NOTES USING PBL

To obtain PBL download it from Peter Graf's web site:

  http://mission.base.com/peter/source/

PBL does not use configure for its build process however its makefile
seems to be appropriate out of the box for Red Hat Linux systems and
may work without changes on other posix-like systems as well.  To
compile SpamProbe with PBL I did the following:

  <downloaded pbl_1_03.tar.gz to the current directory>
  tar xzvf pbl_1_03.tar.gz
  cd pbl_1_03
  make
  cd ..
  <downloaded spamprobe-1.0.tar.gz to the current directory>
  tar xzvf spamprobe-1.0.tar.gz
  cd spamprobe-1.0
  ./configure --with-pbl=../pbl_1_03
  make
  make install


BUILD NOTES USING BERKELEY DB

To obtain BerkeleyDB if you do not already have it installed (many
Linux distributions come with DB preinstalled) visit the SleepCat
website to download a copy.  BerkeleyDB is open source and you can use
it for free.

  http://www.sleepycat.com

Installation instructions for BerkeleyDB can be found here:

  http://www.sleepycat.com/docs/ref/build_unix/intro.html


NOTES USING HASH DATABASE

SpamProbe can use a simple, fixed size hash data file as an
alternative to PBL or BDB.  There are two advantages to the hash
format.  The first is speed.  In my experiments the hash file format
is around 2x the speed of PBL (ranged from 1.8x to 3.5x). The second
advantage is that the hash data file size is fixed.  You choose a size
when you create the file and it never changes.  File size can be
anywhere from 1-100 MB. You need to choose a size large enough to hold
your terms with room to spare.  More on that later.

The hash file format also has significant disadvantages.  Becuase the
file size is fixed you must monitor the file to ensure that it does
not become overly full.  When the file becomes more than half full
performance will suffer.  Also the hash format does not store original
terms so you cannot use the dump command to learn what terms are
spammy or hammy in your database.  Finally, the hash format is
imprecise.  Hash collisions can cause the counts from different terms
to be mixed together which can reduce accuracy.

To create a hash data file you add a prefix to the directory name in
the -d command line option.  You can specify just the directory like
this:

  spamprobe -d hash:$HOME/.spamprobe

or you can add a size in megabytes for the file like this:

  spamprobe -d hash:42:$HOME/.spamprobe

The size is only used when a file is first created.  SP auto detects
the size of an existing hash file.  You need to allow enough space for
twice as many terms as you are likely to have in your file.  In my
database I have 2.2 million terms.  That required a database of are 53
MB.  SP uses 12 bytes per term in the hash file so you can estimate
the file size you'll need by multiplying the number of terms by 24.

The hash format does not store the original terms.  Instead it stores
the 32 bit hash code for each term.  You can do just about anything
with a hash file that you could with a PBL file including
import/export, edit-term, cleanup, purge, etc.  You can use export
your PBL database and import it to build a hash file (note that you
cannot go the other direction) and you can export one hash file and
import into a new one to enlarge your file.


SETUP OF SPAMPROBE FOR USERS

Once you have a spamprobe executable copy it to someplace in your PATH
so that procmail can find it.  Then create a directory for SpamProbe
to store its databases in.  By default SpamProbe wants to use the
directory ~/.spamprobe.  You must create this directory manually in
order to run SpamProbe or else specify some other directory using the
-d option.  Something like this should suffice:

  spamprobe create-db

or to use a different directory add the -d command line option:

  spamprobe -d somedirectory create-db

SpamProbe can use either the PBL or Berkeley DB library for its
databases.  Both are fast on local file systems but very slow over
NFS.  Please ensure that your spamprobe directory is on a local file
system to ensure good performance.


MAILDIR FORMAT

SpamProbe will accept a maildir directory name anywhere that an Mbox
or MBX file name can be specified.  When SpamProbe encounters a
Maildir mailbox (directory) name it will automatically process all of
the non-hidden files in the cur and new subdirectories of the mailbox.
There is no need to individually specify these subdirectories.


GETTING STARTED

SpamProbe is not a stand alone mail filter.  It doesn't sort your mail
or split it into different mailboxes.  Instead it relies on some other
program such as procmail to actually file your mail for you.  What
SpamProbe does do is track the word counts in good and spam emails and
generate a score for each email that indicates whether or not it is
likely to be spam.  Scores range from 0 to 1 with any score of 0.9 or
higher indicating a probable spam.

Personally I use SpamProbe with procmail to filter my incoming email
into mail boxes.  I have procmail score each inbound email using
SpamProbe and insert a special header into each email containing its
score.  Then I have procmail move spams into a special mailbox.

No spam filter is perfect and SpamProbe sometimes makes mistakes.  To
correct those mistakes I have a special mailbox that I put undetected
spams into.  I run SpamProbe periodically and have it reclassify any
emails in that mailbox as spam so that it will make a better guess the
next time around.

This is not a procmail primer.  You will need to ensure that you have
procmail and formail installed before you can use this technique.
Also I recommend that you read the procmail documentation so that you
can fully understand this example and adapt it to your own needs.
That having been said, my .procmailrc file looks like this:

    MAILDIR=$HOME/IMAP

    :0 c
    saved

    :0
    SCORE=| /home/brian/bin/spamprobe train
    :0 wf
    | formail -I "X-SpamProbe: $SCORE"
    :0 a:
    *^X-SpamProbe: SPAM
    spamprobe

I use IMAP to fetch my email so my mailboxes all live in a directory
named IMAP on my mail server.

NOTE: The first stanza copies all incoming emails into a special mbox
called saved.  SpamProbe IS OPEN SOURCE SOFTWARE and though it works
well for me it is possible that it could somehow lose emails.  Caution
is always a good idea.  That having been said, with the procmailrc
file as shown above the worst that could happen if SpamProbe crashes
is that the email would not be scored properly and procmail would
deliver it to your inbox.  Of course if procmail crashes all bets are
off.

The second stanza runs spamprobe in "train" mode to score the email,
classify it as either spam or good, and possibly update the database.
The train command tries to minimize the number of database updates by
only updating the database with terms from an incoming message if
there was insufficient confidence in the message's score.  The train
command always updates the database on the first 1500 of each type
received.  This ensures that sufficient email is classified to allow
the filter to operate reliably.

The next stanza runs formail to add a custom header to the email
containing the SpamProbe score.  The final stanza uses the contents of
the custom header to file detected spams into a special mbox named
spamprobe.

As an alternative to using the train command, you can run spamprobe in
"receive" mode.  In that mode SpamProbe scores the email and then
classifies it as either spam or good based on the score.  It always
automatically adds the word counts for the email to the appropriate
database.  This is essentially like running in score mode followed
immediately by either spam or good mode.  It produces more database
I/O and a bigger database but ensures that every message has its terms
reflected in the database.  Personally I use train mode.  A sample
procmailrc file using the receive command looks like this:

    MAILDIR=$HOME/IMAP

    :0 c
    saved

    :0
    SCORE=| /home/brian/bin/spamprobe receive
    :0 wf
    | formail -I "X-SpamProbe: $SCORE"
    :0 a:
    *^X-SpamProbe: SPAM
    spamprobe

If you need to have a locale set when spamprobe is run by procmail you
may need to use a script inside of the procmailrc file rather than
running spamprobe directly.  The script can set up any command line
options or environment variables before running spamprobe.  The most
common use of this technique is to set the LANG environment variable.


MAKING CORRECTIONS

SpamProbe is not perfect.  It is able to detect over 99% of the spams
that I receive but some still slip through.  To correct these missed
emails I run SpamProbe periodically and have it scan a special mbox.
Since I use IMAP to retrieve my emails I can simply drop undetected
spams into this mbox from my mail client.  If you use POP or some
other system then you will need to find a way get the undetected spams
into a mbox that spamprobe can see.

Periodically I run a script that scans three special mboxes to correct
errors in judgment:

    #!/bin/sh

    IMAPDIR=$HOME/IMAP

    spamprobe remove $IMAPDIR/remove
    spamprobe good $IMAPDIR/nonspam
    spamprobe spam $IMAPDIR/spam
    spamprobe train-spam $IMAPDIR/spamprobe

From this example you can see that I use three special mboxes to make
corrections.  I copy emails that I don't want spamprobe to store into
the remove mbox.  This is useful if you receive email from a friend or
colleague that looks like spam and you don't want it to dilute the
effectiveness of the terms it contains.

Undetected spams go into the spam mbox.  SpamProbe will reclassify
those emails as spam and correct its database accordingly.  Note that
doing this does not guarantee that the spam will always be scored as
spam in the future.  Some spams are too bland to detect perfectly.
Fortunately those are very rare.

The nonspam mbox is for any false positives.  These are always
possible and it is important to have a way to reclassify them when
they do occur.

If you are using receive mode rather than train mode then the above
script can be modified to remove the train-spam line. For example:

    #!/bin/sh

    IMAPDIR=$HOME/IMAP

    spamprobe remove $IMAPDIR/remove
    spamprobe good $IMAPDIR/nonspam
    spamprobe spam $IMAPDIR/spam

Finally you'll need to build a starting database.  Since SpamProbe
relies on word counts from past emails it requires a decent sized
database to be accurate.  To build the database select some of your
mboxes containing past emails.  Ideally you should have one mbox of
spams and one or more of non-spams.  If you don't have any spams handy
then don't worry, SpamProbe will gradually become more accurate as you
receive more spams.  Expect a fairly high false negative (i.e. missed
spams) rate as you first start using SpamProbe.

To import your starting messages use commands such as these.  The
example assumes that you have non-spams stored in a file named mbox in
your home directory and some spams stored in a file named nasty-spams.
Replace these names with real ones.

  spamprobe good ~/mbox
  spamprobe spam ~/nasty-spams


COMMAND LINE USAGE

SpamProbe accepts a small set of commands and a growing set of options
on the command line in addition to zero or more file names of mboxes.
The general usage is:

  spamprobe [options] <command> [filename...]

You can obtain a full list of all available command line options
by running the "help" command.  For example:

  spamprobe help

The recognized options are:

 -a char

    By default SpamProbe converts non-ascii characters (characters
    with the most significant bit set to 1) into the letter 'z'.  This
    is useful for lumping all Asian characters into a single word for
    easy recognition.  The -a option allows you to change the
    character to something else if you don't like the letter 'z' for
    some reason.

 -c

    Tells spamprobe to create the database directory if it does not
    already exist.  Normally spamprobe exits with a usage error if
    the database directory does not already exist.

 -C number

    Tells SpamProbe to assign a default, somewhat neutral, probability
    to any term that does not have a weighted (good count doubled)
    count of at least number in the database.  This prevents terms
    which have been seen only a few times from having an unreasonable
    influence on the score of an email containing them.

    The default value is 5.  For example if number is 5 then in order
    for a term to use its calculated probability it must have been
    seen 3 times in good mails, or 2 times in good mails and once in
    spam, or 5 times in spam, or some other combination adding up to
    at least 5.

 -d directory

    By default SpamProbe stores its database in a directory named
    .spamprobe under your home directory.  The -d option allows you to
    specify a different directory to use.  This is necessary if your
    home directory is NFS mounted for example.

    The directory name can be prefixed with a special code to force
    SpamProbe to use a particular type of data file format.  The type
    codes depend on how your copy of SpamProbe was compiled.  Defined
    types include:

      Example                   Description
      -d pbl:path               Forces the use of PBL data file.
      -d hash:path              Forces the use of an mmapped hash file.
      -d split:path             Forces the use of a hash file and ISAM
                                file (may provide better precision than
                                plain hash in some cases).

    The hash: option can also specify a desired file size in megabytes
    before the path.  For example -d hash:19:path would cause
    SpamProbe to use a 19 MB hash file.  The size must be in the range
    of 1-100.  The default hash file size is 16 MB.  Because hash
    files have a fixed size and capacity they should be cleaned
    relatively often using the cleanup command (see below) to prevent
    them from becoming full or being slowed by too many hash key
    collisions.

    Hash files provide better performance than either of the ISAM
    options (PBL or Berkeley DB).  However hash files do not store the
    original terms.  Only a 32 bit hash key is stored with each term.
    This prevents a user from exploring the terms in the database
    using the dump command to see what words are particularly spammy
    or hammy.

 -D directory

    Tells SpamProbe to use the database in the specified directory
    (must be different than the one specified with the -d option) as a
    shared database from which to draw terms that are not defined in
    the user's own database.  This can be used to provide a baseline
    database shared by all users on a system (in the -D directory) and
    a private database unique to each user of the system
    ($HOME/.spamprobe or -d directory).

 -g field_name

    Tells SpamProbe what header to look for previous score and message
    digest in.  Default is X-SpamProbe.  Field name is not case
    sensitive.  Used by all commands except receive.

 -h

    By default SpamProbe removes HTML markup from the text in emails
    to help avoid false positives.  The -h option allows you to
    override this behavior and force SpamProbe to include words from
    within HTML tags in its word counts.  Note that SpamProbe always
    counts any URLs in hrefs within tags whether -h is used or not.
    Use of this option is discouraged.  It can increase the rate of
    spam detection slightly but unless the user receives a significant
    amount of HTML emails it also tends to increase the number of
    false positives.

 -H option

    By default SpamProbe only scans a meaningful subset of headers
    from the email message when searching for words to score.  The -H
    option allows the user to specify additional headers to scan.
    Legal values are "all", "nox", "none", or "normal".  "all" scans
    all headers, "nox" scans all headers except those starting with
    X-, "none" does not scan headers, and "normal" scans the normal
    set of headers.

    In addition to those values you can also explicitly add a header
    to the list of headers to process by adding the header name in
    lower case preceded by a plus sign.  Multiple headers can be
    specified by using multiple -H options.  For example, to include
    only the From and Received headers in your train command you could
    run spamprobe as follows:

      spamprobe -Hnone -H+from -H+received train

    You can also selectively ignore headers that would otherwise be
    processed by using -H-headername.  For example to process all
    headers except for Subject you could run spamprobe as follows:

      spamprobe -Hall -H-subject train

    To process the normal set of headers but also add the SpamAssassin
    header X-SpamStatus you could run spamprobe as follows:

      spamprobe -H+x-spam-status train

 -l number

   Changes the spam probability threshold for emails from the default
   (0.7) to number.  The number must be a between 0 and 1.  Generally
   the value should be above 0.5 to avoid a high false positive rate.
   Lower numbers tend to produce more false positives while higher
   numbers tend to reduce accuracy.

 -m

    Forces SpamProbe to use mbox format for reading emails in receive
    mode.  Normally SpamProbe assumes that the input to receive mode
    contains a single message so it doesn't look for message breaks.

 -M

    Forces SpamProbe to treat the entire input as a single message.
    This ignores From lines and Content-Length headers in the input.

 -o option_name

    Enables special options by name.  Currently the only special
    options are:

      -o graham

        Causes SpamProbe to emulate the filtering algorithm originally
        outlined in A Plan For Spam.

      -o honor-status-header

        Causes SpamProbe to ignore messages if they have a Status:
        header containing a capital D.  Some mail servers use this
        status to indicate a message that has been flagged for
        deletion but has not yet been purged from the file.

        DO NOT use this option with the receive or train command in
        your procmailrc file!  Doing so could allow spammers to bypass
        the filter.  This option is meant to be used with the
        train-spam and train-good commands in scripts that
        periodically update the database.

      -o honor-xstatus-header

        Causes SpamProbe to ignore messages if they have a X-Status:
        header containing a capital D.  Some mail servers use this
        status to indicate a message that has been flagged for
        deletion but has not yet been purged from the file.

        DO NOT use this option with the receive or train command in
        your procmailrc file!  Doing so could allow spammers to bypass
        the filter.  This option is meant to be used with the
        train-spam and train-good commands in scripts that
        periodically update the database.

      -o ignore-body

        Causes SpamProbe to ignore terms from the message body when
        computing a score.  This is not normally recommended but might
        be useful in conjunction with some other filter.  For example,
        the whitelist option (see below) implicitly ignores the
        message body.

      -o orig-score

        Causes SpamProbe to use its original scoring algorithm that
        produces excellent results but tends to generate scores of
        either 0 or 1 for all messages.

      -o suspicious-tags

        Causes SpamProbe to scan the contents of "suspicious" tags for
        tokens rather than simply throwing them out.  Currently only
        font tags are scanned but other tags may be added to this list
        in later versions.

      -o tokenized

        Causes SpamProbe to read tokens one per line rather than
        processing the input as mbox format.  This allows users to
        completely replace the standard spamprobe tokenizer if they
        wish and instead use some external program as a tokenizer.
        For example in your procmailrc file you could use:

         SCORE=| tokenize.pl | /bin/spamprobe -o tokenized train

        In this mode SpamProbe considers a blank line to indicate the
        end of one message's tokens and the start of a new message's
        tokens.  SpamProbe computes a message digest based on the
        lines of text containing the tokens.

      -o whitelist

        Causes SpamProbe to use information from the email's headers
        to identify whether or not the email is from a legitimate
        correspondent.  The message body is ignored as are any never
        before seen terms and phrases in the headers.  This option can
        be used with the score command in a procmailrc file to use a
        bayesian white list in conjunction with some other filter or
        rule external to SpamProbe.

    The -o option can be used multiple times and all requested options
    will be applied.  Note that some options might conflict with each
    other in which case the last option would take precedence.

 -p number

    Changes the maximum number of words per phrase.  Default value is
    two.  Increasing the limit improves accuracy somewhat but
    increases database size.  Experiments indicate that increasing
    beyond two is not worth the extra cost in space.

 -P number

    Causes spamprobe to perform a purge of all terms with junk count
    less than or equal 2 after every number messages are processed.
    Using this option when classifying a large collection of spam can
    prevent the database from growing overly large at the cost of more
    processing time and possible loss of precision.

 -r number

    Changes the number of times that a single word/phrase can occur
    in the top words array used to calculate the score for each
    message.  Allowing repeats reduces the number of words overall
    (since a single word occupies more than one slot) but allows words
    which occur frequently in the message to have a higher weight.
    Generally this is changed only for optimization purposes.

 -R

    Causes spamprobe to treat the input as a single message and to
    base its exit code on whether or not that message was spam.  The
    exit code will be 0 if the message was spam or 1 if the message
    was good.

 -s number

    SpamProbe maintains an in memory cache of the words it has seen in
    previous messages to reduce disk I/O and improve performance.  By
    default the cache will contain the most recently accessed 2,500
    terms.  This number can be changed using the -s option.  Using a
    larger the cache size will cause SpamProbe to use more memory and,
    potentially, to perform less database I/O.

    A value of zero causes SpamProbe to use 100,000 as the limit which
    effectively means that the cache will only be flushed at program
    exit (unless you have really enormous mailbox files).  The cache
    doesn't affect receive, dump, or export but has a significant
    impact on the others.

 -T
    Causes SpamProbe to write out the top terms associated with each
    message in addition to its normal output.  Works with find-good,
    find-spam, and score.

 -v

    When it appears once on the command line this option tells
    SpamProbe to write verbose information during processing.  When it
    appears twice on the command line this option tells SpamProbe to
    write debugging information to stderr.  This can be useful for
    debugging or for seeing which terms SpamProbe used to score each
    email.

 -V

    Prints version and copyright information and then exits.

 -w number

    Changes the number of most significant words/phrases used by
    SpamProbe to calculate the score for each message.  Generally this
    is changed only for optimization purposes.

 -x

    Normally SpamProbe uses only a fixed number of top terms (as set
    by the -w command line option) when scoring emails.  The -x option
    can be used to allow the array to be extended past the max size if
    more terms are available with probabilities <= 0.1 or >= 0.9.

 -X

    An interesting variation on the scoring settings.  Equivalent to
    using "-w5 -r5 -x" so that generally only words with probabilites
    <= 0.1 or >= 0.9 are used and word frequencies in the email count
    heavily towards the score.  Tests have shown that this setting
    tends to be safer (fewer false positives) and have higher recall
    (proper classification of spams previously scored as spam)
    although its predictive power isn't quite as good as the default
    settings.  WARNING: This setting might work best with a fairly
    large corpus, it has not been tested with a small corpus so it
    might be very inaccurate with fewer than 1000 total messages.

 -Y

    Assume traditional Berkeley mailbox format, ignoring any
    Content-Length: fields.

 -7

    Tells SpamProbe to ignore any characters with the most significant
    bit set to 1 instead of mapping them to the letter 'z'.

 -8

    Tells SpamProbe to store all characters even if their most
    significant bit is set to 1.


SpamProbe recognizes the following commands:

 spamprobe help [command]

   With no arguments spamprobe lists all of the valid commands.
   If one or more commands are specified after the word help,
   spamprobe will print a more verbose description of each command.

 spamprobe create-db

   If no database currently exists spamprobe will attempt to create
   one and then exit.  This can be used to bootstrap a new
   installation.  Strictly speaking this command is not necessary
   since the train-spam, train-good, and auto-train commands will also
   create a database if none already exists but some users like to
   create a database as a separate installation step.

 spamprobe create-config

   Writes a new configuration file named spamprobe.hdl into the
   database directory (normally $HOME/.spamprobe).  Any existing
   configuration file will be overwritten so be sure to make a copy
   before invoking this command.

 spamprobe receive [filename...]

   Tells SpamProbe to read its standard input (or a file specified
   after the receive command) and score it using the current
   databases.  Once the message has been scored the message is
   classified as either spam or non-spam and its word counts are
   written to the appropriate database.  The message's score is
   written to stdout along with a single word.  For example:

     SPAM 0.9999999 595f0150587edd7b395691964069d7af

   or

     GOOD 0.0200000 595f0150587edd7b395691964069d7af

   The string of numbers and letters after the score is the message's
   "digest", a 32 character number which uniquely identifies the
   message.  The digest is used by SpamProbe to recognize messages
   that it has processed previously so that it can keep its word
   counts consistent if the message is reclassified.

   Using the -T option additionally lists the terms used to produce
   the score along with their counts (number of times they were found
   in the message).

 spamprobe train [filename...]

   Functionally identical to receive except that the database is only
   modified if the message was "difficult" to classify.  In practice
   this can reduce the number of database updates to as little as 10%
   of messages received.

 spamprobe score [filename...]

   Similar to receive except that the database is not modified in
   any way.

 spamprobe summarize [filename...]

   Similar to score except that it prints a short summary and score
   for each message.  This can be useful when testing.  Using the -T
   option additionally lists the terms used to produce the score along
   with their counts (number of times they were found in the message).

 spamprobe find-spam [filename...]

   Similar to score except that it prints a short summary and score
   for each message that is determined to be spam.  This can be useful
   when testing.  Using the -T option additionally lists the terms
   used to produce the score along with their counts (number of times
   they were found in the message).

 spamprobe find-good [filename...]

   Similar to score except that it prints a short summary and score
   for each message that is determined to be good.  This can be useful
   when testing.  Using the -T option additionally lists the terms
   used to produce the score along with their counts (number of times
   they were found in the message).

 spamprobe auto-train {SPAM|GOOD filename...}...

   Attempts to efficiently build a database from all of the named
   files.  You may specify one or more file of each type.  Prior to
   each set of file names you must include the word SPAM or GOOD to
   indicate what type of mail is contained in the files which follow
   on the command line.

   The case of the SPAM and GOOD keywords is important.  Any number of
   file names can be specified between the keywords.  The command line
   format is very flexible.  You can even use a find command in
   backticks to process whole directory trees of files. For example:

     spamprobe auto-train SPAM spams/* GOOD `find hams -type f`

   SpamProbe pre-scans the files to determine how many emails of each
   type exist and then trains on hams and spams in a random sequence
   that balances the inflow of each type so that the train command can
   work most effectively.  For example if you had 400 hams and 400
   spams, auto-train will generally process one spam, then one ham,
   etc.  If you had 4000 spams and 400 hams then auto-train will
   generally process 10 spams, then one ham, etc.

   Since this command will likely take a long time to run it is often
   desireable to use it with the -v option to see progress information
   as the messages are processed.

     spamprobe -v auto-train SPAM spams/* GOOD hams/* 

 spamprobe good [filename...]

   Scans each file (or stdin if no file is specified) and reclassifies
   every email in the file as non-spam.  The databases are updated
   appropriately.  Messages previously classified as good (recognized
   using their MD5 digest or message ids) are ignored.  Messages
   previously classified as spam are reclassified as good.

 spamprobe train-good [filename...]

   Functionally identical to "good" command except that it only
   updates the database for messages that are either incorrectly
   classified (i.e. classified as spam) or are "difficult" to
   classify.  In practice this can reduce amount of database updates
   to as little as 10% of messages.

 spamprobe spam [filename...]

   Scans each file (or stdin if no file is specified) and reclassifies
   every email in the file as spam.  The databases are updated
   appropriately.  Messages previously classified as spam (recognized
   using their MD5 digest of message ids) are ignored.  Messages
   previously classified as good are reclassified as spam.

 spamprobe train-spam [filename...]

   Functionally identical to "spam" command except that it only
   updates the database for messages that are either incorrectly
   classified (i.e. classified as good) or are "difficult" to
   classify.  In practice this can reduce amount of database updates
   to as little as 10% of messages.

 spamprobe remove [filename...]

   Scans each file (or stdin if no file is specified) and removes its
   term counts from the database.  Messages which are not in the
   database (recognized using their MD5 digest) are ignored.

 spamprobe cleanup [ junk_count [ max_age ] ]

   Scans the database and removes all terms with junk_count or less
   (default 2) which have not had their counts modified in at least
   max_age days (default 7).  You can specify multiple count/age pairs
   on a single command line but must specify both a count and an age
   for all but the last count.  This should be run periodically to
   keep the database from growing endlessly.

   For my own email I use cron to run the cleanup command every day
   and delete all terms with count of 2 or less that have not been
   modified in the last two weeks.  Here is the excerpt from my
   crontab:

       3 0 * * * /home/brian/bin/spamprobe cleanup 2 14

   Alternatively you might want to use a much higher count (1000 in
   this example) for terms that have not been seen in roughly six
   months:

       3 0 * * * /home/brian/bin/spamprobe cleanup 1000 180 2 14

   Because of the way that PBL and BerkeleyDB work the database file
   will not actually shrink, but newly added terms will be able to use
   the space previously occupied by any removed terms so that the
   file's growth should be significantly slower if this command is
   used.

   To actually shrink the database you can build a new one using the
   BerkeleyDB utility programs db_dump and db_load (Berkeley DB only)
   or the spamprobe import and export commands (either database
   library).  For example:

       cd ~
       mkdir new.spamprobe
       spamprobe export | spamprobe -d new.spamprobe import
       mv .spamprobe old.spamprobe
       mv new.spamprobe .spamprobe

   The -P option can also be used to limit the rate of growth of the
   database when importing a large number of emails.  For example if
   you want to classify 1000 emails and want SP to purge rare terms
   every 100 messages use a command such as:

     spamprobe -P 100 good goodmailboxname

   Using -P slows down the classification but can avoid the need to
   use the db_dump trick.  Using -P only makes sense when classifying
   a large number of messages.

 spamprobe purge [ junk_count ]

   Similar to cleanup but forces the immediate deletion of all terms
   with total count less than junk_count (default is 2) no matter how
   long it has been since they were modified (i.e. even if they were
   just added today). This could be handy immediately after
   classifying a large mailbox of historical spam or good email to
   make room for the next batch.

 spamprobe purge-terms regex

   Similar to purge except that it removes from the database all terms
   which match the specified regular expression.  Be careful with this
   command because it could remove many more terms than you expect.
   Use dump with the same regex before running this command to see
   exactly what will be deleted.

 spamprobe edit-term term good_count spam_count

   Can be used to specifically set the good and spam counts of a term.
   Whether this is truly useful is doubtful but it is provided for
   completeness sake.  For example it could be used to force a
   particular word to be very spammy or very good:

       spamprobe edit-term nigeria 0 1000000
       spamprobe edit-term burton  10000000 0

 spamprobe dump [ regex ]

   Prints the contents of the word counts database one word per line
   in human readable format with spam probability, good count, spam
   count, flags, and word in columns separated by whitespace.  PBL and
   Berkeley DB sort terms alphabetically.  The standard unix sort
   command can be used to sort the terms as desired.  For example to
   list all words from "most good" to "least good" use this command:

       spamprobe dump | sort -k 1nr -k 3nr

   To list all words from "most spammy" to "least spammy" use this
   command:

       spamprobe dump | sort -k 1n -k 2nr

   Optionally you can specify a regular expression.  If specified
   SpamProbe will only dump terms matching the regular expression.
   For example:

       spamprobe dump 'finance'
       spamprobe dump '\bfinance\b'
       spamprobe dump 'HSubject_.*finance'

 spamprobe tokenize [ filename ]

   Prints the tokens found in the file one word per line in human
   readable format with spam probability, good count, spam count,
   message count, and word in columns separated by whitespace.  Terms
   are listed in the order in which they were encountered in the
   message.  The standard unix sort command can be used to sort the
   terms as desired.  For example to list all words from "most good"
   to "least good" use this command:

       spamprobe tokenize filename | sort -k 1nr -k 3nr

   To list all words from "most spammy" to "least spammy" use this
   command:

       spamprobe tokenize filename | sort -k 1n -k 2nr

 spamprobe export

   Similar to the dump command but prints the counts and words in a
   comma separated format with the words surrounded by double quotes.
   This can be more useful for importing into some databases.

 spamprobe import

   Reads the specified files which must contain export data written by
   the export command.  The terms and counts from this file are added
   to the database.  This can be used to convert a database from a
   prior version.

 spamprobe exec command

   Obtains an exclusive lock on the database and then executes the
   command using system(3).  If multiple arguments are given after
   "exec" they are combined to form the command to be executed.  This
   command can be used when you want to perform some operation on the
   database without interference from incoming mail.  For example, to
   back up your .spamprobe directory using tar you could do something
   like this:

       cd
       spamprobe exec tar cf spamprobe-data.tar.gz .spamprobe

   If you simply want to hold the lock while interactively running
   commands in a different xterm you could use "spamprobe exec read".
   The linux read program simply reads a line of text from your
   terminal so the lock would effectively be held until you pressed
   the enter key.  Another option would be to use a shell as the
   command and type the commands into that shell:

       spamprobe /bin/bash
       ls
       date
       exit

   Be careful not to run spamprobe in the shell though since the
   spamprobe in the shell will wind up deadlocked waiting for the
   spamprobe running the exec command to release its lock.

 spamprobe exec-shared command

   Same as exec except that a shared lock is used.  This may be more
   appropriate if you are backing up your database since operations
   like score (but not train or receive) could still be performed on
   the database while the backup was running.

SUPPORT AND WARRANTY

SpamProbe works well for me.  However please keep in mind that there
is NO WARRANTY at all with this software.  Read the QPL (LICENSE.txt)
for details.  YOU ASSUME ALL RISK when using this software.

Be sure to visit the project page on sourceforge.  There you can
submit bug reports or feature requests, read and post messages on the
forums, and download the latest version.

  http://sourceforge.net/projects/spamprobe/

You can also join the spamprobe mailing list to discuss issues with
other SpamProbe users.

  http://lists.sourceforge.net/lists/listinfo/spamprobe-users

Also feel free to contact me at bburton@users.sourceforge.net with any
suggestions for improvements that you don't want to post to the
forums.

Enjoy!

Brian Burton
