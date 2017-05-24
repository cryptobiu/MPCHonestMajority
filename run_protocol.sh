#! /bin/bash
for i in `seq $1 1 $2`;
do	
	./MPCHonestMajority $i $3 $4 output.txt $5 $6 $7 $8 $9 0 &
	echo "Running $i..."
done
