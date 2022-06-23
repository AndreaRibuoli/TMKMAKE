#
# Make description file
# echo will occur if either of the two common source
# members are out of date with the chosen program target.
# For example, two "makes" can be run pgm1 with src1 and src2 or
#                             pgm2 with src1 and src2

PROGRAM1<PGM> PROGRAM2<PGM> : commonsrc1.qcsrc commonsrc2.qcsrc
  echo program $(@F) needs to be updated due to common source
