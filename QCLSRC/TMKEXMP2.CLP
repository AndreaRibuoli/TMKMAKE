#
# Make description file - Nested IF statements
#
LOG = ECHO

ROOTPGM:

NESTING=1

!if &NESTING
    @$(LOG) '    PGM STARTED '
! ifdef NESTING
     @$(LOG) '    PGM STARTED '
!  if &NESTING
      @$(LOG) '    PGM STARTED '
!   ifdef NESTING
       @$(LOG) '    PGM STARTED '
!    if &NESTING
        @$(LOG) '    PGM STARTED '
!     ifdef NESTING
         @$(LOG) '    PGM STARTED '
!      if &NESTING
          @$(LOG) '    PGM STARTED '
!       ifdef NESTING
           @$(LOG) '    PGM STARTED '
!        if &NESTING
            @$(LOG) '    PGM STARTED '
!         ifdef NESTING
             @$(LOG) '    PGM STARTED '
!         endif
            @$(LOG) '    PGM ENDED '
!        endif
           @$(LOG) '    PGM ENDED '
!       endif
          @$(LOG) '    PGM ENDED '
!      endif
         @$(LOG) '    PGM ENDED '
!     endif
        @$(LOG) '    PGM ENDED '
!    endif
       @$(LOG) '    PGM ENDED '
!   endif
      @$(LOG) '    PGM ENDED '
!  endif
     @$(LOG) '    PGM ENDED '
! endif
    @$(LOG) '    PGM ENDED '
!endif
    @$(LOG) '    PGM ENDED '
