(load-extension "libguilemidi" "init")

(define midi-reader (midi-open-reader "mordhau input"))

(define (single-char-string str)
	(map (lambda (c) (make-string 1 c))
	     (string->list str)))

(define (midi-to-mordhau midi) (- midi 44))

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
				(send-key-strokes "`" (mordhau-eqcommand-string (midi-to-mordhau midi)) "\r")
				(usleep 20000)))))
	(loop))

; In mordhau, let's assume 16 is middle C
; which is 60 in midi
; Mordhau's range appears to be 0 - 100
; My keyboards is 21 - 108
; (midi-to-mordhau n (- n 44))
		                   ;"key" "shift+grave" ,@(single-char-string "equipmentcommand")))))

