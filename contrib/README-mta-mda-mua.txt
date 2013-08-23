filename: README-mta-mda-mua.txt
author: Anto Veldre <anto@xyz.ee>
ver 002, section 1.5 completely rewritten, minor clean-ups.

====================================================


What follows is some background information on
MUAs, MDAs and MTAs (mail user, delivery and transfer agents)

See also what opinion wiki has about the matter:
http://twiki.org/cgi-bin/view/Wikilearn/EmailServerTerminology
------------------------------------------------------


1.1 Admin domain versus user domain
===================================

The good news is that unlike with some virus scanners,
spamprobe installation avoids fiddling with sophisticated
system toys (like mail queues, MTA configurations files and
similar), which traditionally belong to UNIX administrator
domain.

All what spamprobe needs to attempt, is done in user space.
This means you could implement spamprobe without neccessarily
consulting your system administrator (which could be a problem at
universities and other over-managed sites).

But ... procmail is a prerequisite nevertheless.


1.2 sendmail + procmail
=======================

Chances are that procmail is already installed on your
system, because of sendmail needs it for local delivery.

Historically, sendmail is the King of MTAs
(http://www.sendmail.org). Provided your system is
sufficiently modern, both sendmail and procmail should be
installed. Sendmail decides whether or not should the message
be delivered to a _local_ user and if the answer
is YES, it hands the message over to procmail, which
is responsible for the actual delivery (appending the message
to the users mailbox somewhere at /var/spool/$user).

Commanding "grep procmail /etc/sendmail.conf" is usually
sufficient to estimate whether you have sendmail+procmail
preinstalled. You also can check the precence of procmail by
directly launching "procmail" (CTRL-D helps you out of there)
or checking the precence by "which procmail".

Another good news is that once procmail is installed
(and somehow implemented into your mail delivery process),
you do not have to configure it otherwise than via
normal rc (run-command) file which is ~/.procmailrc .

Visit http://www.procmail.org. "Interesting and Useful Links"
is the most valuable section there.


1.3 Other MTA's
===============

What to do if your system isn't sendmail based?
To use spamprobe, you have to work out some method
how MTA hands mail over to procmail. Other MTAs
are compatible with procmail to lesser or
greater extent.

For Qmail (http://www.qmail.org), procmail
is a normal satellite.

For Postfix (http://www.postix.org), procmail should be
specifically described as the local delivery agent
(in /etc/postfic/mai.cf).

For Exim (http://www.exim.org), a user only could pipe
messages to procmail via ~/.forward file (if not
prohibited by admin).

But expect trouble with outdated smail or an old MMDF
(SCO UNIX leftover). I hope no such bestia left on the
planet but nevertheless...

Any actual used experience is welcome to this section.



1.4 Bypassing the system-wide mail delivery system
==================================================

Generally, an admin-controlled MTA could be easily
substituted or bypassed by user space tools. This fact
itself is not good or bad, it simply is an option
for the user.

Various POP3 or IMAP clients allow you to collect
your messages from remote. Moreover - you could IMAP
or POP3 to _localhost_, this way the retrieved
messages (by default) are automatically erased from
your system mailbox.

Programs like fetchmail
(http://www.tuxedo.org/~esr/fetchmail/) could be used
to fully automate the mail collection process.

Mail readers generally are widely  configurable,
so you could point to your actual mailbox.

But be beware - graphical mail clients may
distort (sorry, enrich) the classical mailbox
structure used by mail, mailx, elm and pine.
Once read into, it isn't easy way out of an proprietary
mailbox.


1.5 My personal setup
=====================

The ~/.procmailrc file I use looks like:


	MAILDIR=/home/$me/INCOMING-MAIL
	:0 c
	saved
	:0
	SCORE=| /usr/local/bin/spamprobe receive
	:0 wf					
	| formail -I "X-SpamProbe: $SCORE"
	:0 a:
	*^X-SpamProbe: SPAM
	dirty
	:0 a:
	*^X-SpamProbe: GOOD
	cleaned


Now all incoming mail is stored at $MAILDIR/saved.
In addition to this, spamprobe will use two
self-explainingly named mailboxes as $MAILDIR/cleaned
and $MAILDIR/dirty.

The result is that my previous incoming mailbox
at /var/spool/mail/me will no more be used by
the procmail.  Why?  Because of after applying two mutually
excluding rules (store SPAM at dirty and GOOD at goodies)
no messages will be left for procmail to store at my previous
inbox (at /var/spool/mail/$me).

In excellent setups, $MAILDIR/saved file should be linked
to an NFS share (probability two HD's will synchronously 
fail outweights NFS reliability problems). In excellent
setups some cron script should periodically archive
what is stored in saved file (e.g. my-mail-pre-2002-12-27.gz).

Oh yeah, I almost forgot it - I am an elm addict for 10
years. Graphical MUAs may use a different paradigm and
accordingly these may prefer fetching mail directly by POP3/IMAP.


1.6 Bayesian technology
=======================

Seems like famous article by Paul Grahams inspired many
spam-fighters and a lot of more than 1 programmer started
to konqueror evil's territory with  statistical methods.
Searching for "bayesian" at http://www.freshmeat.net or
http://www.sourceforge.net will indicate the popularity
of the technology.


1.7 Security topics
===================

Generally, be cautious when playing with .procmailrc.
It is rather easy to create systemwide security blackholes
via "vacation" program 
(http://www.ayradyss.org/programs/current.html#vacation)
or piping the messages to programs (via ~/.forward file)

1.8 Cartago delenda est
=======================

I still think MUAs should obtain a "SPAM" button beside
"DELETE" one. Junk will be deleted in both cases, the difference
is that "SPAM" knob will first analyse it and only then delete.
Spamprobe is a good candidate to do the dirty work behind
"SPAM" button.

anto@xyz.ee
ES1LAU
