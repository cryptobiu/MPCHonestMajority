#! /bin/bash
for i in `seq $1 1 $2`;
do	
	./MPCHonestMajority $i $3 $4 output.txt $5 ZpMensenne  $6 $7 $8 &
	echo "Running $i..."
done
