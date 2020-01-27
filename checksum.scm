(define crc-32-table
  (case-lambda
    (() (crc-32-table #xedb88320))
    ((polynomial)
     (unless (<= 0 polynomial #xffffffff) (error "Bad CRC-32 polynomial"))
     (let ((table (make-u32vector 256 0)))
       (let loop-bytes ((byte 0))
         (if (= byte 256) table
             (let loop-bits ((bit 0) (crc byte))
               (cond ((= bit 8)
                      (u32vector-set! table byte crc)
                      (loop-bytes (+ byte 1)))
                     (else
                      (loop-bits (+ bit 1) (bitwise-xor
                                            (arithmetic-shift crc -1)
                                            (if (= 0 (bitwise-and crc 1))
                                                0 polynomial))))))))))))

(define (crc-32-byte table crc byte)
  (let ((first-byte (bitwise-and #xff (bitwise-xor crc byte)))
        (rest-bytes (arithmetic-shift crc -8)))
    (bitwise-xor (u32vector-ref table first-byte)
                 rest-bytes)))

(define (bitwise-not-32 crc)
  (bitwise-and #xffffffff (bitwise-xor #xffffffff crc)))

(define (crc-32 table port)
  (let loop ((i -1) (crc (bitwise-not-32 0)))
    (begin (display "byte ")
           (display i)
           (display " = ")
           (display (number->string crc 16))
           (newline))
    (let ((byte (read-u8 port)))
      (if (eof-object? byte)
          (bitwise-not-32 crc)
          (loop (+ i 1) (crc-32-byte table crc byte))))))

(let ((table (crc-32-table)))
  (let loop ((i 0))
    (unless (= i 256)
      (display (number->string (u32vector-ref table i) 16))
      (newline)
      (loop (+ i 1)))))
(display
 (number->string
  (call-with-port (open-input-file "crc.scm")
                  (lambda (port) (crc-32 (crc-32-table) port)))
  16))
(newline)
