#!/usr/bin/ruby
#
# A script to read an mbox of spams and perform a test on each to
# see how well the filter can score spams as they arrive.  After
# each message is score it is classified as spam and the next
# is scored.  This is intended to simulate normal use of the
# filter with incoming email.
#

if ARGV.length != 3
  STDERR.printf("usage: incrtest filename mail_type spamprobe\n")
  exit 1
end

class ScoreKeeper
  def initialize
    @goods = 0.to_f
    @spams = 0.to_f
    @classifies = 0
  end

  def isWrongType(score, mailtype)
    if score >= 0.9
      return mailtype == "good"
    else
      return mailtype == "spam"
    end
  end

  def addScore(score)
    if score >= 0.9
      @spams += 1
    else
      @goods += 1
    end
  end

  def addGood
    @goods += 1
  end

  def addSpam
    @spams += 1
  end

  def total
    @goods + @spams
  end

  def percentage
    100.0 * @spams / (@goods + @spams)
  end

  def printSummary
    printf("GOOD %10d   SPAM %10d  TOTAL  %10d  PCT %10.2f (%d errors)\n", @goods, @spams, total, percentage, @classifies)
  end

  def processMail(baseargs, mailtype, tmpname)
    score = 0
    args = baseargs + " score " + tmpname
    IO.popen(args, "r") do |f|
      a = f.readlines
      addScore(a[0].to_f)
    end
    if isWrongType(score, mailtype)
      @classifies += 1
      system(baseargs + " " + mailtype + " " + tmpname)
    end
  end
end


tmpname = '_temp'
filename = ARGV.shift
mailtype = ARGV.shift
baseargs = ARGV.shift

score_keeper = ScoreKeeper.new
on_blank = true
first = true
tmp = File.open(tmpname, "w")
File.open(filename).each do |line|
  if on_blank and line =~ /^From\s+\S+\s+\w\w\w\s+\w\w\w\s+\d\d?\s+\d\d\s*:\s*\d\d\s*:\s*\d\d\s+\d\d\d\d/
    on_blank = false
    if first
      first = false
    else
      tmp.close
      score_keeper.processMail(baseargs, mailtype, tmpname)
      tmp = File.open(tmpname, "w")
    end
  elsif line.length == 1
    on_blank = true
  end
  tmp.print(line)
end
tmp.close
score_keeper.processMail(baseargs, mailtype, tmpname)
score_keeper.printSummary
