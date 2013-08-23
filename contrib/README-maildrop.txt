HOWTO use SpamProbe with maildrop.

0. INTRODUCTION

  maildrop is a mail-filtering package, somewhat similar to procmail, but
  more robust and with a clearer syntax. It is available as part of the
  "Courier" mail package or standalone from
  http://www.flounder.net/~mrsam/maildrop/README.html

  This document shows two examples for your ~/.mailfilter that make use
  of SpamProbe.

1. LICENSE

  Copyright (c) 2002 Matthias Andree <matthias.andree@gmx.de>

  Permission is granted to copy, distribute and/or modify this document
  under the terms of the GNU Free Documentation License, Version 1.1 or
  any later version published by the Free Software Foundation; with the
  Invariant Sections being LICENSE, with no Front-Cover Texts and no
  Back-Cover Texts.

  A copy of the license can be obtained from
  http://www.gnu.org/licenses/fdl.html


2.1. THE SIMPLE SETUP

  Put this into your ~/.mailfilter file:

  =============================================================================
  # save mail to the "saved" mbox, better safe than sorry
  cc saved

  # score the mail and tag it
  SCORE=`spamprobe -8 receive`
  xfilter "reformail -I \"X-SpamProbe: $SCORE\""

  # if it's spam, reroute it to the spamprobe mbox
  if (/^X-SpamProbe: SPAM/)
    to "spamprobe"
  =============================================================================

2.2. USING SPAMASSASSIN AS TEACHER

  This assumes that spamd is running (for performance reasons, you should
  do this).

  Use the ~/.mailfilter setup from secton 2.1 above, and add these lines
  below the `to "spamprobe"' line:

  =============================================================================
  xfilter "/usr/bin/spamc"
  if (/^X-Spam-Flag: YES/)
  {
    cc "|spamassassin -d -L|spamprobe -8 spam"
    to "spamassassin"
  }
  =============================================================================

