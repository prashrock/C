;; ===== Set the highlight current line minor mode ===== 
;;(global-hl-line-mode 1)

;; ===== Eenable max colors !! ===== 
(setq font-lock-maximum-decoration t)

;; ===== Enable Linum-Mode and add space after the number  ===== 
;;(global-linum-mode t)
(setq linum-format "%d ")


;; ===== Enable SR-Speedbar for use in Terminal ===== 
(require 'sr-speedbar)

;; ========== Line by line scrolling ========== 
(setq scroll-step 1)

;; ===== Set standard indent to 2 rather that 4 ====
(setq standard-indent 4)

;; ========== Support Wheel Mouse Scrolling ==========
(mouse-wheel-mode t)

;; ========== Place Backup Files in Specific Directory ==========
;; Enable backup files.
(setq make-backup-files t)
;; Enable versioning with default values (keep five last versions, I think!)
(setq version-control t)
;; Save all backup file in this directory.
(setq backup-directory-alist (quote ((".*" . "~/.emacs_backups/"))))

;; ========== Enable Line and Column Numbering ==========
(line-number-mode 1)
(column-number-mode 1)

;; ========== Don't use GNU style indenting for braces ====
(setq c-default-style "linux"
      c-basic-offset 4)

;; ========== Backspace to work on terminal  ====
(global-set-key [(control h)] 'delete-backward-char)

;; ========== RET auto-indent  ====
(add-hook 'c-mode-common-hook '(lambda ()
      (local-set-key (kbd "RET") 'newline-and-indent)))

;; ========== Use tabs instead of spaces ====
(setq-default c-basic-offset 4
              tab-width 4
              indent-tabs-mode t)

;; ===== Set Tags For my use-case ===== 
(setq tags-table-list '("~/sample_progs/bld/TAGS"))

;; ===== Use F3 to auto-complete after a TAGS is loaded ===== 
(global-set-key [f3] 'complete-tag)

;; ===== Use F4 and F5 to Find all usage of a tag after TAGS is loaded ===== 
(global-set-key [f4] 'tags-search)
(global-set-key [f5] "\C-u\M-,")

;; ===== Use F6 to Goto Line Number ===== 
(global-set-key [f6] 'goto-line)

;; ===== Use F7 and F8 to Find Definition of a tag after TAGS is loaded ===== 
(global-set-key [f7] 'find-tag)
(global-set-key [f8] "\C-u\M-.")

;; ===== Use F12 to get line numbers ===== 
(global-set-key [f12] 'linum-mode)

;; ===== Use Arrow keeys to move between frames ===== 
(global-set-key (kbd "C-x <up>") 'windmove-up)
(global-set-key (kbd "C-x <down>") 'windmove-down)
(global-set-key (kbd "C-x <right>") 'windmove-right)
(global-set-key (kbd "C-x <left>") 'windmove-left)