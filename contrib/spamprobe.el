;;; spamprobe.el - spamprobe utils for emacs.
;; Copyright 2002 by Dave Pearson <davep@davep.org>
;; Revision: 1.6

;; spamprobe.el is free software distributed under the terms of the GNU
;; General Public Licence, version 2. For details see the file COPYING.

;;; Commentary:

;; I use spamprobe <URL:http://spamprobe.sourceforge.net/> to help me manage
;; my incoming mail and, because of this, I wrote spamprobe.el to provide me
;; with a small set of emacs based spamprobe tools.
;;
;; This code might only be useful to you if you handle UCE in a similar way
;; to how I do. I (attempt to) deliver all UCE to an mbox mail folder called
;; ~/Mail/Junk. All mail in there should contain a "X-SpamProbe:" header
;; (added to each email by a procmail rule). I then retain all that UCE so I
;; can see how well spamprobe is doing.
;;
;; The latest version of spamprobe.el can be found at:
;;
;;   <URL:http://www.davep.org/emacs/#spamprobe.el>

;;; Code:

;; (Not yet)Customize options.

(defvar spamprobe-spam-folder "~/Mail/Junk"
  "*Folder that holds known \"spam\".")

(defvar spamprobe-regexp "^X-SpamProbe: "
  "*Regular expression for a spamprobe result header.")

(defvar spamprobe-good-regexp (concat spamprobe-regexp "GOOD")
  "*Regular expression for a spamprobe \"good\" result header.")

(defvar spamprobe-spam-regexp (concat spamprobe-regexp "SPAM")
  "*Regular expression for a spamprobe \"spam\" result header.")

(defvar spamprobe-score-command "spamprobe score"
  "*Command used to score a file.")

;; Global variables.

(defvar spamprobe-checks 0)
(defvar spamprobe-hits 0)
(defvar spamprobe-misses 0)
(defvar spamprobe-last-file-info nil)

;; Main code:

(defun spamprobe-file-info ()
  "Get the file info for the spam folder"
  (let ((info (file-attributes spamprobe-spam-folder)))
    (list (nth 5 info) (nth 7 info))))

(defun spamprobe-grep-junk (re)
  "Grep the \"spam\" folder."
  (car (read-from-string (shell-command-to-string (format "grep -c \"%s\" %s" re spamprobe-spam-folder)))))

(defun spamprobe-calc ()
  "Do the junk folder calculations."
  (unless (equal spamprobe-last-file-info (spamprobe-file-info))
    (message "Calculating spam stats...")
    (setq spamprobe-last-file-info (spamprobe-file-info)
          spamprobe-hits           (spamprobe-grep-junk spamprobe-spam-regexp)
          spamprobe-misses         (spamprobe-grep-junk spamprobe-good-regexp)
          spamprobe-checks         (+ spamprobe-hits spamprobe-misses))))

(defun spamprobe-calc-checks ()
  "How many emails in the junk folder has spamprobe checked?"
  (spamprobe-calc)
  spamprobe-checks)

(defun spamprobe-calc-hits ()
  "How many emails did spamprobe correctly guess as junk?"
  (spamprobe-calc)
  spamprobe-hits)

(defun spamprobe-calc-misses ()
  "How many emails did spamprobe incorectly guess as ok?"
  (spamprobe-calc)
  spamprobe-misses)

(defun spamprobe-calc-effectiveness ()
  "How effective has spamprobe been so far?"
  (* (/ 100.0 (spamprobe-calc-checks)) (spamprobe-calc-hits)))

;;;###autoload
(defun spamprobe-stats ()
  "Display spamprobe stats."
  (interactive)
  (message "Checked: %d,  Hits: %d,  Missed: %d"
           (spamprobe-calc-checks)
           (spamprobe-calc-hits)
           (spamprobe-calc-misses)))

;;;###autoload
(defun spamprobe-effectiveness ()
  "How effective has spamprobe been at capturing junk mail?"
  (interactive)
  (message "spamprobe has caught %.2f%% of my junk email" (spamprobe-calc-effectiveness)))

(defun spamprobe-region-score (start end)
  "Get the spamprobe score for region START to END.

A `cons' is returned. The `car' is a string that says if the region is GOOD
or SPAM, the `cdr' is the score of the region."
  (let ((temp (make-temp-file "spamprobe-region")))
    (unwind-protect
         (progn
           (write-region start end temp)
           (message "Scoring region...")
           (let ((score (split-string (shell-command-to-string (format "%s %s" spamprobe-score-command temp)))))
             (cons (car score) (string-to-number (cadr score)))))
      (delete-file temp))))
  
;;;###autoload
(defun spamprobe-score-region (start end)
  "Display the spamprobe score for text in region START to END."
  (interactive "r")
  (let ((score (spamprobe-region-score start end)))
    (message "Score: %s (%f)" (car score) (cdr score))))
  
(provide 'spamprobe)

;;; spamprobe.el ends here
