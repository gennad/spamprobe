#!/usr/bin/env ruby
# Simple ruby script to count the number of scores in each
# category (good/bad).  Expects one floating point number
# per line as output by spamprobe score.

threshold = 0.9
if ARGV.size == 1
  threshold = ARGV.shift.to_f
end

min_score = 2.0
max_score = -1.0
spams = 0
goods = 0
STDIN.each do |line|
  judgement,score_str,digest = line.split
  score = score_str.to_f
  if judgement == 'SPAM'
    spams += 1
  else
    goods += 1
  end
  if score > max_score
    max_score = score
  end
  if score < min_score
    min_score = score
  end
end
total = spams + goods
pct = spams.to_f / (spams.to_f + goods.to_f) * 100.0
printf("GOOD %5d  SPAM %5d  TOTAL %5d  PCT %6.2f  MIN %8.6f  MAX %8.6f\n",
       goods, spams, total, pct, min_score, max_score)
