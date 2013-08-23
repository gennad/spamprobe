#!/usr/bin/ruby
#
# A script to split a mbox file into two files with a randomly
# chosen set of emails in each file.  Used to create test cases
# for trying out different sets of filter parameters.
#

require 'md5'

if ARGV.length < 2
  STDERR.printf("usage: splitmail percentage outfileprefix [filename...]\n")
  exit 1
end

max_messages = 8000
ignore_exp = /^Subject:\s+DON.T DELETE THIS MESSAGE -- FOLDER INTERNAL DATA/
header_exp = /(^To:)|(^From:)|(^Date:)|(^Subject:)|(^Cc:)/
skip_pct = 5 + rand(10)

pct = ARGV.shift.to_i
prefix = ARGV.shift
files = [ File.open(prefix + ".1", "w"), File.open(prefix + ".2", "w") ];
first = [ true, true ]
digests = Hash.new

ARGV.each do |filename|
  msg_count = 0
  dup_count = 0
  digester = MD5.new
  message = ""
  on_blank = true
  in_body = false
  ignore_it = false
  File.open(filename).each do |line|
    if on_blank && line =~ /^From\s+\S+\s+\w\w\w\s+\w\w\w\s+\d\d?\s+\d\d\s*:\s*\d\d\s*:\s*\d\d\s+\d\d\d\d/
      if (message.length > 0) && (not digests.has_key?(digester.hexdigest) && (not ignore_it) && (rand(100) >= skip_pct))
        filenum = (rand(100) <= pct) ? 0 : 1
        if first[filenum]
          first[filenum] = false
        else
          files[filenum].print("\n")
        end
        files[filenum].print(message)
        digests.store(digester.hexdigest, 1)
        msg_count += 1
      elsif message.length > 0
        dup_count += 1
      end

      message = ""
      on_blank = false
      in_body = false
      ignore_it = false
      digester = MD5.new
      if msg_count > max_messages
        break
      end
    elsif line.length == 1
      on_blank = true
      in_body = true
    end
    if in_body
      if message.length < 4096
        digester.update(line)
      end
    elsif header_exp =~ line
      digester.update(line)
      ignore_it = ignore_it || (ignore_exp =~ line)
    end
    message.concat(line)
  end

  if (message.length > 0) && (not digests.has_key?(digester.hexdigest) && (not ignore_it))
    filenum = (rand(100) <= pct) ? 0 : 1
    if first[filenum]
      first[filenum] = false
    else
      files[filenum].print("\n")
    end
    files[filenum].print(message)
    msg_count += 1
  elsif message.length > 0
    dup_count += 1
  end

  printf("split file %s with %d unique messages and %d dups\n", filename, msg_count, dup_count)
end
