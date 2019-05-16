(load-extension "libguilemidi" "init")

(define midi-reader (midi-open-reader "Mordhau input"))

(define (single-char-string str)
	(map (lambda (c) (make-string 1 c))
	     (string->list str)))

(define (midi-to-mordhau midi) (- midi 44))

;; The lute in Mordhau has a very limited range.
(define (in-mordhau-range? value) (<= 0 value 24))

(define (mordhau-eqcommand-string value)
	(string-append "equipmentcommand " (number->string value)))

(define (send-key-strokes . rest)
	(apply system* (append '("xdotool" "type" "--delay" "0") rest)))

(let loop () (begin
	(let* ((midi (midi-blocking-read midi-reader))
	       (value (midi-to-mordhau midi)))
		(if (in-mordhau-range? value)
			(begin
				(send-key-strokes "`" (mordhau-eqcommand-string value) "\r")
				;; When two notes are played at the same time somtimes one of
				;; the notes is lost, so sleep a little bit to hopefully prevent
				;; that. (0.16667 is 1/60 of a second, AKA one in-game frame)
				(usleep 16667)))))
	(loop))
