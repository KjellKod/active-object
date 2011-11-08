#
# Execute a unit test program in a loop.
# This can be useful to detect intermittent problems...
#

NOOFRUNS=100
UNIT_TEST=active-object_qt
OUTPUT=result.txt

echo "Running $UNIT_TEST in a loop $NOOFRUNS times with output redirected to $OUTPUT"

x=1
while [ $x -le $NOOFRUNS ]
do
  echo "running $UNIT_TEST..." $x
  echo "***** Running test $UNIT_TEST..#$x ***" >> $OUTPUT
  ../build/$UNIT_TEST >>$OUTPUT
  sleep 2
  x=$(( $x + 1 ))
done

echo "Now look for failed gtests..."
grep "FAIL" $OUTPUT
